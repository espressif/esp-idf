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

import sys
import unittest

try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

try:
    import idf
except ImportError:
    sys.path.append('..')
    import idf


class TestDependencyManagement(unittest.TestCase):
    def test_dependencies(self):
        result = idf.init_cli()(
            args=['--no-run', 'flash'],
            standalone_mode=False,
        )
        self.assertEqual(['all', 'flash'], list(result.keys()))

    def test_order_only_dependencies(self):
        result = idf.init_cli()(
            args=['--no-run', 'build', 'fullclean', 'all'],
            standalone_mode=False,
        )
        self.assertEqual(['fullclean', 'all'], list(result.keys()))

    def test_repeated_dependencies(self):
        result = idf.init_cli()(
            args=['--no-run', 'fullclean', 'app', 'fullclean', 'fullclean'],
            standalone_mode=False,
        )
        self.assertEqual(['fullclean', 'app'], list(result.keys()))

    def test_complex_case(self):
        result = idf.init_cli()(
            args=['--no-run', 'clean', 'monitor', 'clean', 'fullclean', 'flash'],
            standalone_mode=False,
        )
        self.assertEqual(['fullclean', 'clean', 'all', 'flash', 'monitor'], list(result.keys()))

    def test_dupplicated_commands_warning(self):
        capturedOutput = StringIO()
        sys.stdout = capturedOutput
        idf.init_cli()(
            args=['--no-run', 'clean', 'monitor', 'build', 'clean', 'fullclean', 'all'],
            standalone_mode=False,
        )
        sys.stdout = sys.__stdout__
        self.assertIn('WARNING: Commands "all", "clean" are found in the list of commands more than once.',
                      capturedOutput.getvalue())

        sys.stdout = capturedOutput
        idf.init_cli()(
            args=['--no-run', 'clean', 'clean'],
            standalone_mode=False,
        )
        sys.stdout = sys.__stdout__
        self.assertIn('WARNING: Command "clean" is found in the list of commands more than once.',
                      capturedOutput.getvalue())


class TestGlobalAndSubcommandParameters(unittest.TestCase):
    def test_set_twice_same_value(self):
        """Can set -D twice: globally and for subcommand if values are the same"""

        idf.init_cli()(
            args=['--no-run', '-DAAA=BBB', '-DCCC=EEE', 'build', '-DAAA=BBB', '-DCCC=EEE'],
            standalone_mode=False,
        )

    def test_set_twice_different_values(self):
        """Cannot set -D twice: for command and subcommand of idf.py (with different values)"""

        with self.assertRaises(idf.FatalError):
            idf.init_cli()(
                args=['--no-run', '-DAAA=BBB', 'build', '-DAAA=EEE', '-DCCC=EEE'],
                standalone_mode=False,
            )


if __name__ == '__main__':
    unittest.main()
