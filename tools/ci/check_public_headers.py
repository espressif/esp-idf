#!/usr/bin/env python
#
# Checks all public headers in IDF in the ci
#
# SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import argparse
import fnmatch
import json
import os
import queue
import re
import shutil
import subprocess
import tempfile
from threading import Event
from threading import Thread
from typing import Union


class HeaderFailed(Exception):
    """Base header failure exception"""

    pass


class HeaderFailedSdkconfig(HeaderFailed):
    def __str__(self) -> str:
        return 'Sdkconfig Error'


class HeaderFailedBuildError(HeaderFailed):
    def __init__(self, compiler: str):
        self.compiler = compiler

    def __str__(self) -> str:
        return f'Header Build Error with {self.compiler}'


class HeaderFailedPreprocessError(HeaderFailed):
    def __str__(self) -> str:
        return 'Header Procecessing Error'


class HeaderFailedCppGuardMissing(HeaderFailed):
    def __str__(self) -> str:
        return 'Header Missing C++ Guard'


class HeaderFailedContainsCode(HeaderFailed):
    def __str__(self) -> str:
        return 'Header Produced non-zero object'


class HeaderFailedContainsStaticAssert(HeaderFailed):
    def __str__(self) -> str:
        return 'Header uses _Static_assert or static_assert instead of ESP_STATIC_ASSERT'


class HeaderFailedForbiddenFreertosInclude(HeaderFailed):
    def __str__(self) -> str:
        return 'Header includes forbidden "freertos/*" dependency'


#   Creates a temp file and returns both output as a string and a file name
#
def exec_cmd_to_temp_file(what: list, suffix: str = '') -> tuple[int, str, str, str, str]:
    out_file = tempfile.NamedTemporaryFile(suffix=suffix, delete=False)
    rc, out, err, cmd = exec_cmd(what, out_file)
    with open(out_file.name, encoding='utf-8') as f:
        out = f.read()
    return rc, out, err, out_file.name, cmd


def exec_cmd(
    what: list, out_file: Union['tempfile._TemporaryFileWrapper[bytes]', int] = subprocess.PIPE
) -> tuple[int, str, str, str]:
    p = subprocess.Popen(what, stdin=subprocess.PIPE, stdout=out_file, stderr=subprocess.PIPE)
    output_b, err_b = p.communicate()
    rc = p.returncode
    output: str = output_b.decode('utf-8') if output_b is not None else ''
    err: str = err_b.decode('utf-8') if err_b is not None else ''
    return rc, output, err, ' '.join(what)


class PublicHeaderChecker:
    def log(self, message: str, debug: bool = False) -> None:
        if self.verbose or debug:
            print(message)

    def __init__(self, libc_type: str, verbose: bool = False, jobs: int = 1, prefix: str | None = None) -> None:
        self.gcc = f'{prefix}gcc'
        self.gpp = f'{prefix}g++'
        self.verbose = verbose
        self.jobs = jobs
        self.prefix = prefix
        self.extern_c = re.compile(r'extern "C"')
        self.error_macro = re.compile(r'#error')
        self.error_orphan_kconfig = re.compile(r'#error CONFIG_VARS_USED_WHILE_SDKCONFIG_NOT_INCLUDED')
        self.kconfig_macro = re.compile(r'\bCONFIG_[A-Z0-9_]+')
        self.static_assert = re.compile(r'(_Static_assert|static_assert)')
        self.defines_assert = re.compile(r'#define[ \t]+ESP_STATIC_ASSERT')
        self.auto_soc_header = re.compile(
            r'components/soc/esp[a-z0-9_]+(?:/\w+)?/'
            r'(include|register)/'
            r'(soc|modem|hw_ver(?:\d+|_[A-Za-z0-9]+)/soc)/'
            r'[a-zA-Z0-9_]+\.h$'
        )
        self.freertos_forbidden_include = re.compile(r'(?m)^\s*#\s*include\s*"freertos/[^"]+"')
        # Scope for enforcing freertos include rule:
        # - components/esp_adc/**
        # - components/esp_driver_*/**
        # - components/esp_hw_support/**
        # - components/sdmmc/**
        # - components/ieee802154/**
        # - components/esp_eth/**
        # - components/esp_lcd/**
        self.freertos_scope = re.compile(
            r'^(components/(esp_adc|esp_hw_support|sdmmc|ieee802154|esp_eth|esp_lcd)/|components/esp_driver_[^/]+/)'
        )
        self.assembly_nocode = r'^\s*(\.file|\.text|\.ident|\.option|\.attribute|(\.section)?).*$'
        self.check_threads: list[Thread] = []
        self.stdc = '--std=c99'
        self.stdcpp = '--std=c++17'
        self.libc_type = libc_type

        self.job_queue: queue.Queue = queue.Queue()
        self.failed_queue: queue.Queue = queue.Queue()
        self.terminate = Event()
        # Get IDF_PATH early to avoid dependency on method execution order
        idf_path = os.getenv('IDF_PATH')
        if idf_path is None:
            raise RuntimeError("Environment variable 'IDF_PATH' wasn't set.")
        self.idf_path = idf_path
        self.build_dir = tempfile.mkdtemp()

    def __enter__(self) -> 'PublicHeaderChecker':
        for i in range(self.jobs):
            t = Thread(target=self.check_headers, args=(i,))
            self.check_threads.append(t)
            t.start()
        return self

    def __exit__(self, exc_type: str, exc_value: str, traceback: str) -> None:
        self.terminate.set()
        for t in self.check_threads:
            t.join()
        shutil.rmtree(self.build_dir)

    # thread function process incoming header file from a queue
    def check_headers(self, num: int) -> None:
        while not self.terminate.is_set():
            if not self.job_queue.empty():
                task = self.job_queue.get()
                if task is None:
                    self.terminate.set()
                else:
                    try:
                        self.check_one_header(task, num)
                    except HeaderFailed as e:
                        self.failed_queue.put(f'{task}: Failed! {e}')
                    except Exception as e:
                        # Makes sure any unexpected exceptions causes the program to terminate
                        self.failed_queue.put(f'{task}: Failed! {e}')
                        self.terminate.set()
                        raise

    def get_failed(self) -> list:
        return list(self.failed_queue.queue)

    def join(self) -> None:
        for t in self.check_threads:
            while t.is_alive() and not self.terminate.is_set():
                t.join(1)  # joins with timeout to respond to keyboard interrupt

    # Checks one header procedure:
    # - Preprocess the include file with C preprocessor and with CPP preprocessor, compare check for possible issues
    # - Compile the header with both C and C++ compiler
    def check_one_header(self, header: str, num: int) -> None:
        self.preprocess_one_header(header, num)
        temp_file = tempfile.NamedTemporaryFile(mode='w', suffix='.c', dir=self.build_dir, delete=False)
        compile_file = temp_file.name
        with temp_file:
            # There can not `-include {header}` be used because in this case system headers
            # which are overridden in ESP-IDF have wrong include_next behavior.
            # This happens because file was already included but search paths are:
            #   - components/esp_libc/platform_include
            #   - toolchain_system_include_path
            # So, when checking headers from platform_include directory,
            # they will not include system headers by include_next.
            # To fix this, header is included to the source file.
            if 'platform_include' in header:
                sys_header = header.split('platform_include/')[1]
                temp_file.write(f'#include <{sys_header}>\n')
            else:
                temp_file.write(f'#include "{header}"\n')
            temp_file.write(f'#include "{self.main_c}"\n')
        self.compile_one_header_with(self.gcc, self.stdc, header, compile_file)
        self.compile_one_header_with(self.gpp, self.stdcpp, header, compile_file)

    # Checks if the header contains some assembly code and whether it is compilable
    def compile_one_header_with(self, compiler: str, std_flags: str, header: str, compile_file: str) -> None:
        cmd_list = [compiler, std_flags, '-S', '-o-', compile_file] + self.include_dir_flags
        if self.libc_type == 'picolibc':
            cmd_list.append('-specs=picolibc.specs')

        rc, out, err, cmd = exec_cmd(cmd_list)
        if rc == 0:
            if not re.sub(self.assembly_nocode, '', out, flags=re.M).isspace():
                raise HeaderFailedContainsCode()
            return  # Header OK: produced zero code
        self.log(f'{header}: FAILED: compilation issue', True)
        self.log(err, True)
        self.log(f'\nCompilation command failed:\n{cmd}\n', True)
        raise HeaderFailedBuildError(compiler)

    # Checks one header using preprocessing and parsing
    #   1) Remove comments and check
    #        - if we have some `CONFIG_...` macros (for test 2)
    #        - static asserts
    #   2) Preprocess with C++ flags and test orphan Kconfig macros, or compiler error
    #   3) Preprocess with C flags and test for compiler errors
    #   4) Compare outputs from steps 2) and 3)
    #       - outputs are the same, but contain only whitespaces -> header is OK (contains only preprocessor directives)
    #       - outputs are the same, but contain some code -> FAIL the test, we're missing `extern "C"` somewhere
    #       - outputs are different:
    #           - check for `extern "C"` in the diff
    #               - Not present? -> FAIL the test, we're missing `extern "C"` somewhere
    #               - Present? -- extern "C" is there, but could be from included headers
    #                   - check for `extern "C"` in the orig header (output of step 1)
    #                       - Present? -> header is OK (we're have the `extern "C"` in the header under test)
    #                       - Not present? -- we're missing `extern "C"` in our header, but do we really need it?
    #                           - Remove all directives and harmless macro invocations from our current header
    #                               - We still have some code? -> FAIL the test (our header needs extern "C")
    #                               - Only whitespaces -> header is OK (it contains only macros and directives)
    def preprocess_one_header(self, header: str, num: int) -> None:
        all_compilation_flags = [
            '-w',
            '-P',
            '-E',
            '-DESP_PLATFORM',
            '-include',
            header,
            self.main_c,
        ] + self.include_dir_flags
        if self.libc_type == 'picolibc':
            all_compilation_flags.append('-specs=picolibc.specs')

        # just strip comments to check for CONFIG_... macros or static asserts
        rc, out, err, _ = exec_cmd([self.gcc, '-fpreprocessed', '-dD', '-P', '-E', header] + self.include_dir_flags)
        # we ignore the rc here, as the `-fpreprocessed` flag expects the file to have macros already expanded,
        # so we might get some errors here we use it only to remove comments (even if the command returns non-zero
        # code it produces the correct output)
        # forbid direct inclusion of FreeRTOS headers via quotes (only in scoped components)
        if re.search(self.freertos_forbidden_include, out):
            # Determine if current header is inside the scoped component folders
            rel_path_for_scope = os.path.normpath(header)
            try:
                # self.idf_path is set in list_public_headers
                rel_path_for_scope = os.path.relpath(rel_path_for_scope, self.idf_path)
            except Exception:
                pass
            if re.search(self.freertos_scope, rel_path_for_scope):
                # Raise, let final summary print the failure; avoid noisy intermediate logs
                raise HeaderFailedForbiddenFreertosInclude()
        if re.search(self.kconfig_macro, out):
            # enable defined #error if sdkconfig.h not included
            all_compilation_flags.append('-DIDF_CHECK_SDKCONFIG_INCLUDED')
        # If the file contain _Static_assert or static_assert, make sure it doesn't not define ESP_STATIC_ASSERT
        # and that it is not an automatically generated soc header file
        grp = re.search(self.static_assert, out)
        # Normalize the potential A//B, A/./B, A/../A, from the name
        normalized_path = os.path.normpath(header)
        if grp and not re.search(self.defines_assert, out) and not re.search(self.auto_soc_header, normalized_path):
            self.log(f'{header}: FAILED: contains {grp.group(1)}. Please use ESP_STATIC_ASSERT', True)
            raise HeaderFailedContainsStaticAssert()
        try:
            # compile with C++, check for errors, outputs for a temp file
            rc, cpp_out, err, cpp_out_file, cmd = exec_cmd_to_temp_file([self.gpp, self.stdcpp] + all_compilation_flags)
            if rc != 0:
                if re.search(self.error_macro, err):
                    if re.search(self.error_orphan_kconfig, err):
                        self.log(f'{header}: CONFIG_VARS_USED_WHILE_SDKCONFIG_NOT_INCLUDED', True)
                        raise HeaderFailedSdkconfig()
                    self.log(f'{header}: Error directive failure: OK')
                    return
                self.log(f'{header}: FAILED: compilation issue', True)
                self.log(err, True)
                self.log(f'\nCompilation command failed:\n{cmd}\n', True)
                raise HeaderFailedPreprocessError()
            # compile with C compiler, outputs to another temp file
            rc, _, err, c_out_file, _ = exec_cmd_to_temp_file([self.gcc, self.stdc] + all_compilation_flags)
            if rc != 0:
                self.log(f'{header} FAILED: compilation in C (while C++ compilation passes)')
                raise HeaderFailedPreprocessError()
            # diff the two outputs
            rc, diff, err, _ = exec_cmd(['diff', c_out_file, cpp_out_file])
            if not diff or diff.isspace():
                if not cpp_out or cpp_out.isspace():
                    self.log(f'{header} The same, but empty out - OK')
                    return
                self.log(f'{header} FAILED C and C++ preprocessor output is the same!', True)
                raise HeaderFailedCppGuardMissing()
            if re.search(self.extern_c, diff):
                self.log(f'{header} extern C present in the diff')
                # now check the extern "C" is really in the unprocessed header
                if re.search(self.extern_c, out):
                    self.log(f'{header} extern C present in the actual header, too - OK')
                    return
                # at this point we know that the header itself is missing extern-C, so we need to check if it
                # contains an actual *code* we remove all preprocessor's directive to check if there's any code
                # besides macros
                macros = re.compile(r'(?m)^\s*#(?:.*\\\r?\n)*.*$')  # Matches multiline preprocessor directives
                without_macros = macros.sub('', out)
                if without_macros.isspace():
                    self.log(f"{header} Header doesn't need extern-C, it's all just macros - OK")
                    return
                # at this point we know that the header is not only composed of macro definitions, but could
                # just contain some "harmless" macro calls let's remove them and check again
                macros_calls = r'(.*?)ESP_STATIC_ASSERT[^;]+;'  # static assert macro only, we could add more if needed
                without_macros = re.sub(macros_calls, '', without_macros, flags=re.DOTALL)
                if without_macros.isspace():
                    self.log(f"{header} Header doesn't need extern-C, it's all macros definitions and calls - OK")
                    return

            self.log(f'{header} Different but no extern C - FAILED', True)
            raise HeaderFailedCppGuardMissing()
        finally:
            os.unlink(cpp_out_file)
            try:
                os.unlink(c_out_file)
            except Exception:
                pass

    # Get compilation data from an example to list all public header files
    def list_public_headers(self, ignore_dirs: list, ignore_files: list | set, only_dir: str | None = None) -> None:
        idf_path = self.idf_path
        project_dir = os.path.join(idf_path, 'examples', 'get-started', 'blink')
        sdkconfig = os.path.join(self.build_dir, 'sdkconfig')
        if self.libc_type == 'newlib':
            with open(sdkconfig, 'w') as f:
                f.write('CONFIG_LIBC_NEWLIB=y\n')
        try:
            os.unlink(os.path.join(project_dir, 'sdkconfig'))
        except FileNotFoundError:
            pass
        subprocess.check_call(
            ['idf.py', '-B', self.build_dir, f'-DSDKCONFIG={sdkconfig}', '-DCOMPONENTS=', 'reconfigure'],
            cwd=project_dir,
        )

        def get_std(json: list, extension: str) -> str:
            # compile commands for the files with specified extension, containing C(XX) standard flag
            command = [c for c in j if c['file'].endswith('.' + extension) and '-std=' in c['command']][0]
            return str([s for s in command['command'].split() if 'std=' in s][0])  # grab the std flag

        build_commands_json = os.path.join(self.build_dir, 'compile_commands.json')
        with open(build_commands_json, encoding='utf-8') as f:
            j = json.load(f)
            self.stdc = get_std(j, 'c')
            self.stdcpp = get_std(j, 'cpp')
            build_command = j[0]['command'].split()
        include_dir_flags = []
        include_dirs = []
        # process compilation flags (includes and defines)
        for item in build_command:
            if item.startswith('-I'):
                include_dir_flags.append(item)
                if 'components' in item:
                    include_dirs.append(item[2:])  # Removing the leading "-I"
            if item.startswith('-D'):
                include_dir_flags.append(
                    item.replace('\\', '')
                )  # removes escaped quotes, eg: -DMBEDTLS_CONFIG_FILE=\\\"mbedtls/esp_config.h\\\"
        include_dir_flags.append('-I' + os.path.join(self.build_dir, 'config'))
        include_dir_flags.append('-DCI_HEADER_CHECK')
        sdkconfig_h = os.path.join(self.build_dir, 'config', 'sdkconfig.h')
        # prepares a main_c file for easier sdkconfig checks and avoid compilers warning when compiling headers directly
        with open(sdkconfig_h, 'a') as f:
            f.write('#define IDF_SDKCONFIG_INCLUDED')
        main_c = os.path.join(self.build_dir, 'compile.c')
        with open(main_c, 'w') as f:
            f.write(
                '#if defined(IDF_CHECK_SDKCONFIG_INCLUDED) && ! defined(IDF_SDKCONFIG_INCLUDED)\n'
                '#error CONFIG_VARS_USED_WHILE_SDKCONFIG_NOT_INCLUDED\n'
                '#endif'
            )
        # processes public include dirs, removing ignored files
        all_include_files = []
        files_to_check = []
        for d in include_dirs:
            if only_dir is not None and not os.path.relpath(d, idf_path).startswith(
                os.path.relpath(only_dir, idf_path)
            ):
                self.log(f'{d} - directory ignored (not in "{only_dir}")')
                continue
            if os.path.relpath(d, idf_path).startswith(tuple(ignore_dirs)):
                self.log(f'{d} - directory ignored')
                continue
            for root, dirnames, filenames in os.walk(d):
                for filename in fnmatch.filter(filenames, '*.h'):
                    all_include_files.append(os.path.join(root, filename))
        self.main_c = main_c
        self.include_dir_flags = include_dir_flags
        ignore_files = set(ignore_files)
        # processes public include files, removing ignored files
        for file_name in all_include_files:
            rel_path_file = os.path.relpath(file_name, idf_path)
            if any([os.path.commonprefix([d, rel_path_file]) == d for d in ignore_dirs]):
                self.log(f'{file_name} - file ignored (inside ignore dir)')
                continue
            if rel_path_file in ignore_files:
                self.log(f'{file_name} - file ignored')
                continue
            files_to_check.append(file_name)
        # removes duplicates and places headers to a work queue
        for file_name in set(files_to_check):
            self.job_queue.put(file_name)
        self.job_queue.put(None)  # to indicate the last job


def check_all_headers() -> None:
    parser = argparse.ArgumentParser(
        'Public header checker file',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""\
    Tips for fixing failures reported by this script
    ------------------------------------------------
    This checker validates all public headers to detect these types of issues:
    1) "Sdkconfig Error": Using SDK config macros without including "sdkconfig.h"
        * Check if the failing include file or any other included file uses "CONFIG_..." prefixed macros
    2) "Header Build Error": Header itself is not compilable (missing includes, macros, types)
        * Check that all referenced macros, types are available (defined or included)
        * Check that all included header files are available (included in paths)
        * Check for possible compilation issues
        * If only the C++ compilation fails, check that the header is C++ compatible
        * Try to compile only the offending header file
    3) "Header Missing C++ Guard": Preprocessing the header by C and C++ should produce different output
        * Check if the "#ifdef __cplusplus" header sentinels are present
    4) "Header Produced non-zero object": Header contains some object, a definition
        * Check if no definition is present in the offending header file
    5) "Header contains _Static_assert or static_assert": Makes the use of _Static_assert or static_assert
        functions instead of using ESP_STATIC_ASSERT macro
    6) "Header includes forbidden \\"freertos/*\\" dependency": Using #include "freertos/..." inside
        * public headers is not allowed

    Notes:
    * The script validates *all* header files (recursively) in public folders for all components.
    * The script locates include paths from running a default build of  "examples/get-started/blink'
    * The script does not support any other targets than esp32

    General tips:
    * Use "-d" argument to make the script check only the offending header file
    * Use "-v" argument to produce more verbose output
    * Copy, paste and execute the compilation commands to reproduce build errors (script prints out
      the entire compilation command line with absolute paths)
    """,
    )
    parser.add_argument('--verbose', '-v', help='enables verbose mode', action='store_true')
    parser.add_argument('--jobs', '-j', help='number of jobs to run checker', default=1, type=int)
    parser.add_argument('--prefix', '-p', help='compiler prefix', default='xtensa-esp32-elf-', type=str)
    parser.add_argument(
        '--exclude-file', '-e', help='exception file', default='check_public_headers_exceptions.txt', type=str
    )
    parser.add_argument('--only-dir', '-d', help='reduce the analysis to this directory only', default=None, type=str)
    parser.add_argument(
        '--libc-type', '-l', help='type of libc to use', default='picolibc', choices=['picolibc', 'newlib'], type=str
    )
    args = parser.parse_args()

    # process excluded files and dirs
    exclude_file = os.path.join(os.path.dirname(__file__), args.exclude_file)
    with open(exclude_file, encoding='utf-8') as f:
        lines = [line.rstrip() for line in f]
    ignore_files = []
    ignore_dirs = []
    for line in lines:
        if not line or line.isspace() or line.startswith('#'):
            continue
        if os.path.isdir(line):
            ignore_dirs.append(line)
        else:
            ignore_files.append(line)

    # start header check
    with PublicHeaderChecker(args.libc_type, args.verbose, args.jobs, args.prefix) as header_check:
        header_check.list_public_headers(ignore_dirs, ignore_files, only_dir=args.only_dir)
        try:
            header_check.join()
            failures = header_check.get_failed()
            if len(failures) > 0:
                for failed in failures:
                    print(failed)
                print(parser.epilog)
                exit(1)
            print('No errors found')
        except KeyboardInterrupt:
            print('Keyboard interrupt')


if __name__ == '__main__':
    check_all_headers()
