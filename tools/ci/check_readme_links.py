#!/usr/bin/env python
#
# Checks that all links in the readme markdown files are valid
#
# Copyright 2020 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import argparse
import concurrent.futures
import os
import os.path
import re
import sys
import urllib.error
import urllib.request
from collections import defaultdict, namedtuple
from pathlib import Path

EXCLUDE_DOCS_LIST = ['examples/peripherals/secure_element/atecc608_ecdsa/components/esp-cryptoauthlib/cryptoauthlib/**']

# The apple apps links are not accessible from the company network for some reason
EXCLUDE_URL_LIST = ['https://apps.apple.com/in/app/esp-ble-provisioning/id1473590141', 'https://apps.apple.com/in/app/esp-softap-provisioning/id1474040630']

Link = namedtuple('Link', ['file', 'url'])


class ReadmeLinkError(Exception):
    def __init__(self, file, url):
        self.file = file
        self.url = url


class RelativeLinkError(ReadmeLinkError):
    def __str__(self):
        return 'Relative link error, file - {} not found, linked from {}'.format(self.url, self.file)


class UrlLinkError(ReadmeLinkError):
    def __init__(self, file, url, error_code):
        self.error_code = error_code
        super().__init__(file, url)

    def __str__(self):
        files = [str(f) for f in self.file]
        return 'URL error, url - {} in files - {} is not accessible, request returned {}'.format(self.url, ', '.join(files), self.error_code)


# we do not want a failed test just due to bad network conditions, for non 404 errors we simply print a warning
def check_url(url, files, timeout):
    try:
        with urllib.request.urlopen(url, timeout=timeout):
            return
    except urllib.error.HTTPError as e:
        if e.code == 404:
            raise UrlLinkError(files, url, str(e))
        else:
            print('Unable to access {}, err = {}'.format(url, str(e)))
    except Exception as e:
        print('Unable to access {}, err = {}'.format(url, str(e)))


def check_web_links(web_links):

    with concurrent.futures.ThreadPoolExecutor(max_workers=10) as executor:
        errors = []
        future_to_url = {executor.submit(check_url, url, files, timeout=30): (url, files) for url, files in web_links.items()}
        for future in concurrent.futures.as_completed(future_to_url):
            try:
                future.result()
            except UrlLinkError as e:
                errors.append(e)

        return errors


def check_file_links(file_links):
    errors = []

    for link in file_links:
        link_path = link.file.parent / link.url

        if not Path.exists(link_path):
            errors.append(RelativeLinkError(link.file, link.url))

    print('Found {} errors with relative links'.format(len(errors)))
    return errors


def get_md_links(folder):
    MD_LINK_RE = r'\[.+?\]\((.+?)(#.+)?\)'

    idf_path = Path(os.getenv('IDF_PATH'))
    links = []

    for path in (idf_path / folder).rglob('*.md'):
        if any([path.relative_to(idf_path).match(exclude_doc) for exclude_doc in EXCLUDE_DOCS_LIST]):
            print('{} - excluded'.format(path))
            continue

        with path.open(encoding='utf8') as f:
            content = f.read()

        for url in re.findall(MD_LINK_RE, content):
            link = Link(path, url[0].lstrip())
            # Ignore "local" links
            if not link.url.startswith('#'):
                links.append(link)

    return links


def check_readme_links(args):

    links = get_md_links('examples')
    print('Found {} links'.format(len(links)))

    errors = []

    web_links = defaultdict(list)
    file_links = []

    # Sort links into file and web links
    for link in links:
        if link.url.startswith('http'):
                web_links[link.url].append(link.file)
        else:
            file_links.append(link)

    for url in EXCLUDE_URL_LIST:
        del web_links[url]

    errors.extend(check_file_links(file_links))

    if not args.skip_weburl:
        errors.extend(check_web_links(web_links))

    print('Found {} errors:'.format(len(errors)))
    for e in errors:
        print(e)

    return 1 if len(errors) > 0 else 0


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='check_readme_links.py: Checks for dead links in example READMEs', prog='check_readme_links.py')
    parser.add_argument('--skip-weburl', '-w', action='store_true', help='Skip checking of web URLs, only check links to local files')
    args = parser.parse_args()

    sys.exit(check_readme_links(args))
