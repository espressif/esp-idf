#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
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

COMMENT_REGEX = re.compile(r'//.*?$|/\*.*?\*/', re.DOTALL | re.MULTILINE)
ESP_SYSTEM_INIT_FN_REGEX_SIMPLE = re.compile(r'\bESP_SYSTEM_INIT_FN\s*\(')
ESP_SYSTEM_INIT_FN_REGEX = re.compile(
    r'ESP_SYSTEM_INIT_FN\(([a-zA-Z0-9_]+)\s*,\s*([a-zA-Z\ _0-9\(\)|]+)\s*,\s*([a-zA-Z\ _0-9\(\)|]+)\s*,\s*([0-9]+)\)'
)
STARTUP_ENTRIES_FILE = 'components/esp_system/system_init_fn.txt'
EXCLUDED_SOURCE_DIRS = {'test_apps', 'host_test', 'host_tests'}


class StartupEntry:
    def __init__(self, filename: str, func: str, stage: str, affinity: str, priority: int) -> None:
        self.filename = filename
        self.func = func
        self.stage = stage
        self.affinity = affinity
        self.priority = priority

    def __str__(self) -> str:
        return f'{self.stage}: {self.priority:3d}: {self.func} in {self.filename} on {self.affinity}'


def should_skip_source_file(filename: str, idf_path: str) -> bool:
    relpath_parts = os.path.relpath(filename, idf_path).split(os.sep)
    return any(part in EXCLUDED_SOURCE_DIRS for part in relpath_parts)


def strip_comments(contents: str) -> str:
    return COMMENT_REGEX.sub('', contents)


def main() -> None:
    try:
        idf_path = os.environ['IDF_PATH']
    except KeyError:
        raise SystemExit('IDF_PATH must be set before running this script')

    has_errors = False
    startup_entries: list[StartupEntry] = []

    #
    # 1. Iterate over all .c and .cpp source files and find ESP_SYSTEM_INIT_FN definitions
    #
    source_files_iters = []
    for extension in ('c', 'cpp'):
        glob_iter = glob.glob(os.path.join(idf_path, 'components', '**', f'*.{extension}'), recursive=True)
        source_files_iters.append(glob_iter)
    for filename in itertools.chain(*source_files_iters):
        if should_skip_source_file(filename, idf_path):
            continue

        relpath = os.path.relpath(filename, idf_path)
        with open(filename, encoding='utf-8') as f_obj:
            file_contents = f_obj.read()

        file_contents_no_comments = strip_comments(file_contents)
        if not ESP_SYSTEM_INIT_FN_REGEX_SIMPLE.search(file_contents_no_comments):
            continue

        count_expected = len(ESP_SYSTEM_INIT_FN_REGEX_SIMPLE.findall(file_contents_no_comments))
        found = ESP_SYSTEM_INIT_FN_REGEX.findall(file_contents_no_comments)
        if len(found) != count_expected:
            print(
                (
                    f'error: In {filename}, found ESP_SYSTEM_INIT_FN {count_expected} time(s), '
                    f'but regular expression matched {len(found)} time(s)'
                ),
                file=sys.stderr,
            )
            has_errors = True

        for match in found:
            entry = StartupEntry(
                filename=relpath, func=match[0], stage=match[1], affinity=match[2], priority=int(match[3])
            )
            startup_entries.append(entry)

    #
    # 2. Sort the ESP_SYSTEM_INIT_FN functions in C source files.
    #    In addition to the stage and priority, we also add filename to the sort key,
    #    to have a stable sorting order in case when the same startup function is defined in multiple files,
    #    for example for different targets.
    #
    def sort_key(entry: StartupEntry) -> tuple[str, int, str]:
        # luckily 'core' and 'secondary' are in alphabetical order, so we can return the string
        return (entry.stage, entry.priority, entry.filename)

    startup_entries = list(sorted(startup_entries, key=sort_key))
    startup_entries_lines = [str(entry) for entry in startup_entries]

    #
    # 3. Load startup entries list from STARTUP_ENTRIES_FILE, removing comments and empty lines
    #
    startup_entries_expected_lines = []
    with open(os.path.join(idf_path, STARTUP_ENTRIES_FILE), encoding='utf-8') as startup_entries_expected_file:
        for line in startup_entries_expected_file:
            if line.startswith('#') or len(line.strip()) == 0:
                continue
            startup_entries_expected_lines.append(line.rstrip())

    #
    # 4. Print the list of differences, if any
    #
    diff_lines = list(difflib.unified_diff(startup_entries_expected_lines, startup_entries_lines, lineterm=''))
    if len(diff_lines) > 0:
        print(
            (
                "error: startup order doesn't match the reference file. "
                f'please update {STARTUP_ENTRIES_FILE} to match the actual startup order:'
            ),
            file=sys.stderr,
        )
        for line in diff_lines:
            print(f'{line}', file=sys.stderr)
        has_errors = True

    if has_errors:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
