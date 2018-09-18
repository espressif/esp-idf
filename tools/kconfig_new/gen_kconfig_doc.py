#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# gen_kconfig_doc - confgen.py support for generating ReST markup documentation
#
# For each option in the loaded Kconfig (e.g. 'FOO'), CONFIG_FOO link target is
# generated, allowing options to be referenced in other documents
# (using :ref:`CONFIG_FOO`)
#
# Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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
from __future__ import print_function
import os
import kconfiglib

# Indentation to be used in the generated file
INDENT = '    '

# Characters used when underlining section heading
HEADING_SYMBOLS = '#*=-^"+'

# Keep the heading level in sync with api-reference/kconfig.rst
INITIAL_HEADING_LEVEL = 3
MAX_HEADING_LEVEL = len(HEADING_SYMBOLS)-1

def write_docs(config, filename):
    """ Note: writing .rst documentation ignores the current value
    of any items. ie the --config option can be ignored.
    (However at time of writing it still needs to be set to something...) """
    with open(filename, "w") as f:
        config.walk_menu(lambda node: write_menu_item(f, node))


def get_breadcrumbs(node):
    # this is a bit wasteful as it recalculates each time, but still...
    result = []
    node = node.parent
    while node.parent:
        if node.prompt:
            result = [ node.prompt[0] ] + result
        node = node.parent
    return " > ".join(result)

def get_heading_level(node):
    # bit wasteful also
    result = INITIAL_HEADING_LEVEL
    node = node.parent
    while node.parent:
        result += 1
        if result == MAX_HEADING_LEVEL:
            return MAX_HEADING_LEVEL
        node = node.parent
    return result

def format_rest_text(text, indent):
    # Format an indented text block for use with ReST
    text = indent + text.replace('\n', '\n' + indent)
    # Escape some characters which are inline formatting in ReST
    text = text.replace("*", "\\*")
    text = text.replace("_", "\\_")
    text += '\n'
    return text

def write_menu_item(f, node):
    if not node.prompt:
        return  # Don't do anything for invisible menu items

    if isinstance(node.parent.item, kconfiglib.Choice):
        return  # Skip choice nodes, they are handled as part of the parent (see below)

    try:
        name = node.item.name
    except AttributeError:
        name = None

    try:
        is_menu = node.item == kconfiglib.MENU or node.is_menuconfig
    except AttributeError:
        is_menu = False  # not all MenuNodes have is_menuconfig for some reason

    ## Heading
    if name:
        title = name
        # add link target so we can use :ref:`CONFIG_FOO`
        f.write('.. _CONFIG_%s:\n\n' % name)
    else:
        title = node.prompt[0]

    # if no symbol name, use the prompt as the heading
    if True or is_menu:
        f.write('%s\n' % title)
        f.write(HEADING_SYMBOLS[get_heading_level(node)] * len(title))
        f.write('\n\n')
    else:
        f.write('**%s**\n\n\n' % title)

    if name:
        f.write('%s%s\n\n' % (INDENT, node.prompt[0]))
        f.write('%s:emphasis:`Found in: %s`\n\n' % (INDENT, get_breadcrumbs(node)))

    try:
        if node.help:
            # Help text normally contains newlines, but spaces at the beginning of
            # each line are stripped by kconfiglib. We need to re-indent the text
            # to produce valid ReST.
            f.write(format_rest_text(node.help, INDENT))
    except AttributeError:
        pass  # No help

    if isinstance(node.item, kconfiglib.Choice):
        f.write('%sAvailable options:\n' % INDENT)
        choice_node = node.list
        while choice_node:
            # Format available options as a list
            f.write('%s- %-20s (%s)\n' % (INDENT * 2, choice_node.prompt[0], choice_node.item.name))
            if choice_node.help:
                HELP_INDENT = INDENT * 2
                fmt_help = format_rest_text(choice_node.help, '  ' + HELP_INDENT)
                f.write('%s  \n%s\n' % (HELP_INDENT, fmt_help))
            choice_node = choice_node.next

        f.write('\n\n')


if __name__ == '__main__':
    print("Run this via 'confgen.py --output doc FILENAME'")

