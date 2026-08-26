#!/usr/bin/env python
#
# Checks that all links in the readme markdown files are valid
#
# SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import argparse
import concurrent.futures
import os
import os.path
import re
import sys
import urllib.error
import urllib.request
from collections import defaultdict
from collections import namedtuple
from pathlib import Path

Link = namedtuple('Link', ['file', 'url'])


class ReadmeLinkError(Exception):
    def __init__(self, file: str, url: str) -> None:
        self.file = file
        self.url = url


class RelativeLinkError(ReadmeLinkError):
    def __str__(self) -> str:
        return f'Relative link error, file - {self.url} not found, linked from {self.file}'


class UrlLinkError(ReadmeLinkError):
    def __init__(self, file: str, url: str, error_code: str):
        self.error_code = error_code
        super().__init__(file, url)

    def __str__(self) -> str:
        files = [str(f) for f in self.file]
        files_str = ', '.join(files)
        return (
            f'URL error, url - {self.url} in files - {files_str} is not accessible, request returned {self.error_code}'
        )


# we do not want a failed test just due to bad network conditions, for non 404 errors we simply print a warning
def check_url(url: str, files: str, timeout: float) -> None:
    try:
        with urllib.request.urlopen(url, timeout=timeout):
            return
    except urllib.error.HTTPError as e:
        if e.code == 404:
            raise UrlLinkError(files, url, str(e))
        else:
            print(f'Unable to access {url}, err = {str(e)}')
    except Exception as e:
        print(f'Unable to access {url}, err = {str(e)}')


def check_web_links(web_links: defaultdict) -> list:
    with concurrent.futures.ThreadPoolExecutor(max_workers=10) as executor:
        errors = []
        future_to_url = {
            executor.submit(check_url, url, files, timeout=30): (url, files) for url, files in web_links.items()
        }
        for future in concurrent.futures.as_completed(future_to_url):
            try:
                future.result()
            except UrlLinkError as e:
                errors.append(e)

        return errors


def check_file_links(file_links: list) -> list:
    errors = []

    for link in file_links:
        link_path = link.file.parent / link.url

        if not Path.exists(link_path):
            errors.append(RelativeLinkError(link.file, link.url))

    print(f'Found {len(errors)} errors with relative links')
    return errors


def get_md_links(folder: str) -> list:
    MD_LINK_RE = r'\[.+?\]\((.+?)(#.+)?\)'

    idf_path_str = os.getenv('IDF_PATH')
    if idf_path_str is None:
        raise RuntimeError("Environment variable 'IDF_PATH' wasn't set.")
    idf_path = Path(idf_path_str)
    links = []

    for path in (idf_path / folder).rglob('*.md'):
        with path.open(encoding='utf8') as f:
            content = f.read()

        for url in re.findall(MD_LINK_RE, content):
            link = Link(path, url[0].lstrip())
            # Ignore "local" links
            if not link.url.startswith('#'):
                links.append(link)

    return links


def check_readme_links(args: argparse.Namespace) -> int:
    links = get_md_links('examples')
    print(f'Found {len(links)} links')

    errors = []

    web_links = defaultdict(list)
    file_links = []

    # Sort links into file and web links
    for link in links:
        if link.url.startswith('http'):
            web_links[link.url].append(link.file)
        else:
            file_links.append(link)

    errors.extend(check_file_links(file_links))

    if not args.skip_weburl:
        errors.extend(check_web_links(web_links))

    print(f'Found {len(errors)} errors:')
    for e in errors:
        print(e)

    return 1 if len(errors) > 0 else 0


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='check_readme_links.py: Checks for dead links in example READMEs', prog='check_readme_links.py'
    )
    parser.add_argument(
        '--skip-weburl', '-w', action='store_true', help='Skip checking of web URLs, only check links to local files'
    )
    args = parser.parse_args()

    sys.exit(check_readme_links(args))
