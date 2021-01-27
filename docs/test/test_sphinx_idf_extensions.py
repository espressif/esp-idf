#!/usr/bin/env python3

import os
import sys
import unittest
from tempfile import TemporaryDirectory
from unittest.mock import MagicMock

from sphinx.util import tags

try:
    from idf_extensions import exclude_docs
except ImportError:
    sys.path.append('..')
    from idf_extensions import exclude_docs

from idf_extensions import format_idf_target, gen_idf_tools_links, link_roles


class TestFormatIdfTarget(unittest.TestCase):

    def setUp(self):
        self.str_sub = format_idf_target.StringSubstituter()

        config = MagicMock()
        config.idf_target = 'esp32'
        self.str_sub.init_sub_strings(config)

    def test_add_subs(self):

        self.assertEqual(self.str_sub.substitute_strings['{IDF_TARGET_NAME}'], 'ESP32')
        self.assertEqual(self.str_sub.substitute_strings['{IDF_TARGET_PATH_NAME}'], 'esp32')
        self.assertEqual(self.str_sub.substitute_strings['{IDF_TARGET_TOOLCHAIN_PREFIX}'], 'xtensa-esp32-elf')
        self.assertEqual(self.str_sub.substitute_strings['{IDF_TARGET_CFG_PREFIX}'], 'ESP32')
        self.assertEqual(self.str_sub.substitute_strings['{IDF_TARGET_TRM_EN_URL}'],
                         'https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf')
        self.assertEqual(self.str_sub.substitute_strings['{IDF_TARGET_TRM_CN_URL}'],
                         'https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_cn.pdf')

    def test_sub(self):
        content = ('This is a {IDF_TARGET_NAME}, with {IDF_TARGET_PATH_NAME}/soc.c, compiled with '
                   '{IDF_TARGET_TOOLCHAIN_PREFIX}-gcc with CONFIG_{IDF_TARGET_CFG_PREFIX}_MULTI_DOC. '
                   'TRM can be found at {IDF_TARGET_TRM_EN_URL} or {IDF_TARGET_TRM_CN_URL}')

        expected = ('This is a ESP32, with esp32/soc.c, compiled with xtensa-esp32-elf-gcc with CONFIG_ESP32_MULTI_DOC. '
                    'TRM can be found at https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf '
                    'or https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_cn.pdf')

        self.assertEqual(self.str_sub.substitute(content), expected)

    def test_local_sub(self):
        content = ('{IDF_TARGET_TX_PIN:default="IO3", esp32="IO4", esp32s2="IO5"}'
                   'The {IDF_TARGET_NAME} UART {IDF_TARGET_TX_PIN} uses for TX')

        expected = 'The ESP32 UART IO4 uses for TX'
        self.assertEqual(self.str_sub.substitute(content), expected)

    def test_local_sub_default(self):
        content = ('{IDF_TARGET_TX_PIN:default="IO3", esp32s2="IO5"}'
                   'The {IDF_TARGET_NAME} UART {IDF_TARGET_TX_PIN} uses for TX')

        expected = 'The ESP32 UART IO3 uses for TX'
        self.assertEqual(self.str_sub.substitute(content), expected)

    def test_local_sub_no_default(self):
        content = ('{IDF_TARGET_TX_PIN: esp32="IO4", esp32s2="IO5"}'
                   'The {IDF_TARGET_NAME} UART {IDF_TARGET_TX_PIN} uses for TX')

        self.assertRaises(ValueError, self.str_sub.substitute, content)


class TestExclude(unittest.TestCase):

    def setUp(self):
        self.app = MagicMock()
        self.app.tags = tags.Tags()
        self.app.config.conditional_include_dict = {'esp32':['esp32.rst', 'bt.rst'], 'esp32s2':['esp32s2.rst']}
        self.app.config.docs_to_build = None
        self.app.config.exclude_patterns = []

    def test_update_exclude_pattern(self):
        self.app.tags.add('esp32')
        exclude_docs.update_exclude_patterns(self.app, self.app.config)
        docs_to_build = set(self.app.config.conditional_include_dict['esp32'])

        # Check that the set of docs to build and the set of docs to exclude do not overlap
        self.assertFalse(docs_to_build & set(self.app.config.exclude_patterns))


class TestGenIDFToolLinks(unittest.TestCase):
    def setUp(self):
        self.app = MagicMock()
        self.app.config.build_dir = '_build'
        self.app.config.idf_path = os.environ['IDF_PATH']

    def test_gen_idf_tool_links(self):

        with TemporaryDirectory() as temp_dir:
            self.app.config.build_dir = temp_dir
            gen_idf_tools_links.generate_idf_tools_links(self.app, None)
            self.assertTrue(os.path.isfile(os.path.join(self.app.config.build_dir, 'inc', 'idf-tools-inc.rst')))


class TestLinkRoles(unittest.TestCase):
    def test_get_submodules(self):
        submod_dict = link_roles.get_submodules()

        # Test a known submodule to see if it's in the dict
        test_submod_name = 'components/asio/asio'
        self.assertIn(test_submod_name, submod_dict)
        self.assertIsNotNone(submod_dict[test_submod_name].url)
        self.assertIsNotNone(submod_dict[test_submod_name].rev)
        self.assertIsNotNone(submod_dict[test_submod_name].url)


if __name__ == '__main__':
    unittest.main()
