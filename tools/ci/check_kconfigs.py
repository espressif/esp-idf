#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function, unicode_literals

import argparse
import os
import re
import sys
from io import open

from idf_ci_utils import IDF_PATH, get_submodule_dirs

# regular expression for matching Kconfig files
RE_KCONFIG = r'^Kconfig(\.projbuild)?(\.in)?$'

# ouput file with suggestions will get this suffix
OUTPUT_SUFFIX = '.new'

# ignored directories (makes sense only when run on IDF_PATH)
# Note: IGNORE_DIRS is a tuple in order to be able to use it directly with the startswith() built-in function which
# accepts tuples but no lists.
IGNORE_DIRS = (
    # Kconfigs from submodules need to be ignored:
    os.path.join(IDF_PATH, 'components', 'mqtt', 'esp-mqtt'),
    # Test Kconfigs are also ignored
    os.path.join(IDF_PATH, 'tools', 'ldgen', 'test', 'data'),
    os.path.join(IDF_PATH, 'tools', 'kconfig_new', 'test'),
)

SPACES_PER_INDENT = 4

CONFIG_NAME_MAX_LENGTH = 50

CONFIG_NAME_MIN_PREFIX_LENGTH = 3

# The checker will not fail if it encounters this string (it can be used for temporarily resolve conflicts)
RE_NOERROR = re.compile(r'\s+#\s+NOERROR\s+$')

# list or rules for lines
LINE_ERROR_RULES = [
    # (regular expression for finding,      error message,                                  correction)
    (re.compile(r'\t'),                     'tabulators should be replaced by spaces',      r' ' * SPACES_PER_INDENT),
    (re.compile(r'\s+\n'),                  'trailing whitespaces should be removed',       r'\n'),
    (re.compile(r'.{120}'),                 'line should be shorter than 120 characters',   None),
]


class InputError(RuntimeError):
    """
    Represents and error on the input
    """
    def __init__(self, path, line_number, error_msg, suggested_line):
        super(InputError, self).__init__('{}:{}: {}'.format(path, line_number, error_msg))
        self.suggested_line = suggested_line


class BaseChecker(object):
    """
    Base class for all checker objects
    """
    def __init__(self, path_in_idf):
        self.path_in_idf = path_in_idf

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        pass


class SourceChecker(BaseChecker):
    # allow to source only files which will be also checked by the script
    # Note: The rules are complex and the LineRuleChecker cannot be used
    def process_line(self, line, line_number):
        m = re.search(r'^\s*[ro]{0,2}source(\s*)"([^"]+)"', line)

        if m:
            if len(m.group(1)) == 0:
                raise InputError(self.path_in_idf, line_number, '"source" has to been followed by space',
                                 line.replace('source', 'source '))
            path = m.group(2)
            filename = os.path.basename(path)
            if path in ['$COMPONENT_KCONFIGS_SOURCE_FILE', '$COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE']:
                pass
            elif not filename.startswith('Kconfig.'):
                raise InputError(self.path_in_idf, line_number, 'only filenames starting with Kconfig.* can be sourced',
                                 line.replace(path, os.path.join(os.path.dirname(path), 'Kconfig.' + filename)))


class LineRuleChecker(BaseChecker):
    """
    checks LINE_ERROR_RULES for each line
    """
    def process_line(self, line, line_number):
        suppress_errors = RE_NOERROR.search(line) is not None
        errors = []
        for rule in LINE_ERROR_RULES:
            m = rule[0].search(line)
            if m:
                if suppress_errors:
                    # just print but no failure
                    e = InputError(self.path_in_idf, line_number, rule[1], line)
                    print(f'NOERROR: {e}')
                else:
                    errors.append(rule[1])
                if rule[2]:
                    line = rule[0].sub(rule[2], line)
        if len(errors) > 0:
            raise InputError(self.path_in_idf, line_number, '; '.join(errors), line)


class IndentAndNameChecker(BaseChecker):
    """
    checks the indentation of each line and configuration names
    """
    def __init__(self, path_in_idf, debug=False):
        super(IndentAndNameChecker, self).__init__(path_in_idf)
        self.debug = debug
        self.min_prefix_length = CONFIG_NAME_MIN_PREFIX_LENGTH

        # stack of the nested menuconfig items, e.g. ['mainmenu', 'menu', 'config']
        self.level_stack = []

        # stack common prefixes of configs
        self.prefix_stack = []

        # if the line ends with '\' then we force the indent of the next line
        self.force_next_indent = 0

        # menu items which increase the indentation of the next line
        self.re_increase_level = re.compile(r'''^\s*
                                          (
                                               (menu(?!config))
                                              |(mainmenu)
                                              |(choice)
                                              |(config)
                                              |(menuconfig)
                                              |(help)
                                              |(if)
                                              |(source)
                                              |(osource)
                                              |(rsource)
                                              |(orsource)
                                          )
                                       ''', re.X)

        # closing menu items which decrease the indentation
        self.re_decrease_level = re.compile(r'''^\s*
                                          (
                                               (endmenu)
                                              |(endchoice)
                                              |(endif)
                                          )
                                       ''', re.X)

        # matching beginning of the closing menuitems
        self.pair_dic = {'endmenu': 'menu',
                         'endchoice': 'choice',
                         'endif': 'if',
                         }

        # regex for config names
        self.re_name = re.compile(r'''^
                                       (
                                            (?:config)
                                           |(?:menuconfig)
                                           |(?:choice)

                                       )\s+
                                       (\w+)
                                      ''', re.X)

        # regex for new prefix stack
        self.re_new_stack = re.compile(r'''^
                                            (
                                                 (?:menu(?!config))
                                                |(?:mainmenu)
                                                |(?:choice)

                                            )
                                            ''', re.X)

    def __exit__(self, type, value, traceback):
        super(IndentAndNameChecker, self).__exit__(type, value, traceback)
        if len(self.prefix_stack) > 0:
            self.check_common_prefix('', 'EOF')
        if len(self.prefix_stack) != 0:
            if self.debug:
                print(self.prefix_stack)
            raise RuntimeError("Prefix stack should be empty. Perhaps a menu/choice hasn't been closed")

    def del_from_level_stack(self, count):
        """ delete count items from the end of the level_stack """
        if count > 0:
            # del self.level_stack[-0:] would delete everything and we expect not to delete anything for count=0
            del self.level_stack[-count:]

    def update_level_for_inc_pattern(self, new_item):
        if self.debug:
            print('level+', new_item, ': ', self.level_stack, end=' -> ')
        # "config" and "menuconfig" don't have a closing pair. So if new_item is an item which need to be indented
        # outside the last "config" or "menuconfig" then we need to find to a parent where it belongs
        if new_item in ['config', 'menuconfig', 'menu', 'choice', 'if', 'source', 'rsource', 'osource', 'orsource']:
            # item is not belonging to a previous "config" or "menuconfig" so need to indent to parent
            for i, item in enumerate(reversed(self.level_stack)):
                if item in ['menu', 'mainmenu', 'choice', 'if']:
                    # delete items ("config", "menuconfig", "help") until the appropriate parent
                    self.del_from_level_stack(i)
                    break
            else:
                # delete everything when configs are at top level without a parent menu, mainmenu...
                self.del_from_level_stack(len(self.level_stack))

        self.level_stack.append(new_item)
        if self.debug:
            print(self.level_stack)
        # The new indent is for the next line. Use the old one for the current line:
        return len(self.level_stack) - 1

    def update_level_for_dec_pattern(self, new_item):
        if self.debug:
            print('level-', new_item, ': ', self.level_stack, end=' -> ')
        target = self.pair_dic[new_item]
        for i, item in enumerate(reversed(self.level_stack)):
            # find the matching beginning for the closing item in reverse-order search
            # Note: "menuconfig", "config" and "help" don't have closing pairs and they are also on the stack. Now they
            # will be deleted together with the "menu" or "choice" we are closing.
            if item == target:
                i += 1  # delete also the matching beginning
                if self.debug:
                    print('delete ', i, end=' -> ')
                self.del_from_level_stack(i)
                break
        if self.debug:
            print(self.level_stack)
        return len(self.level_stack)

    def check_name_and_update_prefix(self, line, line_number):
        m = self.re_name.search(line)
        if m:
            name = m.group(2)
            name_length = len(name)

            if name_length > CONFIG_NAME_MAX_LENGTH:
                raise InputError(self.path_in_idf, line_number,
                                 '{} is {} characters long and it should be {} at most'
                                 ''.format(name, name_length, CONFIG_NAME_MAX_LENGTH),
                                 line + '\n')  # no suggested correction for this
            if len(self.prefix_stack) == 0:
                self.prefix_stack.append(name)
            elif self.prefix_stack[-1] is None:
                self.prefix_stack[-1] = name
            else:
                # this has nothing common with paths but the algorithm can be used for this also
                self.prefix_stack[-1] = os.path.commonprefix([self.prefix_stack[-1], name])
            if self.debug:
                print('prefix+', self.prefix_stack)
        m = self.re_new_stack.search(line)
        if m:
            self.prefix_stack.append(None)
            if self.debug:
                print('prefix+', self.prefix_stack)

    def check_common_prefix(self, line, line_number):
        common_prefix = self.prefix_stack.pop()
        if self.debug:
            print('prefix-', self.prefix_stack)
        if common_prefix is None:
            return
        common_prefix_len = len(common_prefix)
        if common_prefix_len < self.min_prefix_length:
            raise InputError(self.path_in_idf, line_number,
                             'The common prefix for the config names of the menu ending at this line is "{}".\n'
                             '\tAll config names in this menu should start with the same prefix of {} characters '
                             'or more.'.format(common_prefix, self.min_prefix_length),
                             line)   # no suggested correction for this
        if len(self.prefix_stack) > 0:
            parent_prefix = self.prefix_stack[-1]
            if parent_prefix is None:
                # propagate to parent level where it will influence the prefix checking with the rest which might
                # follow later on that level
                self.prefix_stack[-1] = common_prefix
            else:
                if len(self.level_stack) > 0 and self.level_stack[-1] in ['mainmenu', 'menu']:
                    # the prefix from menu is not required to propagate to the children
                    return
                if not common_prefix.startswith(parent_prefix):
                    raise InputError(self.path_in_idf, line_number,
                                     'Common prefix "{}" should start with {}'
                                     ''.format(common_prefix, parent_prefix),
                                     line)   # no suggested correction for this

    def process_line(self, line, line_number):
        stripped_line = line.strip()
        if len(stripped_line) == 0:
            self.force_next_indent = 0
            return
        # Ignore comment lines
        if stripped_line.startswith('#'):
            return
        current_level = len(self.level_stack)
        m = re.search(r'\S', line)  # indent found as the first non-space character
        if m:
            current_indent = m.start()
        else:
            current_indent = 0

        if current_level > 0 and self.level_stack[-1] == 'help':
            if current_indent >= current_level * SPACES_PER_INDENT:
                # this line belongs to 'help'
                self.force_next_indent = 0
                return

        if self.force_next_indent > 0:
            if current_indent != self.force_next_indent:
                raise InputError(self.path_in_idf, line_number,
                                 'Indentation consists of {} spaces instead of {}'.format(current_indent,
                                                                                          self.force_next_indent),
                                 (' ' * self.force_next_indent) + line.lstrip())
            else:
                if not stripped_line.endswith('\\'):
                    self.force_next_indent = 0
                return

        elif stripped_line.endswith('\\') and stripped_line.startswith(('config', 'menuconfig', 'choice')):
            raise InputError(self.path_in_idf, line_number,
                             'Line-wrap with backslash is not supported here',
                             line)  # no suggestion for this

        m = self.re_increase_level.search(line)
        if m:
            current_level = self.update_level_for_inc_pattern(m.group(1))
        else:
            m = self.re_decrease_level.search(line)
            if m:
                new_item = m.group(1)
                current_level = self.update_level_for_dec_pattern(new_item)
                if new_item not in ['endif']:
                    # endif doesn't require to check the prefix because the items inside if/endif belong to the
                    # same prefix level
                    self.check_common_prefix(line, line_number)

        # name has to be checked after increase/decrease indentation level
        # otherwise false-positive indentation error for lines bellow name is raised
        self.check_name_and_update_prefix(stripped_line, line_number)

        expected_indent = current_level * SPACES_PER_INDENT

        if stripped_line.endswith('\\'):
            self.force_next_indent = expected_indent + SPACES_PER_INDENT
        else:
            self.force_next_indent = 0

        if current_indent != expected_indent:
            raise InputError(self.path_in_idf, line_number,
                             'Indentation consists of {} spaces instead of {}'.format(current_indent, expected_indent),
                             (' ' * expected_indent) + line.lstrip())


def valid_directory(path):
    if not os.path.isdir(path):
        raise argparse.ArgumentTypeError('{} is not a valid directory!'.format(path))
    return path


def validate_kconfig_file(kconfig_full_path, verbose=False):  # type: (str, bool) -> bool
    suggestions_full_path = kconfig_full_path + OUTPUT_SUFFIX
    fail = False

    with open(kconfig_full_path, 'r', encoding='utf-8') as f, \
            open(suggestions_full_path, 'w', encoding='utf-8', newline='\n') as f_o, \
            LineRuleChecker(kconfig_full_path) as line_checker, \
            SourceChecker(kconfig_full_path) as source_checker, \
            IndentAndNameChecker(kconfig_full_path, debug=verbose) as indent_and_name_checker:
        try:
            for line_number, line in enumerate(f, start=1):
                try:
                    for checker in [
                        indent_and_name_checker,    # indent checker has to be before line checker, otherwise
                                                    # false-positive indent error if error in line_checker
                        line_checker,
                        source_checker
                    ]:
                        checker.process_line(line, line_number)
                    # The line is correct therefore we echo it to the output file
                    f_o.write(line)
                except InputError as e:
                    print(e)
                    fail = True
                    f_o.write(e.suggested_line)
        except UnicodeDecodeError:
            raise ValueError('The encoding of {} is not Unicode.'.format(kconfig_full_path))

    if fail:
        print('\t{} has been saved with suggestions for resolving the issues.\n'
              '\tPlease note that the suggestions can be wrong and '
              'you might need to re-run the checker several times '
              'for solving all issues'.format(suggestions_full_path))
        print('\tPlease fix the errors and run {} for checking the correctness of '
              'Kconfig files.'.format(os.path.abspath(__file__)))
        return False
    else:
        print('{}: OK'.format(kconfig_full_path))
        try:
            os.remove(suggestions_full_path)
        except Exception:
            # not a serious error is when the file cannot be deleted
            print('{} cannot be deleted!'.format(suggestions_full_path))
        finally:
            return True


def main():
    parser = argparse.ArgumentParser(description='Kconfig style checker')
    parser.add_argument('files', nargs='*',
                        help='Kconfig files')
    parser.add_argument('--verbose', '-v', action='store_true',
                        help='Print more information (useful for debugging)')
    parser.add_argument('--includes', '-d', nargs='*',
                        help='Extra paths for recursively searching Kconfig files. (for example $IDF_PATH)',
                        type=valid_directory)
    parser.add_argument('--exclude-submodules', action='store_true',
                        help='Exclude submodules')
    args = parser.parse_args()

    success_counter = 0
    failure_counter = 0
    ignore_counter = 0

    ignore_dirs = IGNORE_DIRS
    if args.exclude_submodules:
        ignore_dirs = ignore_dirs + tuple(get_submodule_dirs(full_path=True))

    files = [os.path.abspath(file_path) for file_path in args.files]

    if args.includes:
        for directory in args.includes:
            for root, dirnames, filenames in os.walk(directory):
                for filename in filenames:
                    full_path = os.path.join(root, filename)
                    if re.search(RE_KCONFIG, filename):
                        files.append(full_path)
                    elif re.search(RE_KCONFIG, filename, re.IGNORECASE):
                        # On Windows Kconfig files are working with different cases!
                        print('{}: Incorrect filename. The case should be "Kconfig"!'.format(full_path))
                        failure_counter += 1

    for full_path in files:
        if full_path.startswith(ignore_dirs):
            print('{}: Ignored'.format(full_path))
            ignore_counter += 1
            continue
        is_valid = validate_kconfig_file(full_path, args.verbose)
        if is_valid:
            success_counter += 1
        else:
            failure_counter += 1

    if ignore_counter > 0:
        print('{} files have been ignored.'.format(ignore_counter))
    if success_counter > 0:
        print('{} files have been successfully checked.'.format(success_counter))
    if failure_counter > 0:
        print('{} files have errors. Please take a look at the log.'.format(failure_counter))
        return 1

    if not files:
        print('WARNING: no files specified. Please specify files or use '
              '"--includes" to search Kconfig files recursively')
    return 0


if __name__ == '__main__':
    sys.exit(main())
