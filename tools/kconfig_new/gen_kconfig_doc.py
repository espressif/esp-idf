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
import os
import kconfiglib

# Indentation to be used in the generated file
INDENT = '    '

# Characters used when underlining section heading
HEADING_SYMBOLS = '*=-^#'

# Keep the heading level in sync with api-reference/kconfig.rst
INITIAL_HEADING_LEVEL = 2
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
        if node_is_menu(node) and node.prompt:
            result = [ node.prompt[0] ] + result
        node = node.parent
    return " > ".join(result)

def get_heading_level(node):
    # bit wasteful also
    result = INITIAL_HEADING_LEVEL
    node = node.parent
    while node.parent:
        # Test for 'Component config' is a hack so component config doesn't bury all
        # the components under it in the hierarchy
        if node_is_menu(node) and node.prompt[0] != "Component config":
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

def node_is_menu(node):
    try:
        return node.item == kconfiglib.MENU or node.is_menuconfig
    except AttributeError:
        return False  # not all MenuNodes have is_menuconfig for some reason

def should_print_preview_links(node):
    """
    Return True if we should print the preview links. For each menu,
    the menu with the preview links is the top menu which contains
    actual configuration items.
    """
    child = node.list
    while child:
        if not node_is_menu(child) and child.prompt:
            # we have a non-menu child, so return true if we don't have
            # a parent which already returned true
            return node.parent is None or not should_print_preview_links(node.parent)
        child = child.next
    return False

def write_menu_item(f, node):
    if not node.prompt:
        return  # Don't do anything for invisible menu items

    if isinstance(node.parent.item, kconfiglib.Choice):
        return  # Skip choice nodes, they are handled as part of the parent (see below)

    try:
        name = node.item.name
    except AttributeError:
        name = None

    is_menu = node_is_menu(node)

    ## Heading
    if name:
        f.write('.. envvar:: CONFIG_%s\n\n' % name)

    # menus get a proper heading
    if is_menu:
        title = node.prompt[0]
        f.write('%s\n' % title)
        f.write(HEADING_SYMBOLS[get_heading_level(node)] * len(title))
        f.write('\n\n')
        if should_print_preview_links(node):
            # print preview links to all items in this menu
            # for the first menu which contains at least one non-menu item
            # (ie per component, or per top-level KConfig.projbuild menu)
            def print_previews(parent):
                child = parent.list
                while child:
                    if not node_is_menu(child):
                        f.write('- :envvar:`CONFIG_%s`\n' % child.item.name)
                    if child.list and not isinstance(child.item, kconfiglib.Choice):
                        print_previews(child)
                    child = child.next
            print_previews(node)
            f.write('\n\n')

    if name:
        f.write('%s%s\n\n' % (INDENT, node.prompt[0]))

    try:
        if node.help:
            # Help text normally contains newlines, but spaces at the beginning of
            # each line are stripped by kconfiglib. We need to re-indent the text
            # to produce valid ReST.
            f.write(format_rest_text(node.help, INDENT))
    except AttributeError:
        pass  # No help

    if node.parent is not None and not is_menu:
        f.write('%sFound in\n%s%s\n\n' % (INDENT, INDENT * 2,
                                           get_breadcrumbs(node)))

    if isinstance(node.item, kconfiglib.Choice):
        f.write('%sAvailable options:\n' % INDENT)
        choice_node = node.list
        while choice_node:
            # Format available options as a list
            f.write('%s- %-20s (`CONFIG_%s`)\n' % (INDENT * 2, choice_node.prompt[0], choice_node.item.name))
            if choice_node.help:
                HELP_INDENT = INDENT * 2
                fmt_help = format_rest_text(choice_node.help, '  ' + HELP_INDENT)
                f.write('%s  \n%s\n' % (HELP_INDENT, fmt_help))
            choice_node = choice_node.next

        f.write('\n\n')

if __name__ == '__main__':
    print("Run this via 'confgen.py --output doc FILENAME'")

