#!/usr/bin/env python
#
# 'check_for_file_paths.py' is a CI tool that checks all the unlinked object files
# in a CMake build directory for embedded copies of IDF_PATH.
#
# Designed to be run in CI as a check that __FILE__ macros haven't snuck into any source code.
#
# Checking the unlinked object files means we don't rely on anything being actually linked into the binary,
# just anything which could potentially be linked.
#
# Usage:
# ./check_for_file_paths.py <IDF_PATH> <BUILD_DIR>
#
#
#
# SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import os
import re
import sys

from elftools.elf.elffile import ELFFile

# If an ESP-IDF source file has no option but to include __FILE__ macros, name it here (as re expression).
#
# IMPORTANT: This should only be used for upstream code where there is no other
# option. ESP-IDF code should avoid embedding file names as much as possible to
# limit the binary size and support reproducible builds
#
# Note: once ESP-IDF moves to Python >=3.6 then this can be simplified to use 'glob' and '**'
EXCEPTIONS = [
    r'openssl/.+/ssl_pm.c.obj$',  # openssl API requires __FILE__ in error reporting functions, as per upstream API
    r'openssl/.+/ssl_bio.c.obj$',
    r'unity/.+/unity_runner.c.obj$',  # unity is not for production use, has __FILE__ for test information
]


def main():  # type: () -> None
    idf_path = sys.argv[1]
    build_dir = sys.argv[2]

    assert os.path.exists(idf_path)
    assert os.path.exists(build_dir)

    print(f'Checking object files in {build_dir} for mentions of {idf_path}...')

    # note: once ESP-IDF moves to Python >=3.6 then this can be simplified to use 'glob' and f'{build_dir}**/*.obj'
    files = []
    for dirpath, _, filepaths in os.walk(build_dir):
        files += [os.path.join(dirpath, filepath) for filepath in filepaths if filepath.endswith('.obj')]

    print(f'Found {len(files)} object files...')

    idf_path_binary = (
        idf_path.encode()
    )  # we're going to be checking binary streams (note: probably non-ascii IDF_PATH will not match OK)

    failures = 0
    for obj_file in files:
        if not any(re.search(exception, obj_file) for exception in EXCEPTIONS):
            failures += check_file(obj_file, idf_path_binary)
    if failures > 0:
        raise SystemExit(f'{failures} source files are embedding file paths, see list above.')
    print('No embedded file paths found')


def check_file(obj_file, idf_path):  # type: (str, bytes) -> int
    failures = 0
    with open(obj_file, 'rb') as f:
        elf = ELFFile(f)
        for sec in elf.iter_sections():
            # can't find a better way to filter out only sections likely to contain strings,
            # and exclude debug sections. .dram matches DRAM_STR, which links to .dram1
            if '.rodata' in sec.name or '.dram' in sec.name:
                contents = sec.data()
                if idf_path in contents:
                    print(f'error: {obj_file} contains an unwanted __FILE__ macro')
                    failures += 1
                    break
    return failures


if __name__ == '__main__':
    main()
