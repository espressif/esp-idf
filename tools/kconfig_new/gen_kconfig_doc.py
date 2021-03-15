#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# gen_kconfig_doc - confgen.py support for generating ReST markup documentation
#
# For each option in the loaded Kconfig (e.g. 'FOO'), CONFIG_FOO link target is
# generated, allowing options to be referenced in other documents
# (using :ref:`CONFIG_FOO`)
#
# Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
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


class ConfigTargetVisibility(object):
    """
    Determine the visibility of Kconfig options based on IDF targets. Note that other environment variables should not
    imply invisibility and neither dependencies on visible options with default disabled state. This difference makes
    it necessary to implement our own visibility and cannot use the visibility defined inside Kconfiglib.
    """
    def __init__(self, config, target):
        # target actually is not necessary here because kconfiglib.expr_value() will evaluate it internally
        self.config = config
        self.visibility = dict()  # node name to (x, y) mapping where x is the visibility (True/False) and y is the
        # name of the config which implies the visibility
        self.target_env_var = 'IDF_TARGET'
        self.direct_eval_set = frozenset([kconfiglib.EQUAL, kconfiglib.UNEQUAL, kconfiglib.LESS, kconfiglib.LESS_EQUAL,
                                         kconfiglib.GREATER, kconfiglib.GREATER_EQUAL])

    def _implies_invisibility(self, item):
        if isinstance(item, tuple):
            if item[0] == kconfiglib.NOT:
                (invisibility, source) = self._implies_invisibility(item[1])
                if source is not None and source.startswith(self.target_env_var):
                    return (not invisibility, source)
                else:
                    # we want to be visible all configs which are not dependent on target variables,
                    # e.g. "depends on XY" and "depends on !XY" as well
                    return (False, None)
            elif item[0] == kconfiglib.AND:
                (invisibility, source) = self._implies_invisibility(item[1])
                if invisibility:
                    return (True, source)
                (invisibility, source) = self._implies_invisibility(item[2])
                if invisibility:
                    return (True, source)
                return (False, None)
            elif item[0] == kconfiglib.OR:
                implication_list = [self._implies_invisibility(item[1]), self._implies_invisibility(item[2])]
                if all([implies for (implies, _) in implication_list]):
                    source_list = [s for (_, s) in implication_list if s.startswith(self.target_env_var)]
                    # if source_list has more items then it should not matter which will imply the invisibility
                    return (True, source_list[0])
                return (False, None)
            elif item[0] in self.direct_eval_set:
                def node_is_invisible(item):
                    return all([node.prompt is None for node in item.nodes])
                if node_is_invisible(item[1]) or node_is_invisible(item[1]):
                    # it makes no sense to call self._implies_invisibility() here because it won't generate any useful
                    # "source"
                    return (not kconfiglib.expr_value(item), None)
                else:
                    # expressions with visible configs can be changed to make the item visible
                    return (False, None)
            else:
                raise RuntimeError('Unimplemented operation in {}'.format(item))
        else:  # Symbol or Choice
            vis_list = [self._visible(node) for node in item.nodes]
            if len(vis_list) > 0 and all([not visible for (visible, _) in vis_list]):
                source_list = [s for (_, s) in vis_list if s is not None and s.startswith(self.target_env_var)]
                # if source_list has more items then it should not matter which will imply the invisibility
                return (True, source_list[0])

            if item.name.startswith(self.target_env_var):
                return (not kconfiglib.expr_value(item), item.name)

            if len(vis_list) == 1:
                (visible, source) = vis_list[0]
                if visible:
                    return (False, item.name)  # item.name is important here in case the result will be inverted: if
                    # the dependency is on another config then it can be still visible

            return (False, None)

    def _visible(self, node):
        if isinstance(node.item, kconfiglib.Symbol) or isinstance(node.item, kconfiglib.Choice):
            dependencies = node.item.direct_dep  # "depends on" for configs
            name_id = node.item.name
            simple_def = len(node.item.nodes) <= 1  # defined only in one source file
            # Probably it is not necessary to check the default statements.
        else:
            dependencies = node.visibility  # "visible if" for menu
            name_id = node.prompt[0]
            simple_def = False  # menus can be defined with the same name at multiple locations and they don't know
            # about each other like configs through node.item.nodes. Therefore, they cannot be stored and have to be
            # re-evaluated always.

        try:
            (visib, source) = self.visibility[name_id]
        except KeyError:
            def invert_first_arg(_tuple):
                return (not _tuple[0], _tuple[1])

            (visib, source) = self._visible(node.parent) if node.parent else (True, None)

            if visib:
                (visib, source) = invert_first_arg(self._implies_invisibility(dependencies))

            if simple_def:
                # Configs defined at multiple places are not stored because they could have different visibility based
                # on different targets. kconfiglib.expr_value() will handle the visibility.
                self.visibility[name_id] = (visib, source)

        return (visib, source)  # not used in "finally" block because failure messages from _implies_invisibility are
        # this way more understandable

    def visible(self, node):
        if not node.prompt:
            # don't store this in self.visibility because don't want to stop at invisible nodes when recursively
            # searching for invisible targets
            return False

        return self._visible(node)[0]


def write_docs(config, visibility, filename):
    """ Note: writing .rst documentation ignores the current value
    of any items. ie the --config option can be ignored.
    (However at time of writing it still needs to be set to something...) """
    with open(filename, 'w') as f:
        for node in config.node_iter():
            write_menu_item(f, node, visibility)


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
            result = [':ref:`%s`' % get_link_anchor(node)] + result
        node = node.parent
    return ' > '.join(result)


def get_link_anchor(node):
    try:
        return 'CONFIG_%s' % node.item.name
    except AttributeError:
        assert(node_is_menu(node))  # only menus should have no item.name

    # for menus, build a link anchor out of the parents
    result = []
    while node.parent:
        if node.prompt:
            result = [re.sub(r'[^a-zA-z0-9]+', '-', node.prompt[0])] + result
        node = node.parent
    result = '-'.join(result).lower()
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
    text = text.replace('*', '\\*')
    text = text.replace('_', '\\_')
    # replace absolute links to documentation by relative ones
    text = re.sub(r'https://docs.espressif.com/projects/esp-idf/\w+/\w+/(.+)\.html', r':doc:`../\1`', text)
    text += '\n'
    return text


def _minimize_expr(expr, visibility):
    def expr_nodes_invisible(e):
        return hasattr(e, 'nodes') and len(e.nodes) > 0 and all(not visibility.visible(i) for i in e.nodes)

    if isinstance(expr, tuple):
        if expr[0] == kconfiglib.NOT:
            new_expr = _minimize_expr(expr[1], visibility)
            return kconfiglib.Kconfig.y if new_expr == kconfiglib.Kconfig.n else new_expr
        else:
            new_expr1 = _minimize_expr(expr[1], visibility)
            new_expr2 = _minimize_expr(expr[2], visibility)
            if expr[0] == kconfiglib.AND:
                if new_expr1 == kconfiglib.Kconfig.n or new_expr2 == kconfiglib.Kconfig.n:
                    return kconfiglib.Kconfig.n
                if new_expr1 == kconfiglib.Kconfig.y:
                    return new_expr2
                if new_expr2 == kconfiglib.Kconfig.y:
                    return new_expr1
            elif expr[0] == kconfiglib.OR:
                if new_expr1 == kconfiglib.Kconfig.y or new_expr2 == kconfiglib.Kconfig.y:
                    return kconfiglib.Kconfig.y
                if new_expr1 == kconfiglib.Kconfig.n:
                    return new_expr2
                if new_expr2 == kconfiglib.Kconfig.n:
                    return new_expr1
            elif expr[0] == kconfiglib.EQUAL:
                if not isinstance(new_expr1, type(new_expr2)):
                    return kconfiglib.Kconfig.n
                if new_expr1 == new_expr2:
                    return kconfiglib.Kconfig.y
            elif expr[0] == kconfiglib.UNEQUAL:
                if not isinstance(new_expr1, type(new_expr2)):
                    return kconfiglib.Kconfig.y
                if new_expr1 != new_expr2:
                    return kconfiglib.Kconfig.n
            else:  # <, <=, >, >=
                if not isinstance(new_expr1, type(new_expr2)):
                    return kconfiglib.Kconfig.n  # e.g "True < 2"

                if expr_nodes_invisible(new_expr1) or expr_nodes_invisible(new_expr2):
                    return kconfiglib.Kconfig.y if kconfiglib.expr_value(expr) else kconfiglib.Kconfig.n

            return (expr[0], new_expr1, new_expr2)

    if (not kconfiglib.expr_value(expr) and len(expr.config_string) == 0 and expr_nodes_invisible(expr)):
        # nodes which are invisible
        # len(expr.nodes) > 0 avoids constant symbols without actual node definitions, e.g. integer constants
        # len(expr.config_string) == 0 avoids hidden configs which reflects the values of choices
        return kconfiglib.Kconfig.n

    if (kconfiglib.expr_value(expr) and len(expr.config_string) > 0 and expr_nodes_invisible(expr)):
        # hidden config dependencies which will be written to sdkconfig as enabled ones.
        return kconfiglib.Kconfig.y

    if any(node.item.name.startswith(visibility.target_env_var) for node in expr.nodes):
        # We know the actual values for IDF_TARGETs
        return kconfiglib.Kconfig.y if kconfiglib.expr_value(expr) else kconfiglib.Kconfig.n

    return expr


def write_menu_item(f, node, visibility):
    def is_choice(node):
        """ Skip choice nodes, they are handled as part of the parent (see below) """
        return isinstance(node.parent.item, kconfiglib.Choice)

    if is_choice(node) or not visibility.visible(node):
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

    f.write('.. _%s:\n\n' % get_link_anchor(node))
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
            f.write('\n')
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

    if isinstance(node.item, kconfiglib.Symbol):
        def _expr_str(sc):
            if sc.is_constant or not sc.nodes or sc.choice:
                return '{}'.format(sc.name)
            return ':ref:`%s%s`' % (sc.kconfig.config_prefix, sc.name)

        range_strs = []
        for low, high, cond in node.item.ranges:
            cond = _minimize_expr(cond, visibility)
            if cond == kconfiglib.Kconfig.n:
                continue
            if not isinstance(cond, tuple) and cond != kconfiglib.Kconfig.y:
                if len(cond.nodes) > 0 and all(not visibility.visible(i) for i in cond.nodes):
                    if not kconfiglib.expr_value(cond):
                        continue
            range_str = '%s- from %s to %s' % (INDENT * 2, low.str_value, high.str_value)
            if cond != kconfiglib.Kconfig.y and not kconfiglib.expr_value(cond):
                range_str += ' if %s' % kconfiglib.expr_str(cond, _expr_str)
            range_strs.append(range_str)
        if len(range_strs) > 0:
            f.write('%sRange:\n' % INDENT)
            f.write('\n'.join(range_strs))
            f.write('\n\n')

        default_strs = []
        for default, cond in node.item.defaults:
            cond = _minimize_expr(cond, visibility)
            if cond == kconfiglib.Kconfig.n:
                continue
            if not isinstance(cond, tuple) and cond != kconfiglib.Kconfig.y:
                if len(cond.nodes) > 0 and all(not visibility.visible(i) for i in cond.nodes):
                    if not kconfiglib.expr_value(cond):
                        continue
            # default.type is mostly UNKNOWN so it cannot be used reliably for detecting the type
            d = default.str_value
            if d in ['y', 'Y']:
                d = 'Yes (enabled)'
            elif d in ['n', 'N']:
                d = 'No (disabled)'
            elif re.search(r'[^0-9a-fA-F]', d):  # simple string detection: if it not a valid number
                d = '"%s"' % d
            default_str = '%s- %s' % (INDENT * 2, d)
            if cond != kconfiglib.Kconfig.y and not kconfiglib.expr_value(cond):
                default_str += ' if %s' % kconfiglib.expr_str(cond, _expr_str)
            default_strs.append(default_str)
        if len(default_strs) > 0:
            f.write('%sDefault value:\n' % INDENT)
            f.write('\n'.join(default_strs))
            f.write('\n\n')

    if is_menu:
        # enumerate links to child items
        child_list = []
        child = node.list
        while child:
            if not is_choice(child) and child.prompt and visibility.visible(child):
                child_list.append((child.prompt[0], get_link_anchor(child)))
            child = child.next
        if len(child_list) > 0:
            f.write('Contains:\n\n')
            sorted_child_list = sorted(child_list, key=lambda pair: pair[0].lower())
            ref_list = ['- :ref:`{}`'.format(anchor) for _, anchor in sorted_child_list]
            f.write('\n'.join(ref_list))
            f.write('\n\n')


if __name__ == '__main__':
    print("Run this via 'confgen.py --output doc FILENAME'")
