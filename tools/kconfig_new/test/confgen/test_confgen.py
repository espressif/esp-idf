#!/usr/bin/env python

import os
import re
import subprocess
import sys
import tempfile
import textwrap
import unittest

from future.utils import iteritems


class ConfgenBaseTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.args = dict()
        self.functions = {'in': self.assertIn,
                          'not in': self.assertNotIn,
                          'equal': self.assertEqual,
                          'not equal': self.assertNotEqual}

        try:
            regex_func = self.assertRegex
        except AttributeError:
            # Python 2 fallback
            regex_func = self.assertRegexpMatches
        finally:
            self.functions['regex'] = lambda instance, s, expr: regex_func(instance, expr, s)  # reverse args order

    def setUp(self):
        with tempfile.NamedTemporaryFile(prefix='test_confgen_', delete=False) as f:
            self.output_file = f.name
            self.addCleanup(os.remove, self.output_file)

    def invoke_confgen(self, args):
        call_args = [sys.executable, '../../confgen.py']

        for (k, v) in iteritems(args):
            if k != 'output':
                if isinstance(v, type('')):  # easy Python 2/3 compatible str/unicode
                    call_args += ['--{}'.format(k), v]
                else:
                    for i in v:
                        call_args += ['--{}'.format(k), i]
        call_args += ['--output', args['output'], self.output_file]  # these arguments belong together

        subprocess.check_call(call_args)

    def invoke_and_test(self, in_text, out_text, test='in'):
        """
        Main utility function for testing confgen:

        - Runs confgen via invoke_confgen(), using output method pre-set in test class setup
        - in_text is the Kconfig file input content
        - out_text is some expected output from confgen
        - 'test' can be any function key from self.functions dict (see above). Default is 'in' to test if
          out_text is a substring of the full confgen output.
        """

        with tempfile.NamedTemporaryFile(mode='w+', prefix='test_confgen_', delete=False) as f:
            self.addCleanup(os.remove, f.name)
            f.write(textwrap.dedent(in_text))

        self.args['kconfig'] = f.name

        self.invoke_confgen(self.args)

        with open(self.output_file) as f_result:
            result = f_result.read()

        try:
            out_text = textwrap.dedent(out_text)
        except TypeError:
            pass  # probably a regex

        self.functions[test](self, out_text, result)


class CmakeTestCase(ConfgenBaseTestCase):
    @classmethod
    def setUpClass(self):
        super(CmakeTestCase, self).setUpClass()
        self.args.update({'output': 'cmake'})

    def testStringEscape(self):
        self.invoke_and_test("""
        config PASSWORD
            string "password"
            default "\\\\~!@#$%^&*()\\\""
        """, 'set(CONFIG_PASSWORD "\\\\~!@#$%^&*()\\\"")')

    def testHexPrefix(self):
        self.invoke_and_test(HEXPREFIX_KCONFIG, 'set(CONFIG_HEX_NOPREFIX "0x33")')
        self.invoke_and_test(HEXPREFIX_KCONFIG, 'set(CONFIG_HEX_PREFIX "0x77")')


class JsonTestCase(ConfgenBaseTestCase):
    @classmethod
    def setUpClass(self):
        super(JsonTestCase, self).setUpClass()
        self.args.update({'output': 'json'})

    def testStringEscape(self):
        self.invoke_and_test("""
        config PASSWORD
            string "password"
            default "\\\\~!@#$%^&*()\\\""
        """, '"PASSWORD": "\\\\~!@#$%^&*()\\\""')

    def testHexPrefix(self):
        # hex values come out as integers in JSON, due to no hex type
        self.invoke_and_test(HEXPREFIX_KCONFIG, '"HEX_NOPREFIX": %d' % 0x33)
        self.invoke_and_test(HEXPREFIX_KCONFIG, '"HEX_PREFIX": %d' % 0x77)


class JsonMenuTestCase(ConfgenBaseTestCase):
    @classmethod
    def setUpClass(self):
        super(JsonMenuTestCase, self).setUpClass()
        self.args.update({'output': 'json_menus'})

    def testMultipleRanges(self):
        self.invoke_and_test("""
        config IDF_TARGET
            string "IDF target"
            default "esp32"

        config SOME_SETTING
            int "setting for the chip"
            range 0 100 if IDF_TARGET="esp32s0"
            range 0 10 if IDF_TARGET="esp32"
            range -10 1 if IDF_TARGET="esp32s2"
        """, re.compile(r'"range":\s+\[\s+0,\s+10\s+\]'), 'regex')

    def testHexRanges(self):
        self.invoke_and_test("""
        config SOME_SETTING
            hex "setting for the chip"
            range 0x0 0xaf if UNDEFINED
            range 0x10 0xaf
        """, r'"range":\s+\[\s+16,\s+175\s+\]', 'regex')


class ConfigTestCase(ConfgenBaseTestCase):
    @classmethod
    def setUpClass(self):
        super(ConfigTestCase, self).setUpClass()
        self.args.update({'output': 'config'})
        self.input = """
        config TEST
            bool "test"
            default "n"
        """

    def setUp(self):
        super(ConfigTestCase, self).setUp()
        with tempfile.NamedTemporaryFile(mode='w+', prefix='test_confgen_', delete=False) as f:
            self.addCleanup(os.remove, f.name)
            self.args.update({'config': f.name})  # this is input in contrast with {'output': 'config'}
            f.write(textwrap.dedent("""
            CONFIG_TEST=y
            CONFIG_UNKNOWN=y
            """))

    def testKeepSavedOption(self):
        self.invoke_and_test(self.input, 'CONFIG_TEST=y')

    def testDiscardUnknownOption(self):
        self.invoke_and_test(self.input, 'CONFIG_UNKNOWN', 'not in')


class RenameConfigTestCase(ConfgenBaseTestCase):
    @classmethod
    def setUpClass(self):
        super(RenameConfigTestCase, self).setUpClass()
        # `args` attribute is a dictionary containing the parameters to pass to `confgen.py`.
        # Specify the name of the output file, this will generate the argument `--output config`.
        self.args.update({'output': 'config'})
        # Setup the KConfig file content in the `input` attribute.
        # Let's define an option that is enabled by default, this is very important.
        # Indeed, as we explicitly disables it by its former name below, rename will be considered as functional
        # if the new name, `(CONFIG_)RENAMED_OPTION` is also disabled in the final configuration file.
        self.input = """
        config RENAMED_OPTION
            bool "Renamed option"
            default y
        """

    def setUp(self):
        super(RenameConfigTestCase, self).setUp()
        # Setup the actual test. What we want to do is to have a configuration file containing which
        # option should be enabled or not, this is the equivalent of the `sdkconfig` that we can find
        # in the examples.
        with tempfile.NamedTemporaryFile(mode='w+', prefix='test_confgen_', delete=False) as f:
            self.addCleanup(os.remove, f.name)
            # The current file name will be given to `confgen.py` after `--config` argument.
            self.args.update({'config': f.name})
            # Specify the content of that configuration file, in our case, we want to explicitely
            # have an option, which needs to be renamed, disabled/not set.
            f.write(textwrap.dedent("""
            # CONFIG_NAMED_OPTION is not set
            """))
        # The configuration file is ready, we need to prepare a `rename` configuration file which will
        # provide the new name for `CONFIG_NAMED_OPTION` we defined above
        with tempfile.NamedTemporaryFile(mode='w+', prefix='test_confgen_', delete=False) as f:
            self.addCleanup(os.remove, f.name)
            # Same as above, the following entry will result in the generation of `--sdkconfig-rename`
            # parameter followed by the current temporary file name.
            self.args.update({'sdkconfig-rename': f.name})
            # The content of our `rename` file is simple: replace `CONFIG_NAMED_OPTION` by `CONFIG_RENAMED_OPTION`
            f.write(textwrap.dedent("""
            CONFIG_NAMED_OPTION             CONFIG_RENAMED_OPTION
            """))

    def testRenamedOptionDisabled(self):
        # Invoke the unit test, specify that the final `sdkconfig` generated must contain the string:
        # "# CONFIG_RENAMED_OPTION is not set"
        self.invoke_and_test(self.input, '# CONFIG_RENAMED_OPTION is not set')


class HeaderTestCase(ConfgenBaseTestCase):
    @classmethod
    def setUpClass(self):
        super(HeaderTestCase, self).setUpClass()
        self.args.update({'output': 'header'})

    def testStringEscape(self):
        self.invoke_and_test("""
        config PASSWORD
            string "password"
            default "\\\\~!@#$%^&*()\\\""
        """, '#define CONFIG_PASSWORD "\\\\~!@#$%^&*()\\\""')

    def testHexPrefix(self):
        self.invoke_and_test(HEXPREFIX_KCONFIG, '#define CONFIG_HEX_NOPREFIX 0x33')
        self.invoke_and_test(HEXPREFIX_KCONFIG, '#define CONFIG_HEX_PREFIX 0x77')


class DocsTestCase(ConfgenBaseTestCase):
    @classmethod
    def setUpClass(self):
        super(DocsTestCase, self).setUpClass()
        self.args.update({'output': 'docs',
                          'env': 'IDF_TARGET=esp32'})

    def testChoice(self):
        self.invoke_and_test("""
        menu "TEST"
            choice TYPES
                prompt "types"
                default TYPES_OP2
                help
                    Description of TYPES

                config TYPES_OP1
                    bool "option 1"
                config TYPES_OP2
                    bool "option 2"
            endchoice
        endmenu
        """, """
        TEST
        ----

        Contains:

        - :ref:`CONFIG_TYPES`

        .. _CONFIG_TYPES:

        CONFIG_TYPES
        ^^^^^^^^^^^^

            types

            :emphasis:`Found in:` :ref:`test`

            Description of TYPES

            Available options:
                - option 1             (TYPES_OP1)
                - option 2             (TYPES_OP2)
        """)  # this is more readable than regex


# Used by multiple testHexPrefix() test cases to verify correct hex output for each format
HEXPREFIX_KCONFIG = """
config HEX_NOPREFIX
hex "Hex Item default no prefix"
default 33

config HEX_PREFIX
hex "Hex Item default prefix"
default 0x77
"""

if __name__ == '__main__':
    unittest.main()
