# Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
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


# Mechanism to generate static HTML redirect pages in the output
#
# Uses redirect_template.html and the list of pages given in
# the file conf.html_redirect_pages
#
# Redirections can be internal or absolute (i.e., external).
#   - Internal redirects are supplied without quotation marks and must be relative to the document root
#   - External redirects are wrapped in doulbe quotation marks and are used verbatim
#
# Adapted from ideas in https://tech.signavio.com/2017/managing-sphinx-redirects
import os.path

from sphinx.builders.html import StandaloneHTMLBuilder

REDIRECT_TEMPLATE = """
<html>
  <head>
    <meta http-equiv="refresh" content="0; url=$NEWURL" />
    <script>
      window.location.href = "$NEWURL"
    </script>
  </head>
  <body>
  <p>Page has moved <a href="$NEWURL">here</a>.</p>
  </body>
</html>
"""


def setup(app):
    app.add_config_value('html_redirect_pages', [], 'html')
    # attaching to this event is a hack, but it's a convenient stage in the build
    # to create HTML redirects
    app.connect('html-collect-pages', create_redirect_pages)

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.1'}


def create_redirect_pages(app):
    if not isinstance(app.builder, StandaloneHTMLBuilder):
        return  # only relevant for standalone HTML output

    for (old_url, new_url) in app.config.html_redirect_pages:
        if old_url.startswith('/'):
            print('Stripping leading / from URL in config file...')
            old_url = old_url[1:]
        out_file = app.builder.get_outfilename(old_url)

        if new_url.startswith('\"') and new_url.endswith('\"'):
            # This is an absolute redirect. Slice away the surrouding quotation marks and copy the url verbatim
            new_url = new_url[1:-1]
        else:
            # This is an internal redirect. Find the relative url to the target document
            new_url = app.builder.get_relative_uri(old_url, new_url)

        print('HTML file %s redirects to URL %s' % (out_file, new_url))
        out_dir = os.path.dirname(out_file)
        if not os.path.exists(out_dir):
            os.makedirs(out_dir)

        content = REDIRECT_TEMPLATE.replace('$NEWURL', new_url)

        with open(out_file, 'w') as rp:
            rp.write(content)

    return []
