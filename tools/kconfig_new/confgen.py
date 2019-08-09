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
import json
import os
import os.path
import re
import sys
import tempfile

import gen_kconfig_doc
import kconfiglib

__version__ = "0.1"

if "IDF_CMAKE" not in os.environ:
    os.environ["IDF_CMAKE"] = ""


class DeprecatedOptions(object):
    _REN_FILE = 'sdkconfig.rename'
    _DEP_OP_BEGIN = '# Deprecated options for backward compatibility'
    _DEP_OP_END = '# End of deprecated options'
    _RE_DEP_OP_BEGIN = re.compile(_DEP_OP_BEGIN)
    _RE_DEP_OP_END = re.compile(_DEP_OP_END)

    def __init__(self, config_prefix, path_rename_files=[]):
        self.config_prefix = config_prefix
        # r_dic maps deprecated options to new options; rev_r_dic maps in the opposite direction
        self.r_dic, self.rev_r_dic = self._parse_replacements(path_rename_files)

        # note the '=' at the end of regex for not getting partial match of configs
        self._RE_CONFIG = re.compile(r'{}(\w+)='.format(self.config_prefix))

    def _parse_replacements(self, repl_paths):
        rep_dic = {}
        rev_rep_dic = {}

        def remove_config_prefix(string):
            if string.startswith(self.config_prefix):
                return string[len(self.config_prefix):]
            raise RuntimeError('Error in {} (line {}): Config {} is not prefixed with {}'
                               ''.format(rep_path, line_number, string, self.config_prefix))

        for rep_path in repl_paths:
            with open(rep_path) as f_rep:
                for line_number, line in enumerate(f_rep, start=1):
                    sp_line = line.split()
                    if len(sp_line) == 0 or sp_line[0].startswith('#'):
                        # empty line or comment
                        continue
                    if len(sp_line) != 2 or not all(x.startswith(self.config_prefix) for x in sp_line):
                        raise RuntimeError('Syntax error in {} (line {})'.format(rep_path, line_number))
                    if sp_line[0] in rep_dic:
                        raise RuntimeError('Error in {} (line {}): Replacement {} exist for {} and new '
                                           'replacement {} is defined'.format(rep_path, line_number,
                                                                              rep_dic[sp_line[0]], sp_line[0],
                                                                              sp_line[1]))

                    (dep_opt, new_opt) = (remove_config_prefix(x) for x in sp_line)
                    rep_dic[dep_opt] = new_opt
                    rev_rep_dic[new_opt] = dep_opt
        return rep_dic, rev_rep_dic

    def get_deprecated_option(self, new_option):
        return self.rev_r_dic.get(new_option, None)

    def get_new_option(self, deprecated_option):
        return self.r_dic.get(deprecated_option, None)

    def replace(self, sdkconfig_in, sdkconfig_out):
        replace_enabled = True
        with open(sdkconfig_in, 'r') as f_in, open(sdkconfig_out, 'w') as f_out:
            for line_num, line in enumerate(f_in, start=1):
                if self._RE_DEP_OP_BEGIN.search(line):
                    replace_enabled = False
                elif self._RE_DEP_OP_END.search(line):
                    replace_enabled = True
                elif replace_enabled:
                    m = self._RE_CONFIG.search(line)
                    if m and m.group(1) in self.r_dic:
                        depr_opt = self.config_prefix + m.group(1)
                        new_opt = self.config_prefix + self.r_dic[m.group(1)]
                        line = line.replace(depr_opt, new_opt)
                        print('{}:{} {} was replaced with {}'.format(sdkconfig_in, line_num, depr_opt, new_opt))
                f_out.write(line)

    def append_doc(self, config, path_output):

        def option_was_written(opt):
            return any(gen_kconfig_doc.node_should_write(node) for node in config.syms[opt].nodes)

        if len(self.r_dic) > 0:
            with open(path_output, 'a') as f_o:
                header = 'Deprecated options and their replacements'
                f_o.write('.. _configuration-deprecated-options:\n\n{}\n{}\n\n'.format(header, '-' * len(header)))
                for dep_opt in sorted(self.r_dic):
                    new_opt = self.r_dic[dep_opt]
                    if new_opt not in config.syms or (config.syms[new_opt].choice is None and option_was_written(new_opt)):
                        # everything except config for a choice (no link reference for those in the docs)
                        f_o.write('- {}{} (:ref:`{}{}`)\n'.format(config.config_prefix, dep_opt,
                                                                  config.config_prefix, new_opt))

                        if new_opt in config.named_choices:
                            # here are printed config options which were filtered out
                            syms = config.named_choices[new_opt].syms
                            for sym in syms:
                                if sym.name in self.rev_r_dic:
                                    # only if the symbol has been renamed
                                    dep_name = self.rev_r_dic[sym.name]

                                    # config options doesn't have references
                                    f_o.write('    - {}{}\n'.format(config.config_prefix, dep_name))

    def append_config(self, config, path_output):
        tmp_list = []

        def append_config_node_process(node):
            item = node.item
            if isinstance(item, kconfiglib.Symbol) and item.env_var is None:
                if item.name in self.rev_r_dic:
                    c_string = item.config_string
                    if c_string:
                        tmp_list.append(c_string.replace(self.config_prefix + item.name,
                                                         self.config_prefix + self.rev_r_dic[item.name]))

        config.walk_menu(append_config_node_process)

        if len(tmp_list) > 0:
            with open(path_output, 'a') as f_o:
                f_o.write('\n{}\n'.format(self._DEP_OP_BEGIN))
                f_o.writelines(tmp_list)
                f_o.write('{}\n'.format(self._DEP_OP_END))

    def append_header(self, config, path_output):
        if len(self.r_dic) > 0:
            with open(path_output, 'a') as f_o:
                f_o.write('\n/* List of deprecated options */\n')
                for dep_opt in sorted(self.r_dic):
                    new_opt = self.r_dic[dep_opt]
                    f_o.write('#ifdef {}{}\n#define {}{} {}{}\n#endif\n\n'.format(self.config_prefix, new_opt,
                                                                                  self.config_prefix, dep_opt, self.config_prefix, new_opt))


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

    parser.add_argument('--sdkconfig-rename',
                        help='File with deprecated Kconfig options',
                        required=False)

    parser.add_argument('--output', nargs=2, action='append',
                        help='Write output file (format and output filename)',
                        metavar=('FORMAT', 'FILENAME'),
                        default=[])

    parser.add_argument('--env', action='append', default=[],
                        help='Environment to set when evaluating the config file', metavar='NAME=VAL')

    parser.add_argument('--env-file', type=argparse.FileType('r'),
                        help='Optional file to load environment variables from. Contents '
                             'should be a JSON object where each key/value pair is a variable.')

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

    if args.env_file is not None:
        env = json.load(args.env_file)
        os.environ.update(env)

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

    sdkconfig_renames = [args.sdkconfig_rename] if args.sdkconfig_rename else []
    sdkconfig_renames += os.environ.get("COMPONENT_SDKCONFIG_RENAMES", "").split()
    deprecated_options = DeprecatedOptions(config.config_prefix, path_rename_files=sdkconfig_renames)

    # If config file previously exists, load it
    if args.config and os.path.exists(args.config):
        # ... but replace deprecated options before that
        with tempfile.NamedTemporaryFile(prefix="confgen_tmp", delete=False) as f:
            temp_file = f.name
        try:
            deprecated_options.replace(sdkconfig_in=args.config, sdkconfig_out=temp_file)
            config.load_config(temp_file, replace=False)
            update_if_changed(temp_file, args.config)
        finally:
            try:
                os.remove(temp_file)
            except OSError:
                pass

    # Output the files specified in the arguments
    for output_type, filename in args.output:
        with tempfile.NamedTemporaryFile(prefix="confgen_tmp", delete=False) as f:
            temp_file = f.name
        try:
            output_function = OUTPUT_FORMATS[output_type]
            output_function(deprecated_options, config, temp_file)
            update_if_changed(temp_file, filename)
        finally:
            try:
                os.remove(temp_file)
            except OSError:
                pass


def write_config(deprecated_options, config, filename):
    CONFIG_HEADING = """#
# Automatically generated file. DO NOT EDIT.
# Espressif IoT Development Framework (ESP-IDF) Project Configuration
#
"""
    config.write_config(filename, header=CONFIG_HEADING)
    deprecated_options.append_config(config, filename)


def write_makefile(deprecated_options, config, filename):
    CONFIG_HEADING = """#
# Automatically generated file. DO NOT EDIT.
# Espressif IoT Development Framework (ESP-IDF) Project Makefile Configuration
#
"""
    with open(filename, "w") as f:
        tmp_dep_lines = []
        f.write(CONFIG_HEADING)

        def get_makefile_config_string(name, value, orig_type):
            if orig_type in (kconfiglib.BOOL, kconfiglib.TRISTATE):
                return "{}{}={}\n".format(config.config_prefix, name, '' if value == 'n' else value)
            elif orig_type in (kconfiglib.INT, kconfiglib.HEX):
                return "{}{}={}\n".format(config.config_prefix, name, value)
            elif orig_type == kconfiglib.STRING:
                return '{}{}="{}"\n'.format(config.config_prefix, name, kconfiglib.escape(value))
            else:
                raise RuntimeError('{}{}: unknown type {}'.format(config.config_prefix, name, orig_type))

        def write_makefile_node(node):
            item = node.item
            if isinstance(item, kconfiglib.Symbol) and item.env_var is None:
                # item.config_string cannot be used because it ignores hidden config items
                val = item.str_value
                f.write(get_makefile_config_string(item.name, val, item.orig_type))

                dep_opt = deprecated_options.get_deprecated_option(item.name)
                if dep_opt:
                    # the same string but with the deprecated name
                    tmp_dep_lines.append(get_makefile_config_string(dep_opt, val, item.orig_type))

        config.walk_menu(write_makefile_node, True)

        if len(tmp_dep_lines) > 0:
            f.write('\n# List of deprecated options\n')
            f.writelines(tmp_dep_lines)


def write_header(deprecated_options, config, filename):
    CONFIG_HEADING = """/*
 * Automatically generated file. DO NOT EDIT.
 * Espressif IoT Development Framework (ESP-IDF) Configuration Header
 */
#pragma once
"""
    config.write_autoconf(filename, header=CONFIG_HEADING)
    deprecated_options.append_header(config, filename)


def write_cmake(deprecated_options, config, filename):
    with open(filename, "w") as f:
        tmp_dep_list = []
        write = f.write
        prefix = config.config_prefix

        write("""#
# Automatically generated file. DO NOT EDIT.
# Espressif IoT Development Framework (ESP-IDF) Configuration cmake include file
#
""")

        configs_list = list()

        def write_node(node):
            sym = node.item
            if not isinstance(sym, kconfiglib.Symbol):
                return

            if sym.config_string:
                val = sym.str_value
                if sym.orig_type in (kconfiglib.BOOL, kconfiglib.TRISTATE) and val == "n":
                    val = ""  # write unset values as empty variables
                write("set({}{} \"{}\")\n".format(
                    prefix, sym.name, val))

                configs_list.append(prefix + sym.name)
                dep_opt = deprecated_options.get_deprecated_option(sym.name)
                if dep_opt:
                    tmp_dep_list.append("set({}{} \"{}\")\n".format(prefix, dep_opt, val))
                    configs_list.append(prefix + dep_opt)

        config.walk_menu(write_node)
        write("set(CONFIGS_LIST {})".format(";".join(configs_list)))

        if len(tmp_dep_list) > 0:
            write('\n# List of deprecated options for backward compatibility\n')
            f.writelines(tmp_dep_list)


def get_json_values(config):
    config_dict = {}

    def write_node(node):
        sym = node.item
        if not isinstance(sym, kconfiglib.Symbol):
            return

        if sym.config_string:
            val = sym.str_value
            if sym.type in [kconfiglib.BOOL, kconfiglib.TRISTATE]:
                val = (val != "n")
            elif sym.type == kconfiglib.HEX:
                val = int(val, 16)
            elif sym.type == kconfiglib.INT:
                val = int(val)
            config_dict[sym.name] = val
    config.walk_menu(write_node)
    return config_dict


def write_json(deprecated_options, config, filename):
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


def write_json_menus(deprecated_options, config, filename):
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
                        base = 16 if sym.type == kconfiglib.HEX else 10
                        greatest_range = [int(min_range.str_value, base), int(max_range.str_value, base)]
                        break

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


def write_docs(deprecated_options, config, filename):
    gen_kconfig_doc.write_docs(config, filename)
    deprecated_options.append_doc(config, filename)


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
                  "makefile": write_makefile,  # only used with make in order to generate auto.conf
                  "header": write_header,
                  "cmake": write_cmake,
                  "docs": write_docs,
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
