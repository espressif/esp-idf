#!/usr/bin/env python
#
# Copyright 2019 Espressif Systems (Shanghai) CO LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import subprocess
import sys
from unittest import TestCase, main

try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

try:
    import idf
except ImportError:
    sys.path.append('..')
    import idf

current_dir = os.path.dirname(os.path.realpath(__file__))
idf_py_path = os.path.join(current_dir, '..', 'idf.py')
extension_path = os.path.join(current_dir, 'test_idf_extensions', 'test_ext')
link_path = os.path.join(current_dir, '..', 'idf_py_actions', 'test_ext')


class TestWithoutExtensions(TestCase):
    def setUp(self):
        self.initial_env = dict(os.environ)
        os.environ['IDF_COMPONENT_MANAGER'] = '0'
        os.environ['IDF_EXTRA_ACTIONS_PATH'] = ''

    def tearDown(self):
        os.environ = self.initial_env


class TestExtensions(TestWithoutExtensions):
    def test_extension_loading(self):
        try:
            os.symlink(extension_path, link_path)
            os.environ["IDF_EXTRA_ACTIONS_PATH"] = os.path.join(current_dir, 'extra_path')
            output = subprocess.check_output([sys.executable, idf_py_path, "--help"],
                                             env=os.environ).decode('utf-8', 'ignore')

            self.assertIn('--test-extension-option', output)
            self.assertIn('test_subcommand', output)
            self.assertIn('--some-extension-option', output)
            self.assertIn('extra_subcommand', output)
        finally:
            os.remove(link_path)

    def test_extension_execution(self):
        try:
            os.symlink(extension_path, link_path)
            os.environ["IDF_EXTRA_ACTIONS_PATH"] = ";".join([os.path.join(current_dir, 'extra_path')])
            output = subprocess.check_output(
                [sys.executable, idf_py_path, "--some-extension-option=awesome", 'test_subcommand', "extra_subcommand"],
                env=os.environ).decode('utf-8', 'ignore')
            self.assertIn('!!! From some global callback: awesome', output)
            self.assertIn('!!! From some subcommand', output)
            self.assertIn('!!! From test global callback: test', output)
            self.assertIn('!!! From some subcommand', output)
        finally:
            os.remove(link_path)

    def test_hidden_commands(self):
        try:
            os.symlink(extension_path, link_path)
            os.environ["IDF_EXTRA_ACTIONS_PATH"] = ";".join([os.path.join(current_dir, 'extra_path')])
            output = subprocess.check_output([sys.executable, idf_py_path, "--help"],
                                             env=os.environ).decode('utf-8', 'ignore')
            self.assertIn('test_subcommand', output)
            self.assertNotIn('hidden_one', output)

        finally:
            os.remove(link_path)


class TestDependencyManagement(TestWithoutExtensions):
    def test_dependencies(self):
        result = idf.init_cli()(
            args=['--dry-run', 'flash'],
            standalone_mode=False,
        )
        self.assertEqual(['flash'], list(result.keys()))

    def test_order_only_dependencies(self):
        result = idf.init_cli()(
            args=['--dry-run', 'build', 'fullclean', 'all'],
            standalone_mode=False,
        )
        self.assertEqual(['fullclean', 'all'], list(result.keys()))

    def test_repeated_dependencies(self):
        result = idf.init_cli()(
            args=['--dry-run', 'fullclean', 'app', 'fullclean', 'fullclean'],
            standalone_mode=False,
        )
        self.assertEqual(['fullclean', 'app'], list(result.keys()))

    def test_complex_case(self):
        result = idf.init_cli()(
            args=['--dry-run', 'clean', 'monitor', 'clean', 'fullclean', 'flash'],
            standalone_mode=False,
        )
        self.assertEqual(['fullclean', 'clean', 'flash', 'monitor'], list(result.keys()))

    def test_dupplicated_commands_warning(self):
        capturedOutput = StringIO()
        sys.stdout = capturedOutput
        idf.init_cli()(
            args=['--dry-run', 'clean', 'monitor', 'build', 'clean', 'fullclean', 'all'],
            standalone_mode=False,
        )
        sys.stdout = sys.__stdout__
        self.assertIn(
            'WARNING: Commands "all", "clean" are found in the list of commands more than once.',
            capturedOutput.getvalue())

        sys.stdout = capturedOutput
        idf.init_cli()(
            args=['--dry-run', 'clean', 'clean'],
            standalone_mode=False,
        )
        sys.stdout = sys.__stdout__
        self.assertIn(
            'WARNING: Command "clean" is found in the list of commands more than once.', capturedOutput.getvalue())


class TestVerboseFlag(TestWithoutExtensions):
    def test_verbose_messages(self):
        output = subprocess.check_output(
            [
                sys.executable,
                idf_py_path,
                "-C%s" % current_dir,
                "-v",
                "test-verbose",
            ], env=os.environ).decode('utf-8', 'ignore')

        self.assertIn('Verbose mode on', output)

    def test_verbose_messages_not_shown_by_default(self):
        output = subprocess.check_output(
            [
                sys.executable,
                idf_py_path,
                "-C%s" % current_dir,
                "test-verbose",
            ], env=os.environ).decode('utf-8', 'ignore')

        self.assertIn('Output from test-verbose', output)
        self.assertNotIn('Verbose mode on', output)


class TestGlobalAndSubcommandParameters(TestWithoutExtensions):
    def test_set_twice_same_value(self):
        """Can set -D twice: globally and for subcommand if values are the same"""

        idf.init_cli()(
            args=['--dry-run', '-DAAA=BBB', '-DCCC=EEE', 'build', '-DAAA=BBB', '-DCCC=EEE'],
            standalone_mode=False,
        )

    def test_set_twice_different_values(self):
        """Cannot set -D twice: for command and subcommand of idf.py (with different values)"""

        with self.assertRaises(idf.FatalError):
            idf.init_cli()(
                args=['--dry-run', '-DAAA=BBB', 'build', '-DAAA=EEE', '-DCCC=EEE'],
                standalone_mode=False,
            )


class TestDeprecations(TestWithoutExtensions):
    def test_exit_with_error_for_subcommand(self):
        try:
            subprocess.check_output([sys.executable, idf_py_path, "-C%s" % current_dir, "test-2"], env=os.environ)
        except subprocess.CalledProcessError as e:
            self.assertIn('Error: Command "test-2" is deprecated and was removed.', e.output.decode('utf-8', 'ignore'))

    def test_exit_with_error_for_option(self):
        try:
            subprocess.check_output(
                [sys.executable, idf_py_path, "-C%s" % current_dir, "--test-5=asdf"], env=os.environ)
        except subprocess.CalledProcessError as e:
            self.assertIn(
                'Error: Option "test_5" is deprecated since v2.0 and was removed in v3.0.',
                e.output.decode('utf-8', 'ignore'))

    def test_deprecation_messages(self):
        output = subprocess.check_output(
            [
                sys.executable,
                idf_py_path,
                "-C%s" % current_dir,
                "--test-0=a",
                "--test-1=b",
                "--test-2=c",
                "--test-3=d",
                "test-0",
                "--test-sub-0=sa",
                "--test-sub-1=sb",
                "ta",
                "test-1",
            ],
            env=os.environ).decode('utf-8', 'ignore')

        self.assertIn('Warning: Option "test_sub_1" is deprecated and will be removed in future versions.', output)
        self.assertIn(
            'Warning: Command "test-1" is deprecated and will be removed in future versions. '
            'Please use alternative command.', output)
        self.assertIn('Warning: Option "test_1" is deprecated and will be removed in future versions.', output)
        self.assertIn(
            'Warning: Option "test_2" is deprecated and will be removed in future versions. '
            'Please update your parameters.', output)
        self.assertIn('Warning: Option "test_3" is deprecated and will be removed in future versions.', output)
        self.assertNotIn('"test-0" is deprecated', output)
        self.assertNotIn('"test_0" is deprecated', output)


if __name__ == '__main__':
    main()
