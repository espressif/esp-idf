#!/usr/bin/env python
#
# Command line tool to take in ESP-IDF sdkconfig files with project
# settings and output data in multiple formats (update config, generate
# header file, generate .cmake include file, documentation, etc).
#
# Used internally by the ESP-IDF build system. But designed to be
# non-IDF-specific.
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
import argparse
import sys
import os
import os.path
import tempfile
import json
import re

import gen_kconfig_doc

try:
    from . import kconfiglib
except Exception:
    sys.path.insert(0, os.path.dirname(os.path.realpath(__file__)))
    import kconfiglib

__version__ = "0.1"

if "IDF_CMAKE" not in os.environ:
    os.environ["IDF_CMAKE"] = ""


def main():
    parser = argparse.ArgumentParser(description='confgen.py v%s - Config Generation Tool' % __version__, prog=os.path.basename(sys.argv[0]))

    parser.add_argument('--config',
                        help='Project configuration settings',
                        nargs='?',
                        default=None)

    parser.add_argument('--defaults',
                        help='Optional project defaults file, used if --config file doesn\'t exist. '
                             'Multiple files can be specified using multiple --defaults arguments.',
                        nargs='?',
                        default=[],
                        action='append')

    parser.add_argument('--kconfig',
                        help='KConfig file with config item definitions',
                        required=True)

    parser.add_argument('--output', nargs=2, action='append',
                        help='Write output file (format and output filename)',
                        metavar=('FORMAT', 'FILENAME'),
                        default=[])

    parser.add_argument('--env', action='append', default=[],
                        help='Environment to set when evaluating the config file', metavar='NAME=VAL')

    args = parser.parse_args()

    for fmt, filename in args.output:
        if fmt not in OUTPUT_FORMATS.keys():
            print("Format '%s' not recognised. Known formats: %s" % (fmt, OUTPUT_FORMATS.keys()))
            sys.exit(1)

    try:
        args.env = [(name,value) for (name,value) in (e.split("=",1) for e in args.env)]
    except ValueError:
        print("--env arguments must each contain =. To unset an environment variable, use 'ENV='")
        sys.exit(1)

    for name, value in args.env:
        os.environ[name] = value

    config = kconfiglib.Kconfig(args.kconfig)
    config.disable_redun_warnings()
    config.disable_override_warnings()

    if len(args.defaults) > 0:
        # always load defaults first, so any items which are not defined in that config
        # will have the default defined in the defaults file
        for name in args.defaults:
            print("Loading defaults file %s..." % name)
            if not os.path.exists(name):
                raise RuntimeError("Defaults file not found: %s" % name)
            config.load_config(name, replace=False)

    # If config file previously exists, load it
    if args.config and os.path.exists(args.config):
        config.load_config(args.config, replace=False)

    # Output the files specified in the arguments
    for output_type, filename in args.output:
        temp_file = tempfile.mktemp(prefix="confgen_tmp")
        try:
            output_function = OUTPUT_FORMATS[output_type]
            output_function(config, temp_file)
            update_if_changed(temp_file, filename)
        finally:
            try:
                os.remove(temp_file)
            except OSError:
                pass


def write_config(config, filename):
    CONFIG_HEADING = """#
# Automatically generated file. DO NOT EDIT.
# Espressif IoT Development Framework (ESP-IDF) Project Configuration
#
"""
    config.write_config(filename, header=CONFIG_HEADING)


def write_header(config, filename):
    CONFIG_HEADING = """/*
 * Automatically generated file. DO NOT EDIT.
 * Espressif IoT Development Framework (ESP-IDF) Configuration Header
 */
#pragma once
"""
    config.write_autoconf(filename, header=CONFIG_HEADING)


def write_cmake(config, filename):
    with open(filename, "w") as f:
        write = f.write
        prefix = config.config_prefix

        write("""#
# Automatically generated file. DO NOT EDIT.
# Espressif IoT Development Framework (ESP-IDF) Configuration cmake include file
#
""")

        def write_node(node):
            sym = node.item
            if not isinstance(sym, kconfiglib.Symbol):
                return

            # Note: str_value calculates _write_to_conf, due to
            # internal magic in kconfiglib...
            val = sym.str_value
            if sym._write_to_conf:
                if sym.orig_type in (kconfiglib.BOOL, kconfiglib.TRISTATE) and val == "n":
                    val = ""  # write unset values as empty variables
                elif sym.orig_type == kconfiglib.STRING:
                    val = kconfiglib.escape(val)
                write("set({}{} \"{}\")\n".format(
                    prefix, sym.name, val))
        config.walk_menu(write_node)


def get_json_values(config):
    config_dict = {}

    def write_node(node):
        sym = node.item
        if not isinstance(sym, kconfiglib.Symbol):
            return

        val = sym.str_value  # this calculates _write_to_conf, due to kconfiglib magic
        if sym._write_to_conf:
            if sym.type in [kconfiglib.BOOL, kconfiglib.TRISTATE]:
                val = (val != "n")
            elif sym.type == kconfiglib.HEX:
                val = int(val, 16)
            elif sym.type == kconfiglib.INT:
                val = int(val)
            config_dict[sym.name] = val
    config.walk_menu(write_node)
    return config_dict


def write_json(config, filename):
    config_dict = get_json_values(config)
    with open(filename, "w") as f:
        json.dump(config_dict, f, indent=4, sort_keys=True)


def get_menu_node_id(node):
    """ Given a menu node, return a unique id
    which can be used to identify it in the menu structure

    Will either be the config symbol name, or a menu identifier
    'slug'

    """
    try:
        if not isinstance(node.item, kconfiglib.Choice):
            return node.item.name
    except AttributeError:
        pass

    result = []
    while node.parent is not None:
        slug = re.sub(r'\W+', '-', node.prompt[0]).lower()
        result.append(slug)
        node = node.parent

    result = "-".join(reversed(result))
    return result


def write_json_menus(config, filename):
    existing_ids = set()
    result = []  # root level items
    node_lookup = {}  # lookup from MenuNode to an item in result

    def write_node(node):
        try:
            json_parent = node_lookup[node.parent]["children"]
        except KeyError:
            assert node.parent not in node_lookup  # if fails, we have a parent node with no "children" entity (ie a bug)
            json_parent = result  # root level node

        # node.kconfig.y means node has no dependency,
        if node.dep is node.kconfig.y:
            depends = None
        else:
            depends = kconfiglib.expr_str(node.dep)

        try:
            is_menuconfig = node.is_menuconfig
        except AttributeError:
            is_menuconfig = False

        new_json = None
        if node.item == kconfiglib.MENU or is_menuconfig:
            new_json = {"type": "menu",
                        "title": node.prompt[0],
                        "depends_on": depends,
                        "children": [],
                        }
            if is_menuconfig:
                sym = node.item
                new_json["name"] = sym.name
                new_json["help"] = node.help
                new_json["is_menuconfig"] = is_menuconfig
                greatest_range = None
                if len(sym.ranges) > 0:
                    # Note: Evaluating the condition using kconfiglib's expr_value
                    # should have one condition which is true
                    for min_range, max_range, cond_expr in sym.ranges:
                        if kconfiglib.expr_value(cond_expr):
                            greatest_range = [min_range, max_range]
                new_json["range"] = greatest_range

        elif isinstance(node.item, kconfiglib.Symbol):
            sym = node.item
            greatest_range = None
            if len(sym.ranges) > 0:
                # Note: Evaluating the condition using kconfiglib's expr_value
                # should have one condition which is true
                for min_range, max_range, cond_expr in sym.ranges:
                    if kconfiglib.expr_value(cond_expr):
                        greatest_range = [int(min_range.str_value), int(max_range.str_value)]

            new_json = {
                "type": kconfiglib.TYPE_TO_STR[sym.type],
                "name": sym.name,
                "title": node.prompt[0] if node.prompt else None,
                "depends_on": depends,
                "help": node.help,
                "range": greatest_range,
                "children": [],
            }
        elif isinstance(node.item, kconfiglib.Choice):
            choice = node.item
            new_json = {
                "type": "choice",
                "title": node.prompt[0],
                "name": choice.name,
                "depends_on": depends,
                "help": node.help,
                "children": []
            }

        if new_json:
            node_id = get_menu_node_id(node)
            if node_id in existing_ids:
                raise RuntimeError("Config file contains two items with the same id: %s (%s). " +
                                   "Please rename one of these items to avoid ambiguity." % (node_id, node.prompt[0]))
            new_json["id"] = node_id

            json_parent.append(new_json)
            node_lookup[node] = new_json

    config.walk_menu(write_node)
    with open(filename, "w") as f:
        f.write(json.dumps(result, sort_keys=True, indent=4))


def update_if_changed(source, destination):
    with open(source, "r") as f:
        source_contents = f.read()

    if os.path.exists(destination):
        with open(destination, "r") as f:
            dest_contents = f.read()
        if source_contents == dest_contents:
            return  # nothing to update

    with open(destination, "w") as f:
        f.write(source_contents)


OUTPUT_FORMATS = {"config": write_config,
                  "header": write_header,
                  "cmake": write_cmake,
                  "docs": gen_kconfig_doc.write_docs,
                  "json": write_json,
                  "json_menus": write_json_menus,
                  }


class FatalError(RuntimeError):
    """
    Class for runtime errors (not caused by bugs but by user input).
    """
    pass


if __name__ == '__main__':
    try:
        main()
    except FatalError as e:
        print("A fatal error occurred: %s" % e)
        sys.exit(2)
