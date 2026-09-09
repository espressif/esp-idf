#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# This file checks the order of execution of system startup handlers.
# It compares the priorities found in .c source files to the contents of system_init_fn.txt
# In case of an inconsistency, the script prints the differences found and returns with a
# non-zero exit code.
import difflib
import glob
import itertools
import os
import re
import sys

from esp_pylib.excepthook import install_exception_reporting
from esp_pylib.logger import log
from rich.markup import escape

COMMENT_REGEX = re.compile(r'//.*?$|/\*.*?\*/', re.DOTALL | re.MULTILINE)
ESP_SYSTEM_INIT_FN_REGEX = (
    r'{macro}\((?P<func>[a-zA-Z0-9_]+)\s*,\s*'
    r'(?P<stage>[a-zA-Z\ _0-9\(\)|]+)\s*,\s*'
    r'(?P<affinity>[a-zA-Z\ _0-9\(\)|]+)\s*,\s*'
    r'(?P<priority>[0-9]+)\)'
)
APP_INIT_FN_REGEX = r'{macro}\s*\(\s*(?P<func>[a-zA-Z0-9_]+)\s*,\s*(?P<priority>[0-9]+)\s*\)'
STARTUP_REGISTRATIONS = (
    (
        'ESP_SYSTEM_INIT_FN',
        re.compile(ESP_SYSTEM_INIT_FN_REGEX.format(macro='ESP_SYSTEM_INIT_FN')),
        '',
        '',
    ),
    (
        'ESP_PRE_SCHEDULER_HANDLER_REGISTER',
        re.compile(APP_INIT_FN_REGEX.format(macro='ESP_PRE_SCHEDULER_HANDLER_REGISTER')),
        'PRE_SCHEDULER',
        'BIT(0)',
    ),
    (
        'ESP_PRE_SCHEDULER_HANDLER_REGISTER_PER_CPU',
        re.compile(APP_INIT_FN_REGEX.format(macro='ESP_PRE_SCHEDULER_HANDLER_REGISTER_PER_CPU')),
        'PRE_SCHEDULER',
        'ESP_SYSTEM_INIT_ALL_CORES',
    ),
    (
        'ESP_PRE_APP_MAIN_HANDLER_REGISTER',
        re.compile(APP_INIT_FN_REGEX.format(macro='ESP_PRE_APP_MAIN_HANDLER_REGISTER')),
        'PRE_APP_MAIN',
        'MAIN_TASK',
    ),
)
STARTUP_ENTRIES_FILE = 'components/esp_system/system_init_fn.txt'
EXCLUDED_SOURCE_DIRS = {'test_apps', 'host_test', 'host_tests'}
STAGE_ORDER = {
    'CORE': 0,
    'SECONDARY': 1,
    'PRE_SCHEDULER': 2,
    'PRE_APP_MAIN': 3,
}


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
    install_exception_reporting()
    try:
        idf_path = os.environ['IDF_PATH']
    except KeyError:
        log.die('IDF_PATH must be set before running this script')

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
        for macro, pattern, default_stage, default_affinity in STARTUP_REGISTRATIONS:
            count_expected = len(re.findall(rf'\b{macro}\s*\(', file_contents_no_comments))
            found = list(pattern.finditer(file_contents_no_comments))
            if len(found) != count_expected:
                log.err(
                    f'In {filename}, found {macro} {count_expected} time(s), '
                    f'but regular expression matched {len(found)} time(s)',
                )
                has_errors = True

            for match in found:
                startup_entries.append(
                    StartupEntry(
                        relpath,
                        match.group('func'),
                        match.groupdict().get('stage') or default_stage,
                        match.groupdict().get('affinity') or default_affinity,
                        int(match.group('priority')),
                    )
                )

    #
    # 2. Sort the startup handlers in C source files.
    #    In addition to the stage and priority, we also add filename to the sort key,
    #    to have a stable sorting order in case when the same startup function is defined in multiple files,
    #    for example for different targets.
    #
    def sort_key(entry: StartupEntry) -> tuple[int, int, str]:
        return (STAGE_ORDER[entry.stage], entry.priority, entry.filename)

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
        log.err(
            "startup order doesn't match the reference file. "
            f'please update {STARTUP_ENTRIES_FILE} to match the actual startup order:'
        )
        for line in diff_lines:
            log.print(escape(line), file=sys.stderr)
        has_errors = True

    if has_errors:
        sys.exit(1)


if __name__ == '__main__':
    main()
