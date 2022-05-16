#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import json
import os
import shutil
import sys
import tempfile
import unittest

try:
    from contextlib import redirect_stdout
except ImportError:
    import contextlib

    @contextlib.contextmanager  # type: ignore
    def redirect_stdout(target):
        original = sys.stdout
        sys.stdout = target
        yield
        sys.stdout = original

try:
    from cStringIO import StringIO
except ImportError:
    from io import StringIO

# Need to do this before importing idf_tools.py
os.environ['IDF_MAINTAINER'] = '1'

try:
    import idf_tools
except ImportError:
    sys.path.append('..')
    import idf_tools

ESP32ULP = 'esp32ulp-elf'
ESP32ULP_ARCHIVE = 'binutils-esp32ulp'
ESP32S2ULP = 'esp32s2ulp-elf'
ESP32S2ULP_ARCHIVE = 'binutils-esp32s2ulp'
OPENOCD = 'openocd-esp32'
RISCV_ELF = 'riscv32-esp-elf'
XTENSA_ESP32_ELF = 'xtensa-esp32-elf'
XTENSA_ESP32S2_ELF = 'xtensa-esp32s2-elf'
XTENSA_ESP32S3_ELF = 'xtensa-esp32s3-elf'


def get_version_dict():
    '''
    Return a dictionary with tool name to tool version mapping.

    It works with tools.json directly and not through idf_tools.py in order to bypass the script under test. This is
    a little hacky but thanks to this, versions are not required to be updated here every time a tool is updated.
    '''
    with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'tools.json')) as f:
        tools_obj = json.loads(f.read())

    return dict((tool['name'], tool['versions'][0]['name']) for tool in tools_obj['tools'])


version_dict = get_version_dict()

ESP32ULP_VERSION = version_dict[ESP32ULP]
ESP32S2ULP_VERSION = version_dict[ESP32S2ULP]
OPENOCD_VERSION = version_dict[OPENOCD]
RISCV_ELF_VERSION = version_dict[RISCV_ELF]
XTENSA_ESP32_ELF_VERSION = version_dict[XTENSA_ESP32_ELF]
XTENSA_ESP32S2_ELF_VERSION = version_dict[XTENSA_ESP32S2_ELF]
XTENSA_ESP32S3_ELF_VERSION = version_dict[XTENSA_ESP32S3_ELF]


class TestUsage(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        old_tools_dir = os.environ.get('IDF_TOOLS_PATH') or os.path.expanduser(idf_tools.IDF_TOOLS_PATH_DEFAULT)

        mirror_prefix_map = None
        if os.path.exists(old_tools_dir):
            mirror_prefix_map = 'https://dl.espressif.com/dl/toolchains/preview,file://' + os.path.join(old_tools_dir,
                                                                                                        'dist')
            mirror_prefix_map += ';https://dl.espressif.com/dl,file://' + os.path.join(old_tools_dir, 'dist')
            mirror_prefix_map += ';https://github.com/espressif/.*/releases/download/.*/,file://' + os.path.join(
                old_tools_dir, 'dist', '')
        if mirror_prefix_map:
            print('Using IDF_MIRROR_PREFIX_MAP={}'.format(mirror_prefix_map))
            os.environ['IDF_MIRROR_PREFIX_MAP'] = mirror_prefix_map

        cls.temp_tools_dir = tempfile.mkdtemp(prefix='idf_tools_tmp')

        print('Using IDF_TOOLS_PATH={}'.format(cls.temp_tools_dir))
        os.environ['IDF_TOOLS_PATH'] = cls.temp_tools_dir

    @classmethod
    def tearDownClass(cls):
        shutil.rmtree(cls.temp_tools_dir)

    def tearDown(self):
        if os.path.isdir(os.path.join(self.temp_tools_dir, 'dist')):
            shutil.rmtree(os.path.join(self.temp_tools_dir, 'dist'))

        if os.path.isdir(os.path.join(self.temp_tools_dir, 'tools')):
            shutil.rmtree(os.path.join(self.temp_tools_dir, 'tools'))

        if os.path.isfile(os.path.join(self.temp_tools_dir, 'idf-env.json')):
            os.remove(os.path.join(self.temp_tools_dir, 'idf-env.json'))

    def assert_tool_installed(self, output, tool, tool_version, tool_archive_name=None):
        if tool_archive_name is None:
            tool_archive_name = tool
        self.assertIn('Installing %s@' % tool + tool_version, output)
        self.assertIn('Downloading %s' % tool_archive_name, output)

    def assert_tool_not_installed(self, output, tool, tool_version, tool_archive_name=None):
        if tool_archive_name is None:
            tool_archive_name = tool
        self.assertNotIn('Installing %s@' % tool + tool_version, output)
        self.assertNotIn('Downloading %s' % tool_archive_name, output)

    def run_idf_tools_with_action(self, action):
        output_stream = StringIO()
        with redirect_stdout(output_stream):
            idf_tools.main(['--non-interactive'] + action)
        output = output_stream.getvalue()
        return output

    def test_usage_basic(self):
        output = self.run_idf_tools_with_action(['list'])
        self.assertIn('* %s:' % ESP32ULP, output)
        self.assertIn('- %s (recommended)' % ESP32ULP_VERSION, output)
        self.assertIn('* %s:' % ESP32S2ULP, output)
        self.assertIn('- %s (recommended)' % ESP32S2ULP_VERSION, output)
        self.assertIn('* %s:' % OPENOCD, output)
        self.assertIn('- %s (recommended)' % OPENOCD_VERSION, output)
        self.assertIn('* %s:' % RISCV_ELF, output)
        self.assertIn('- %s (recommended)' % RISCV_ELF_VERSION, output)
        self.assertIn('* %s:' % XTENSA_ESP32_ELF, output)
        self.assertIn('- %s (recommended)' % XTENSA_ESP32_ELF_VERSION, output)
        self.assertIn('* %s:' % XTENSA_ESP32S2_ELF, output)
        self.assertIn('- %s (recommended)' % XTENSA_ESP32S2_ELF_VERSION, output)
        self.assertIn('* %s:' % XTENSA_ESP32S3_ELF, output)
        self.assertIn('- %s (recommended)' % XTENSA_ESP32S3_ELF_VERSION, output)

        required_tools_installed = 7
        output = self.run_idf_tools_with_action(['install'])
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP32_ELF, XTENSA_ESP32_ELF_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP32S2_ELF, XTENSA_ESP32S2_ELF_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP32S3_ELF, XTENSA_ESP32S3_ELF_VERSION)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION, ESP32ULP_ARCHIVE)
        self.assert_tool_installed(output, ESP32S2ULP, ESP32S2ULP_VERSION, ESP32S2ULP_ARCHIVE)
        self.assertIn('to ' + os.path.join(self.temp_tools_dir, 'dist'), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        self.assertIn('version installed in tools directory: ' + ESP32ULP_VERSION, output)
        self.assertIn('version installed in tools directory: ' + ESP32S2ULP_VERSION, output)
        self.assertIn('version installed in tools directory: ' + OPENOCD_VERSION, output)
        self.assertIn('version installed in tools directory: ' + RISCV_ELF_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP32_ELF_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP32S2_ELF_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP32S3_ELF_VERSION, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn('%s/tools/esp32ulp-elf/%s/esp32ulp-elf-binutils/bin' %
                      (self.temp_tools_dir, ESP32ULP_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp32-elf/%s/xtensa-esp32-elf/bin' %
                      (self.temp_tools_dir, XTENSA_ESP32_ELF_VERSION), output)
        self.assertIn('%s/tools/openocd-esp32/%s/openocd-esp32/bin' %
                      (self.temp_tools_dir, OPENOCD_VERSION), output)
        self.assertIn('%s/tools/riscv32-esp-elf/%s/riscv32-esp-elf/bin' %
                      (self.temp_tools_dir, RISCV_ELF_VERSION), output)
        self.assertIn('%s/tools/esp32s2ulp-elf/%s/esp32s2ulp-elf-binutils/bin' %
                      (self.temp_tools_dir, ESP32S2ULP_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp32s2-elf/%s/xtensa-esp32s2-elf/bin' %
                      (self.temp_tools_dir, XTENSA_ESP32S2_ELF_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp32s3-elf/%s/xtensa-esp32s3-elf/bin' %
                      (self.temp_tools_dir, XTENSA_ESP32S3_ELF_VERSION), output)

    def test_tools_for_esp32(self):
        required_tools_installed = 3
        output = self.run_idf_tools_with_action(['install', '--targets=esp32'])
        self.assert_tool_installed(output, XTENSA_ESP32_ELF, XTENSA_ESP32_ELF_VERSION)
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION, ESP32ULP_ARCHIVE)
        self.assert_tool_not_installed(output, RISCV_ELF, RISCV_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32S2_ELF, XTENSA_ESP32S2_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32S3_ELF, XTENSA_ESP32S3_ELF_VERSION)
        self.assert_tool_not_installed(output, ESP32S2ULP, ESP32S2ULP_VERSION, ESP32S2ULP_ARCHIVE)
        self.assertIn('to ' + os.path.join(self.temp_tools_dir, 'dist'), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        self.assertIn('version installed in tools directory: ' + ESP32ULP_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP32_ELF_VERSION, output)
        self.assertIn('version installed in tools directory: ' + OPENOCD_VERSION, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn('%s/tools/esp32ulp-elf/%s/esp32ulp-elf-binutils/bin' %
                      (self.temp_tools_dir, ESP32ULP_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp32-elf/%s/xtensa-esp32-elf/bin' %
                      (self.temp_tools_dir, XTENSA_ESP32_ELF_VERSION), output)
        self.assertIn('%s/tools/openocd-esp32/%s/openocd-esp32/bin' %
                      (self.temp_tools_dir, OPENOCD_VERSION), output)
        self.assertNotIn('%s/tools/riscv32-esp-elf/%s/riscv32-esp-elf/bin' %
                         (self.temp_tools_dir, RISCV_ELF_VERSION), output)
        self.assertNotIn('%s/tools/esp32s2ulp-elf/%s/esp32s2ulp-elf-binutils/bin' %
                         (self.temp_tools_dir, ESP32S2ULP_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32s2-elf/%s/xtensa-esp32s2-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32S2_ELF_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32s3-elf/%s/xtensa-esp32s3-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32S3_ELF_VERSION), output)

    def test_tools_for_esp32c3(self):
        required_tools_installed = 2
        output = self.run_idf_tools_with_action(['install', '--targets=esp32c3'])
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32_ELF, XTENSA_ESP32_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32S2_ELF, XTENSA_ESP32S2_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32S3_ELF, XTENSA_ESP32S3_ELF_VERSION)
        self.assert_tool_not_installed(output, ESP32ULP, ESP32ULP_VERSION, ESP32ULP_ARCHIVE)
        self.assert_tool_not_installed(output, ESP32S2ULP, ESP32S2ULP_VERSION, ESP32S2ULP_ARCHIVE)
        self.assertIn('to ' + os.path.join(self.temp_tools_dir, 'dist'), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        self.assertIn('version installed in tools directory: ' + OPENOCD_VERSION, output)
        self.assertIn('version installed in tools directory: ' + RISCV_ELF_VERSION, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn('%s/tools/openocd-esp32/%s/openocd-esp32/bin' %
                      (self.temp_tools_dir, OPENOCD_VERSION), output)
        self.assertIn('%s/tools/riscv32-esp-elf/%s/riscv32-esp-elf/bin' %
                      (self.temp_tools_dir, RISCV_ELF_VERSION), output)
        self.assertNotIn('%s/tools/esp32ulp-elf/%s/esp32ulp-elf-binutils/bin' %
                         (self.temp_tools_dir, ESP32ULP_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32-elf/%s/xtensa-esp32-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32_ELF_VERSION), output)
        self.assertNotIn('%s/tools/esp32s2ulp-elf/%s/esp32s2ulp-elf-binutils/bin' %
                         (self.temp_tools_dir, ESP32S2ULP_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32s2-elf/%s/xtensa-esp32s2-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32S2_ELF_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32s3-elf/%s/xtensa-esp32s3-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32S3_ELF_VERSION), output)

    def test_tools_for_esp32s2(self):
        required_tools_installed = 4
        output = self.run_idf_tools_with_action(['install', '--targets=esp32s2'])
        self.assert_tool_installed(output, XTENSA_ESP32S2_ELF, XTENSA_ESP32S2_ELF_VERSION)
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32_ELF, XTENSA_ESP32_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32S3_ELF, XTENSA_ESP32S3_ELF_VERSION)
        self.assert_tool_not_installed(output, ESP32ULP, ESP32ULP_VERSION, ESP32ULP_ARCHIVE)
        self.assert_tool_installed(output, ESP32S2ULP, ESP32S2ULP_VERSION, ESP32S2ULP_ARCHIVE)
        self.assertIn('to ' + os.path.join(self.temp_tools_dir, 'dist'), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        self.assertIn('version installed in tools directory: ' + ESP32S2ULP_VERSION, output)
        self.assertIn('version installed in tools directory: ' + OPENOCD_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP32S2_ELF_VERSION, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn('%s/tools/esp32s2ulp-elf/%s/esp32s2ulp-elf-binutils/bin' %
                      (self.temp_tools_dir, ESP32S2ULP_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp32s2-elf/%s/xtensa-esp32s2-elf/bin' %
                      (self.temp_tools_dir, XTENSA_ESP32S2_ELF_VERSION), output)
        self.assertIn('%s/tools/openocd-esp32/%s/openocd-esp32/bin' %
                      (self.temp_tools_dir, OPENOCD_VERSION), output)
        self.assertNotIn('%s/tools/esp32ulp-elf/%s/esp32ulp-elf-binutils/bin' %
                         (self.temp_tools_dir, ESP32ULP_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32-elf/%s/xtensa-esp32-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32_ELF_VERSION), output)
        self.assertIn('%s/tools/riscv32-esp-elf/%s/riscv32-esp-elf/bin' %
                      (self.temp_tools_dir, RISCV_ELF_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32s3-elf/%s/xtensa-esp32s3-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32S3_ELF_VERSION), output)

    def test_tools_for_esp32s3(self):
        required_tools_installed = 4
        output = self.run_idf_tools_with_action(['install', '--targets=esp32s3'])
        self.assert_tool_installed(output, XTENSA_ESP32S3_ELF, XTENSA_ESP32S3_ELF_VERSION)
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32_ELF, XTENSA_ESP32_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32S2_ELF, XTENSA_ESP32S2_ELF_VERSION)
        self.assert_tool_not_installed(output, ESP32ULP, ESP32ULP_VERSION, ESP32ULP_ARCHIVE)
        self.assert_tool_installed(output, ESP32S2ULP, ESP32S2ULP_VERSION, ESP32S2ULP_ARCHIVE)
        self.assertIn('to ' + os.path.join(self.temp_tools_dir, 'dist'), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        self.assertIn('version installed in tools directory: ' + OPENOCD_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP32S3_ELF_VERSION, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn('%s/tools/openocd-esp32/%s/openocd-esp32/bin' %
                      (self.temp_tools_dir, OPENOCD_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp32s3-elf/%s/xtensa-esp32s3-elf/bin' %
                      (self.temp_tools_dir, XTENSA_ESP32S3_ELF_VERSION), output)
        self.assertNotIn('%s/tools/esp32ulp-elf/%s/esp32ulp-elf-binutils/bin' %
                         (self.temp_tools_dir, ESP32ULP_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32-elf/%s/xtensa-esp32-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32_ELF_VERSION), output)
        self.assertIn('%s/tools/riscv32-esp-elf/%s/riscv32-esp-elf/bin' %
                      (self.temp_tools_dir, RISCV_ELF_VERSION), output)
        self.assertIn('%s/tools/esp32s2ulp-elf/%s/esp32s2ulp-elf-binutils/bin' %
                      (self.temp_tools_dir, ESP32S2ULP_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32s2-elf/%s/xtensa-esp32s2-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32S2_ELF_VERSION), output)


class TestMaintainer(unittest.TestCase):

    def test_validation(self):
        idf_tools.main(['validate'])

    def test_json_rewrite(self):
        idf_tools.main(['rewrite'])
        idf_path = os.getenv('IDF_PATH')
        if not idf_path:
            self.fail('IDF_PATH needs to be set to run this test')
        with open(os.path.join(idf_path, 'tools/tools.json'), 'r') as f:
            json_old = f.read()
        with open(os.path.join(idf_path, 'tools/tools.new.json'), 'r') as f:
            json_new = f.read()
        self.assertEqual(json_old, json_new, "Please check 'tools/tools.new.json' to find a cause!")


if __name__ == '__main__':
    unittest.main()
