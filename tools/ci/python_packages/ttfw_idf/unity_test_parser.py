# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Modification version of https://github.com/ETCLabs/unity-test-parser/blob/develop/unity_test_parser.py
since only python 3.6 or higher version have ``enum.auto()``

unity_test_parser.py

Parse the output of the Unity Test Framework for C. Parsed results are held in the TestResults
object format, which can then be converted to various XML formats.
"""

import enum
import re

import junit_xml

_NORMAL_TEST_REGEX = re.compile(r'(?P<file>.+):(?P<line>\d+):(?P<test_name>[^\s:]+):(?P<result>PASS|FAIL|IGNORE)(?:: (?P<message>.+))?')
_UNITY_FIXTURE_VERBOSE_PREFIX_REGEX = re.compile(r'(?P<prefix>TEST\((?P<test_group>[^\s,]+), (?P<test_name>[^\s\)]+)\))(?P<remainder>.+)?$')
_UNITY_FIXTURE_REMAINDER_REGEX = re.compile(r'^(?P<file>.+):(?P<line>\d+)::(?P<result>PASS|FAIL|IGNORE)(?:: (?P<message>.+))?')
_TEST_SUMMARY_BLOCK_REGEX = re.compile(
    r'^(?P<num_tests>\d+) Tests (?P<num_failures>\d+) Failures (?P<num_ignored>\d+) Ignored\s*\r?\n(?P<overall_result>OK|FAIL)(?:ED)?', re.MULTILINE
)
_TEST_RESULT_ENUM = ['PASS', 'FAIL', 'IGNORE']


class TestFormat(enum.Enum):
    """Represents the flavor of Unity used to produce a given output."""

    UNITY_BASIC = 0
    # UNITY_FIXTURE = enum.auto()
    UNITY_FIXTURE_VERBOSE = 1


globals().update(TestFormat.__members__)


class TestStats:
    """Statistics about a test collection"""

    def __init__(self):
        self.total = 0
        self.passed = 0
        self.failed = 0
        self.ignored = 0

    def __eq__(self, other):
        if isinstance(other, self.__class__):
            return (self.total == other.total
                    and self.passed == other.passed
                    and self.failed == other.failed
                    and self.ignored == other.ignored)
        return False


class TestResult:
    """
    Class representing the result of a single test.

    Contains the test name, its result (either PASS, FAIL or IGNORE), the file and line number if
    the test result was not PASS, and an optional message.
    """

    def __init__(
            self,
            test_name,
            result,
            group='default',
            file='',
            line=0,
            message='',
            full_line='',
    ):
        if result not in _TEST_RESULT_ENUM:
            raise ValueError('result must be one of {}.'.format(_TEST_RESULT_ENUM))

        self._test_name = test_name
        self._result = result
        self._group = group
        self._message = message
        self._full_line = full_line

        if result != 'PASS':
            self._file = file
            self._line = line
        else:
            self._file = ''
            self._line = 0

    def file(self):
        """The file name - returns empty string if the result is PASS."""
        return self._file

    def line(self):
        """The line number - returns 0 if the result is PASS."""
        return self._line

    def name(self):
        """The test name."""
        return self._test_name

    def result(self):
        """The test result, one of PASS, FAIL or IGNORED."""
        return self._result

    def group(self):
        """
        The test group, if applicable.

        For basic Unity output, this will always be "default".
        """
        return self._group

    def message(self):
        """The accompanying message - returns empty string if the result is PASS."""
        return self._message

    def full_line(self):
        """The original, full line of unit test output that this object was created from."""
        return self._full_line


class TestResults:
    """
    Class representing Unity test results.

    After being initialized with raw test output, it parses the output and represents it as a list
    of TestResult objects which can be inspected or converted to other types of output, e.g. JUnit
    XML.
    """

    def __init__(self, test_output, test_format=TestFormat.UNITY_BASIC):
        """
        Create a new TestResults object from Unity test output.

        Keyword arguments:
        test_output -- The full test console output, must contain the overall result and summary
                       block at the bottom.

        Optional arguments:
        test_format -- TestFormat enum representing the flavor of Unity used to create the output.

        Exceptions:
        ValueError, if the test output is not formatted properly.
        """
        self._tests = []
        self._test_stats = self._find_summary_block(test_output)

        if test_format is TestFormat.UNITY_BASIC:
            self._parse_unity_basic(test_output)
        elif test_format is TestFormat.UNITY_FIXTURE_VERBOSE:
            self._parse_unity_fixture_verbose(test_output)
        else:
            raise ValueError(
                'test_format must be one of UNITY_BASIC or UNITY_FIXTURE_VERBOSE.'
            )

    def num_tests(self):
        """The total number of tests parsed."""
        return self._test_stats.total

    def num_passed(self):
        """The number of tests with result PASS."""
        return self._test_stats.passed

    def num_failed(self):
        """The number of tests with result FAIL."""
        return self._test_stats.failed

    def num_ignored(self):
        """The number of tests with result IGNORE."""
        return self._test_stats.ignored

    def test_iter(self):
        """Get an iterator for iterating over individual tests.

        Returns an iterator over TestResult objects.

        Example:
        for test in unity_results.test_iter():
            print(test.name())
        """
        return iter(self._tests)

    def tests(self):
        """Get a list of all the tests (TestResult objects)."""
        return self._tests

    def to_junit(
            self, suite_name='all_tests',
    ):
        """
        Convert the tests to JUnit XML.

        Returns a junit_xml.TestSuite containing all of the test cases. One test suite will be
        generated with the name given in suite_name. Unity Fixture test groups are mapped to the
        classname attribute of test cases; for basic Unity output there will be one class named
        "default".

        Optional arguments:
        suite_name -- The name to use for the "name" and "package" attributes of the testsuite element.

        Sample output:
        <testsuite disabled="0" errors="0" failures="1" name="[suite_name]" package="[suite_name]" skipped="0" tests="8" time="0">
            <testcase classname="test_group_1" name="group_1_test" />
            <testcase classname="test_group_2" name="group_2_test" />
        </testsuite>
        """
        test_case_list = []

        for test in self._tests:
            if test.result() == 'PASS':
                test_case_list.append(
                    junit_xml.TestCase(name=test.name(), classname=test.group())
                )
            else:
                junit_tc = junit_xml.TestCase(
                    name=test.name(),
                    classname=test.group(),
                    file=test.file(),
                    line=test.line(),
                )
                if test.result() == 'FAIL':
                    junit_tc.add_failure_info(
                        message=test.message(), output=test.full_line()
                    )
                elif test.result() == 'IGNORE':
                    junit_tc.add_skipped_info(
                        message=test.message(), output=test.full_line()
                    )
                test_case_list.append(junit_tc)

        return junit_xml.TestSuite(
            name=suite_name, package=suite_name, test_cases=test_case_list
        )

    def _find_summary_block(self, unity_output):
        """
        Find and parse the test summary block.

        Unity prints a test summary block at the end of a test run of the form:
        -----------------------
        X Tests Y Failures Z Ignored
        [PASS|FAIL]

        Returns the contents of the test summary block as a TestStats object.
        """
        match = _TEST_SUMMARY_BLOCK_REGEX.search(unity_output)
        if not match:
            raise ValueError('A Unity test summary block was not found.')

        try:
            stats = TestStats()
            stats.total = int(match.group('num_tests'))
            stats.failed = int(match.group('num_failures'))
            stats.ignored = int(match.group('num_ignored'))
            stats.passed = stats.total - stats.failed - stats.ignored
            return stats
        except ValueError:
            raise ValueError('The Unity test summary block was not valid.')

    def _parse_unity_basic(self, unity_output):
        """
        Parse basic unity output.

        This is of the form file:line:test_name:result[:optional_message]
        """
        found_test_stats = TestStats()

        for test in _NORMAL_TEST_REGEX.finditer(unity_output):
            try:
                new_test = TestResult(
                    test.group('test_name'),
                    test.group('result'),
                    file=test.group('file'),
                    line=int(test.group('line')),
                    message=test.group('message')
                    if test.group('message') is not None
                    else '',
                    full_line=test.group(0),
                )
            except ValueError:
                continue

            self._add_new_test(new_test, found_test_stats)

        if len(self._tests) == 0:
            raise ValueError('No tests were found.')

        if found_test_stats != self._test_stats:
            raise ValueError('Test output does not match summary block.')

    def _parse_unity_fixture_verbose(self, unity_output):
        """
        Parse the output of the unity_fixture add-in invoked with the -v flag.

        This is a more complex operation than basic unity output, because the output for a single
        test can span multiple lines. There is a prefix of the form "TEST(test_group, test_name)"
        that always exists on the first line for a given test. Immediately following that can be a
        pass or fail message, or some number of diagnostic messages followed by a pass or fail
        message.
        """
        found_test_stats = TestStats()

        line_iter = iter(unity_output.splitlines())
        try:
            line = next(line_iter)
            while True:
                prefix_match = _UNITY_FIXTURE_VERBOSE_PREFIX_REGEX.search(line)
                line = next(line_iter)
                if prefix_match:
                    # Handle the remaining portion of a test case line after the unity_fixture
                    # prefix.
                    remainder = prefix_match.group('remainder')
                    if remainder:
                        self._parse_unity_fixture_remainder(
                            prefix_match, remainder, found_test_stats
                        )
                    # Handle any subsequent lines with more information on the same test case.
                    while not _UNITY_FIXTURE_VERBOSE_PREFIX_REGEX.search(line):
                        self._parse_unity_fixture_remainder(
                            prefix_match, line, found_test_stats
                        )
                        line = next(line_iter)
        except StopIteration:
            pass

        if len(self._tests) == 0:
            raise ValueError('No tests were found.')

        if found_test_stats != self._test_stats:
            raise ValueError('Test output does not match summary block.')

    def _parse_unity_fixture_remainder(self, prefix_match, remainder, test_stats):
        """
        Parse the remainder of a Unity Fixture test case.

        Can be on the same line as the prefix or on subsequent lines.
        """
        new_test = None

        if remainder == ' PASS':
            new_test = TestResult(
                prefix_match.group('test_name'),
                'PASS',
                group=prefix_match.group('test_group'),
                full_line=prefix_match.group(0),
            )
        else:
            remainder_match = _UNITY_FIXTURE_REMAINDER_REGEX.match(remainder)
            if remainder_match:
                new_test = TestResult(
                    prefix_match.group('test_name'),
                    remainder_match.group('result'),
                    group=prefix_match.group('test_group'),
                    file=remainder_match.group('file'),
                    line=int(remainder_match.group('line')),
                    message=remainder_match.group('message')
                    if remainder_match.group('message') is not None
                    else '',
                    full_line=prefix_match.group('prefix') + remainder_match.group(0),
                )

        if new_test is not None:
            self._add_new_test(new_test, test_stats)

    def _add_new_test(self, new_test, test_stats):
        """Add a new test and increment the proper members of test_stats."""
        test_stats.total += 1
        if new_test.result() == 'PASS':
            test_stats.passed += 1
        elif new_test.result() == 'FAIL':
            test_stats.failed += 1
        else:
            test_stats.ignored += 1

        self._tests.append(new_test)
