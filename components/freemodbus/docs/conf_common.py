# -*- coding: utf-8 -*-
#
# Common (non-language-specific) configuration for Sphinx
#

# type: ignore
# pylint: disable=wildcard-import
# pylint: disable=undefined-variable

from __future__ import print_function, unicode_literals

from esp_docs.conf_docs import *  # noqa: F403,F401

extensions += ['sphinx_copybutton',
               # Needed as a trigger for running doxygen
               'esp_docs.esp_extensions.dummy_build_system',
               'esp_docs.esp_extensions.run_doxygen'
               ]

# link roles config
github_repo = 'espressif/esp-modbus'

# context used by sphinx_idf_theme
html_context['github_user'] = 'espressif'
html_context['github_repo'] = 'esp-modbus'
html_static_path = ['../_static']

# Extra options required by sphinx_idf_theme
project_slug = 'esp-modbus'
versions_url = './_static/modbus_docs_versions.js'

idf_targets = ['esp32', 'esp32s2', 'esp32c3']
languages = ['en']
