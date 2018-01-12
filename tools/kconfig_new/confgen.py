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
import argparse
import sys
import os
import os.path
import tempfile

import gen_kconfig_doc
import kconfiglib

__version__ = "0.1"

def main():
    parser = argparse.ArgumentParser(description='confgen.py v%s - Config Generation Tool' % __version__, prog='conftool')

    parser.add_argument('--config',
                        help='Project configuration settings',
                        nargs='?',
                        default=None)

    parser.add_argument('--kconfig',
                        help='KConfig file with config item definitions',
                        required=True)

    parser.add_argument('--output', nargs=2, action='append',
                        help='Write output file (format and output filename)',
                        metavar=('FORMAT', 'FILENAME'))

    parser.add_argument('--env', action='append', default=[],
                        help='Environment to set when evaluating the config file', metavar='NAME=VAL')

    args = parser.parse_args()

    for fmt, filename in args.output:
        if not fmt in OUTPUT_FORMATS.keys():
            print("Format '%s' not recognised. Known formats: %s" % (fmt, OUTPUT_FORMATS))
            sys.exit(1)

    try:
       args.env = [ (name,value) for (name,value) in ( e.split("=",1) for e in args.env) ]
    except ValueError:
       print("--env arguments must each contain =. To unset an environment variable, use 'ENV='")
       sys.exit(1)

    for name, value in args.env:
        os.environ[name] = value

    config = kconfiglib.Kconfig(args.kconfig)

    if args.config is not None:
        config.load_config(args.config)

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
                write("set({}{} \"{}\")\n".format(
                    prefix, sym.name, val))
        config.walk_menu(write_node)


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


OUTPUT_FORMATS = {
    "config" : write_config,
    "header" : write_header,
    "cmake" : write_cmake,
    "docs" : gen_kconfig_doc.write_docs }

if __name__ == '__main__':
    main()
