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
import re
import kconfiglib

# Indentation to be used in the generated file
INDENT = '    '

# Characters used when underlining section heading
HEADING_SYMBOLS = '#*=-^"+'

# Keep the heading level in sync with api-reference/kconfig.rst
INITIAL_HEADING_LEVEL = 3
MAX_HEADING_LEVEL = len(HEADING_SYMBOLS) - 1


def write_docs(config, filename):
    """ Note: writing .rst documentation ignores the current value
    of any items. ie the --config option can be ignored.
    (However at time of writing it still needs to be set to something...) """
    with open(filename, "w") as f:
        config.walk_menu(lambda node: write_menu_item(f, node))


def node_is_menu(node):
    try:
        return node.item == kconfiglib.MENU or node.is_menuconfig
    except AttributeError:
        return False  # not all MenuNodes have is_menuconfig for some reason


def get_breadcrumbs(node):
    # this is a bit wasteful as it recalculates each time, but still...
    result = []
    node = node.parent
    while node.parent:
        if node.prompt:
            result = [":ref:`%s`" % get_link_anchor(node)] + result
        node = node.parent
    return " > ".join(result)


def get_link_anchor(node):
    try:
        return "CONFIG_%s" % node.item.name
    except AttributeError:
        assert(node_is_menu(node))  # only menus should have no item.name

    # for menus, build a link anchor out of the parents
    result = []
    while node.parent:
        if node.prompt:
            result = [re.sub(r"[^a-zA-z0-9]+", "-", node.prompt[0])] + result
        node = node.parent
    result = "-".join(result).lower()
    return result


def get_heading_level(node):
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
    # replace absolute links to documentation by relative ones
    text = re.sub(r'https://docs.espressif.com/projects/esp-idf/\w+/\w+/(.+)\.html', r':doc:`../\1`', text)
    text += '\n'
    return text


def node_should_write(node):
    if not node.prompt:
        return False  # Don't do anything for invisible menu items

    if isinstance(node.parent.item, kconfiglib.Choice):
        return False  # Skip choice nodes, they are handled as part of the parent (see below)

    return True


def write_menu_item(f, node):
    if not node_should_write(node):
        return

    try:
        name = node.item.name
    except AttributeError:
        name = None

    is_menu = node_is_menu(node)

    # Heading
    if name:
        title = 'CONFIG_%s' % name
    else:
        # if no symbol name, use the prompt as the heading
        title = node.prompt[0]

    f.write(".. _%s:\n\n" % get_link_anchor(node))
    f.write('%s\n' % title)
    f.write(HEADING_SYMBOLS[get_heading_level(node)] * len(title))
    f.write('\n\n')

    if name:
        f.write('%s%s\n\n' % (INDENT, node.prompt[0]))
        f.write('%s:emphasis:`Found in:` %s\n\n' % (INDENT, get_breadcrumbs(node)))

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

    if is_menu:
        # enumerate links to child items
        first = True
        child = node.list
        while child:
            try:
                if node_should_write(child):
                    if first:
                        f.write("Contains:\n\n")
                        first = False
                    f.write('- :ref:`%s`\n' % get_link_anchor(child))
            except AttributeError:
                pass
            child = child.next
        f.write('\n')


if __name__ == '__main__':
    print("Run this via 'confgen.py --output doc FILENAME'")
