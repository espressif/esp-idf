#!/usr/bin/env python
#
# Utility script for ESP-IDF developers to work with the CODEOWNERS file.
#
# SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import re
import subprocess
import sys
from typing import List, Optional

from idf_ci_utils import IDF_PATH

CODEOWNERS_PATH = os.path.join(IDF_PATH, '.gitlab', 'CODEOWNERS')
CODEOWNER_GROUP_PREFIX = '@esp-idf-codeowners/'


def get_all_files() -> List[str]:
    """
    Get list of all file paths in the repository.
    """
    # only split on newlines, since file names may contain spaces
    return subprocess.check_output(['git', 'ls-files'], cwd=IDF_PATH).decode('utf-8').strip().split('\n')


def pattern_to_regex(pattern: str) -> str:
    """
    Convert the CODEOWNERS path pattern into a regular expression string.
    """
    orig_pattern = pattern  # for printing errors later

    # Replicates the logic from normalize_pattern function in Gitlab ee/lib/gitlab/code_owners/file.rb:
    if not pattern.startswith('/'):
        pattern = '/**/' + pattern
    if pattern.endswith('/'):
        pattern = pattern + '**/*'

    # Convert the glob pattern into a regular expression:
    # first into intermediate tokens
    pattern = (pattern.replace('**/', ':REGLOB:')
                      .replace('**', ':INVALID:')
                      .replace('*', ':GLOB:')
                      .replace('.', ':DOT:')
                      .replace('?', ':ANY:'))

    if pattern.find(':INVALID:') >= 0:
        raise ValueError("Likely invalid pattern '{}': '**' should be followed by '/'".format(orig_pattern))

    # then into the final regex pattern:
    re_pattern = (pattern.replace(':REGLOB:', '(?:.*/)?')
                         .replace(':GLOB:', '[^/]*')
                         .replace(':DOT:', '[.]')
                         .replace(':ANY:', '.') + '$')
    if re_pattern.startswith('/'):
        re_pattern = '^' + re_pattern

    return re_pattern


def files_by_regex(all_files: List, regex: re.Pattern) -> List:
    """
    Return all files in the repository matching the given regular expresion.
    """
    return [file for file in all_files if regex.search('/' + file)]


def files_by_pattern(all_files: list, pattern: Optional[str]=None) -> List:
    """
    Return all the files in the repository matching the given CODEOWNERS pattern.
    """
    if not pattern:
        return all_files

    return files_by_regex(all_files, re.compile(pattern_to_regex(pattern)))


def action_identify(args: argparse.Namespace) -> None:
    best_match = []
    all_files = get_all_files()
    with open(CODEOWNERS_PATH) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            tokens = line.split()
            path_pattern = tokens[0]
            owners = tokens[1:]
            files = files_by_pattern(all_files, path_pattern)
            if args.path in files:
                best_match = owners
    for owner in best_match:
        print(owner)


def action_test_pattern(args: argparse.Namespace) -> None:
    re_pattern = pattern_to_regex(args.pattern)

    if args.regex:
        print(re_pattern)
        return

    files = files_by_regex(get_all_files(), re.compile(re_pattern))
    for f in files:
        print(f)


def action_ci_check(args: argparse.Namespace) -> None:
    errors = []

    def add_error(msg: str) -> None:
        errors.append('{}:{}: {}'.format(CODEOWNERS_PATH, line_no, msg))

    all_files = get_all_files()
    prev_path_pattern = ''
    with open(CODEOWNERS_PATH) as f:
        for line_no, line in enumerate(f, start=1):
            # Skip empty lines and comments
            line = line.strip()
            if line.startswith('# sort-order-reset'):
                prev_path_pattern = ''

            if (not line
                    or line.startswith('#')  # comment
                    or line.startswith('[')  # file group
                    or line.startswith('^[')):  # optional file group
                continue

            # Each line has a form of "<path> <owners>+"
            tokens = line.split()
            path_pattern = tokens[0]
            owners = tokens[1:]
            if not owners:
                add_error('no owners specified for {}'.format(path_pattern))

            # Check that the file is sorted by path patterns
            if not in_order(prev_path_pattern, path_pattern):
                add_error('file is not sorted: {} < {}'.format(path_pattern, prev_path_pattern))
            prev_path_pattern = path_pattern

            # Check that the pattern matches at least one file
            files = files_by_pattern(all_files, path_pattern)
            if not files:
                add_error('no files matched by pattern {}'.format(path_pattern))

            for o in owners:
                # Sanity-check the owner group name
                if not o.startswith(CODEOWNER_GROUP_PREFIX):
                    add_error("owner {} doesn't start with {}".format(o, CODEOWNER_GROUP_PREFIX))

    if not errors:
        print('No errors found.')
    else:
        print('Errors found!')
        for e in errors:
            print(e)
        raise SystemExit(1)


def in_order(prev: str, current: str) -> bool:
    """
    Return True if the ordering is correct for these two lines ('prev' should be before 'current').

    Codeowners should be ordered alphabetically, except that order is also significant for the codeowners
    syntax (the last matching line has priority).

    This means that wildcards are allowed in either order (if wildcard placed first, it's placed before a
    more specific pattern as a catch-all fallback. If wildcard placed second, it's to override the match
    made on a previous line i.e. '/xyz/**/*.py' to override the owner of the Python files inside /xyz/ ).
    """
    if not prev:
        return True  # first element in file

    def is_separator(c: str) -> bool:
        return c in '-_/'  # ignore differences between separators for ordering purposes

    def is_wildcard(c: str) -> bool:
        return c in '?*'

    # looping until we see a different character
    for a,b in zip(prev, current):
        if is_separator(a) and is_separator(b):
            continue
        if is_wildcard(a) or is_wildcard(b):
            return True  # if the strings matched up to one of them having a wildcard, treat as in order
        if a != b:
            return b > a
        assert a == b

    # common substrings up to the common length are the same, so the longer string should be after
    return len(current) >= len(prev)


def main() -> None:
    parser = argparse.ArgumentParser(
        sys.argv[0], description='Internal helper script for working with the CODEOWNERS file.'
    )
    subparsers = parser.add_subparsers(dest='action')

    identify = subparsers.add_parser(
        'identify',
        help='List the owners of the specified path within IDF.'
        "This command doesn't support files inside submodules, or files not added to git repository.",
    )
    identify.add_argument('path', help='Path of the file relative to the root of the repository')

    subparsers.add_parser(
        'ci-check',
        help='Check CODEOWNERS file: every line should match at least one file, sanity-check group names, '
        'check that the file is sorted by paths',
    )

    test_pattern = subparsers.add_parser(
        'test-pattern',
        help='Print files in the repository for a given CODEOWNERS pattern. Useful when adding new rules.'
    )
    test_pattern.add_argument('--regex', action='store_true', help='Print the equivalent regular expression instead of the file list.')
    test_pattern.add_argument('pattern', help='Path pattern to get the list of files for')

    args = parser.parse_args()

    if args.action is None:
        parser.print_help()
        parser.exit(1)

    action_func_name = 'action_' + args.action.replace('-', '_')
    action_func = globals()[action_func_name]
    action_func(args)


if __name__ == '__main__':
    main()
