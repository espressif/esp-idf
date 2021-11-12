#!/usr/bin/env python3

import os
import subprocess
import sys
import unittest

CURRENT_DIR = os.path.dirname(os.path.realpath(__file__))
ESP32_DOC = 'esp32_page'
ESP32_S2_DOC = 'esp32s2_page'
BT_DOC = 'bt_page'
LINK_ROLES_DOC = 'link_roles'
IDF_FORMAT_DOC = 'idf_target_format'


class DocBuilder():
    build_docs_py_path = os.path.join(CURRENT_DIR, '..', 'build_docs.py')

    def __init__(self, src_dir, build_dir, target, language):
        self.language = language
        self.target = target
        self.src_dir = src_dir
        self.build_dir = build_dir
        self.html_out_dir = os.path.join(CURRENT_DIR, build_dir, language, target, 'html')

    def build(self, opt_args=[]):
        args = [sys.executable, self.build_docs_py_path, '-b', self.build_dir, '-s', self.src_dir, '-t', self.target, '-l', self.language]
        args.extend(opt_args)
        return subprocess.call(args)


class TestDocs(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.builder = DocBuilder('test', '_build/test_docs', 'esp32s2', 'en')
        cls.build_ret_flag = cls.builder.build()

    def setUp(self):
        if self.build_ret_flag:
            self.fail('Build docs failed with return: {}'.format(self.build_ret_flag))

    def assert_str_not_in_doc(self, doc_name, str_to_find):
        with open(os.path.join(self.builder.html_out_dir, doc_name)) as f:
            content = f.read()
            self.assertFalse(str_to_find in content, 'Found {} in {}'.format(str_to_find, doc_name))

    def assert_str_in_doc(self, doc_name, str_to_find):
        with open(os.path.join(self.builder.html_out_dir, doc_name)) as f:
            content = f.read()
            self.assertTrue(str_to_find in content, 'Did not find {} in {}'.format(str_to_find, doc_name))

    def test_only_dir(self):
        # Test that ESP32 content was excluded
        self.assert_str_not_in_doc(ESP32_S2_DOC + '.html', '!ESP32_CONTENT!')

        # Test that ESP32 S2 content was included
        self.assert_str_in_doc(ESP32_S2_DOC + '.html', '!ESP32_S2_CONTENT!')

        # Test that BT content was excluded
        self.assert_str_not_in_doc(ESP32_S2_DOC + '.html', '!BT_CONTENT!')

    def test_toctree_filter(self):
        # ESP32 page should NOT be built
        esp32_doc = os.path.join(self.builder.html_out_dir, ESP32_DOC + '.html')
        self.assertFalse(os.path.isfile(esp32_doc), 'Found {}'.format(esp32_doc))
        self.assert_str_not_in_doc('index.html', '!ESP32_CONTENT!')

        esp32s2_doc = os.path.join(self.builder.html_out_dir, ESP32_S2_DOC + '.html')
        self.assertTrue(os.path.isfile(esp32s2_doc), '{} not found'.format(esp32s2_doc))

        # Spot check a few other tags
        # No Bluetooth on ESP32 S2
        bt_doc = os.path.join(self.builder.html_out_dir, BT_DOC + '.html')
        self.assertFalse(os.path.isfile(bt_doc), 'Found {}'.format(bt_doc))
        self.assert_str_not_in_doc('index.html', '!BT_CONTENT!')

    def test_link_roles(self):
        print('test')


class TestBuildSubset(unittest.TestCase):
    def test_build_subset(self):
        builder = DocBuilder('test', '_build/test_build_subset', 'esp32', 'en')

        docs_to_build = 'esp32_page.rst'

        self.assertFalse(builder.build(['-i', docs_to_build]))

        # Check that we only built the input docs
        bt_doc = os.path.join(builder.html_out_dir, BT_DOC + '.html')
        esp32_doc = os.path.join(builder.html_out_dir, ESP32_DOC + '.html')
        self.assertFalse(os.path.isfile(bt_doc), 'Found {}'.format(bt_doc))
        self.assertTrue(os.path.isfile(esp32_doc), 'Found {}'.format(esp32_doc))


if __name__ == '__main__':
    unittest.main()
