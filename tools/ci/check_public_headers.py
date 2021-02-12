#!/usr/bin/env python
#
# Checks all public headers in IDF in the ci
#
# Copyright 2020 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

from __future__ import print_function
from __future__ import unicode_literals
import re
import os
import subprocess
import json
import fnmatch
import argparse
import queue
from threading import Thread, Event
import tempfile
from io import open


class HeaderFailed(Exception):
    """Base header failure exeption"""
    pass


class HeaderFailedSdkconfig(HeaderFailed):
    def __str__(self):
        return "Sdkconfig Error"


class HeaderFailedBuildError(HeaderFailed):
    def __str__(self):
        return "Header Build Error"


class HeaderFailedCppGuardMissing(HeaderFailed):
    def __str__(self):
        return "Header Missing C++ Guard"


class HeaderFailedContainsCode(HeaderFailed):
    def __str__(self):
        return "Header Produced non-zero object"


#   Creates a temp file and returns both output as a string and a file name
#
def exec_cmd_to_temp_file(what, suffix=""):
    out_file = tempfile.NamedTemporaryFile(suffix=suffix, delete=False)
    rc, out, err = exec_cmd(what, out_file)
    with open(out_file.name, "r", encoding='utf-8') as f:
        out = f.read()
    return rc, out, err, out_file.name


def exec_cmd(what, out_file=subprocess.PIPE):
    p = subprocess.Popen(what, stdin=subprocess.PIPE, stdout=out_file, stderr=subprocess.PIPE)
    output, err = p.communicate()
    rc = p.returncode
    output = output.decode('utf-8') if output is not None else None
    err = err.decode('utf-8') if err is not None else None
    return rc, output, err


class PublicHeaderChecker:
    # Intermediate results
    COMPILE_ERR_REF_CONFIG_HDR_FAILED = 1  # -> Cannot compile and failed with injected SDKCONFIG #error (header FAILs)
    COMPILE_ERR_ERROR_MACRO_HDR_OK = 2     # -> Cannot compile, but failed with "#error" directive (header seems OK)
    COMPILE_ERR_HDR_FAILED = 3             # -> Cannot compile with another issue, logged if verbose (header FAILs)
    PREPROC_OUT_ZERO_HDR_OK = 4            # -> Both preprocessors produce zero out (header file is OK)
    PREPROC_OUT_SAME_HRD_FAILED = 5        # -> Both preprocessors produce the same, non-zero output (header file FAILs)
    PREPROC_OUT_DIFFERENT_WITH_EXT_C_HDR_OK = 6    # -> Both preprocessors produce different, non-zero output with extern "C" (header seems OK)
    PREPROC_OUT_DIFFERENT_NO_EXT_C_HDR_FAILED = 7  # -> Both preprocessors produce different, non-zero output without extern "C" (header fails)

    def log(self, message, debug=False):
        if self.verbose or debug:
            print(message)

    def __init__(self, verbose=False, jobs=1, prefix=None):
        self.gcc = "{}gcc".format(prefix)
        self.gpp = "{}g++".format(prefix)
        self.verbose = verbose
        self.jobs = jobs
        self.prefix = prefix
        self.extern_c = re.compile(r'extern "C"')
        self.error_macro = re.compile(r'#error')
        self.error_orphan_kconfig = re.compile(r"#error CONFIG_VARS_USED_WHILE_SDKCONFIG_NOT_INCLUDED")
        self.kconfig_macro = re.compile(r'\bCONFIG_[A-Z0-9_]+')
        self.assembly_nocode = r'^\s*(\.file|\.text|\.ident).*$'
        self.check_threads = []

        self.job_queue = queue.Queue()
        self.failed_queue = queue.Queue()
        self.terminate = Event()

    def __enter__(self):
        for i in range(self.jobs):
            t = Thread(target=self.check_headers, args=(i, ))
            self.check_threads.append(t)
            t.start()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.terminate.set()
        for t in self.check_threads:
            t.join()

    # thread function process incoming header file from a queue
    def check_headers(self, num):
        while not self.terminate.is_set():
            if not self.job_queue.empty():
                task = self.job_queue.get()
                if task is None:
                    self.terminate.set()
                else:
                    try:
                        self.check_one_header(task, num)
                    except HeaderFailed as e:
                        self.failed_queue.put("{}: Failed! {}".format(task, e))
                    except Exception as e:
                        # Makes sure any unexpected exceptions causes the program to terminate
                        self.failed_queue.put("{}: Failed! {}".format(task, e))
                        self.terminate.set()
                        raise

    def get_failed(self):
        return list(self.failed_queue.queue)

    def join(self):
        for t in self.check_threads:
            while t.isAlive and not self.terminate.is_set():
                t.join(1)  # joins with timeout to respond to keyboard interrupt

    # Checks one header calling:
    # - preprocess_one_header() to test and compare preprocessor outputs
    # - check_no_code() to test if header contains some executable code
    # Procedure
    # 1) Preprocess the include file with C preprocessor and with CPP preprocessor
    #   - Pass the test if the preprocessor outputs are the same and whitespaces only (#define only header)
    #   - Fail the test if the preprocessor outputs are the same (but with some code)
    #   - If outputs different, continue with 2)
    # 2) Strip out all include directives to generate "temp.h"
    # 3) Preprocess the temp.h the same way in (1)
    #   - Pass the test if the preprocessor outputs are the same and whitespaces only (#include only header)
    #   - Fail the test if the preprocessor outputs are the same (but with some code)
    #   - If outputs different, pass the test
    # 4) If header passed the steps 1) and 3) test that it produced zero assembly code
    def check_one_header(self, header, num):
        res = self.preprocess_one_header(header, num)
        if res == self.COMPILE_ERR_REF_CONFIG_HDR_FAILED:
            raise HeaderFailedSdkconfig()
        elif res == self.COMPILE_ERR_ERROR_MACRO_HDR_OK:
            return self.compile_one_header(header)
        elif res == self.COMPILE_ERR_HDR_FAILED:
            raise HeaderFailedBuildError()
        elif res == self.PREPROC_OUT_ZERO_HDR_OK:
            return self.compile_one_header(header)
        elif res == self.PREPROC_OUT_SAME_HRD_FAILED:
            raise HeaderFailedCppGuardMissing()
        else:
            self.compile_one_header(header)
            temp_header = None
            try:
                _, _, _, temp_header = exec_cmd_to_temp_file(["sed", "/#include/d; /#error/d", header], suffix=".h")
                res = self.preprocess_one_header(temp_header, num, ignore_sdkconfig_issue=True)
                if res == self.PREPROC_OUT_SAME_HRD_FAILED:
                    raise HeaderFailedCppGuardMissing()
                elif res == self.PREPROC_OUT_DIFFERENT_NO_EXT_C_HDR_FAILED:
                    raise HeaderFailedCppGuardMissing()
            finally:
                if temp_header:
                    os.unlink(temp_header)

    def compile_one_header(self, header):
        rc, out, err = exec_cmd([self.gcc, "-S", "-o-", "-include", header, self.main_c] + self.include_dir_flags)
        if rc == 0:
            if not re.sub(self.assembly_nocode, '', out, flags=re.M).isspace():
                raise HeaderFailedContainsCode()
            return  # Header OK: produced zero code
        self.log("{}: FAILED: compilation issue".format(header), True)
        self.log(err, True)
        raise HeaderFailedBuildError()

    def preprocess_one_header(self, header, num, ignore_sdkconfig_issue=False):
        all_compilation_flags = ["-w", "-P", "-E", "-DESP_PLATFORM", "-include", header, self.main_c] + self.include_dir_flags
        if not ignore_sdkconfig_issue:
            # just strip commnets to check for CONFIG_... macros
            rc, out, err = exec_cmd([self.gcc, "-fpreprocessed", "-dD",  "-P",  "-E", header] + self.include_dir_flags)
            if re.search(self.kconfig_macro, out):
                # enable defined #error if sdkconfig.h not included
                all_compilation_flags.append("-DIDF_CHECK_SDKCONFIG_INCLUDED")
        try:
            # compile with C++, check for errors, outputs for a temp file
            rc, cpp_out, err, cpp_out_file = exec_cmd_to_temp_file([self.gpp, "--std=c++17"] + all_compilation_flags)
            if rc != 0:
                if re.search(self.error_macro, err):
                    if re.search(self.error_orphan_kconfig, err):
                        self.log("{}: CONFIG_VARS_USED_WHILE_SDKCONFIG_NOT_INCLUDED".format(header), True)
                        return self.COMPILE_ERR_REF_CONFIG_HDR_FAILED
                    self.log("{}: Error directive failure: OK".format(header))
                    return self.COMPILE_ERR_ERROR_MACRO_HDR_OK
                self.log("{}: FAILED: compilation issue".format(header), True)
                self.log(err)
                return self.COMPILE_ERR_HDR_FAILED
            # compile with C compiler, outputs to another temp file
            rc, c99_out, err, c99_out_file = exec_cmd_to_temp_file([self.gcc, "--std=c99"] + all_compilation_flags)
            if rc != 0:
                self.log("{} FAILED should never happen".format(header))
                return self.COMPILE_ERR_HDR_FAILED
            # diff the two outputs
            rc, diff, err = exec_cmd(["diff", c99_out_file, cpp_out_file])
            if not diff or diff.isspace():
                if not cpp_out or cpp_out.isspace():
                    self.log("{} The same, but empty out - OK".format(header))
                    return self.PREPROC_OUT_ZERO_HDR_OK
                self.log("{} FAILED C and C++ preprocessor output is the same!".format(header), True)
                return self.PREPROC_OUT_SAME_HRD_FAILED
            if re.search(self.extern_c, diff):
                self.log("{} extern C present - OK".format(header))
                return self.PREPROC_OUT_DIFFERENT_WITH_EXT_C_HDR_OK
            self.log("{} Different but no extern C - FAILED".format(header), True)
            return self.PREPROC_OUT_DIFFERENT_NO_EXT_C_HDR_FAILED
        finally:
            os.unlink(cpp_out_file)
            try:
                os.unlink(c99_out_file)
            except Exception:
                pass

    # Get compilation data from an example to list all public header files
    def list_public_headers(self, ignore_dirs, ignore_files, only_dir=None):
        idf_path = os.getenv('IDF_PATH')
        project_dir = os.path.join(idf_path, "examples", "get-started", "blink")
        subprocess.check_call(["idf.py", "reconfigure"], cwd=project_dir)
        build_commands_json = os.path.join(project_dir, "build", "compile_commands.json")
        with open(build_commands_json, "r", encoding='utf-8') as f:
            build_command = json.load(f)[0]["command"].split()
        include_dir_flags = []
        include_dirs = []
        # process compilation flags (includes and defines)
        for item in build_command:
            if item.startswith("-I"):
                include_dir_flags.append(item)
                if "components" in item:
                    include_dirs.append(item[2:])  # Removing the leading "-I"
            if item.startswith("-D"):
                include_dir_flags.append(item.replace('\\',''))  # removes escaped quotes, eg: -DMBEDTLS_CONFIG_FILE=\\\"mbedtls/esp_config.h\\\"
        include_dir_flags.append("-I" + os.path.join(project_dir, "build", "config"))
        sdkconfig_h = os.path.join(project_dir, "build", "config", "sdkconfig.h")
        # prepares a main_c file for easier sdkconfig checks and avoid compilers warning when compiling headers directly
        with open(sdkconfig_h, "a") as f:
            f.write("#define IDF_SDKCONFIG_INCLUDED")
        main_c = os.path.join(project_dir, "build", "compile.c")
        with open(main_c, "w") as f:
            f.write("#if defined(IDF_CHECK_SDKCONFIG_INCLUDED) && ! defined(IDF_SDKCONFIG_INCLUDED)\n"
                    "#error CONFIG_VARS_USED_WHILE_SDKCONFIG_NOT_INCLUDED\n"
                    "#endif")
        # processes public include dirs, removing ignored files
        all_include_files = []
        files_to_check = []
        for d in include_dirs:
            if only_dir is not None and not os.path.relpath(d, idf_path).startswith(only_dir):
                self.log('{} - directory ignored (not in "{}")'.format(d, only_dir))
                continue
            if os.path.relpath(d, idf_path).startswith(tuple(ignore_dirs)):
                self.log("{} - directory ignored".format(d))
                continue
            for root, dirnames, filenames in os.walk(d):
                for filename in fnmatch.filter(filenames, '*.h'):
                    all_include_files.append(os.path.join(root, filename))
        self.main_c = main_c
        self.include_dir_flags = include_dir_flags
        ignore_files = set(ignore_files)
        # processes public include files, removing ignored files
        for f in all_include_files:
            rel_path_file = os.path.relpath(f, idf_path)
            if any([os.path.commonprefix([d, rel_path_file]) == d for d in ignore_dirs]):
                self.log("{} - file ignored (inside ignore dir)".format(f))
                continue
            if rel_path_file in ignore_files:
                self.log("{} - file ignored".format(f))
                continue
            files_to_check.append(f)
        # removes duplicates and places headers to a work queue
        for f in set(files_to_check):
            self.job_queue.put(f)
        self.job_queue.put(None)  # to indicate the last job


def check_all_headers():
    parser = argparse.ArgumentParser("Public header checker file")
    parser.add_argument("--verbose", "-v", help="enables verbose mode", action="store_true")
    parser.add_argument("--jobs", "-j", help="number of jobs to run checker", default=1, type=int)
    parser.add_argument("--prefix", "-p", help="compiler prefix", default="xtensa-esp32-elf-", type=str)
    parser.add_argument("--exclude-file", "-e", help="exception file", default="check_public_headers_exceptions.txt", type=str)
    parser.add_argument("--only-dir", "-d", help="reduce the analysis to this directory only", default=None, type=str)
    args = parser.parse_args()

    # process excluded files and dirs
    exclude_file = os.path.join(os.path.dirname(__file__), args.exclude_file)
    with open(exclude_file, "r", encoding='utf-8') as f:
        lines = [line.rstrip() for line in f]
    ignore_files = []
    ignore_dirs = []
    for line in lines:
        if not line or line.isspace() or line.startswith("#"):
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
                exit(1)
            print("No errors found")
        except KeyboardInterrupt:
            print("Keyboard interrupt")


if __name__ == '__main__':
    check_all_headers()
