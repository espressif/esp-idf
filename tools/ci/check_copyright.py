#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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
import ast
import configparser
import datetime
import os
import re
import sys
import textwrap
from typing import List, Optional, Tuple

import pathspec
import yaml
# importing the whole comment_parser causes a crash when running inside of gitbash environment on Windows.
from comment_parser.parsers import c_parser, python_parser
from comment_parser.parsers.common import Comment
from thefuzz import fuzz

IDF_PATH = os.getenv('IDF_PATH', os.getcwd())
IGNORE_LIST_FN = os.path.join(IDF_PATH, 'tools/ci/check_copyright_ignore.txt')
CONFIG_FN = os.path.join(IDF_PATH, 'tools', 'ci', 'check_copyright_config.yaml')

CHECK_FAIL_MESSAGE = textwrap.dedent('''\
    To make a file pass the test, it needs to contain both:
    an SPDX-FileCopyrightText and an SPDX-License-Identifier with an allowed license for the section.
    More information about SPDX license identifiers can be found here:
    https://spdx.github.io/spdx-spec/appendix-V-using-SPDX-short-identifiers-in-source-files/
    To have this hook automatically insert the standard Espressif copyright notice,
    ensure the word "copyright" is not in any comment up to line 30 and the file is not on the ignore list.
    Below is a list of files, which failed the copyright check.
    ''')

CHECK_MODIFY_MESSAGE = textwrap.dedent('''\
    Above is a list of files, which were modified. Please check their contents, stage them and run the commit again!
    Files prefixed with "(ignore)" were on the ignore list at the time of invoking this script.
    They may have been removed if noted above.
    ''')

CHECK_FOOTER_MESSAGE = textwrap.dedent('''\

    Additional information about this hook and copyright headers may be found here:
    https://docs.espressif.com/projects/esp-idf/en/latest/esp32/contribute/copyright-guide.html
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

NEW_APACHE_HEADER = textwrap.dedent('''\
    /*
     * SPDX-FileCopyrightText: {years} Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: Apache-2.0
     */
    ''')
# filetype -> mime
MIME = {
    'python': 'text/x-python',
    'c': 'text/x-c',
    'cpp': 'text/x-c++'
}

# mime -> parser
MIME_PARSER = {
    'text/x-c': c_parser,
    'text/x-c++': c_parser,
    'text/x-python': python_parser,
}

# terminal color output
TERMINAL_RESET = '\33[0m'
TERMINAL_BOLD = '\33[1m'
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
    def __init__(self, file_name: str, message: str = 'this file type is not supported') -> None:
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
    def __init__(self, thing: str = 'something') -> None:
        self.thing = thing
        super().__init__(self.thing)

    def __str__(self) -> str:
        return f'{self.thing} was not found'


class CustomFile:
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


class CommentHolder(Comment):
    """
    Hold the comment, its line number and when it is multiline,
    also store if it's the first in a comment block
    """
    def __init__(self, text: str, line_number: int, multiline: bool = False, first_in_multiline: bool = False):
        """
        Args:
            text: String text of comment.
            line_number: Line number (int) comment was found on.
            multiline: bool is it multiline
            first_in_multiline: bool if multiline, is it first in that comment block
        """
        super(self.__class__, self).__init__(text, line_number, multiline)
        self._first_in_multiline = first_in_multiline and multiline

    def is_first_in_multiline(self) -> bool:
        """
        Returns whether this comment was a first in a multiline comment.
        """
        return self._first_in_multiline


def get_file_mime(fn: str) -> str:
    """
    Return the mime type based on file's extension
    """
    if fn.endswith('.py'):
        return MIME['python']
    if fn.endswith(('.cpp', '.hpp')):
        return MIME['cpp']
    if fn.endswith(('.c', '.h', '.ld', '.s', '.S')):
        return MIME['c']
    raise UnsupportedFileType(fn)


def get_comments(code: str, mime: str) -> list:
    """
    Extracts all comments from source code and does a multiline split
    """
    parser = MIME_PARSER[mime]
    comments = parser.extract_comments(code)
    new_comments = []
    for comment in comments:
        if comment.is_multiline():
            comment_lines = comment.text().splitlines()
            for line_number, line in enumerate(comment_lines, start=comment.line_number()):
                # the third argument of Comment is a bool multiline. Store the relative line number inside the multiline comment
                new_comments.append(CommentHolder(line, line_number, True, line_number == comment.line_number()))
        else:
            new_comments.append(CommentHolder(comment.text(), comment.line_number()))
    return new_comments


def has_valid_copyright(file_name: str, mime: str, is_on_ignore: bool, config_section: configparser.SectionProxy,
                        args: argparse.Namespace) -> Tuple[bool, bool]:
    """
    Detects if a file has a valid SPDX copyright notice.
    returns: Tuple[valid, modified]
    """
    detected_licenses = []
    detected_notices = []
    detected_contributors = []

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
            if args.debug:
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
                years = extract_years_from_espressif_notice(matches.group(1))
            except NotFound as e:
                if args.verbose:
                    print(f'{TERMINAL_GRAY}Not an {e.thing} {file_name}:{comment.line_number()}{TERMINAL_RESET}')
            else:
                template = '// SPDX-FileCopyrightText: ' + config_section['espressif_copyright']
                if comment.is_multiline():
                    template = ' * SPDX-FileCopyrightText: ' + config_section['espressif_copyright']
                if comment.is_first_in_multiline():
                    template = '/* SPDX-FileCopyrightText: ' + config_section['espressif_copyright']
                if mime == MIME['python']:
                    template = '# SPDX-FileCopyrightText: ' + config_section['espressif_copyright']
                candidate_line = template.format(years=format_years(years[0], file_name))
                no_time_update = template.format(years=format_years(years[0], file_name, years[1]))
                if code_lines[comment.line_number() - 1] != no_time_update:
                    # update the line only in cases when not only the dates are changing
                    code_lines[comment.line_number() - 1] = candidate_line

        matches = re.search(r'SPDX-FileContributor: ?(.*)', comment.text(), re.IGNORECASE)
        if matches:
            detected_contributors.append((matches.group(1), comment.line_number()))
            try:
                years = extract_years_from_espressif_notice(matches.group(1))
            except NotFound as e:
                if args.debug:
                    print(f'{TERMINAL_GRAY}Not an {e.thing} {file_name}:{comment.line_number()}{TERMINAL_RESET}')
            else:
                template = '// SPDX-FileContributor: ' + config_section['espressif_copyright']
                if comment.is_multiline():
                    template = ' * SPDX-FileContributor: ' + config_section['espressif_copyright']
                if comment.is_first_in_multiline():
                    template = '/* SPDX-FileContributor: ' + config_section['espressif_copyright']
                if mime == MIME['python']:
                    template = '# SPDX-FileContributor: ' + config_section['espressif_copyright']
                candidate_line = template.format(years=format_years(years[0], file_name))
                no_time_update = template.format(years=format_years(years[0], file_name, years[1]))
                if code_lines[comment.line_number() - 1] != no_time_update:
                    # update the line only in cases when not only the dates are changing
                    code_lines[comment.line_number() - 1] = candidate_line

        matches = re.search(r'SPDX-License-Identifier: ?(.*)', comment.text(), re.IGNORECASE)
        if matches:
            detected_licenses.append((matches.group(1), comment.line_number()))

    if not is_on_ignore and not contains_any_copyright(comments, args):
        code_lines = insert_copyright(code_lines, file_name, mime, config_section)
        print(f'"{file_name}": inserted copyright notice - please check the content and run commit again!')
        valid = True
    new_code = '\n'.join(code_lines) + '\n'

    if code != new_code:
        with open(file_name, 'w') as f:
            f.write(new_code)
        modified = True

    if detected_licenses and detected_notices:
        valid = True
        if args.debug:
            print(f'{file_name} notices: {detected_notices}')
            print(f'{file_name} licenses: {detected_licenses}')

    if detected_licenses:
        for detected_license, line_number in detected_licenses:
            allowed_licenses = ast.literal_eval(config_section['allowed_licenses'])
            if not allowed_license_combination(detected_license, allowed_licenses):
                valid = False
                print(f'{TERMINAL_RED}{file_name}:{line_number} License "{detected_license}" is not allowed! Allowed licenses: {allowed_licenses}.')

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


def insert_copyright(code_lines: list, file_name: str, mime: str, config_section: configparser.SectionProxy) -> list:
    """
    Insert a copyright notice in the beginning of a file, respecting a potential shebang
    """
    new_code_lines = []
    # if first line contains a shebang, keep it first
    if code_lines[0].startswith('#!'):
        new_code_lines.append(code_lines[0])
        del code_lines[0]
    template = config_section['new_notice_c']
    if mime == MIME['python']:
        template = config_section['new_notice_python']
    new_code_lines.extend(template.format(license=config_section['license_for_new_files'], years=format_years(0, file_name)).splitlines())
    new_code_lines.extend(code_lines)
    return new_code_lines


def extract_years_from_espressif_notice(notice: str) -> Tuple[int, Optional[int]]:
    """
    Extracts copyright years from a Espressif copyright notice. It returns a tuple (x, y) where x is the first year of
    the copyright and y is the second year. y is None if the copyright notice contains only one year.
    """
    matches = re.search(r'(\d{4})(-(\d{4}))? Espressif Systems', notice, re.IGNORECASE)
    if matches:
        years = matches.group(1, 3)
        return (int(years[0]), int(years[1]) if years[1] else None)
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

    print(f'{TERMINAL_BOLD}"{file_name}": replacing old Apache-2.0 header (lines: {line}-{end}) with the new SPDX header.{TERMINAL_RESET}')

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
                year = extract_years_from_espressif_notice(comment.text())[0]
            except NotFound:
                pass
            else:
                return (year, comment.line_number())
    raise NotFound('Old Espressif header')


def format_years(past: int, file_name: str, today: Optional[int]=None) -> str:
    """
    Function to format a year:
     - just current year -> output: [year]
     - some year in the past -> output: [past year]-[current year]
    """
    _today = today or datetime.datetime.now().year
    if past == 0:
        # use the current year
        past = _today
    if past == _today:
        return str(past)
    if past > _today or past < 1972:
        error_msg = f'{file_name}: invalid year in the copyright header detected. ' \
            + 'Check your system clock and the copyright header.'
        raise ValueError(error_msg)
    return '{past}-{today}'.format(past=past, today=_today)


def check_copyrights(args: argparse.Namespace, config: configparser.ConfigParser) -> Tuple[List, List]:
    """
    Main logic and for loop
    returns:
        list of files with wrong headers
        list of files which were modified
    """
    wrong_header_files = []
    modified_files = []
    pathspecs = {}

    with open(IGNORE_LIST_FN, 'r') as f:
        ignore_list = [item.strip() for item in f.readlines()]
        updated_ignore_list = ignore_list.copy()

    # compile the file patterns
    for section in config.sections():

        # configparser stores all values as strings
        patterns = ast.literal_eval(config[section]['include'])
        try:
            pathspecs[section] = pathspec.PathSpec.from_lines('gitwildmatch', patterns)
        except TypeError:
            print(f'Error while compiling file patterns. Section {section} has invalid include option. Must be a list of file patterns.')
            sys.exit(1)

    for file_name in args.filenames:
        try:
            mime = get_file_mime(file_name)
        except UnsupportedFileType:
            print(f'{TERMINAL_GRAY}"{file_name}" is not of a supported type! Skipping.{TERMINAL_RESET}')
            continue

        matched_section = 'DEFAULT'
        for section in config.sections():
            if pathspecs[section].match_file(file_name):
                if args.debug:
                    print(f'{TERMINAL_GRAY}{file_name} matched {section}{TERMINAL_RESET}')
                matched_section = section

        if config[matched_section]['perform_check'] == 'False':  # configparser stores all values as strings
            print(f'{TERMINAL_GRAY}"{file_name}" is using config section "{matched_section}" which does not perform the check! Skipping.{TERMINAL_RESET}')
            continue

        if file_name in ignore_list:
            if args.verbose:
                print(f'{TERMINAL_GRAY}"{file_name}" is on the ignore list.{TERMINAL_RESET}')
            valid, modified = has_valid_copyright(file_name, mime, True, config[matched_section], args)
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
            valid, modified = has_valid_copyright(file_name, mime, False, config[matched_section], args)
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


def debug_output(args: argparse.Namespace, config: configparser.ConfigParser) -> None:
    print(f'{TERMINAL_GRAY}Running with args: {args}')
    print(f'Config file: {CONFIG_FN}')
    print(f'Ignore list: {IGNORE_LIST_FN}{TERMINAL_RESET}')
    print(f'Sections: {config.sections()}')
    for section in config:
        print(f'section: "{section}"')
        for key in config[section]:
            print(f'    {key}: "{config[section][key]}"')


def allowed_license_combination(license_to_match: str, all_licenses: List[str]) -> bool:
    """
    Licenses can be combined together with the OR keyword. Therefore, a simple "in" lookup in a list is not enough.
    For example, if "A" and "B" are supported then "A OR B" and "B OR A" should be supported as well.
    """
    if license_to_match in all_licenses:
        # This is the simple case, for example, when "A" is used from the list ["A", "B"]
        return True

    # for example, if license_to_match is "A OR B" then the following split will be ["A", "B"]
    split_list = [sp for sp in map(str.strip, license_to_match.split(' OR ')) if len(sp) > 0]

    # for example, "A" and "B" needs to be in the supported list in order to match "A OR B".
    return all(i in all_licenses for i in split_list)


def verify_config(config: configparser.ConfigParser) -> None:
    fail = False
    for section in config:
        license_for_new_files = config[section]['license_for_new_files']

        # configparser stores all values as strings
        allowed_licenses = ast.literal_eval(config[section]['allowed_licenses'])
        if not allowed_license_combination(license_for_new_files, allowed_licenses):
            print(f'Invalid config, section "{section}":\nDefault license for new files '
                  f'({license_for_new_files}) is not on the allowed licenses list {allowed_licenses}.')
            fail = True
    for section in config.sections():
        if 'include' not in config[section]:
            print(f'Invalid config, section "{section}":\nSection does not have the "include" option set.')
            fail = True
    if fail:
        sys.exit(1)


def main() -> None:

    args = build_parser().parse_args()
    config = configparser.ConfigParser()
    with open(CONFIG_FN, 'r') as f:
        yaml_dict = yaml.safe_load(f)
        config.read_dict(yaml_dict)

    if args.debug:
        debug_output(args, config)
    verify_config(config)
    wrong_header_files, modified_files = check_copyrights(args, config)
    abort_commit = bool(modified_files)
    num_files_wrong = 0
    if wrong_header_files:
        print(f'{TERMINAL_YELLOW}Information about this test{TERMINAL_RESET}')
        print(CHECK_FAIL_MESSAGE.format())
        print(f'{TERMINAL_YELLOW}Files which failed the copyright check:{TERMINAL_RESET}')
        for wrong_file in wrong_header_files:
            if not wrong_file.is_on_ignore_list:
                abort_commit = True
                num_files_wrong += 1
                print(wrong_file)
    if modified_files:
        print(f'\n{TERMINAL_YELLOW}Modified files:{TERMINAL_RESET}')
        for file in modified_files:
            print(file)
        print(CHECK_MODIFY_MESSAGE)

    num_files_processed = len(args.filenames)
    print(CHECK_FOOTER_MESSAGE)
    if abort_commit:
        num_files_modified = len(modified_files)

        print(f'{TERMINAL_RED}Processed {num_files_processed} source file{"s"[:num_files_processed^1]},', end=' ')
        print(f'{num_files_modified} were modified and {num_files_wrong} have an invalid copyright (excluding ones on the ignore list).{TERMINAL_RESET}')
        sys.exit(1)  # sys.exit(1) to abort the commit
    # pre-commit also automatically aborts a commit if files are modified on disk
    print(f'{TERMINAL_GREEN}Successfully processed {num_files_processed} file{"s"[:num_files_processed^1]}.{TERMINAL_RESET}')


if __name__ == '__main__':
    main()
