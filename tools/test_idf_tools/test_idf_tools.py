#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import json
import os
import re
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
OPENOCD = 'openocd-esp32'
RISCV_ELF = 'riscv32-esp-elf'
XTENSA_ESP32_ELF = 'xtensa-esp32-elf'
XTENSA_ESP32S2_ELF = 'xtensa-esp32s2-elf'
XTENSA_ESP32S3_ELF = 'xtensa-esp32s3-elf'
XTENSA_ESP_GDB = 'xtensa-esp-elf-gdb'
RISCV_ESP_GDB = 'riscv32-esp-elf-gdb'


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
OPENOCD_VERSION = version_dict[OPENOCD]
RISCV_ELF_VERSION = version_dict[RISCV_ELF]
XTENSA_ESP32_ELF_VERSION = version_dict[XTENSA_ESP32_ELF]
XTENSA_ESP32S2_ELF_VERSION = version_dict[XTENSA_ESP32S2_ELF]
XTENSA_ESP32S3_ELF_VERSION = version_dict[XTENSA_ESP32S3_ELF]
XTENSA_ESP_GDB_VERSION = version_dict[XTENSA_ESP_GDB]
RISCV_ESP_GDB_VERSION = version_dict[RISCV_ESP_GDB]


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
        cls.idf_env_json = os.path.join(cls.temp_tools_dir, 'idf-env.json')

    @classmethod
    def tearDownClass(cls):
        shutil.rmtree(cls.temp_tools_dir)

    def tearDown(self):
        if os.path.isdir(os.path.join(self.temp_tools_dir, 'dist')):
            shutil.rmtree(os.path.join(self.temp_tools_dir, 'dist'))

        if os.path.isdir(os.path.join(self.temp_tools_dir, 'tools')):
            shutil.rmtree(os.path.join(self.temp_tools_dir, 'tools'))

        if os.path.isfile(self.idf_env_json):
            os.remove(self.idf_env_json)

    def assert_tool_installed(self, output, tool, tool_version, tool_archive_name=None):
        if tool_archive_name is None:
            tool_archive_name = tool
        self.assertIn('Installing %s@' % tool + tool_version, output)
        self.assertRegex(output, re.compile(rf'Downloading \S+/{tool_archive_name}'))

    def assert_tool_not_installed(self, output, tool, tool_version, tool_archive_name=None):
        if tool_archive_name is None:
            tool_archive_name = tool
        self.assertNotIn('Installing %s@' % tool + tool_version, output)
        self.assertNotRegex(output, re.compile(rf'Downloading \S+/{tool_archive_name}'))

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

        required_tools_installed = 8
        output = self.run_idf_tools_with_action(['install'])
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP32_ELF, XTENSA_ESP32_ELF_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP32S2_ELF, XTENSA_ESP32S2_ELF_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP32S3_ELF, XTENSA_ESP32S3_ELF_VERSION)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        self.assertIn('version installed in tools directory: ' + ESP32ULP_VERSION, output)
        self.assertIn('version installed in tools directory: ' + OPENOCD_VERSION, output)
        self.assertIn('version installed in tools directory: ' + RISCV_ELF_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP32_ELF_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP32S2_ELF_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP32S3_ELF_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP_GDB_VERSION, output)
        self.assertIn('version installed in tools directory: ' + RISCV_ESP_GDB_VERSION, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn('%s/tools/esp32ulp-elf/%s/esp32ulp-elf/bin' %
                      (self.temp_tools_dir, ESP32ULP_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp32-elf/%s/xtensa-esp32-elf/bin' %
                      (self.temp_tools_dir, XTENSA_ESP32_ELF_VERSION), output)
        self.assertIn('%s/tools/openocd-esp32/%s/openocd-esp32/bin' %
                      (self.temp_tools_dir, OPENOCD_VERSION), output)
        self.assertIn('%s/tools/riscv32-esp-elf/%s/riscv32-esp-elf/bin' %
                      (self.temp_tools_dir, RISCV_ELF_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp32s2-elf/%s/xtensa-esp32s2-elf/bin' %
                      (self.temp_tools_dir, XTENSA_ESP32S2_ELF_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp32s3-elf/%s/xtensa-esp32s3-elf/bin' %
                      (self.temp_tools_dir, XTENSA_ESP32S3_ELF_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp-elf-gdb/%s/xtensa-esp-elf-gdb/bin' %
                      (self.temp_tools_dir, XTENSA_ESP_GDB_VERSION), output)
        self.assertIn('%s/tools/riscv32-esp-elf-gdb/%s/riscv32-esp-elf-gdb/bin' %
                      (self.temp_tools_dir, RISCV_ESP_GDB_VERSION), output)

    def test_tools_for_esp32(self):
        required_tools_installed = 4
        output = self.run_idf_tools_with_action(['install', '--targets=esp32'])
        self.assert_tool_installed(output, XTENSA_ESP32_ELF, XTENSA_ESP32_ELF_VERSION)
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, RISCV_ELF, RISCV_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32S2_ELF, XTENSA_ESP32S2_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32S3_ELF, XTENSA_ESP32S3_ELF_VERSION)
        self.assert_tool_not_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        self.assertIn('version installed in tools directory: ' + ESP32ULP_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP32_ELF_VERSION, output)
        self.assertIn('version installed in tools directory: ' + OPENOCD_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP_GDB_VERSION, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn('%s/tools/esp32ulp-elf/%s/esp32ulp-elf/bin' %
                      (self.temp_tools_dir, ESP32ULP_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp32-elf/%s/xtensa-esp32-elf/bin' %
                      (self.temp_tools_dir, XTENSA_ESP32_ELF_VERSION), output)
        self.assertIn('%s/tools/openocd-esp32/%s/openocd-esp32/bin' %
                      (self.temp_tools_dir, OPENOCD_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp-elf-gdb/%s/xtensa-esp-elf-gdb/bin' %
                      (self.temp_tools_dir, XTENSA_ESP_GDB_VERSION), output)
        self.assertNotIn('%s/tools/riscv32-esp-elf/%s/riscv32-esp-elf/bin' %
                         (self.temp_tools_dir, RISCV_ELF_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32s2-elf/%s/xtensa-esp32s2-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32S2_ELF_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32s3-elf/%s/xtensa-esp32s3-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32S3_ELF_VERSION), output)
        self.assertNotIn('%s/tools/riscv32-esp-elf-gdb/%s/riscv32-esp-elf-gdb/bin' %
                         (self.temp_tools_dir, RISCV_ESP_GDB_VERSION), output)

    def test_tools_for_esp32c3(self):
        required_tools_installed = 3
        output = self.run_idf_tools_with_action(['install', '--targets=esp32c3'])
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION)
        self.assert_tool_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32_ELF, XTENSA_ESP32_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32S2_ELF, XTENSA_ESP32S2_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32S3_ELF, XTENSA_ESP32S3_ELF_VERSION)
        self.assert_tool_not_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP_GDB_VERSION, XTENSA_ESP_GDB_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        self.assertIn('version installed in tools directory: ' + OPENOCD_VERSION, output)
        self.assertIn('version installed in tools directory: ' + RISCV_ELF_VERSION, output)
        self.assertIn('version installed in tools directory: ' + RISCV_ESP_GDB_VERSION, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn('%s/tools/openocd-esp32/%s/openocd-esp32/bin' %
                      (self.temp_tools_dir, OPENOCD_VERSION), output)
        self.assertIn('%s/tools/riscv32-esp-elf/%s/riscv32-esp-elf/bin' %
                      (self.temp_tools_dir, RISCV_ELF_VERSION), output)
        self.assertNotIn('%s/tools/esp32ulp-elf/%s/esp32ulp-elf/bin' %
                         (self.temp_tools_dir, ESP32ULP_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32-elf/%s/xtensa-esp32-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32_ELF_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32s2-elf/%s/xtensa-esp32s2-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32S2_ELF_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32s3-elf/%s/xtensa-esp32s3-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32S3_ELF_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp-elf-gdb/%s/xtensa-esp-elf-gdb/bin' %
                         (self.temp_tools_dir, XTENSA_ESP_GDB_VERSION), output)

    def test_tools_for_esp32s2(self):
        required_tools_installed = 5
        output = self.run_idf_tools_with_action(['install', '--targets=esp32s2'])
        self.assert_tool_installed(output, XTENSA_ESP32S2_ELF, XTENSA_ESP32S2_ELF_VERSION)
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32_ELF, XTENSA_ESP32_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32S3_ELF, XTENSA_ESP32S3_ELF_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        self.assertIn('version installed in tools directory: ' + OPENOCD_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP32S2_ELF_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP_GDB_VERSION, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn('%s/tools/xtensa-esp32s2-elf/%s/xtensa-esp32s2-elf/bin' %
                      (self.temp_tools_dir, XTENSA_ESP32S2_ELF_VERSION), output)
        self.assertIn('%s/tools/openocd-esp32/%s/openocd-esp32/bin' %
                      (self.temp_tools_dir, OPENOCD_VERSION), output)
        self.assertIn('%s/tools/esp32ulp-elf/%s/esp32ulp-elf/bin' %
                      (self.temp_tools_dir, ESP32ULP_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32-elf/%s/xtensa-esp32-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32_ELF_VERSION), output)
        self.assertIn('%s/tools/riscv32-esp-elf/%s/riscv32-esp-elf/bin' %
                      (self.temp_tools_dir, RISCV_ELF_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32s3-elf/%s/xtensa-esp32s3-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32S3_ELF_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp-elf-gdb/%s/xtensa-esp-elf-gdb/bin' %
                      (self.temp_tools_dir, XTENSA_ESP_GDB_VERSION), output)
        self.assertNotIn('%s/tools/riscv32-esp-elf-gdb/%s/riscv32-esp-elf-gdb/bin' %
                         (self.temp_tools_dir, RISCV_ESP_GDB_VERSION), output)

    def test_tools_for_esp32s3(self):
        required_tools_installed = 5
        output = self.run_idf_tools_with_action(['install', '--targets=esp32s3'])
        self.assert_tool_installed(output, XTENSA_ESP32S3_ELF, XTENSA_ESP32S3_ELF_VERSION)
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32_ELF, XTENSA_ESP32_ELF_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP32S2_ELF, XTENSA_ESP32S2_ELF_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        self.assertIn('version installed in tools directory: ' + OPENOCD_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP32S3_ELF_VERSION, output)
        self.assertIn('version installed in tools directory: ' + XTENSA_ESP_GDB_VERSION, output)
        self.assertIn('version installed in tools directory: ' + RISCV_ESP_GDB_VERSION, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn('%s/tools/openocd-esp32/%s/openocd-esp32/bin' %
                      (self.temp_tools_dir, OPENOCD_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp32s3-elf/%s/xtensa-esp32s3-elf/bin' %
                      (self.temp_tools_dir, XTENSA_ESP32S3_ELF_VERSION), output)
        self.assertIn('%s/tools/esp32ulp-elf/%s/esp32ulp-elf/bin' %
                      (self.temp_tools_dir, ESP32ULP_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32-elf/%s/xtensa-esp32-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32_ELF_VERSION), output)
        self.assertIn('%s/tools/riscv32-esp-elf/%s/riscv32-esp-elf/bin' %
                      (self.temp_tools_dir, RISCV_ELF_VERSION), output)
        self.assertNotIn('%s/tools/xtensa-esp32s2-elf/%s/xtensa-esp32s2-elf/bin' %
                         (self.temp_tools_dir, XTENSA_ESP32S2_ELF_VERSION), output)
        self.assertIn('%s/tools/xtensa-esp-elf-gdb/%s/xtensa-esp-elf-gdb/bin' %
                      (self.temp_tools_dir, XTENSA_ESP_GDB_VERSION), output)
        self.assertNotIn('%s/tools/riscv32-esp-elf-gdb/%s/riscv32-esp-elf-gdb/bin' %
                         (self.temp_tools_dir, RISCV_ESP_GDB_VERSION), output)

    def test_uninstall_option(self):
        self.run_idf_tools_with_action(['install', '--targets=esp32,esp32c3'])
        output = self.run_idf_tools_with_action(['uninstall', '--dry-run'])
        self.assertEqual(output, '')

        with open(self.idf_env_json, 'r') as idf_env_file:
            idf_env_json = json.load(idf_env_file)
        idf_env_json['idfInstalled'][idf_env_json['idfSelectedId']]['targets'].remove('esp32')
        with open(self.idf_env_json, 'w') as w:
            json.dump(idf_env_json, w)

        output = self.run_idf_tools_with_action(['uninstall'])
        self.assertIn(XTENSA_ESP32_ELF, output)
        self.assertIn(ESP32ULP, output)
        output = self.run_idf_tools_with_action(['uninstall', '--dry-run'])
        self.assertEqual(output, '')

    def test_unset(self):
        self.run_idf_tools_with_action(['install'])
        self.run_idf_tools_with_action(['export'])
        self.assertTrue(os.path.isfile(self.idf_env_json), 'File {} was not found. '.format(self.idf_env_json))
        self.assertNotEqual(os.stat(self.idf_env_json).st_size, 0, 'File {} is empty. '.format(self.idf_env_json))
        with open(self.idf_env_json, 'r') as idf_env_file:
            idf_env_json = json.load(idf_env_file)
            selected_idf = idf_env_json['idfSelectedId']
            self.assertIn('unset', idf_env_json['idfInstalled'][selected_idf],
                          'Unset was not created for active environment in {}.'.format(self.idf_env_json))


class TestMaintainer(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        idf_path = os.getenv('IDF_PATH')
        cls.tools_old = os.path.join(idf_path, 'tools/tools.json')
        cls.tools_new = os.path.join(idf_path, 'tools/tools.new.json')
        cls.test_tool_name = 'xtensa-esp32-elf'

    def test_validation(self):
        idf_tools.main(['validate'])

    def test_json_rewrite(self):
        idf_tools.main(['rewrite'])
        idf_path = os.getenv('IDF_PATH')
        if not idf_path:
            self.fail('IDF_PATH needs to be set to run this test')
        with open(self.tools_old, 'r') as f:
            json_old = f.read()
        with open(self.tools_new, 'r') as f:
            json_new = f.read()
        self.assertEqual(json_old, json_new, "Please check 'tools/tools.new.json' to find a cause!")

    def add_version_get_expected_json(self, addition_file, replace=False):
        with open(self.tools_old, 'r') as f:
            expected_json = json.load(f)
        with open(addition_file, 'r') as f:
            addition_json = json.load(f)
        for tool in expected_json['tools']:
            if tool['name'] == self.test_tool_name:
                if replace:
                    tool['versions'] = [addition_json]
                else:
                    tool['versions'].append(addition_json)
                return expected_json
        return None

    def test_add_version_artifact_addition(self):
        filenames = []
        with open('add_version/artifact_input.json', 'r') as f:
            add_tools_info = json.load(f)
        for tool in add_tools_info:
            filenames.append(tool['filename'])
            with open(tool['filename'], 'w') as f:
                self.addCleanup(os.remove, f.name)
                f.write('1' * tool['size'])
        idf_tools.main(
            [
                'add-version',
                '--tool',
                self.test_tool_name,
                '--url-prefix',
                'http://test.com',
                '--version',
                'test',
                '--artifact-file'
            ] + filenames
        )
        expected_json = self.add_version_get_expected_json('add_version/artifact_expected_addition.json')
        with open(self.tools_new, 'r') as f1:
            self.assertEqual(json.load(f1), expected_json, "Please check 'tools/tools.new.json' to find a cause!")

    def test_add_version_checksum_addition(self):
        idf_tools.main(
            [
                'add-version',
                '--tool',
                self.test_tool_name,
                '--url-prefix',
                'http://test.com',
                '--version',
                'test',
                '--checksum-file',
                'add_version/checksum.sha256',
            ]
        )
        expected_json = self.add_version_get_expected_json('add_version/checksum_expected_addition.json')
        with open(self.tools_new, 'r') as f1:
            self.assertEqual(json.load(f1), expected_json, "Please check 'tools/tools.new.json' to find a cause!")

    def test_add_version_checksum_with_override(self):
        idf_tools.main(
            [
                'add-version',
                '--tool',
                self.test_tool_name,
                '--url-prefix',
                'http://test.com',
                '--version',
                'test',
                '--override',
                '--checksum-file',
                'add_version/checksum.sha256'
            ]
        )
        expected_json = self.add_version_get_expected_json('add_version/checksum_expected_override.json', True)
        with open(self.tools_new, 'r') as f1:
            self.assertEqual(json.load(f1), expected_json, "Please check 'tools/tools.new.json' to find a cause!")


if __name__ == '__main__':
    unittest.main()
