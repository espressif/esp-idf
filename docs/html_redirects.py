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
# conf.html_redirect_pages
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
    app.connect('build-finished', create_redirect_pages)


def create_redirect_pages(app, docname):
    if not isinstance(app.builder, StandaloneHTMLBuilder):
        return  # only relevant for standalone HTML output

    for (old_url, new_url) in app.config.html_redirect_pages:
        print("Creating redirect %s to %s..." % (old_url, new_url))
        if old_url.startswith('/'):
            print("Stripping leading / from URL in config file...")
            old_url = old_url[1:]

        new_url = app.builder.get_relative_uri(old_url, new_url)
        out_file = app.builder.get_outfilename(old_url)
        print("HTML file %s redirects to relative URL %s" % (out_file, new_url))

        out_dir = os.path.dirname(out_file)
        if not os.path.exists(out_dir):
            os.makedirs(out_dir)

        content = REDIRECT_TEMPLATE.replace("$NEWURL", new_url)

        with open(out_file, "w") as rp:
            rp.write(content)
