#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# gen-kconfig-doc.py — generate Sphinx .rst file from Kconfig files
#
# This script iterates over Kconfig and Kconfig.projbuild files in
# ESP-IDF component directories, and outputs documentation for these options
# as ReST markup.
# For each option in Kconfig file (e.g. 'FOO'), CONFIG_FOO link target is
# generated, allowing options to be referenced in other documents 
# (using :ref:`CONFIG_FOO`)
#
# This script uses kconfiglib library to do all the work of parsing Kconfig
# files: https://github.com/ulfalizer/Kconfiglib
#
# Copyright 2017 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


import os
import kconfiglib

# Indentation to be used in the generated file
INDENT = '    '

# Characters used when underlining section heading
HEADING_SYMBOLS = '#*=-^"+'

# Keep the heading level in sync with api-reference/kconfig.rst
INITIAL_HEADING_LEVEL = 2
MAX_HEADING_LEVEL = 5
OPTION_HEADING_LEVEL = 6


def print_menu_contents(title, items, heading_level, breadcrumbs):
    if title:
        print_section_heading(title, heading_level)
    for entry in items:
        if entry.is_menu():
            if len(breadcrumbs) > 0:
                new_breadcrumbs = breadcrumbs + ' > ' + entry.get_title()
            else:
                new_breadcrumbs = entry.get_title()

            print_menu_contents(entry.get_title(), entry.get_items(),
                                min(heading_level + 1, MAX_HEADING_LEVEL),
                                new_breadcrumbs)
        elif entry.is_choice():
            print_choice(entry, breadcrumbs)
        else:
            if len(entry.get_prompts()) == 0:
                # Skip entries which can never be visible
                continue
            # Currently this does not handle 'menuconfig' entires in any special way,
            # as Kconfglib offers no way of recognizing them automatically.
            print_option(entry, breadcrumbs)
        # Trailing newline after every option
        print

def print_choice(choice, breadcrumbs):
    print_option(choice, breadcrumbs)
    print
    print '%sAvailable options:' % INDENT
    for opt in choice.get_symbols():
        # Format available options as a list
        print '%s- %s' % (INDENT * 2, opt.name)

def print_section_heading(title, heading_level):
    print title
    print HEADING_SYMBOLS[heading_level] * len(title)
    print

def print_option(opt, breadcrumbs):
    # add link target so we can use :ref:`CONFIG_FOO` 
    print '.. _CONFIG_%s:' % opt.name
    print
    print_section_heading(opt.name, OPTION_HEADING_LEVEL)
    if len(opt.prompts) > 0:
        print '%s%s' % (INDENT, opt.prompts[0][0])
        print
    print '%s:emphasis:`Found in: %s`' % (INDENT, breadcrumbs)
    print
    if opt.get_help() is not None:
        # Help text normally contains newlines, but spaces at the beginning of
        # each line are stripped by kconfiglib. We need to re-indent the text
        # to produce valid ReST.
        print '%s%s' % (INDENT, opt.get_help().replace('\n', '\n%s' % INDENT))

def process_kconfig_file(kconfig_file, heading_level, breadcrumbs):
    if os.path.exists(kconfig_file):
        cfg = kconfiglib.Config(kconfig_file, print_warnings=True)
        print_menu_contents(None, cfg.get_top_level_items(), heading_level, breadcrumbs)

def print_all_components():
    heading_level = INITIAL_HEADING_LEVEL
    # Currently this works only for IDF components.
    # TODO: figure out if this can be re-used for documenting applications?
    components_path = os.path.join(os.path.curdir, '../..', 'components')
    for component_name in os.listdir(components_path):
        if component_name.startswith('.'):
            continue    # skip system thumbnail folders

        kconfig_file_path = os.path.join(components_path, component_name, 'Kconfig')

        process_kconfig_file(kconfig_file_path, heading_level, 'Component config')
        process_kconfig_file(kconfig_file_path + '.projbuild', heading_level, '')

if __name__ == '__main__':
    print_all_components()
