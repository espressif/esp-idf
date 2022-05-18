#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# This file is used to check the order of execution of ESP_SYSTEM_INIT_FN functions.
# It compares the priorities found in .c source files to the contents of system_init_fn.txt
# In case of an inconsistency, the script prints the differences found and returns with a
# non-zero exit code.

import difflib
import glob
import itertools
import os
import re
import sys
import typing

ESP_SYSTEM_INIT_FN_STR = r'ESP_SYSTEM_INIT_FN'
ESP_SYSTEM_INIT_FN_REGEX_SIMPLE = re.compile(r'ESP_SYSTEM_INIT_FN')
ESP_SYSTEM_INIT_FN_REGEX = re.compile(r'ESP_SYSTEM_INIT_FN\(([a-zA-Z0-9_]+)\s*,\s*([a-zA-Z\ _0-9\(\)|]+)\s*,\s*([0-9]+)\)')
STARTUP_ENTRIES_FILE = 'components/esp_system/system_init_fn.txt'


class StartupEntry:
    def __init__(self, filename: str, func: str, affinity: str, priority: int) -> None:
        self.filename = filename
        self.func = func
        self.affinity = affinity
        self.priority = priority

    def __str__(self) -> str:
        return f'{self.priority:3d}: {self.func} in {self.filename} on {self.affinity}'


def main() -> None:
    try:
        idf_path = os.environ['IDF_PATH']
    except KeyError:
        raise SystemExit('IDF_PATH must be set before running this script')

    has_errors = False
    startup_entries = []  # type: typing.List[StartupEntry]

    #
    # 1. Iterate over all .c and .cpp source files and find ESP_SYSTEM_INIT_FN definitions
    #
    source_files_iters = []
    for extension in ('c', 'cpp'):
        glob_iter = glob.glob(os.path.join(idf_path, 'components', '**', f'*.{extension}'), recursive=True)
        source_files_iters.append(glob_iter)
    for filename in itertools.chain(*source_files_iters):
        with open(filename, 'r') as f_obj:
            file_contents = f_obj.read()
        if ESP_SYSTEM_INIT_FN_STR not in file_contents:
            continue
        count_expected = len(ESP_SYSTEM_INIT_FN_REGEX_SIMPLE.findall(file_contents))
        found = ESP_SYSTEM_INIT_FN_REGEX.findall(file_contents)
        if len(found) != count_expected:
            print((f'error: In {filename}, found ESP_SYSTEM_INIT_FN {count_expected} time(s), '
                   f'but regular expression matched {len(found)} time(s)'), file=sys.stderr)
            has_errors = True

        for match in found:
            entry = StartupEntry(
                filename=os.path.relpath(filename, idf_path),
                func=match[0],
                affinity=match[1],
                priority=int(match[2])
            )
            startup_entries.append(entry)

    #
    # 2. Sort the ESP_SYSTEM_INIT_FN functions in C source files by priority
    #
    startup_entries = list(sorted(startup_entries, key=lambda e: e.priority))
    startup_entries_lines = [str(entry) for entry in startup_entries]

    #
    # 3. Load startup entries list from STARTUP_ENTRIES_FILE, removing comments and empty lines
    #
    startup_entries_expected_lines = []
    with open(os.path.join(idf_path, STARTUP_ENTRIES_FILE), 'r') as startup_entries_expected_file:
        for line in startup_entries_expected_file:
            if line.startswith('#') or len(line.strip()) == 0:
                continue
            startup_entries_expected_lines.append(line.rstrip())

    #
    # 4. Print the list of differences, if any
    #
    diff_lines = list(difflib.unified_diff(startup_entries_expected_lines, startup_entries_lines, lineterm=''))
    if len(diff_lines) > 0:
        print(('error: startup order doesn\'t match the reference file. '
               f'please update {STARTUP_ENTRIES_FILE} to match the actual startup order:'), file=sys.stderr)
        for line in diff_lines:
            print(f'{line}', file=sys.stderr)
        has_errors = True

    if has_errors:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
