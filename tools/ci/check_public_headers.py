#!/usr/bin/env python
#
# Checks all public headers in IDF in the ci
#
# SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import argparse
import fnmatch
import json
import os
import queue
import re
import subprocess
import tempfile
from threading import Event
from threading import Thread
from typing import List
from typing import Optional
from typing import Set
from typing import Tuple
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
        return 'Header Build Error with {}'.format(self.compiler)


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


#   Creates a temp file and returns both output as a string and a file name
#
def exec_cmd_to_temp_file(what: List, suffix: str='') -> Tuple[int, str, str, str, str]:
    out_file = tempfile.NamedTemporaryFile(suffix=suffix, delete=False)
    rc, out, err, cmd = exec_cmd(what, out_file)
    with open(out_file.name, 'r', encoding='utf-8') as f:
        out = f.read()
    return rc, out, err, out_file.name, cmd


def exec_cmd(what: List, out_file: Union['tempfile._TemporaryFileWrapper[bytes]', int]=subprocess.PIPE) -> Tuple[int, str, str, str]:
    p = subprocess.Popen(what, stdin=subprocess.PIPE, stdout=out_file, stderr=subprocess.PIPE)
    output_b, err_b = p.communicate()
    rc = p.returncode
    output: str = output_b.decode('utf-8') if output_b is not None else ''
    err: str = err_b.decode('utf-8') if err_b is not None else ''
    return rc, output, err, ' '.join(what)


class PublicHeaderChecker:

    def log(self, message: str, debug: bool=False) -> None:
        if self.verbose or debug:
            print(message)

    def __init__(self, verbose: bool=False, jobs: int=1, prefix: Optional[str]=None) -> None:
        self.gcc = '{}gcc'.format(prefix)
        self.gpp = '{}g++'.format(prefix)
        self.verbose = verbose
        self.jobs = jobs
        self.prefix = prefix
        self.extern_c = re.compile(r'extern "C"')
        self.error_macro = re.compile(r'#error')
        self.error_orphan_kconfig = re.compile(r'#error CONFIG_VARS_USED_WHILE_SDKCONFIG_NOT_INCLUDED')
        self.kconfig_macro = re.compile(r'\bCONFIG_[A-Z0-9_]+')
        self.static_assert = re.compile(r'(_Static_assert|static_assert)')
        self.defines_assert = re.compile(r'#define[ \t]+ESP_STATIC_ASSERT')
        self.auto_soc_header = re.compile(r'components/soc/esp[a-z0-9_]+(?:/\w+)?/include/(soc|modem)/[a-zA-Z0-9_]+.h')
        self.assembly_nocode = r'^\s*(\.file|\.text|\.ident|\.option|\.attribute).*$'
        self.check_threads: List[Thread] = []
        self.stdc = '--std=c99'
        self.stdcpp = '--std=c++17'

        self.job_queue: queue.Queue = queue.Queue()
        self.failed_queue: queue.Queue = queue.Queue()
        self.terminate = Event()

    def __enter__(self) -> 'PublicHeaderChecker':
        for i in range(self.jobs):
            t = Thread(target=self.check_headers, args=(i, ))
            self.check_threads.append(t)
            t.start()
        return self

    def __exit__(self, exc_type: str, exc_value: str, traceback: str) -> None:
        self.terminate.set()
        for t in self.check_threads:
            t.join()

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
                        self.failed_queue.put('{}: Failed! {}'.format(task, e))
                    except Exception as e:
                        # Makes sure any unexpected exceptions causes the program to terminate
                        self.failed_queue.put('{}: Failed! {}'.format(task, e))
                        self.terminate.set()
                        raise

    def get_failed(self) -> List:
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
        self.compile_one_header_with(self.gcc, self.stdc, header)
        self.compile_one_header_with(self.gpp, self.stdcpp, header)

    # Checks if the header contains some assembly code and whether it is compilable
    def compile_one_header_with(self, compiler: str, std_flags: str, header: str) -> None:
        rc, out, err, cmd = exec_cmd([compiler, std_flags, '-S', '-o-', '-include', header, self.main_c] + self.include_dir_flags)
        if rc == 0:
            if not re.sub(self.assembly_nocode, '', out, flags=re.M).isspace():
                raise HeaderFailedContainsCode()
            return  # Header OK: produced zero code
        self.log('{}: FAILED: compilation issue'.format(header), True)
        self.log(err, True)
        self.log('\nCompilation command failed:\n{}\n'.format(cmd), True)
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
        all_compilation_flags = ['-w', '-P', '-E', '-DESP_PLATFORM', '-include', header, self.main_c] + self.include_dir_flags
        # just strip comments to check for CONFIG_... macros or static asserts
        rc, out, err, _ = exec_cmd([self.gcc, '-fpreprocessed', '-dD',  '-P',  '-E', header] + self.include_dir_flags)
        # we ignore the rc here, as the `-fpreprocessed` flag expects the file to have macros already expanded, so we might get some errors
        # here we use it only to remove comments (even if the command returns non-zero code it produces the correct output)
        if re.search(self.kconfig_macro, out):
            # enable defined #error if sdkconfig.h not included
            all_compilation_flags.append('-DIDF_CHECK_SDKCONFIG_INCLUDED')
        # If the file contain _Static_assert or static_assert, make sure it doesn't not define ESP_STATIC_ASSERT and that it
        # is not an automatically generated soc header file
        grp = re.search(self.static_assert, out)
        # Normalize the potential A//B, A/./B, A/../A, from the name
        normalized_path = os.path.normpath(header)
        if grp and not re.search(self.defines_assert, out) and not re.search(self.auto_soc_header, normalized_path):
            self.log('{}: FAILED: contains {}. Please use ESP_STATIC_ASSERT'.format(header, grp.group(1)), True)
            raise HeaderFailedContainsStaticAssert()
        try:
            # compile with C++, check for errors, outputs for a temp file
            rc, cpp_out, err, cpp_out_file, cmd = exec_cmd_to_temp_file([self.gpp, self.stdcpp] + all_compilation_flags)
            if rc != 0:
                if re.search(self.error_macro, err):
                    if re.search(self.error_orphan_kconfig, err):
                        self.log('{}: CONFIG_VARS_USED_WHILE_SDKCONFIG_NOT_INCLUDED'.format(header), True)
                        raise HeaderFailedSdkconfig()
                    self.log('{}: Error directive failure: OK'.format(header))
                    return
                self.log('{}: FAILED: compilation issue'.format(header), True)
                self.log(err, True)
                self.log('\nCompilation command failed:\n{}\n'.format(cmd), True)
                raise HeaderFailedPreprocessError()
            # compile with C compiler, outputs to another temp file
            rc, _, err, c_out_file, _ = exec_cmd_to_temp_file([self.gcc, self.stdc] + all_compilation_flags)
            if rc != 0:
                self.log('{} FAILED: compilation in C (while C++ compilation passes)'.format(header))
                raise HeaderFailedPreprocessError()
            # diff the two outputs
            rc, diff, err, _ = exec_cmd(['diff', c_out_file, cpp_out_file])
            if not diff or diff.isspace():
                if not cpp_out or cpp_out.isspace():
                    self.log('{} The same, but empty out - OK'.format(header))
                    return
                self.log('{} FAILED C and C++ preprocessor output is the same!'.format(header), True)
                raise HeaderFailedCppGuardMissing()
            if re.search(self.extern_c, diff):
                self.log('{} extern C present in the diff'.format(header))
                # now check the extern "C" is really in the unprocessed header
                if re.search(self.extern_c, out):
                    self.log('{} extern C present in the actual header, too - OK'.format(header))
                    return
                # at this point we know that the header itself is missing extern-C, so we need to check if it contains an actual *code*
                # we remove all preprocessor's directive to check if there's any code besides macros
                macros = re.compile(r'(?m)^\s*#(?:.*\\\r?\n)*.*$')  # Matches multiline preprocessor directives
                without_macros = macros.sub('', out)
                if without_macros.isspace():
                    self.log("{} Header doesn't need extern-C, it's all just macros - OK".format(header))
                    return
                # at this point we know that the header is not only composed of macro definitions, but could just contain some "harmless" macro calls
                # let's remove them and check again
                macros_calls = r'(.*?)ESP_STATIC_ASSERT[^;]+;'   # static assert macro only, we could add more if needed
                without_macros = re.sub(macros_calls, '', without_macros, flags=re.DOTALL)
                if without_macros.isspace():
                    self.log("{} Header doesn't need extern-C, it's all macros definitions and calls - OK".format(header))
                    return

            self.log('{} Different but no extern C - FAILED'.format(header), True)
            raise HeaderFailedCppGuardMissing()
        finally:
            os.unlink(cpp_out_file)
            try:
                os.unlink(c_out_file)
            except Exception:
                pass

    # Get compilation data from an example to list all public header files
    def list_public_headers(self, ignore_dirs: List, ignore_files: Union[List, Set], only_dir: str=None) -> None:
        idf_path = os.getenv('IDF_PATH')
        if idf_path is None:
            raise RuntimeError("Environment variable 'IDF_PATH' wasn't set.")
        project_dir = os.path.join(idf_path, 'examples', 'get-started', 'blink')
        build_dir = tempfile.mkdtemp()
        sdkconfig = os.path.join(build_dir, 'sdkconfig')
        try:
            os.unlink(os.path.join(project_dir, 'sdkconfig'))
        except FileNotFoundError:
            pass
        subprocess.check_call(['idf.py', '-B', build_dir, f'-DSDKCONFIG={sdkconfig}', 'reconfigure'], cwd=project_dir)

        def get_std(json: List, extension: str) -> str:
            # compile commands for the files with specified extension, containing C(XX) standard flag
            command = [c for c in j if c['file'].endswith('.' + extension) and '-std=' in c['command']][0]
            return str([s for s in command['command'].split() if 'std=' in s][0])     # grab the std flag

        build_commands_json = os.path.join(build_dir, 'compile_commands.json')
        with open(build_commands_json, 'r', encoding='utf-8') as f:
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
                include_dir_flags.append(item.replace('\\',''))  # removes escaped quotes, eg: -DMBEDTLS_CONFIG_FILE=\\\"mbedtls/esp_config.h\\\"
        include_dir_flags.append('-I' + os.path.join(build_dir, 'config'))
        include_dir_flags.append('-DCI_HEADER_CHECK')
        sdkconfig_h = os.path.join(build_dir, 'config', 'sdkconfig.h')
        # prepares a main_c file for easier sdkconfig checks and avoid compilers warning when compiling headers directly
        with open(sdkconfig_h, 'a') as f:
            f.write('#define IDF_SDKCONFIG_INCLUDED')
        main_c = os.path.join(build_dir, 'compile.c')
        with open(main_c, 'w') as f:
            f.write('#if defined(IDF_CHECK_SDKCONFIG_INCLUDED) && ! defined(IDF_SDKCONFIG_INCLUDED)\n'
                    '#error CONFIG_VARS_USED_WHILE_SDKCONFIG_NOT_INCLUDED\n'
                    '#endif')
        # processes public include dirs, removing ignored files
        all_include_files = []
        files_to_check = []
        for d in include_dirs:
            if only_dir is not None and not os.path.relpath(d, idf_path).startswith(os.path.relpath(only_dir, idf_path)):
                self.log('{} - directory ignored (not in "{}")'.format(d, only_dir))
                continue
            if os.path.relpath(d, idf_path).startswith(tuple(ignore_dirs)):
                self.log('{} - directory ignored'.format(d))
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
                self.log('{} - file ignored (inside ignore dir)'.format(file_name))
                continue
            if rel_path_file in ignore_files:
                self.log('{} - file ignored'.format(file_name))
                continue
            files_to_check.append(file_name)
        # removes duplicates and places headers to a work queue
        for file_name in set(files_to_check):
            self.job_queue.put(file_name)
        self.job_queue.put(None)  # to indicate the last job


def check_all_headers() -> None:
    parser = argparse.ArgumentParser('Public header checker file', formatter_class=argparse.RawDescriptionHelpFormatter, epilog='''\
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

    Notes:
    * The script validates *all* header files (recursively) in public folders for all components.
    * The script locates include paths from running a default build of  "examples/get-started/blink'
    * The script does not support any other targets than esp32

    General tips:
    * Use "-d" argument to make the script check only the offending header file
    * Use "-v" argument to produce more verbose output
    * Copy, paste and execute the compilation commands to reproduce build errors (script prints out
      the entire compilation command line with absolute paths)
    ''')
    parser.add_argument('--verbose', '-v', help='enables verbose mode', action='store_true')
    parser.add_argument('--jobs', '-j', help='number of jobs to run checker', default=1, type=int)
    parser.add_argument('--prefix', '-p', help='compiler prefix', default='xtensa-esp32-elf-', type=str)
    parser.add_argument('--exclude-file', '-e', help='exception file', default='check_public_headers_exceptions.txt', type=str)
    parser.add_argument('--only-dir', '-d', help='reduce the analysis to this directory only', default=None, type=str)
    args = parser.parse_args()

    # process excluded files and dirs
    exclude_file = os.path.join(os.path.dirname(__file__), args.exclude_file)
    with open(exclude_file, 'r', encoding='utf-8') as f:
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
    with PublicHeaderChecker(args.verbose, args.jobs, args.prefix) as header_check:
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
