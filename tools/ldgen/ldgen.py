#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import argparse
import errno
import json
import os
import subprocess
import sys
import tempfile
from io import StringIO

from ldgen.entity import EntityDB
from ldgen.fragments import parse_fragment_file
from ldgen.generation import Generation
from ldgen.ldgen_common import LdGenFailure
from ldgen.linker_script import LinkerScript
from ldgen.sdkconfig import SDKConfig
from pyparsing import ParseException
from pyparsing import ParseFatalException


def _update_environment(args):
    env = [(name, value) for (name,value) in (e.split('=',1) for e in args.env)]
    for name, value in env:
        value = ' '.join(value.split())
        os.environ[name] = value

    if args.env_file is not None:
        env = json.load(args.env_file)
        os.environ.update(env)


def main():

    argparser = argparse.ArgumentParser(description='ESP-IDF linker script generator')

    argparser.add_argument(
        '--input', '-i',
        help='Linker template file',
        type=argparse.FileType('r'))

    fragments_group = argparser.add_mutually_exclusive_group()

    fragments_group.add_argument(
        '--fragments', '-f',
        type=argparse.FileType('r'),
        help='Input fragment files',
        nargs='+'
    )

    fragments_group.add_argument(
        '--fragments-list',
        help='Input fragment files as a semicolon-separated list',
        type=str
    )

    argparser.add_argument(
        '--libraries-file',
        type=argparse.FileType('r'),
        help='File that contains the list of libraries in the build')

    argparser.add_argument(
        '--output', '-o',
        help='Output linker script',
        type=str)

    argparser.add_argument(
        '--config', '-c',
        help='Project configuration')

    argparser.add_argument(
        '--kconfig', '-k',
        help='IDF Kconfig file')

    argparser.add_argument(
        '--check-mapping',
        help='Perform a check if a mapping (archive, obj, symbol) exists',
        action='store_true'
    )

    argparser.add_argument(
        '--check-mapping-exceptions',
        help='Mappings exempted from check',
        type=argparse.FileType('r')
    )

    argparser.add_argument(
        '--env', '-e',
        action='append', default=[],
        help='Environment to set when evaluating the config file', metavar='NAME=VAL')

    argparser.add_argument('--env-file', type=argparse.FileType('r'),
                           help='Optional file to load environment variables from. Contents '
                           'should be a JSON object where each key/value pair is a variable.')

    argparser.add_argument(
        '--objdump',
        help='Path to toolchain objdump')

    args = argparser.parse_args()

    input_file = args.input
    libraries_file = args.libraries_file
    config_file = args.config
    output_path = args.output
    kconfig_file = args.kconfig
    objdump = args.objdump

    fragment_files = []
    if args.fragments_list:
        fragment_files = args.fragments_list.split(';')
    elif args.fragments:
        fragment_files = args.fragments

    check_mapping = args.check_mapping
    if args.check_mapping_exceptions:
        check_mapping_exceptions = [line.strip() for line in args.check_mapping_exceptions]
    else:
        check_mapping_exceptions = None

    try:
        sections_infos = EntityDB()
        for library in libraries_file:
            library = library.strip()
            if library:
                new_env = os.environ.copy()
                new_env['LC_ALL'] = 'C'
                dump = StringIO(subprocess.check_output([objdump, '-h', library], env=new_env).decode())
                dump.name = library
                sections_infos.add_sections_info(dump)

        generation_model = Generation(check_mapping, check_mapping_exceptions)

        _update_environment(args)  # assign args.env and args.env_file to os.environ

        sdkconfig = SDKConfig(kconfig_file, config_file)

        for fragment_file in fragment_files:
            try:
                fragment_file = parse_fragment_file(fragment_file, sdkconfig)
            except (ParseException, ParseFatalException) as e:
                # ParseException is raised on incorrect grammar
                # ParseFatalException is raised on correct grammar, but inconsistent contents (ex. duplicate
                # keys, key unsupported by fragment, unexpected number of values, etc.)
                raise LdGenFailure('failed to parse %s\n%s' % (fragment_file, str(e)))
            generation_model.add_fragments_from_file(fragment_file)

        non_contiguous_sram = sdkconfig.evaluate_expression('SOC_MEM_NON_CONTIGUOUS_SRAM')
        mapping_rules = generation_model.generate(sections_infos, non_contiguous_sram)

        script_model = LinkerScript(input_file)
        script_model.fill(mapping_rules)

        with tempfile.TemporaryFile('w+') as output:
            script_model.write(output)
            output.seek(0)

            if not os.path.exists(os.path.dirname(output_path)):
                try:
                    os.makedirs(os.path.dirname(output_path))
                except OSError as exc:
                    if exc.errno != errno.EEXIST:
                        raise

            with open(output_path, 'w') as f:  # only create output file after generation has suceeded
                f.write(output.read())
    except LdGenFailure as e:
        print('linker script generation failed for %s\nERROR: %s' % (input_file.name, e))
        sys.exit(1)


if __name__ == '__main__':
    main()
