#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import argparse
import errno
import hashlib
import json
import os
import pickle
import re
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

_RE_SECTION_NAME = re.compile(r'^\s*\d+\s+(\.\S+)', re.MULTILINE)


def _compute_fingerprint(sections_infos, fragment_files, config_file, kconfig_file):
    """Compute a fingerprint from section names and mtimes of all inputs."""
    hasher = hashlib.md5()

    # Section names from objdump output
    for archive, info in sorted(sections_infos.sections.items()):
        names = _RE_SECTION_NAME.findall(info.content)
        hasher.update((archive + ':' + ','.join(names)).encode())

    # Mtimes of fragment files, sdkconfig, kconfig
    input_files = [p.name if hasattr(p, 'name') else p for p in fragment_files]
    input_files += [p for p in (config_file, kconfig_file) if p]
    for path in input_files:
        try:
            hasher.update(f'{path}:{os.path.getmtime(path)}'.encode())
        except OSError:
            return None

    return hasher.hexdigest()


def _can_skip_generation(output_path, fingerprint):
    """Check if fingerprint matches cached value from previous run."""
    try:
        with open(output_path + '.fingerprint') as f:
            if f.read().strip() == fingerprint:
                os.utime(output_path, None)
                return True
    except OSError:
        pass
    return False


def _save_fingerprint(output_path, fingerprint):
    """Save fingerprint for next run."""
    try:
        with open(output_path + '.fingerprint', 'w') as f:
            f.write(fingerprint)
    except OSError:
        pass


def _compute_lf_cache_key(fragment_files, config_file, kconfig_file):
    """Compute a cache key for parsed fragment files.

    Keyed on fragment file paths+mtimes plus sdkconfig and kconfig mtimes,
    because fragment parsing evaluates `if/elif/else` conditional blocks
    against sdkconfig at parse time — so a sdkconfig change can change the
    parsed FragmentFile output even if the fragment files themselves are
    unchanged.
    """
    hasher = hashlib.md5()
    paths = [p.name if hasattr(p, 'name') else p for p in fragment_files]
    paths += [p for p in (config_file, kconfig_file) if p]
    for path in sorted(paths):
        try:
            hasher.update(f'{path}:{os.path.getmtime(path)}'.encode())
        except OSError:
            return None
    return hasher.hexdigest()


def _load_lf_cache(cache_path, key):
    """Load parsed FragmentFile list from cache if key matches.

    The lf cache is written and read only by ldgen, in the same build
    directory where sections.ld, compiled object files, and the rest of
    the build state already live. The trust boundary matches the build
    system's trust boundary: anyone who can modify <output>.lfcache can
    also modify sections.ld, *.o, or the toolchain binaries directly.
    pickle is safe in this context; it is not exposed to untrusted input.
    """
    try:
        with open(cache_path, 'rb') as f:
            data = pickle.load(f)
        if isinstance(data, dict) and data.get('key') == key:
            return data.get('fragments')
    except (OSError, pickle.UnpicklingError, EOFError, AttributeError, ImportError, ValueError):
        pass
    return None


def _save_lf_cache(cache_path, key, fragments):
    """Save parsed FragmentFile list for next run."""
    try:
        with open(cache_path, 'wb') as f:
            pickle.dump({'key': key, 'fragments': fragments}, f, pickle.HIGHEST_PROTOCOL)
    except (OSError, pickle.PicklingError):
        pass


def _update_environment(args):
    env = [(name, value) for (name, value) in (e.split('=', 1) for e in args.env)]
    for name, value in env:
        value = ' '.join(value.split())
        os.environ[name] = value

    if args.env_file is not None:
        env = json.load(args.env_file)
        os.environ.update(env)


def main():
    argparser = argparse.ArgumentParser(description='ESP-IDF linker script generator')

    argparser.add_argument('--input', '-i', help='Linker template file', type=argparse.FileType('r'))

    fragments_group = argparser.add_mutually_exclusive_group()

    fragments_group.add_argument(
        '--fragments', '-f', type=argparse.FileType('r'), help='Input fragment files', nargs='+'
    )

    fragments_group.add_argument(
        '--fragments-list', help='Input fragment files as a semicolon-separated list', type=str
    )

    fragments_group.add_argument(
        '--fragments-list-file',
        type=argparse.FileType('r'),
        help='File containing fragment file paths, one per line',
    )

    argparser.add_argument(
        '--libraries-file', type=argparse.FileType('r'), help='File that contains the list of libraries in the build'
    )

    argparser.add_argument(
        '--mutable-libraries-file',
        type=argparse.FileType('r'),
        help='File that contains the list of mutable libraries in the build',
    )

    argparser.add_argument('--output', '-o', help='Output linker script', type=str)

    argparser.add_argument('--config', '-c', help='Project configuration')

    argparser.add_argument('--kconfig', '-k', help='IDF Kconfig file')

    argparser.add_argument(
        '--check-mapping', help='Perform a check if a mapping (archive, obj, symbol) exists', action='store_true'
    )

    argparser.add_argument(
        '--check-mapping-exceptions', help='Mappings exempted from check', type=argparse.FileType('r')
    )

    argparser.add_argument(
        '--env',
        '-e',
        action='append',
        default=[],
        help='Environment to set when evaluating the config file',
        metavar='NAME=VAL',
    )

    argparser.add_argument(
        '--env-file',
        type=argparse.FileType('r'),
        help='Optional file to load environment variables from. Contents '
        'should be a JSON object where each key/value pair is a variable.',
    )

    argparser.add_argument('--objdump', help='Path to toolchain objdump')

    argparser.add_argument('--debug', '-d', help='Print debugging information.', action='store_true')

    args = argparser.parse_args()

    input_file = args.input
    libraries_file = args.libraries_file
    mutable_libraries_file = args.mutable_libraries_file or []
    config_file = args.config
    output_path = args.output
    kconfig_file = args.kconfig
    objdump = args.objdump

    fragment_files = []
    if args.fragments_list:
        fragment_files = args.fragments_list.split(';')
    elif args.fragments_list_file:
        fragment_files = [line.strip() for line in args.fragments_list_file if line.strip()]
    elif args.fragments:
        fragment_files = args.fragments

    check_mapping = args.check_mapping
    if args.check_mapping_exceptions:
        check_mapping_exceptions = [line.strip() for line in args.check_mapping_exceptions]
    else:
        check_mapping_exceptions = None

    no_cache = os.environ.get('LDGEN_NO_CACHE') == '1'
    if no_cache:
        print('Linker script generation caches disabled by LDGEN_NO_CACHE')

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

        # Check if we can skip generation entirely — section names and other
        # inputs unchanged since last run.
        fingerprint = (
            None if no_cache else _compute_fingerprint(sections_infos, fragment_files, config_file, kconfig_file)
        )
        if (
            output_path
            and fingerprint
            and os.path.exists(output_path)
            and _can_skip_generation(output_path, fingerprint)
        ):
            print('Skipping linker script generation, section names unchanged')
            sys.exit(0)

        mutable_libs = [lib.strip() for lib in mutable_libraries_file]
        generation_model = Generation(check_mapping, check_mapping_exceptions, mutable_libs, args.debug)

        _update_environment(args)  # assign args.env and args.env_file to os.environ

        sdkconfig = SDKConfig(kconfig_file, config_file)

        # Try to load parsed fragment files from cache. The lf cache is
        # complementary to the fingerprint skip above: if we get here, section
        # names changed, but the fragment files themselves may still be
        # identical and don't need re-parsing.
        lf_cache_path = None if no_cache or not output_path else output_path + '.lfcache'
        lf_cache_key = None if no_cache else _compute_lf_cache_key(fragment_files, config_file, kconfig_file)
        parsed_fragments = None
        if lf_cache_path and lf_cache_key:
            parsed_fragments = _load_lf_cache(lf_cache_path, lf_cache_key)
            if parsed_fragments is not None:
                print('Skipping linker fragment parsing, fragment files unchanged')

        if parsed_fragments is None:
            parsed_fragments = []
            for fragment_file in fragment_files:
                try:
                    parsed = parse_fragment_file(fragment_file, sdkconfig)
                except (ParseException, ParseFatalException) as e:
                    # ParseException is raised on incorrect grammar
                    # ParseFatalException is raised on correct grammar, but inconsistent contents (ex. duplicate
                    # keys, key unsupported by fragment, unexpected number of values, etc.)
                    raise LdGenFailure(f'failed to parse {fragment_file}\n{e}')
                parsed_fragments.append(parsed)
            if lf_cache_path and lf_cache_key:
                _save_lf_cache(lf_cache_path, lf_cache_key, parsed_fragments)

        for parsed in parsed_fragments:
            generation_model.add_fragments_from_file(parsed)

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

            with open(
                output_path, 'w', encoding='utf-8'
            ) as f:  # only create output file after generation has succeeded
                f.write(output.read())

        if output_path and fingerprint:
            _save_fingerprint(output_path, fingerprint)
    except LdGenFailure as e:
        print(f'linker script generation failed for {input_file.name}\nERROR: {e}')
        sys.exit(1)


if __name__ == '__main__':
    main()
