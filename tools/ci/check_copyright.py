#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Check files for copyright headers:
- file not on ignore list:
    - old Espressif copyright -> replace with SPDX
    - SPDX with invalid year or old company name -> replace with valid SPDX
    - other SPDX copyright -> PASS
    - non-SPDX copyright -> FAIL
    - no copyright -> insert Espressif copyright
- file on ignore list:
    - old Espressif copyright -> replace with SPDX, remove from ignore list
    - SPDX with invalid year or company format -> replace with valid SPDX and remove from ignore list
    else -> keep on ignore list
"""
import argparse
import datetime
import fnmatch
import os
import re
import sys
import textwrap
from typing import List, Tuple

from comment_parser import comment_parser
from comment_parser.parsers.common import Comment
from thefuzz import fuzz

IDF_PATH = os.getenv('IDF_PATH', os.getcwd())
IGNORE_LIST_FN = os.path.join(IDF_PATH, 'tools/ci/check_copyright_ignore.txt')
PERMANENT_IGNORE_LIST_FN = os.path.join(IDF_PATH, 'tools/ci/check_copyright_permanent_ignore.txt')

CHECK_FAIL_MESSAGE = textwrap.dedent('''\
    To make a file, not on the ignore list to pass the test it needs to contain both:
    an SPDX-FileCopyrightText and
    an SPDX-License-Identifier. For example:
    {example}
    More information about SPDX license identifiers can be found here:
    https://spdx.github.io/spdx-spec/appendix-V-using-SPDX-short-identifiers-in-source-files/
    To have this hook automatically insert the standard Espressif copyright notice,
    ensure the word "copyright" is not in any comment up to line 30 and the file is not on the ignore list.
    Below is a list of files, which failed the copyright check.
    Files prefixed with "(ignore)" are on the ignore list and their presence alone won't cause the check to fail.
    ''')

CHECK_MODIFY_MESSAGE = textwrap.dedent('''\
    Above is a list of files, which were modified. Please check their contents, stage them and run the commit again!
    Files prefixed with "(ignore)" were on the ignore list at the time of invoking this script.
    They may have been removed if noted above.
    Pre-commit's option --show-diff-on-failure may be used to show a diff when hooks modify files.
    ''')

# This is an old header style, which this script
# attempts to detect and replace with a new SPDX license identifier
OLD_APACHE_HEADER = textwrap.dedent('''\
    Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
    ''')


# New headers to be used
NEW_APACHE_HEADER_PYTHON = textwrap.dedent('''\
    # SPDX-FileCopyrightText: {years} Espressif Systems (Shanghai) CO LTD
    # SPDX-License-Identifier: Apache-2.0
    ''')

PYTHON_NOTICE = '# SPDX-FileCopyrightText: {years} Espressif Systems (Shanghai) CO LTD'

NOTICE_MULTILINE = ' * SPDX-FileCopyrightText: {years} Espressif Systems (Shanghai) CO LTD'
NOTICE = '// SPDX-FileCopyrightText: {years} Espressif Systems (Shanghai) CO LTD'

NEW_APACHE_HEADER = textwrap.dedent('''\
    /*
     * SPDX-FileCopyrightText: {years} Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: Apache-2.0
     */
    ''')

MIME = {
    'python': 'text/x-python',
    'c': 'text/x-c',
    'cpp': 'text/x-c++'
}

# terminal color output

TERMINAL_RESET = '\33[0m'
TERMINAL_YELLOW = '\33[93m'
TERMINAL_GREEN = '\33[92m'
TERMINAL_RED = '\33[91m'
TERMINAL_GRAY = '\33[90m'


class UnsupportedFileType(Exception):
    """Exception raised for unsupported file types.

    Attributes:
        file_name -- input file which caused the error
        message -- explanation of the error
    """
    def __init__(self, file_name: str, message: str='this file type is not supported') -> None:
        self.fine_name = file_name
        self.message = message
        super().__init__(self.message)

    def __str__(self) -> str:
        return f'{self.fine_name}: {self.message}'


class NotFound(Exception):
    """Exception raised when something is not found.

    Attributes:
        thing -- what was not found
    """
    def __init__(self, thing: str='something') -> None:
        self.thing = thing
        super().__init__(self.thing)

    def __str__(self) -> str:
        return f'{self.thing} was not found'


class CustomFile():
    """
    Custom data object to hold file name and if it's on the ignore list
    and to make it easier to print
    """
    def __init__(self, file_name: str, is_on_ignore_list: bool) -> None:
        self.file_name = file_name
        self.is_on_ignore_list = is_on_ignore_list

    def __str__(self) -> str:
        if self.is_on_ignore_list:
            return f'(ignore) {self.file_name}'
        return f'         {self.file_name}'


def get_file_mime(fn: str) -> str:
    """
    Return the mime type based on file's extension
    """
    if fn.endswith('.py'):
        return MIME['python']
    if fn.endswith(('.cpp', '.hpp')):
        return MIME['cpp']
    if fn.endswith(('.c', '.h', '.ld')):
        return MIME['c']
    raise UnsupportedFileType(fn)


def get_comments(code: str, mime: str) -> list:
    """
    Extracts all comments from source code and does a multiline split
    """
    comments = comment_parser.extract_comments_from_str(code, mime)
    new_comments = []
    for comment in comments:
        if comment.is_multiline():
            comment_lines = comment.text().splitlines()
            for line_number, line in enumerate(comment_lines, start=comment.line_number()):
                new_comments.append(Comment(line, line_number, True))
        else:
            new_comments.append(comment)
    return new_comments


def has_valid_copyright(file_name: str, mime: str, is_on_ignore: bool, args: argparse.Namespace) -> Tuple[bool, bool]:
    """
    Detects if a file has a valid SPDX copyright notice.
    returns: Tuple[valid, modified]
    """
    detected_licenses = []
    detected_notices = []

    valid, modified = False, False

    with open(file_name, 'r') as f:
        code = f.read()
    comments = get_comments(code, mime)
    code_lines = code.splitlines()
    if not code_lines:  # file is empty
        print(f'{TERMINAL_YELLOW}"{file_name}" is empty!{TERMINAL_RESET}')
        valid = True
        return valid, modified

    if args.replace:
        try:
            year, line = detect_old_header_style(file_name, comments, args)
        except NotFound as e:
            if args.verbose:
                print(f'{TERMINAL_GRAY}{e} in {file_name}{TERMINAL_RESET}')
        else:
            code_lines = replace_copyright(code_lines, year, line, mime, file_name)
            valid = True
    for comment in comments:
        if comment.line_number() > args.max_lines:
            break
        matches = re.search(r'SPDX-FileCopyrightText: ?(.*)', comment.text(), re.IGNORECASE)
        if matches:
            detected_notices.append((matches.group(1), comment.line_number()))
            try:
                year = extract_year_from_espressif_notice(matches.group(1))
            except NotFound as e:
                if args.verbose:
                    print(f'{TERMINAL_GRAY}Not an {e.thing} {file_name}:{comment.line_number()}{TERMINAL_RESET}')
            else:
                template = NOTICE
                if comment.is_multiline():
                    template = NOTICE_MULTILINE
                if mime == MIME['python']:
                    template = PYTHON_NOTICE
                code_lines[comment.line_number() - 1] = template.format(years=format_years(year, file_name))

        matches = re.search(r'SPDX-License-Identifier: ?(.*)', comment.text(), re.IGNORECASE)
        if matches:
            detected_licenses.append((matches.group(1), comment.line_number()))

    if not is_on_ignore and not contains_any_copyright(comments, args):
        code_lines = insert_copyright(code_lines, file_name, mime)
        print(f'"{file_name}": inserted copyright notice - please check the content and run commit again!')
        valid = True
    new_code = '\n'.join(code_lines) + '\n'
    if code != new_code:
        with open(file_name, 'w') as f:
            f.write(new_code)
        modified = True
    if detected_licenses and detected_notices:
        if args.debug:
            print(f'{file_name} notices: {detected_notices}')
            print(f'{file_name} licenses: {detected_licenses}')
        valid = True
    return valid, modified


def contains_any_copyright(comments: list, args: argparse.Namespace) -> bool:
    """
    Return True if any comment contain the word "copyright"
    """
    return any(
        comment.line_number() <= args.max_lines
        and re.search(r'copyright', comment.text(), re.IGNORECASE)
        for comment in comments
    )


def insert_copyright(code_lines: list, file_name: str, mime: str) -> list:
    """
    Insert a copyright notice in the begining of a file, respecting a potencial shebang
    """
    new_code_lines = []
    # if first line contains a shebang, keep it first
    if code_lines[0].startswith('#!'):
        new_code_lines.append(code_lines[0])
        del code_lines[0]
    template = NEW_APACHE_HEADER
    if mime == MIME['python']:
        template = NEW_APACHE_HEADER_PYTHON
    new_code_lines.extend(template.format(years=format_years(0, file_name)).splitlines())
    new_code_lines.extend(code_lines)
    return new_code_lines


def extract_year_from_espressif_notice(notice: str) -> int:
    """
    Extracts copyright year (creation date) from a Espressif copyright notice
    """
    matches = re.search(r'(\d{4})(?:-\d{4})? Espressif Systems', notice, re.IGNORECASE)
    if matches:
        return int(matches.group(1))
    raise NotFound('Espressif copyright notice')


def replace_copyright(code_lines: list, year: int, line: int, mime: str, file_name: str) -> list:
    """
    Replaces old header style with new SPDX form.
    """
    # replace from line number (line) to line number (line + number of lines in the OLD HEADER)
    # with new header depending on file type
    end = line + OLD_APACHE_HEADER.count('\n')
    del code_lines[line - 1:end - 1]

    template = NEW_APACHE_HEADER
    if mime == MIME['python']:
        template = NEW_APACHE_HEADER_PYTHON
    code_lines[line - 1:line - 1] = template.format(years=format_years(year, file_name)).splitlines()

    print(f'{TERMINAL_GRAY}"{file_name}": replacing old header (lines: {line}-{end}) with new SPDX header style.{TERMINAL_RESET}')

    return code_lines


def detect_old_header_style(file_name: str, comments: list, args: argparse.Namespace) -> Tuple[int, int]:
    """
    Detects old header style (Apache-2.0) and extracts the year and line number.
    returns: Tuple[year, comment line number]
    """
    comments_text = str()
    for comment in comments:
        if comment.line_number() > args.max_lines:
            break
        comments_text = f'{comments_text}\n{comment.text().strip()}'
    ratio = fuzz.partial_ratio(comments_text, OLD_APACHE_HEADER)
    if args.debug:
        print(f'{TERMINAL_GRAY}ratio for {file_name}: {ratio}{TERMINAL_RESET}')
    if ratio > args.fuzzy_ratio:
        for comment in comments:
            # only check up to line number MAX_LINES
            if comment.line_number() > args.max_lines:
                break
            try:
                year = extract_year_from_espressif_notice(comment.text())
            except NotFound:
                pass
            else:
                return (year, comment.line_number())
    raise NotFound('Old Espressif header')


def format_years(past: int, file_name: str) -> str:
    """
    Function to format a year:
     - just current year -> output: [year]
     - some year in the past -> output: [past year]-[current year]
    """
    today = datetime.datetime.now().year
    if past == 0:
        # use the current year
        past = today
    if past == today:
        return str(past)
    if past > today or past < 1972:
        error_msg = f'{file_name}: invalid year in the copyright header detected. ' \
            + 'Check your system clock and the copyright header.'
        raise ValueError(error_msg)
    return '{past}-{today}'.format(past=past, today=today)


def check_copyrights(args: argparse.Namespace) -> Tuple[List, List]:
    """
    Main logic and for loop
    returns:
        list of files with wrong headers
        list of files which were modified
    """
    wrong_header_files = []
    modified_files = []

    with open(IGNORE_LIST_FN, 'r') as f:
        ignore_list = [item.strip() for item in f.readlines()]
        updated_ignore_list = ignore_list.copy()

    with open(PERMANENT_IGNORE_LIST_FN) as f:
        permanent_ignore_list = [item.strip() for item in f.readlines()]

    for file_name in args.filenames:
        try:
            mime = get_file_mime(file_name)
        except UnsupportedFileType:
            print(f'{TERMINAL_GRAY}"{file_name}" is not of a supported type! Skipping.{TERMINAL_RESET}')
            continue

        if any(fnmatch.fnmatch(file_name, pattern) for pattern in permanent_ignore_list):
            print(f'{TERMINAL_YELLOW}"{file_name}" is ignored by a permanent pattern!{TERMINAL_RESET}')
            continue

        if file_name in ignore_list:
            if args.verbose:
                print(f'{TERMINAL_GRAY}"{file_name}" is on the ignore list.{TERMINAL_RESET}')
            valid, modified = has_valid_copyright(file_name, mime, True, args)
            if modified:
                modified_files.append(CustomFile(file_name, True))
            if valid:
                if args.dont_update_ignore_list:
                    print(f'{TERMINAL_YELLOW}"{file_name}" now has a correct copyright header - remove it from the ignore list '
                          f'or run this script without the --dont-update-ignore-list option to do this automatically!{TERMINAL_RESET}')
                else:
                    updated_ignore_list.remove(file_name)
            else:
                wrong_header_files.append(CustomFile(file_name, True))
        else:
            valid, modified = has_valid_copyright(file_name, mime, False, args)
            if modified:
                modified_files.append(CustomFile(file_name, False))
            if not valid:
                wrong_header_files.append(CustomFile(file_name, False))

    if updated_ignore_list != ignore_list:
        with open(IGNORE_LIST_FN, 'w') as f:
            for item in updated_ignore_list:
                f.write(f'{item}\n')
        modified_files.append(CustomFile(IGNORE_LIST_FN, False))
        print(f'\n{TERMINAL_GREEN}Files removed from ignore list:{TERMINAL_RESET}')
        for file in ignore_list:
            if file not in updated_ignore_list:
                print(f'    {file}')
    return wrong_header_files, modified_files


def build_parser() -> argparse.ArgumentParser:

    parser = argparse.ArgumentParser(description='Check copyright headers')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='print more information (useful for debugging)')
    parser.add_argument('-r', '--replace', action='store_true',
                        help='tries to update copyright notices')
    parser.add_argument('-m', '--max-lines', type=int, default=30,
                        help='how far to check for copyright notice in a file (default 30)')
    parser.add_argument('-f', '--fuzzy-ratio', type=int, default=95,
                        help='minimum %% ratio to be considered as equal to the old header style (default 95)')
    parser.add_argument('-d', '--debug', action='store_true',
                        help='print debug info')
    parser.add_argument('-du', '--dont-update-ignore-list', action='store_true')
    parser.add_argument('filenames', nargs='+', help='file(s) to check', metavar='file')
    return parser


def main() -> None:

    args = build_parser().parse_args()

    if args.debug:
        print(f'{TERMINAL_GRAY}Running with args: {args}')
        print(f'Permanent ignore list: {PERMANENT_IGNORE_LIST_FN}')
        print(f'Ignore list: {IGNORE_LIST_FN}{TERMINAL_RESET}')

    wrong_header_files, modified_files = check_copyrights(args)
    if modified_files:
        print(f'\n{TERMINAL_YELLOW}Modified files:{TERMINAL_RESET}')
        for file in modified_files:
            print(file)
        print(CHECK_MODIFY_MESSAGE)
    abort_commit = bool(modified_files)
    if wrong_header_files:
        print(f'{TERMINAL_YELLOW}Information about this test{TERMINAL_RESET}')
        print(CHECK_FAIL_MESSAGE.format(example=NEW_APACHE_HEADER.format(years=datetime.datetime.now().year)))
        print(f'{TERMINAL_RED}Files which failed the copyright check:{TERMINAL_RESET}')
        for wrong_file in wrong_header_files:
            if not wrong_file.is_on_ignore_list:
                abort_commit = True
            print(wrong_file)
    num_files_processed = len(args.filenames)
    if abort_commit:
        num_files_modified = len(modified_files)
        num_files_wrong = len(wrong_header_files)
        print(f'{TERMINAL_YELLOW}Processed {num_files_processed} source file{"s"[:num_files_processed^1]},', end=' ')
        print(f'{num_files_modified} modified and {num_files_wrong} with invalid copyright.{TERMINAL_RESET}')
        sys.exit(1)  # sys.exit(1) to abort the commit
    # pre-commit also automatically aborts a commit if files are modified on disk
    print(f'\n{TERMINAL_GREEN}Successfuly processed {num_files_processed} file{"s"[:num_files_processed^1]}.{TERMINAL_RESET}\n')


if __name__ == '__main__':
    main()
