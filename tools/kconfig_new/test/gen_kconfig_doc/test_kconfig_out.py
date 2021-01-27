#!/usr/bin/env python
from __future__ import unicode_literals

import io
import os
import sys
import unittest

import kconfiglib

try:
    import gen_kconfig_doc
except ImportError:
    sys.path.insert(0, os.path.normpath(os.path.join(os.path.dirname(os.path.realpath(__file__)), '../..')))
    import gen_kconfig_doc


class TestDocOutput(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        os.environ['IDF_TARGET'] = 'chipa'
        cls.target = os.environ['IDF_TARGET']
        cls.config = kconfiglib.Kconfig('Kconfig')
        cls.visibility = gen_kconfig_doc.ConfigTargetVisibility(cls.config, cls.target)

    def get_config(self, name):
        sym = self.config.syms.get(name)
        if sym:
            return sym.nodes[0]
        choice = self.config.named_choices.get(name)
        if choice:
            return choice.nodes[0]
        raise RuntimeError('Unimplemented {}'.format(name))

    def get_doc_out(self, config_name):
        with io.StringIO() if sys.version_info.major == 3 else io.BytesIO() as output:
            gen_kconfig_doc.write_menu_item(output, self.get_config(config_name), self.visibility)
            output.seek(0)
            return output.read()

    def test_simple_default(self):
        s = self.get_doc_out('EXT_CONFIG3_FOR_CHIPA_MENU')
        self.assertIn('- 5', s)

    def test_multiple_defaults(self):
        s = self.get_doc_out('CHIPA_OPTION')
        self.assertNotIn('- 5', s)
        self.assertIn('- 4 if CHIPA_VERSION = 2', s)
        self.assertNotIn('- 9', s)

    def test_string_default(self):
        s = self.get_doc_out('COMPILER')
        self.assertIn('- ca', s)
        self.assertNotIn('- cb', s)

    def test_bool_default(self):
        s = self.get_doc_out('BOOL_OPTION')
        self.assertIn('- Yes', s)

    def test_bool_default_dependency(self):
        s = self.get_doc_out('BOOL_OPTION2')
        self.assertIn('- Yes', s)

    def test_hex_default(self):
        s = self.get_doc_out('HEX_OPTION')
        self.assertIn('- "0xce"', s)
        self.assertNotIn('- "0xff"', s)

    def test_hex_range(self):
        s = self.get_doc_out('HEX_OPTION')
        self.assertIn('- from 0xf to 0xce', s)
        self.assertNotIn('- from 0xfe', s)

    def test_int_range(self):
        s = self.get_doc_out('INT_OPTION')
        self.assertIn('- from 1 to 10', s)
        self.assertNotIn('- from 100', s)


if __name__ == '__main__':
    unittest.main()
