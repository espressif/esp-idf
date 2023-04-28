#!/usr/bin/env python
# coding=utf-8
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#
# This script converts space-separated EXTRA_COMPONENT_DIRS and COMPONENT_DIRS
# CMake variables into semicolon-separated lists.
#
# IDF versions <=v4.4 didn't support spaces in paths to ESP-IDF or projects.
# Therefore it was okay to use spaces as separators in EXTRA_COMPONENT_DIRS,
# same as it was done in the legacy GNU Make based build system.
# CMake build system used 'spaces2list' function to convert space-separated
# variables into semicolon-separated lists, replacing every space with a
# semicolon.
#
# In IDF 5.0 and later, spaces in project path and ESP-IDF path are supported.
# This means that EXTRA_COMPONENT_DIRS and COMPONENT_DIRS variables now should
# be semicolon-separated CMake lists.
#
# To provide compatibility with the projects written for older ESP-IDF versions,
# this script attempts to convert these space-separated variables into semicolon-
# separated ones. Note that in general this cannot be done unambiguously, so this
# script will still report an error if there are multiple ways to interpret the
# variable, and ask the user to fix the project CMakeLists.txt file.
#


import argparse
import os
import pprint
import sys
import textwrap
import typing
import unittest


class PathSplitError(RuntimeError):
    pass


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('--var-name', required=True, help='Name of CMake variable, for printing errors and warnings')
    parser.add_argument('in_variable', help='Input variable, may contain a mix of spaces and semicolons as separators')
    args = parser.parse_args()

    # Initially split the paths by semicolons
    semicolon_separated_parts = args.in_variable.split(';')

    # Every resulting part may contain space separators. Handle each part:
    paths = []
    ctx = dict(warnings=False)
    errors = False
    for part in semicolon_separated_parts:
        def warning_cb(warning_str: str) -> None:
            print('\n  '.join(
                textwrap.wrap('Warning: in CMake variable {}: {}'.format(args.var_name, warning_str), width=120,
                              break_on_hyphens=False)), file=sys.stderr)
            ctx['warnings'] = True

        try:
            paths += split_paths_by_spaces(part, warning_cb=warning_cb)
        except PathSplitError as e:
            print('\n  '.join(textwrap.wrap('Error: in CMake variable {}: {}'.format(args.var_name, str(e)), width=120,
                                            break_on_hyphens=False)), file=sys.stderr)
            errors = True

    if errors or ctx['warnings']:
        print(textwrap.dedent("""
            Note: In ESP-IDF v5.0 and later, COMPONENT_DIRS and EXTRA_COMPONENT_DIRS should be defined
                  as CMake lists, not as space separated strings.

            Examples:
                * set(EXTRA_COMPONENT_DIRS path/to/components path/to/more/components)
                  # Correct, EXTRA_COMPONENT_DIRS is defined as a CMake list, with two paths added

                * list(APPEND EXTRA_COMPONENT_DIRS path/to/component)
                  list(APPEND EXTRA_COMPONENT_DIRS path/to/more/components)
                  # Correct, use when building EXTRA_COMPONENT_DIRS incrementally

                * set(EXTRA_COMPONENT_DIRS path/to/components "another/path with space/components")
                  # Literal path with spaces has to be quoted

                * set(EXTRA_COMPONENT_DIRS $ENV{MY_PATH}/components dir/more_components)
                  # Correct, even if MY_PATH contains spaces

                * set(EXTRA_COMPONENT_DIRS ${ROOT}/component1 ${ROOT}/component2 ${ROOT}/component3)
                  # Correct, even if ROOT contains spaces

            Avoid string concatenation!
                set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} component1")
                set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} component2")
                # Incorrect. String "component1 component2" may indicate a single directory
                # name with a space, or two directory names separated by space.

            Instead use:
                list(APPEND component1)
                list(APPEND component2)

            Defining COMPONENT_DIRS and EXTRA_COMPONENT_DIRS as CMake lists is backwards compatible
            with ESP-IDF 4.4 and below.

            (If you think these variables are defined correctly in your project and this message
            is not relevant, please report this as an issue.)
        """), file=sys.stderr)

        print('Diagnostic info: {} was invoked in {} with arguments: {}'.format(
            sys.argv[0], os.getcwd(), sys.argv[1:]
        ), file=sys.stderr)

    if errors:
        raise SystemExit(1)

    sys.stdout.write(';'.join(paths))
    sys.stdout.flush()


def split_paths_by_spaces(src: str, path_exists_cb: typing.Callable[[str], bool] = os.path.exists,
                          warning_cb: typing.Optional[typing.Callable[[str], None]] = None) -> typing.List[str]:
    if ' ' not in src:
        # no spaces, complete string should be the path
        return [src]

    def path_exists_or_empty(path: str) -> bool:
        return path == '' or path_exists_cb(path)

    # remove leading and trailing spaces
    delayed_warnings = []
    trimmed = src.lstrip(' ')
    if trimmed != src:
        delayed_warnings.append("Path component '{}' contains leading spaces".format(src))
    src = trimmed

    trimmed = src.rstrip(' ')
    if trimmed != src:
        delayed_warnings.append("Path component '{}' contains trailing spaces".format(src))
    src = trimmed

    # Enumerate all possible ways to split the string src into paths by spaces.
    # The number of these ways is equal to sum(C(n, k), 0<=k<n) == 2^n
    #   (where n is the number of spaces, k is the number of splits, C(n, k) are binomial coefficients)
    #
    # We do this by associating every space with a bit of an integer in the range [0, 2^n - 1],
    # such that when the bit is 0 there is no split in the given space, and bit is 1 when there is a split.

    parts = src.split(' ')
    num_spaces = len(parts) - 1
    valid_ways_to_split = []
    all_ways_to_split = [selective_join(parts, i) for i in range(2 ** num_spaces)]
    for paths_list in all_ways_to_split:
        nonempty_paths = list(filter(bool, paths_list))
        if all(map(path_exists_or_empty, nonempty_paths)):
            valid_ways_to_split.append(nonempty_paths)

    num_candidates = len(valid_ways_to_split)
    if num_candidates == 1:
        # Success, found only one correct way to split.
        result = valid_ways_to_split[0]

        # Report warnings
        if warning_cb:
            if len(result) > 1:
                warning_cb("Path component '{}' contains a space separator. It was automatically split into {}".format(
                    src, pprint.pformat(result)
                ))
            for w in delayed_warnings:
                warning_cb(w)

        return result

    if num_candidates == 0:
        raise PathSplitError(("Didn't find a valid way to split path '{}'. "
                              'This error may be reported if one or more paths '
                              "are separated with spaces, and at least one path doesn't exist.").format(src))

    # if num_candidates > 1
    raise PathSplitError("Found more than one valid way to split path '{}':{}".format(
        src, ''.join('\n\t- ' + pprint.pformat(p) for p in valid_ways_to_split)
    ))


def selective_join(parts: typing.List[str], n: int) -> typing.List[str]:
    """
    Given the list of N+1 strings, and an integer n in [0, 2**N - 1] range,
    concatenate i-th and (i+1)-th string with space inbetween if bit i is not set in n.
    Examples:
         selective_join(['a', 'b', 'c'], 0b00) == ['a b c']
         selective_join(['a', 'b', 'c'], 0b01) == ['a', 'b c']
         selective_join(['a', 'b', 'c'], 0b10) == ['a b', 'c']
         selective_join(['a', 'b', 'c'], 0b11) == ['a', 'b', 'c']

    This function is used as part of finding all the ways to split a string by spaces.

    :param parts: Strings to join
    :param n: Integer (bit map) to set the positions to join
    :return: resulting list of strings
    """
    result = []
    concatenated = [parts[0]]
    for part in parts[1:]:
        if n & 1:
            result.append(' '.join(concatenated))
            concatenated = [part]
        else:
            concatenated.append(part)
        n >>= 1
    if concatenated:
        result.append(' '.join(concatenated))
    return result


class HelperTests(unittest.TestCase):
    def test_selective_join(self) -> None:
        self.assertListEqual(['a b c'], selective_join(['a', 'b', 'c'], 0b00))
        self.assertListEqual(['a', 'b c'], selective_join(['a', 'b', 'c'], 0b01))
        self.assertListEqual(['a b', 'c'], selective_join(['a', 'b', 'c'], 0b10))
        self.assertListEqual(['a', 'b', 'c'], selective_join(['a', 'b', 'c'], 0b11))


class SplitTests(unittest.TestCase):
    def test_split_paths_absolute(self) -> None:
        self.check_paths_concatenated('/absolute/path/one', '/absolute/path/two')

    def test_split_paths_absolute_spaces(self) -> None:
        self.check_paths_concatenated('/absolute/path with spaces')
        self.check_paths_concatenated('/absolute/path  with   more   spaces')
        self.check_paths_concatenated('/absolute/path with spaces/one', '/absolute/path with spaces/two')

        self.check_paths_concatenated('/absolute/path with spaces/one',
                                      '/absolute/path with spaces/two',
                                      '/absolute/path with spaces/three')

    def test_split_paths_absolute_relative(self) -> None:
        self.check_paths_concatenated('/absolute/path/one', 'two')

    def test_split_paths_relative(self) -> None:
        self.check_paths_concatenated('one', 'two')

    def test_split_paths_absolute_spaces_relative(self) -> None:
        self.check_paths_concatenated('/absolute/path with spaces/one', 'two')

    def test_split_paths_ambiguous(self) -> None:
        self.check_paths_concatenated_ambiguous('/absolute/path one', 'two',
                                                additional_paths_exist=['/absolute/path', 'one'])

        self.check_paths_concatenated_ambiguous('/path    ', '/path',
                                                additional_paths_exist=['/path    /path'])

    def test_split_paths_nonexistent(self) -> None:
        self.check_paths_concatenated_nonexistent('one', 'two')

    def test_split_paths_extra_whitespace(self) -> None:
        paths = ['/path']
        path_exists = self.path_exists_by_list(paths)
        self.assertListEqual(paths, split_paths_by_spaces(' /path', path_exists_cb=path_exists))
        self.assertListEqual(paths, split_paths_by_spaces('/path ', path_exists_cb=path_exists))
        self.assertListEqual(paths + paths, split_paths_by_spaces('/path   /path', path_exists_cb=path_exists))

    def test_split_paths_warnings(self) -> None:
        paths = ['/path']
        ctx = {'warnings': []}  # type: typing.Dict[str, typing.List[str]]

        def add_warning(warning: str) -> None:
            ctx['warnings'].append(warning)

        path_exists = self.path_exists_by_list(paths)

        self.assertListEqual(paths,
                             split_paths_by_spaces(' /path', path_exists_cb=path_exists, warning_cb=add_warning))
        self.assertEqual(1, len(ctx['warnings']))
        self.assertIn('leading', ctx['warnings'][0])

        ctx['warnings'] = []
        self.assertListEqual(paths,
                             split_paths_by_spaces('/path ', path_exists_cb=path_exists, warning_cb=add_warning))
        self.assertEqual(1, len(ctx['warnings']))
        self.assertIn('trailing', ctx['warnings'][0])

        ctx['warnings'] = []
        self.assertListEqual(paths + paths,
                             split_paths_by_spaces('/path /path', path_exists_cb=path_exists, warning_cb=add_warning))
        self.assertEqual(1, len(ctx['warnings']))
        self.assertIn('contains a space separator', ctx['warnings'][0])

    @staticmethod
    def path_exists_by_list(paths_which_exist: typing.List[str]) -> typing.Callable[[str], bool]:
        """
        Returns a function to check whether a path exists, similar to os.path.exists, but instead of checking
        for files on the real filesystem it considers only the paths provided in 'paths_which_exist' argument.
        :param paths_which_exist: list of paths which should be considered as existing
        :return: function to check if path exists
        """
        all_paths = set()
        for path in paths_which_exist or []:
            # for path /a/b/c, add it and also add components of the path: /a, /a/b
            end = len(path)
            while end > 0:
                all_paths.add(path[0:end])
                end = path.rfind('/', 0, end)

        def path_exists(path: str) -> bool:
            return path in all_paths

        return path_exists

    def split_paths_concatenated_base(self, paths_to_concatentate: typing.List[str],
                                      paths_existing: typing.List[str]) -> typing.List[str]:
        concatenated = ' '.join(paths_to_concatentate)
        path_exists = self.path_exists_by_list(paths_existing)
        return split_paths_by_spaces(concatenated, path_exists_cb=path_exists)

    def check_paths_concatenated(self, *args: str) -> None:
        paths = [*args]
        paths_split = self.split_paths_concatenated_base(paths_to_concatentate=paths, paths_existing=paths)
        self.assertListEqual(paths, paths_split)

    def check_paths_concatenated_ambiguous(self, *args: str,
                                           additional_paths_exist: typing.Optional[typing.List[str]] = None) -> None:
        paths = [*args]
        self.assertRaises(PathSplitError, self.split_paths_concatenated_base, paths_to_concatentate=paths,
                          paths_existing=paths + (additional_paths_exist or []))

    def check_paths_concatenated_nonexistent(self, *args: str,
                                             additional_paths_exist: typing.List[str] = None) -> None:
        paths = [*args]
        self.assertRaises(PathSplitError, self.split_paths_concatenated_base, paths_to_concatentate=paths,
                          paths_existing=additional_paths_exist)


if __name__ == '__main__':
    main()
