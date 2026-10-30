#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import errno
import json
import os
import re
import shutil
import socket
import ssl
import struct
import sys
import tempfile
import threading
import time
import unittest
from http.server import BaseHTTPRequestHandler
from http.server import ThreadingHTTPServer
from subprocess import STDOUT
from subprocess import check_call
from unittest.mock import patch

try:
    from contextlib import redirect_stderr
    from contextlib import redirect_stdout
except ImportError:
    import contextlib

    @contextlib.contextmanager  # type: ignore
    def redirect_stdout(target):
        original = sys.stdout
        sys.stdout = target
        yield
        sys.stdout = original

    @contextlib.contextmanager  # type: ignore
    def redirect_stderr(target):
        original = sys.stderr
        sys.stderr = target
        yield
        sys.stderr = original


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
XTENSA_ELF = 'xtensa-esp-elf'
XTENSA_ESP_GDB = 'xtensa-esp-elf-gdb'
RISCV_ESP_GDB = 'riscv32-esp-elf-gdb'
ESP_ROM_ELFS = 'esp-rom-elfs'
QEMU_RISCV = 'qemu-riscv32'
QEMU_XTENSA = 'qemu-xtensa'
CLANGD = 'esp-clangd'
CLANGD_ARCHIVE = 'clangd'
# Win tools
CMAKE = 'cmake'
NINJA = 'ninja'
IDF_EXE = 'idf-exe'
CCACHE = 'ccache'
DFU_UTIL = 'dfu-util'
ESP_IDF_CONFIGDEP = 'esp-idf-configdep'


def get_version_dict():
    """
    Return a dictionary with tool name to tool version mapping.

    It works with tools.json directly and not through idf_tools.py in order to bypass the script under test. This is
    a little hacky but thanks to this, versions are not required to be updated here every time a tool is updated.
    """
    with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'tools.json')) as f:
        tools_obj = json.loads(f.read())

    return dict((tool['name'], tool['versions'][0]['name']) for tool in tools_obj['tools'])


version_dict = get_version_dict()

ESP32ULP_VERSION = version_dict[ESP32ULP]
OPENOCD_VERSION = version_dict[OPENOCD]
RISCV_ELF_VERSION = version_dict[RISCV_ELF]
XTENSA_ELF_VERSION = version_dict[XTENSA_ELF]
XTENSA_ESP_GDB_VERSION = version_dict[XTENSA_ESP_GDB]
RISCV_ESP_GDB_VERSION = version_dict[RISCV_ESP_GDB]
ESP_ROM_ELFS_VERSION = version_dict[ESP_ROM_ELFS]
QEMU_RISCV_VERSION = version_dict[QEMU_RISCV]
QEMU_XTENSA_VERSION = version_dict[QEMU_XTENSA]
CLANGD_VERSION = version_dict[CLANGD]
# Win tools
CMAKE_VERSION = version_dict[CMAKE]
NINJA_VERSION = version_dict[NINJA]
IDF_EXE_VERSION = version_dict[IDF_EXE]
CCACHE_VERSION = version_dict[CCACHE]
DFU_UTIL_VERSION = version_dict[DFU_UTIL]
ESP_IDF_CONFIGDEP_VERSION = version_dict[ESP_IDF_CONFIGDEP]

# There are some complex search patterns to detect download snippets

# Avoiding an ambiguity with a substring 'riscv32-esp-elf' in the `riscv32-esp-elf-gdb`
# (removing esp- prefix from version)
RISCV_ELF_ARCHIVE_PATTERN = (
    RISCV_ELF + '-' + (RISCV_ELF_VERSION[len('esp-') :] if RISCV_ELF_VERSION.startswith('esp-') else RISCV_ELF_VERSION)
)

# The same like above
XTENSA_ELF_ARCHIVE_PATTERN = (
    XTENSA_ELF
    + '-'
    + (XTENSA_ELF_VERSION[len('esp-') :] if XTENSA_ELF_VERSION.startswith('esp-') else XTENSA_ELF_VERSION)
)


# TestUsageBase takes care of general test setup to use the idf_tools commands
class TestUsageBase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        with open(os.path.join(os.getenv('IDF_PATH'), 'tools/tools.json')) as json_file:
            tools_dict = json.load(json_file)
        cls.tools_dict = tools_dict

        old_tools_dir = os.environ.get('IDF_TOOLS_PATH') or os.path.expanduser(idf_tools.IDF_TOOLS_PATH_DEFAULT)

        mirror_prefix_map = None
        if os.path.exists(old_tools_dir) and sys.platform != 'win32':
            # These are are all mapping to filesystem. Windows cannot download from there not even if file:// is omitted
            local = ''.join(['file://', os.path.join(old_tools_dir, 'dist', '')])

            mirror_prefix_map = ';'.join(
                [
                    f'https://dl.espressif.com/dl,{local}',
                    f'https://github.com/.*/.*/releases/download/.*/,{local}',
                ]
            )

        # Windows will keep downloading the tools from default location or IDF_MIRROR_PREFIX_MAP if set globally

        if mirror_prefix_map:
            print(f'Using IDF_MIRROR_PREFIX_MAP={mirror_prefix_map}')
            os.environ['IDF_MIRROR_PREFIX_MAP'] = mirror_prefix_map

        cls.temp_tools_dir = tempfile.mkdtemp(prefix='idf_tools_tmp')

        print(f'Using IDF_TOOLS_PATH={cls.temp_tools_dir}')
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
        self.assertIn(f'Installing {tool}@{tool_version}', output)
        self.assertRegex(output, re.compile(rf'Downloading \S+{tool_archive_name}'))

    def assert_tool_not_installed(self, output, tool, tool_version, tool_archive_name=None):
        if tool_archive_name is None:
            tool_archive_name = tool
        self.assertNotIn(f'Installing {tool}@{tool_version}', output)
        self.assertNotRegex(output, re.compile(rf'Downloading \S+{tool_archive_name}'))

    def run_idf_tools_with_action(self, action):
        output_stream = StringIO()
        with redirect_stdout(output_stream):
            idf_tools.main(['--non-interactive'] + action)
        output = output_stream.getvalue()
        return output

    def run_idf_tools_with_error(self, action, assertError=False):
        output_stream = StringIO()
        error_stream = StringIO()
        is_error_occured = False
        try:
            with redirect_stderr(error_stream), redirect_stdout(output_stream):
                idf_tools.main(['--non-interactive'] + action)
        except SystemExit:
            is_error_occured = True
            if assertError:
                pass
            else:
                raise
        if assertError:
            self.assertTrue(is_error_occured, 'idf_tools should fail if assertError is set')
        output = output_stream.getvalue()
        error = error_stream.getvalue()
        return output, error


# TestUsage executes tests that behaves the same on both platforms
# TestUsage class serves as a parent for classes TestUsageUnix and TestUsageWin
class TestUsage(TestUsageBase):
    def test_tools_for_wildcards1(self):
        required_tools_installed = 2
        output = self.run_idf_tools_with_action(['install', '*gdb*'])
        self.assert_tool_not_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_not_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_not_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

    def test_tools_for_wildcards2(self):
        required_tools_installed = 1
        output = self.run_idf_tools_with_action(['install', '*gdb*', '--targets=esp32c3'])
        self.assert_tool_not_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_not_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_not_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

    def test_tools_for_wildcards3(self):
        required_tools_installed = 1
        output = self.run_idf_tools_with_action(['install', '*gdb*', '--targets=esp32s3'])
        self.assert_tool_not_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_not_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_not_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_not_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

    def test_uninstall_option(self):
        self.run_idf_tools_with_action(['install', '--targets=esp32'])
        test_tool_name = XTENSA_ELF
        test_tool_version = 'test_version'
        tools_json_new = os.path.join(self.temp_tools_dir, 'tools', 'tools.new.json')
        self.run_idf_tools_with_action(
            [
                'add-version',
                '--tool',
                test_tool_name,
                '--url-prefix',
                'http://test.com',
                '--version',
                test_tool_version,
                '--override',
                '--checksum-file',
                'add_version/checksum.sha256',
                '--output',
                tools_json_new,
            ]
        )
        output = self.run_idf_tools_with_action(['--tools-json', tools_json_new, 'uninstall', '--dry-run'])
        self.assertIn('For removing old versions of ' + test_tool_name, output)
        output = self.run_idf_tools_with_action(['--tools-json', tools_json_new, 'uninstall'])
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', test_tool_name, XTENSA_ELF_VERSION) + ' was removed.', output
        )
        output = self.run_idf_tools_with_action(['uninstall'])
        self.assertEqual('', output)

    def test_deactivate(self):
        self.run_idf_tools_with_action(['install'])
        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(
            'export IDF_DEACTIVATE_FILE_PATH=', output, 'No IDF_DEACTIVATE_FILE_PATH exported into environment'
        )
        deactivate_file = re.findall(r'(?:IDF_DEACTIVATE_FILE_PATH=")(.*)(?:")', output)[0]
        self.assertTrue(os.path.isfile(deactivate_file), f'File {deactivate_file} was not found. ')
        self.assertNotEqual(os.stat(self.idf_env_json).st_size, 0, f'File {deactivate_file} is empty. ')

    def test_export_recommended_version(self):
        always_install_and_recommended_tools = []
        for tool in self.tools_dict['tools']:
            if tool['install'] != 'always':
                continue
            for version in tool['versions']:
                if version['status'] != 'recommended':
                    continue
                always_install_and_recommended_tools.append({'name': tool['name'], 'version': version['name']})
        self.run_idf_tools_with_action(['install'])
        output = self.run_idf_tools_with_action(['export'])

        for tool in always_install_and_recommended_tools:
            self.assertIn(os.path.join(tool['name'], tool['version']), output)

    def test_export_recommended_version_cmake(self):
        tool_to_test = 'cmake'
        tool_version = ''
        for tool in self.tools_dict['tools']:
            if tool['name'] != tool_to_test:
                continue
            for version in tool['versions']:
                if version['status'] == 'recommended':
                    tool_version = version['name']
                    break

        self.run_idf_tools_with_action(['install'])
        self.run_idf_tools_with_action(['install', tool_to_test])
        output = self.run_idf_tools_with_action(['export'])

        self.assertIn(os.path.join(tool_to_test, tool_version), output)

    def test_export_supported_version_cmake(self):
        tool_to_test = 'cmake'
        supported_version = ''
        recommended_version = ''
        for tool in self.tools_dict['tools']:
            if tool['name'] != tool_to_test:
                continue
            for version in tool['versions']:
                if version['status'] == 'supported':
                    supported_version = version['name']
                elif version['status'] == 'recommended':
                    recommended_version = version['name']

        self.run_idf_tools_with_action(['install'])
        output = self.run_idf_tools_with_action(['install', f'{tool_to_test}@{supported_version}'])
        self.assert_tool_installed(output, tool_to_test, supported_version)

        # Remove the recommended version folder installed by install command (in case of Windows)
        recommended_version_folder = os.path.join(self.temp_tools_dir, 'tools', tool_to_test, recommended_version)
        if os.path.exists(recommended_version_folder):
            shutil.rmtree(recommended_version_folder)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(os.path.join(tool_to_test, supported_version), output)
        self.assertNotIn(os.path.join(tool_to_test, recommended_version), output)

    def test_export_prefer_system_cmake(self):
        tool_to_test = 'cmake'
        self.run_idf_tools_with_action(['install'])
        self.run_idf_tools_with_action(['install', tool_to_test])
        # cmake is installed via apt
        output = self.run_idf_tools_with_action(['export', '--prefer-system'])

        self.assertNotIn(tool_to_test, output)

    def test_export_with_required_tools_check_skipped(self):
        self.run_idf_tools_with_error(['export'], assertError=True)

        new_os_environ = os.environ.copy()
        new_os_environ['IDF_SKIP_TOOLS_CHECK'] = '1'
        with patch('os.environ', new_os_environ):
            self.run_idf_tools_with_action(['export'])

            self.run_idf_tools_with_action(['install', OPENOCD])
            output = self.run_idf_tools_with_action(['export'])
            self.assertIn(
                os.path.join(self.temp_tools_dir, 'tools', 'openocd-esp32', OPENOCD_VERSION, 'openocd-esp32', 'bin'),
                output,
            )


# TestUsageUnix tests installed tools on UNIX platforms
@unittest.skipIf(sys.platform == 'win32', reason='Tools for UNIX differ')
class TestUsageUnix(TestUsage):
    def test_usage_basic(self):
        output = self.run_idf_tools_with_action(['list'])
        self.assertIn(f'* {ESP32ULP}:', output)
        self.assertIn(f'- {ESP32ULP_VERSION} (recommended)', output)
        self.assertIn(f'* {OPENOCD}:', output)
        self.assertIn(f'- {OPENOCD_VERSION} (recommended)', output)
        self.assertIn(f'* {RISCV_ELF}:', output)
        self.assertIn(f'- {RISCV_ELF_VERSION} (recommended)', output)
        self.assertIn(f'* {XTENSA_ELF}:', output)
        self.assertIn(f'- {XTENSA_ELF_VERSION} (recommended)', output)
        self.assertIn(f'* {CLANGD}:', output)
        self.assertIn(f'- {CLANGD_VERSION} (recommended)', output)

        required_tools_installed = 9
        output = self.run_idf_tools_with_action(['install'])
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        for tool_version in [
            ESP32ULP_VERSION,
            OPENOCD_VERSION,
            RISCV_ELF_VERSION,
            XTENSA_ELF_VERSION,
            XTENSA_ESP_GDB_VERSION,
            RISCV_ESP_GDB_VERSION,
            ESP_ROM_ELFS_VERSION,
            CLANGD_VERSION,
            ESP_IDF_CONFIGDEP_VERSION,
        ]:
            self.assertIn('version installed in tools directory: ' + tool_version, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(f'{self.temp_tools_dir}/tools/esp32ulp-elf/{ESP32ULP_VERSION}/esp32ulp-elf/bin', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/xtensa-esp-elf/{XTENSA_ELF_VERSION}/xtensa-esp-elf/bin', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/openocd-esp32/{OPENOCD_VERSION}/openocd-esp32/bin', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/riscv32-esp-elf/{RISCV_ELF_VERSION}/riscv32-esp-elf/bin', output)
        self.assertIn(
            f'{self.temp_tools_dir}/tools/xtensa-esp-elf-gdb/{XTENSA_ESP_GDB_VERSION}/xtensa-esp-elf-gdb/bin',
            output,
        )
        self.assertIn(
            f'{self.temp_tools_dir}/tools/riscv32-esp-elf-gdb/{RISCV_ESP_GDB_VERSION}/riscv32-esp-elf-gdb/bin',
            output,
        )
        self.assertIn(f'{self.temp_tools_dir}/tools/esp-rom-elfs/{ESP_ROM_ELFS_VERSION}/', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/esp-clangd/{CLANGD_VERSION}/esp-clangd/bin', output)
        self.assertIn(
            f'{self.temp_tools_dir}/tools/{ESP_IDF_CONFIGDEP}/{ESP_IDF_CONFIGDEP_VERSION}/'
            f'{ESP_IDF_CONFIGDEP}-{ESP_IDF_CONFIGDEP_VERSION}/bin',
            output,
        )

        output = self.run_idf_tools_with_action(['list', '--outdated'])
        self.assertEqual('', output)

        tools_json_outdated = os.path.join(self.temp_tools_dir, 'tools', 'tools.outdated.json')
        new_version = 'zzzzzz'
        self.run_idf_tools_with_action(
            [
                'add-version',
                '--tool',
                XTENSA_ELF,
                '--url-prefix',
                'http://test.com',
                '--version',
                new_version,
                '--override',
                '--checksum-file',
                'add_version/checksum.sha256',
                '--output',
                tools_json_outdated,
            ]
        )

        output = self.run_idf_tools_with_action(['--tools-json', tools_json_outdated, 'list', '--outdated'])
        self.assertIn((f'{XTENSA_ELF}: version {XTENSA_ELF_VERSION} is outdated by {new_version}'), output)

    def test_tools_for_esp32(self):
        required_tools_installed = 7
        output = self.run_idf_tools_with_action(['install', '--targets=esp32'])
        self.assert_tool_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_not_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        for tool_version in [
            ESP32ULP_VERSION,
            OPENOCD_VERSION,
            XTENSA_ELF_VERSION,
            XTENSA_ESP_GDB_VERSION,
            ESP_ROM_ELFS_VERSION,
            CLANGD_VERSION,
            ESP_IDF_CONFIGDEP_VERSION,
        ]:
            self.assertIn('version installed in tools directory: ' + tool_version, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(f'{self.temp_tools_dir}/tools/esp32ulp-elf/{ESP32ULP_VERSION}/esp32ulp-elf/bin', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/xtensa-esp-elf/{XTENSA_ELF_VERSION}/xtensa-esp-elf/bin', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/openocd-esp32/{OPENOCD_VERSION}/openocd-esp32/bin', output)
        self.assertIn(
            f'{self.temp_tools_dir}/tools/xtensa-esp-elf-gdb/{XTENSA_ESP_GDB_VERSION}/xtensa-esp-elf-gdb/bin',
            output,
        )
        self.assertNotIn(f'{self.temp_tools_dir}/tools/riscv32-esp-elf/{RISCV_ELF_VERSION}/riscv32-esp-elf/bin', output)
        self.assertNotIn(
            f'{self.temp_tools_dir}/tools/riscv32-esp-elf-gdb/{RISCV_ESP_GDB_VERSION}/riscv32-esp-elf-gdb/bin',
            output,
        )
        self.assertIn(f'{self.temp_tools_dir}/tools/esp-rom-elfs/{ESP_ROM_ELFS_VERSION}/', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/esp-clangd/{CLANGD_VERSION}/esp-clangd/bin', output)
        self.assertIn(
            f'{self.temp_tools_dir}/tools/{ESP_IDF_CONFIGDEP}/{ESP_IDF_CONFIGDEP_VERSION}/'
            f'{ESP_IDF_CONFIGDEP}-{ESP_IDF_CONFIGDEP_VERSION}/bin',
            output,
        )

    def test_tools_for_esp32c3(self):
        required_tools_installed = 6
        output = self.run_idf_tools_with_action(['install', '--targets=esp32c3'])
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_not_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        for tool_version in [
            OPENOCD_VERSION,
            RISCV_ELF_VERSION,
            RISCV_ESP_GDB_VERSION,
            ESP_ROM_ELFS_VERSION,
            ESP_IDF_CONFIGDEP_VERSION,
        ]:
            self.assertIn('version installed in tools directory: ' + tool_version, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(f'{self.temp_tools_dir}/tools/openocd-esp32/{OPENOCD_VERSION}/openocd-esp32/bin', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/riscv32-esp-elf/{RISCV_ELF_VERSION}/riscv32-esp-elf/bin', output)
        self.assertNotIn(f'{self.temp_tools_dir}/tools/esp32ulp-elf/{ESP32ULP_VERSION}/esp32ulp-elf/bin', output)
        self.assertNotIn(f'{self.temp_tools_dir}/tools/xtensa-esp-elf/{XTENSA_ELF_VERSION}/xtensa-esp-elf/bin', output)
        self.assertNotIn(
            f'{self.temp_tools_dir}/tools/xtensa-esp-elf-gdb/{XTENSA_ESP_GDB_VERSION}/xtensa-esp-elf-gdb/bin',
            output,
        )
        self.assertIn(f'{self.temp_tools_dir}/tools/esp-rom-elfs/{ESP_ROM_ELFS_VERSION}/', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/esp-clangd/{CLANGD_VERSION}/esp-clangd/bin', output)
        self.assertIn(
            f'{self.temp_tools_dir}/tools/{ESP_IDF_CONFIGDEP}/{ESP_IDF_CONFIGDEP_VERSION}/'
            f'{ESP_IDF_CONFIGDEP}-{ESP_IDF_CONFIGDEP_VERSION}/bin',
            output,
        )

    def test_tools_for_esp32s2(self):
        required_tools_installed = 8
        output = self.run_idf_tools_with_action(['install', '--targets=esp32s2'])
        self.assert_tool_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_not_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        for tool_version in [
            OPENOCD_VERSION,
            XTENSA_ELF_VERSION,
            XTENSA_ESP_GDB_VERSION,
            ESP_ROM_ELFS_VERSION,
            ESP_IDF_CONFIGDEP_VERSION,
        ]:
            self.assertIn('version installed in tools directory: ' + tool_version, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(f'{self.temp_tools_dir}/tools/xtensa-esp-elf/{XTENSA_ELF_VERSION}/xtensa-esp-elf/bin', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/openocd-esp32/{OPENOCD_VERSION}/openocd-esp32/bin', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/esp32ulp-elf/{ESP32ULP_VERSION}/esp32ulp-elf/bin', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/riscv32-esp-elf/{RISCV_ELF_VERSION}/riscv32-esp-elf/bin', output)
        self.assertIn(
            f'{self.temp_tools_dir}/tools/xtensa-esp-elf-gdb/{XTENSA_ESP_GDB_VERSION}/xtensa-esp-elf-gdb/bin',
            output,
        )
        self.assertNotIn(
            f'{self.temp_tools_dir}/tools/riscv32-esp-elf-gdb/{RISCV_ESP_GDB_VERSION}/riscv32-esp-elf-gdb/bin',
            output,
        )
        self.assertIn(f'{self.temp_tools_dir}/tools/esp-rom-elfs/{ESP_ROM_ELFS_VERSION}/', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/esp-clangd/{CLANGD_VERSION}/esp-clangd/bin', output)
        self.assertIn(
            f'{self.temp_tools_dir}/tools/{ESP_IDF_CONFIGDEP}/{ESP_IDF_CONFIGDEP_VERSION}/'
            f'{ESP_IDF_CONFIGDEP}-{ESP_IDF_CONFIGDEP_VERSION}/bin',
            output,
        )

    def test_tools_for_esp32s3(self):
        required_tools_installed = 8
        output = self.run_idf_tools_with_action(['install', '--targets=esp32s3'])
        self.assert_tool_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_not_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        for tool_version in [
            OPENOCD_VERSION,
            XTENSA_ELF_VERSION,
            XTENSA_ESP_GDB_VERSION,
            RISCV_ESP_GDB_VERSION,
            ESP_ROM_ELFS_VERSION,
            CLANGD_VERSION,
            ESP_IDF_CONFIGDEP_VERSION,
        ]:
            self.assertIn('version installed in tools directory: ' + tool_version, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(f'{self.temp_tools_dir}/tools/openocd-esp32/{OPENOCD_VERSION}/openocd-esp32/bin', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/xtensa-esp-elf/{XTENSA_ELF_VERSION}/xtensa-esp-elf/bin', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/esp32ulp-elf/{ESP32ULP_VERSION}/esp32ulp-elf/bin', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/riscv32-esp-elf/{RISCV_ELF_VERSION}/riscv32-esp-elf/bin', output)
        self.assertIn(
            f'{self.temp_tools_dir}/tools/xtensa-esp-elf-gdb/{XTENSA_ESP_GDB_VERSION}/xtensa-esp-elf-gdb/bin',
            output,
        )
        self.assertNotIn(
            f'{self.temp_tools_dir}/tools/riscv32-esp-elf-gdb/{RISCV_ESP_GDB_VERSION}/riscv32-esp-elf-gdb/bin',
            output,
        )
        self.assertIn(f'{self.temp_tools_dir}/tools/esp-rom-elfs/{ESP_ROM_ELFS_VERSION}/', output)
        self.assertIn(f'{self.temp_tools_dir}/tools/esp-clangd/{CLANGD_VERSION}/esp-clangd/bin', output)
        self.assertIn(
            f'{self.temp_tools_dir}/tools/{ESP_IDF_CONFIGDEP}/{ESP_IDF_CONFIGDEP_VERSION}/'
            f'{ESP_IDF_CONFIGDEP}-{ESP_IDF_CONFIGDEP_VERSION}/bin',
            output,
        )

    def test_tools_for_esp32p4(self):
        required_tools_installed = 6
        output = self.run_idf_tools_with_action(['install', '--targets=esp32p4'])
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_not_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        for tool_version in [
            OPENOCD_VERSION,
            RISCV_ELF_VERSION,
            ESP_ROM_ELFS_VERSION,
            RISCV_ESP_GDB_VERSION,
            ESP_IDF_CONFIGDEP_VERSION,
        ]:
            self.assertIn(f'version installed in tools directory: {tool_version}', output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', OPENOCD, OPENOCD_VERSION, OPENOCD, 'bin'), output)
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF, 'bin'), output
        )
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION, ''), output)
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION, RISCV_ESP_GDB, 'bin'),
            output,
        )
        self.assertIn(
            os.path.join(
                self.temp_tools_dir,
                'tools',
                ESP_IDF_CONFIGDEP,
                ESP_IDF_CONFIGDEP_VERSION,
                f'{ESP_IDF_CONFIGDEP}-{ESP_IDF_CONFIGDEP_VERSION}',
                'bin',
            ),
            output,
        )
        self.assertNotIn(
            os.path.join(self.temp_tools_dir, 'tools', ESP32ULP, ESP32ULP_VERSION, ESP32ULP, 'bin'), output
        )
        self.assertNotIn(
            os.path.join(self.temp_tools_dir, 'tools', XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF, 'bin'), output
        )
        self.assertNotIn(
            os.path.join(self.temp_tools_dir, 'tools', XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION, XTENSA_ESP_GDB, 'bin'),
            output,
        )
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', CLANGD, CLANGD_VERSION, CLANGD, 'bin'), output)

    # a different test for qemu because of "on_request"
    def test_tools_for_qemu_with_required(self):
        required_tools_installed = 11
        output = self.run_idf_tools_with_action(['install', 'required', 'qemu-xtensa', 'qemu-riscv32'])
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assert_tool_installed(output, QEMU_RISCV, QEMU_RISCV_VERSION)
        self.assert_tool_installed(output, QEMU_XTENSA, QEMU_XTENSA_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

    def test_export_supported_version_cmake(self):
        tool_to_test = 'cmake'
        self.run_idf_tools_with_action(['install'])
        output = self.run_idf_tools_with_action(['export'])

        self.assertNotIn(tool_to_test, output)


# TestUsageWin tests installed tools on Windows platforms
@unittest.skipIf(sys.platform != 'win32', reason='Tools for WIN differ')
class TestUsageWin(TestUsage):
    def test_usage_basic_win(self):
        output = self.run_idf_tools_with_action(['list'])
        self.assertIn(f'* {ESP32ULP}:', output)
        self.assertIn(f'- {ESP32ULP_VERSION} (recommended)', output)
        self.assertIn(f'* {OPENOCD}:', output)
        self.assertIn(f'- {OPENOCD_VERSION} (recommended)', output)
        self.assertIn(f'* {RISCV_ELF}:', output)
        self.assertIn(f'- {RISCV_ELF_VERSION} (recommended)', output)
        self.assertIn(f'* {XTENSA_ELF}:', output)
        self.assertIn(f'- {XTENSA_ELF_VERSION} (recommended)', output)
        self.assertIn(f'* {CLANGD}:', output)
        self.assertIn(f'- {CLANGD_VERSION} (recommended)', output)

        required_tools_installed = 13
        output = self.run_idf_tools_with_action(['install'])
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_installed(output, CMAKE, CMAKE_VERSION)
        self.assert_tool_installed(output, NINJA, NINJA_VERSION)
        self.assert_tool_installed(output, IDF_EXE, IDF_EXE_VERSION)
        self.assert_tool_installed(output, CCACHE, CCACHE_VERSION)
        self.assert_tool_installed(output, DFU_UTIL, DFU_UTIL_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        for tool_version in [
            ESP32ULP_VERSION,
            OPENOCD_VERSION,
            RISCV_ELF_VERSION,
            XTENSA_ELF_VERSION,
            XTENSA_ESP_GDB_VERSION,
            RISCV_ESP_GDB_VERSION,
            ESP_ROM_ELFS_VERSION,
            CMAKE_VERSION,
            NINJA_VERSION,
            IDF_EXE_VERSION,
            CCACHE_VERSION,
            DFU_UTIL_VERSION,
            CLANGD_VERSION,
            ESP_IDF_CONFIGDEP_VERSION,
        ]:
            self.assertIn('version installed in tools directory: ' + tool_version, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'esp32ulp-elf', ESP32ULP_VERSION, 'esp32ulp-elf', 'bin'), output
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'xtensa-esp-elf', XTENSA_ELF_VERSION, 'xtensa-esp-elf', 'bin'),
            output,
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'openocd-esp32', OPENOCD_VERSION, 'openocd-esp32', 'bin'), output
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'riscv32-esp-elf', RISCV_ELF_VERSION, 'riscv32-esp-elf', 'bin'),
            output,
        )
        self.assertIn(
            os.path.join(
                self.temp_tools_dir, 'tools', 'xtensa-esp-elf-gdb', XTENSA_ESP_GDB_VERSION, 'xtensa-esp-elf-gdb', 'bin'
            ),
            output,
        )
        self.assertIn(
            os.path.join(
                self.temp_tools_dir, 'tools', 'riscv32-esp-elf-gdb', RISCV_ESP_GDB_VERSION, 'riscv32-esp-elf-gdb', 'bin'
            ),
            output,
        )
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', 'esp-rom-elfs', ESP_ROM_ELFS_VERSION), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', CMAKE, CMAKE_VERSION, 'bin'), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', NINJA, NINJA_VERSION), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', IDF_EXE, IDF_EXE_VERSION), output)
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', CCACHE, CCACHE_VERSION, 'ccache-4.12.1-windows-x86_64'), output
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', DFU_UTIL, DFU_UTIL_VERSION, 'dfu-util-0.11-win64'), output
        )
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', CLANGD, CLANGD_VERSION, CLANGD, 'bin'), output)
        self.assertIn(
            os.path.join(
                self.temp_tools_dir,
                'tools',
                ESP_IDF_CONFIGDEP,
                ESP_IDF_CONFIGDEP_VERSION,
                f'{ESP_IDF_CONFIGDEP}-{ESP_IDF_CONFIGDEP_VERSION}',
                'bin',
            ),
            output,
        )

        output = self.run_idf_tools_with_action(['list', '--outdated'])
        self.assertEqual('', output)

        tools_json_outdated = os.path.join(self.temp_tools_dir, 'tools', 'tools.outdated.json')
        new_version = 'zzzzzz'
        self.run_idf_tools_with_action(
            [
                'add-version',
                '--tool',
                XTENSA_ELF,
                '--url-prefix',
                'http://test.com',
                '--version',
                new_version,
                '--override',
                '--checksum-file',
                'add_version/checksum.sha256',
                '--output',
                tools_json_outdated,
            ]
        )

        output = self.run_idf_tools_with_action(['--tools-json', tools_json_outdated, 'list', '--outdated'])
        self.assertIn((f'{XTENSA_ELF}: version {XTENSA_ELF_VERSION} is outdated by {new_version}'), output)

    def test_tools_for_esp32_win(self):
        required_tools_installed = 10
        output = self.run_idf_tools_with_action(['install', '--targets=esp32'])
        self.assert_tool_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_not_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_installed(output, CMAKE, CMAKE_VERSION)
        self.assert_tool_installed(output, NINJA, NINJA_VERSION)
        self.assert_tool_installed(output, IDF_EXE, IDF_EXE_VERSION)
        self.assert_tool_installed(output, CCACHE, CCACHE_VERSION)
        self.assert_tool_not_installed(output, DFU_UTIL, DFU_UTIL_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        for tool_version in [
            ESP32ULP_VERSION,
            OPENOCD_VERSION,
            XTENSA_ELF_VERSION,
            XTENSA_ESP_GDB_VERSION,
            ESP_ROM_ELFS_VERSION,
            CMAKE_VERSION,
            NINJA_VERSION,
            IDF_EXE_VERSION,
            CCACHE_VERSION,
            CLANGD_VERSION,
            ESP_IDF_CONFIGDEP_VERSION,
        ]:
            self.assertIn('version installed in tools directory: ' + tool_version, output)
        self.assertNotIn('version installed in tools directory: ' + DFU_UTIL_VERSION, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'esp32ulp-elf', ESP32ULP_VERSION, 'esp32ulp-elf', 'bin'), output
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'xtensa-esp-elf', XTENSA_ELF_VERSION, 'xtensa-esp-elf', 'bin'),
            output,
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'openocd-esp32', OPENOCD_VERSION, 'openocd-esp32', 'bin'), output
        )
        self.assertIn(
            os.path.join(
                self.temp_tools_dir, 'tools', 'xtensa-esp-elf-gdb', XTENSA_ESP_GDB_VERSION, 'xtensa-esp-elf-gdb', 'bin'
            ),
            output,
        )
        self.assertNotIn(
            os.path.join(self.temp_tools_dir, 'tools', 'riscv32-esp-elf', RISCV_ELF_VERSION, 'riscv32-esp-elf', 'bin'),
            output,
        )
        self.assertNotIn(
            os.path.join(
                self.temp_tools_dir, 'tools', 'riscv32-esp-elf-gdb', RISCV_ESP_GDB_VERSION, 'riscv32-esp-elf-gdb', 'bin'
            ),
            output,
        )
        self.assertIn(
            os.path.join(
                self.temp_tools_dir,
                'tools',
                'esp-rom-elfs',
                ESP_ROM_ELFS_VERSION,
            ),
            output,
        )
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', CMAKE, CMAKE_VERSION, 'bin'), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', NINJA, NINJA_VERSION), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', IDF_EXE, IDF_EXE_VERSION), output)
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', CCACHE, CCACHE_VERSION, 'ccache-4.12.1-windows-x86_64'), output
        )
        self.assertNotIn(
            os.path.join(self.temp_tools_dir, 'tools', DFU_UTIL, DFU_UTIL_VERSION, 'dfu-util-0.11-win64'), output
        )
        self.assertNotIn(os.path.join(self.temp_tools_dir, 'tools', CLANGD, CLANGD_VERSION, CLANGD, 'bin'), output)
        self.assertIn(
            os.path.join(
                self.temp_tools_dir,
                'tools',
                ESP_IDF_CONFIGDEP,
                ESP_IDF_CONFIGDEP_VERSION,
                f'{ESP_IDF_CONFIGDEP}-{ESP_IDF_CONFIGDEP_VERSION}',
                'bin',
            ),
            output,
        )

    def test_tools_for_esp32c3_win(self):
        required_tools_installed = 9
        output = self.run_idf_tools_with_action(['install', '--targets=esp32c3'])
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_not_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_installed(output, CMAKE, CMAKE_VERSION)
        self.assert_tool_installed(output, NINJA, NINJA_VERSION)
        self.assert_tool_installed(output, IDF_EXE, IDF_EXE_VERSION)
        self.assert_tool_installed(output, CCACHE, CCACHE_VERSION)
        self.assert_tool_not_installed(output, DFU_UTIL, DFU_UTIL_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        for tool_version in [
            OPENOCD_VERSION,
            RISCV_ELF_VERSION,
            RISCV_ESP_GDB_VERSION,
            ESP_ROM_ELFS_VERSION,
            CMAKE_VERSION,
            NINJA_VERSION,
            IDF_EXE_VERSION,
            CCACHE_VERSION,
            CLANGD_VERSION,
            ESP_IDF_CONFIGDEP_VERSION,
        ]:
            self.assertIn('version installed in tools directory: ' + tool_version, output)
        self.assertNotIn('version installed in tools directory: ' + DFU_UTIL_VERSION, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'openocd-esp32', OPENOCD_VERSION, 'openocd-esp32', 'bin'), output
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'riscv32-esp-elf', RISCV_ELF_VERSION, 'riscv32-esp-elf', 'bin'),
            output,
        )
        self.assertNotIn(
            os.path.join(self.temp_tools_dir, 'tools', 'esp32ulp-elf', ESP32ULP_VERSION, 'esp32ulp-elf', 'bin'), output
        )
        self.assertNotIn(
            os.path.join(self.temp_tools_dir, 'tools', 'xtensa-esp-elf', XTENSA_ELF_VERSION, 'xtensa-esp-elf', 'bin'),
            output,
        )
        self.assertNotIn(
            os.path.join(
                self.temp_tools_dir, 'tools', 'xtensa-esp-elf-gdb', XTENSA_ESP_GDB_VERSION, 'xtensa-esp-elf-gdb', 'bin'
            ),
            output,
        )
        self.assertIn(
            os.path.join(
                self.temp_tools_dir,
                'tools',
                'esp-rom-elfs',
                ESP_ROM_ELFS_VERSION,
            ),
            output,
        )
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', CMAKE, CMAKE_VERSION, 'bin'), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', NINJA, NINJA_VERSION), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', IDF_EXE, IDF_EXE_VERSION), output)
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', CCACHE, CCACHE_VERSION, 'ccache-4.12.1-windows-x86_64'), output
        )
        self.assertNotIn(
            os.path.join(self.temp_tools_dir, 'tools', DFU_UTIL, DFU_UTIL_VERSION, 'dfu-util-0.11-win64'), output
        )
        self.assertNotIn(os.path.join(self.temp_tools_dir, 'tools', CLANGD, CLANGD_VERSION, CLANGD, 'bin'), output)
        self.assertIn(
            os.path.join(
                self.temp_tools_dir,
                'tools',
                ESP_IDF_CONFIGDEP,
                ESP_IDF_CONFIGDEP_VERSION,
                f'{ESP_IDF_CONFIGDEP}-{ESP_IDF_CONFIGDEP_VERSION}',
                'bin',
            ),
            output,
        )

    def test_tools_for_esp32s2_win(self):
        required_tools_installed = 12
        output = self.run_idf_tools_with_action(['install', '--targets=esp32s2'])
        self.assert_tool_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_not_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_installed(output, CMAKE, CMAKE_VERSION)
        self.assert_tool_installed(output, NINJA, NINJA_VERSION)
        self.assert_tool_installed(output, IDF_EXE, IDF_EXE_VERSION)
        self.assert_tool_installed(output, CCACHE, CCACHE_VERSION)
        self.assert_tool_installed(output, DFU_UTIL, DFU_UTIL_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        for tool_version in [
            OPENOCD_VERSION,
            XTENSA_ELF_VERSION,
            XTENSA_ESP_GDB_VERSION,
            DFU_UTIL_VERSION,
            ESP_ROM_ELFS_VERSION,
            CMAKE_VERSION,
            NINJA_VERSION,
            IDF_EXE_VERSION,
            CCACHE_VERSION,
            CLANGD_VERSION,
            ESP_IDF_CONFIGDEP_VERSION,
        ]:
            self.assertIn('version installed in tools directory: ' + tool_version, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'xtensa-esp-elf', XTENSA_ELF_VERSION, 'xtensa-esp-elf', 'bin'),
            output,
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'openocd-esp32', OPENOCD_VERSION, 'openocd-esp32', 'bin'), output
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'esp32ulp-elf', ESP32ULP_VERSION, 'esp32ulp-elf', 'bin'), output
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'riscv32-esp-elf', RISCV_ELF_VERSION, 'riscv32-esp-elf', 'bin'),
            output,
        )
        self.assertIn(
            os.path.join(
                self.temp_tools_dir, 'tools', 'xtensa-esp-elf-gdb', XTENSA_ESP_GDB_VERSION, 'xtensa-esp-elf-gdb', 'bin'
            ),
            output,
        )
        self.assertNotIn(
            os.path.join(
                self.temp_tools_dir, 'tools', 'riscv32-esp-elf-gdb', RISCV_ESP_GDB_VERSION, 'riscv32-esp-elf-gdb', 'bin'
            ),
            output,
        )
        self.assertIn(
            os.path.join(
                self.temp_tools_dir,
                'tools',
                'esp-rom-elfs',
                ESP_ROM_ELFS_VERSION,
            ),
            output,
        )
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', CMAKE, CMAKE_VERSION, 'bin'), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', NINJA, NINJA_VERSION), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', IDF_EXE, IDF_EXE_VERSION), output)
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', CCACHE, CCACHE_VERSION, 'ccache-4.12.1-windows-x86_64'), output
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', DFU_UTIL, DFU_UTIL_VERSION, 'dfu-util-0.11-win64'), output
        )
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', CLANGD, CLANGD_VERSION, CLANGD, 'bin'), output)
        self.assertIn(
            os.path.join(
                self.temp_tools_dir,
                'tools',
                ESP_IDF_CONFIGDEP,
                ESP_IDF_CONFIGDEP_VERSION,
                f'{ESP_IDF_CONFIGDEP}-{ESP_IDF_CONFIGDEP_VERSION}',
                'bin',
            ),
            output,
        )

    def test_tools_for_esp32s3_win(self):
        required_tools_installed = 12
        output = self.run_idf_tools_with_action(['install', '--targets=esp32s3'])
        print(output)
        self.assert_tool_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_not_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_installed(output, CMAKE, CMAKE_VERSION)
        self.assert_tool_installed(output, NINJA, NINJA_VERSION)
        self.assert_tool_installed(output, IDF_EXE, IDF_EXE_VERSION)
        self.assert_tool_installed(output, CCACHE, CCACHE_VERSION)
        self.assert_tool_installed(output, DFU_UTIL, DFU_UTIL_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        for tool_version in [
            OPENOCD_VERSION,
            XTENSA_ELF_VERSION,
            XTENSA_ESP_GDB_VERSION,
            RISCV_ESP_GDB_VERSION,
            DFU_UTIL_VERSION,
            ESP_ROM_ELFS_VERSION,
            CMAKE_VERSION,
            NINJA_VERSION,
            IDF_EXE_VERSION,
            CCACHE_VERSION,
            CLANGD_VERSION,
            ESP_IDF_CONFIGDEP_VERSION,
        ]:
            self.assertIn('version installed in tools directory: ' + tool_version, output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'openocd-esp32', OPENOCD_VERSION, 'openocd-esp32', 'bin'), output
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'xtensa-esp-elf', XTENSA_ELF_VERSION, 'xtensa-esp-elf', 'bin'),
            output,
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'esp32ulp-elf', ESP32ULP_VERSION, 'esp32ulp-elf', 'bin'), output
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', 'riscv32-esp-elf', RISCV_ELF_VERSION, 'riscv32-esp-elf', 'bin'),
            output,
        )
        self.assertIn(
            os.path.join(
                self.temp_tools_dir, 'tools', 'xtensa-esp-elf-gdb', XTENSA_ESP_GDB_VERSION, 'xtensa-esp-elf-gdb', 'bin'
            ),
            output,
        )
        self.assertNotIn(
            os.path.join(
                self.temp_tools_dir, 'tools', 'riscv32-esp-elf-gdb', RISCV_ESP_GDB_VERSION, 'riscv32-esp-elf-gdb', 'bin'
            ),
            output,
        )
        self.assertIn(
            os.path.join(
                self.temp_tools_dir,
                'tools',
                'esp-rom-elfs',
                ESP_ROM_ELFS_VERSION,
            ),
            output,
        )
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', CMAKE, CMAKE_VERSION, 'bin'), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', NINJA, NINJA_VERSION), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', IDF_EXE, IDF_EXE_VERSION), output)
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', CCACHE, CCACHE_VERSION, 'ccache-4.12.1-windows-x86_64'), output
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', DFU_UTIL, DFU_UTIL_VERSION, 'dfu-util-0.11-win64'), output
        )
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', CLANGD, CLANGD_VERSION, CLANGD, 'bin'), output)
        self.assertIn(
            os.path.join(
                self.temp_tools_dir,
                'tools',
                ESP_IDF_CONFIGDEP,
                ESP_IDF_CONFIGDEP_VERSION,
                f'{ESP_IDF_CONFIGDEP}-{ESP_IDF_CONFIGDEP_VERSION}',
                'bin',
            ),
            output,
        )

    def test_tools_for_esp32p4_win(self):
        required_tools_installed = 9
        output = self.run_idf_tools_with_action(['install', '--targets=esp32p4'])
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, CMAKE, CMAKE_VERSION)
        self.assert_tool_installed(output, NINJA, NINJA_VERSION)
        self.assert_tool_installed(output, IDF_EXE, IDF_EXE_VERSION)
        self.assert_tool_installed(output, CCACHE, CCACHE_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_not_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_not_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_not_installed(output, DFU_UTIL, DFU_UTIL_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assertEqual(required_tools_installed, output.count('Done'))

        output = self.run_idf_tools_with_action(['check'])
        for tool_version in [
            RISCV_ELF_VERSION,
            RISCV_ESP_GDB_VERSION,
            ESP_ROM_ELFS_VERSION,
            OPENOCD_VERSION,
            CMAKE_VERSION,
            NINJA_VERSION,
            IDF_EXE_VERSION,
            CCACHE_VERSION,
            CLANGD_VERSION,
            ESP_IDF_CONFIGDEP_VERSION,
        ]:
            self.assertIn(f'version installed in tools directory: {tool_version}', output)

        output = self.run_idf_tools_with_action(['export'])
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF, 'bin'), output
        )
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION, RISCV_ESP_GDB, 'bin'),
            output,
        )
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', OPENOCD, OPENOCD_VERSION, OPENOCD, 'bin'), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', CMAKE, CMAKE_VERSION, 'bin'), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', NINJA, NINJA_VERSION), output)
        self.assertIn(os.path.join(self.temp_tools_dir, 'tools', IDF_EXE, IDF_EXE_VERSION), output)
        self.assertIn(
            os.path.join(self.temp_tools_dir, 'tools', CCACHE, CCACHE_VERSION, 'ccache-4.12.1-windows-x86_64'), output
        )
        self.assertIn(
            os.path.join(
                self.temp_tools_dir,
                'tools',
                ESP_IDF_CONFIGDEP,
                ESP_IDF_CONFIGDEP_VERSION,
                f'{ESP_IDF_CONFIGDEP}-{ESP_IDF_CONFIGDEP_VERSION}',
                'bin',
            ),
            output,
        )
        self.assertNotIn(
            os.path.join(self.temp_tools_dir, 'tools', XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF, 'bin'), output
        )
        self.assertNotIn(
            os.path.join(self.temp_tools_dir, 'tools', ESP32ULP, ESP32ULP_VERSION, ESP32ULP, 'bin'), output
        )
        self.assertNotIn(
            os.path.join(self.temp_tools_dir, 'tools', XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION, XTENSA_ESP_GDB, 'bin'),
            output,
        )
        self.assertNotIn(
            os.path.join(self.temp_tools_dir, 'tools', DFU_UTIL, DFU_UTIL_VERSION, 'dfu-util-0.11-win64'), output
        )
        self.assertNotIn(os.path.join(self.temp_tools_dir, 'tools', CLANGD, CLANGD_VERSION, CLANGD, 'bin'), output)

    # a different test for qemu because of "on_request"
    def test_tools_for_qemu_with_required_win(self):
        required_tools_installed = 15
        output = self.run_idf_tools_with_action(['install', 'required', 'qemu-xtensa', 'qemu-riscv32'])
        self.assert_tool_installed(output, OPENOCD, OPENOCD_VERSION)
        self.assert_tool_installed(output, RISCV_ELF, RISCV_ELF_VERSION, RISCV_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, XTENSA_ELF, XTENSA_ELF_VERSION, XTENSA_ELF_ARCHIVE_PATTERN)
        self.assert_tool_installed(output, ESP32ULP, ESP32ULP_VERSION)
        self.assert_tool_installed(output, XTENSA_ESP_GDB, XTENSA_ESP_GDB_VERSION)
        self.assert_tool_installed(output, RISCV_ESP_GDB, RISCV_ESP_GDB_VERSION)
        self.assert_tool_installed(output, ESP_ROM_ELFS, ESP_ROM_ELFS_VERSION)
        self.assert_tool_installed(output, QEMU_RISCV, QEMU_RISCV_VERSION)
        self.assert_tool_installed(output, QEMU_XTENSA, QEMU_XTENSA_VERSION)
        self.assert_tool_installed(output, CMAKE, CMAKE_VERSION)
        self.assert_tool_installed(output, NINJA, NINJA_VERSION)
        self.assert_tool_installed(output, IDF_EXE, IDF_EXE_VERSION)
        self.assert_tool_installed(output, CCACHE, CCACHE_VERSION)
        self.assert_tool_installed(output, DFU_UTIL, DFU_UTIL_VERSION)
        self.assert_tool_installed(output, CLANGD, CLANGD_VERSION, CLANGD_ARCHIVE)
        self.assert_tool_installed(output, ESP_IDF_CONFIGDEP, ESP_IDF_CONFIGDEP_VERSION)
        self.assertIn('Destination: {}'.format(os.path.join(self.temp_tools_dir, 'dist')), output)
        self.assertEqual(required_tools_installed, output.count('Done'))


class TestMaintainer(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        idf_path = os.getenv('IDF_PATH')
        assert idf_path, 'IDF_PATH needs to be set to run this test'
        cls.tools_old = os.path.join(idf_path, 'tools/tools.json')
        cls.tools_new = os.path.join(idf_path, 'tools/tools.new.json')
        cls.test_tool_name = 'xtensa-esp-elf'

    def test_validation(self):
        idf_tools.main(['validate'])

    def test_json_rewrite(self):
        idf_tools.main(['rewrite'])
        with open(self.tools_old) as f:
            json_old = f.read()
        with open(self.tools_new) as f:
            json_new = f.read()
        self.assertEqual(json_old, json_new, "Please check 'tools/tools.new.json' to find a cause!")

    def add_version_get_expected_json(self, addition_file, replace=False):
        with open(self.tools_old) as f:
            expected_json = json.load(f)
        with open(addition_file) as f:
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
        with open('add_version/artifact_input.json') as f:
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
                '--artifact-file',
            ]
            + filenames
        )
        expected_json = self.add_version_get_expected_json('add_version/artifact_expected_addition.json')
        with open(self.tools_new) as f1:
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
        with open(self.tools_new) as f1:
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
                'add_version/checksum.sha256',
            ]
        )
        expected_json = self.add_version_get_expected_json('add_version/checksum_expected_override.json', True)
        with open(self.tools_new) as f1:
            self.assertEqual(json.load(f1), expected_json, "Please check 'tools/tools.new.json' to find a cause!")


class TestArmDetection(unittest.TestCase):
    ELF_HEADERS = {
        idf_tools.PLATFORM_LINUX_ARM64: 'platform_detection/arm64_header.elf',
        idf_tools.PLATFORM_LINUX_ARMHF: 'platform_detection/armhf_header.elf',
        idf_tools.PLATFORM_LINUX_ARM32: 'platform_detection/arm32_header.elf',
    }

    ARM_PLATFORMS = {
        idf_tools.PLATFORM_LINUX_ARM64,
        idf_tools.PLATFORM_LINUX_ARMHF,
        idf_tools.PLATFORM_LINUX_ARM32,
    }

    def test_arm_detection(self):
        for platform in idf_tools.Platforms.PLATFORM_FROM_NAME.values():
            # use invalid ELF as executable. In this case passed parameter must return
            with patch('sys.executable', __file__):
                self.assertEqual(idf_tools.Platforms.detect_linux_arm_platform(platform), platform)
        # detect_linux_arm_platform() intended to return arch that detected in sys.executable ELF
        arm_platforms = (idf_tools.PLATFORM_LINUX_ARM64, idf_tools.PLATFORM_LINUX_ARMHF, idf_tools.PLATFORM_LINUX_ARM32)
        for exec_platform in arm_platforms:
            with patch('sys.executable', TestArmDetection.ELF_HEADERS[exec_platform]):
                for platform in TestArmDetection.ARM_PLATFORMS:
                    self.assertEqual(idf_tools.Platforms.detect_linux_arm_platform(platform), exec_platform)


DOWNLOAD_PAYLOAD = bytes(range(256)) * 4096
DOWNLOAD_CUT = 300000


class DownloadHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.server.paths.append(self.path)
        self.server.ranges.append(self.headers.get('Range'))
        behaviours = self.server.behaviours
        # Requests after the last behaviour repeat it, so one behaviour can answer every resume attempt.
        behaviours[min(len(self.server.ranges), len(behaviours)) - 1](self)

    def respond(self, sent=None, resume=True, status=206, offset_error=0, length=True, length_error=0, etag='"1"'):
        start = 0
        requested = self.headers.get('Range')
        if resume and requested:
            start = int(requested[len('bytes=') : -1]) + offset_error
            self.send_response(status)
            self.send_header('Content-Range', f'bytes {start}-{len(DOWNLOAD_PAYLOAD) - 1}/{len(DOWNLOAD_PAYLOAD)}')
        else:
            self.send_response(200)
        if length:
            self.send_header('Content-Length', str(len(DOWNLOAD_PAYLOAD) - start + length_error))
        if etag:
            self.send_header('ETag', etag)
        self.end_headers()
        self.wfile.write(DOWNLOAD_PAYLOAD[start:][:sent])

    def redirect(self):
        # Stands for a signed GitHub URL, which expires and must not be requested again by a resume.
        self.send_response(302)
        self.send_header('Location', f'/signed/{len(self.server.paths)}')
        self.send_header('Content-Length', '0')
        self.end_headers()

    def reset(self):
        self.respond(sent=DOWNLOAD_CUT)
        # On some systems a reset discards data the client has not read yet. The delay lets the client read the
        # headers first, because a failure before them is not resumed.
        time.sleep(0.2)
        self.connection.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack('ii', 1, 0))
        self.connection.close()

    def stall(self, length=True):
        self.respond(sent=DOWNLOAD_CUT, length=length)
        # Holds the connection open until the test ends, so the client always times out first.
        self.server.release.wait(10)

    def log_message(self, *args):
        pass


class TestDownloadResume(unittest.TestCase):
    def setUp(self):
        self.server = ThreadingHTTPServer(('127.0.0.1', 0), DownloadHandler)
        # Rejected responses are closed unread and some tests reset connections. The server would print a traceback
        # for each resulting BrokenPipeError or ConnectionResetError.
        self.server.handle_error = lambda request, client_address: None
        self.server.release = threading.Event()
        # The default poll interval would make shutdown() add half a second to each test.
        threading.Thread(target=self.server.serve_forever, args=(0.01,), daemon=True).start()
        self.addCleanup(self.server.server_close)
        self.addCleanup(self.server.shutdown)
        self.addCleanup(self.server.release.set)
        self.url = f'http://127.0.0.1:{self.server.server_port}/dl/archive.tar.gz'
        self.temp_dir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, self.temp_dir)
        self.destination = os.path.join(self.temp_dir, 'archive.tar.gz')
        for patcher in (
            # A proxy set in the environment must not intercept requests to the local server.
            patch.dict(os.environ, {'no_proxy': '*'}),
            patch.object(idf_tools, 'DOWNLOAD_RESUME_COUNT', 2),
            patch.object(idf_tools, 'DOWNLOAD_RESUME_MAX_DELAY', 0),
            patch.object(idf_tools.g, '_quiet', False),
            # Progress output is enabled to cover the line break printed before the resume warning.
            patch.object(idf_tools.g, '_non_interactive', False),
        ):
            patcher.start()
            self.addCleanup(patcher.stop)

    def serve(self, *behaviours):
        self.server.paths = []
        self.server.ranges = []
        self.server.behaviours = behaviours

    def download(self, *behaviours):
        self.serve(*behaviours)
        # One buffer for both streams keeps the order of progress output and warnings.
        output = StringIO()
        with redirect_stdout(output), redirect_stderr(output):
            err = idf_tools.download(self.url, self.destination)
        self.output = output.getvalue()
        return err

    def assert_downloaded(self, err):
        self.assertIsNone(err)
        with open(self.destination, 'rb') as f:
            self.assertEqual(f.read(), DOWNLOAD_PAYLOAD)

    def test_resume_after_connection_closed(self):
        err = self.download(lambda h: h.respond(sent=DOWNLOAD_CUT), DownloadHandler.respond)
        self.assert_downloaded(err)
        self.assertEqual(self.server.ranges, [None, f'bytes={DOWNLOAD_CUT}-'])
        self.assertIn(
            f'%\nWARNING: Download interrupted after {DOWNLOAD_CUT} of {len(DOWNLOAD_PAYLOAD)} bytes', self.output
        )

    def test_resume_from_original_url(self):
        err = self.download(
            DownloadHandler.redirect,
            lambda h: h.respond(sent=DOWNLOAD_CUT),
            DownloadHandler.redirect,
            DownloadHandler.respond,
        )
        self.assert_downloaded(err)
        self.assertEqual(self.server.paths, ['/dl/archive.tar.gz', '/signed/1', '/dl/archive.tar.gz', '/signed/3'])

    def test_resume_uses_timeout_and_context(self):
        self.serve(lambda h: h.respond(sent=DOWNLOAD_CUT), DownloadHandler.respond)
        context = ssl.create_default_context()
        with patch.object(idf_tools, 'urlopen', wraps=idf_tools.urlopen) as urlopen:
            with redirect_stderr(StringIO()):
                idf_tools.urlretrieve_ctx(self.url, self.destination, context=context)
        self.assertEqual(urlopen.call_count, 2)
        for call in urlopen.call_args_list:
            self.assertEqual(call.args[2], idf_tools.DOWNLOAD_TIMEOUT)
            self.assertIs(call.kwargs['context'], context)

    @unittest.skipIf(sys.platform == 'win32', reason='SO_LINGER has a different layout on Windows')
    def test_resume_after_connection_reset(self):
        err = self.download(DownloadHandler.reset, DownloadHandler.respond)
        self.assert_downloaded(err)
        self.assertEqual(len(self.server.ranges), 2)

    def test_resume_after_stall(self):
        with patch.object(idf_tools, 'DOWNLOAD_TIMEOUT', 1):
            err = self.download(DownloadHandler.stall, DownloadHandler.respond)
        self.assert_downloaded(err)
        self.assertIn('timed out', self.output)

    def test_no_resume_without_content_length(self):
        with patch.object(idf_tools, 'DOWNLOAD_TIMEOUT', 1):
            err = self.download(lambda h: h.stall(length=False))
        self.assertIsInstance(err, OSError)
        self.assertEqual(self.server.ranges, [None])

    def test_resume_after_more_drops_than_resume_count(self):
        step = 100000
        err = self.download(*([lambda h: h.respond(sent=step)] * 6), DownloadHandler.respond)
        self.assert_downloaded(err)
        self.assertEqual(len(self.server.ranges), 7)

    def test_resume_without_etag(self):
        for name, first_etag in (('no ETag', None), ('ETag only in the first response', '"1"')):
            with self.subTest(name):
                err = self.download(
                    lambda h: h.respond(sent=DOWNLOAD_CUT, etag=first_etag), lambda h: h.respond(etag=None)
                )
                self.assert_downloaded(err)

    def test_resume_rejected(self):
        for name, behaviour in (
            ('Range ignored', lambda h: h.respond(resume=False)),
            ('status 200 with Content-Range', lambda h: h.respond(status=200)),
            ('wrong Content-Range', lambda h: h.respond(offset_error=1)),
            ('wrong Content-Length', lambda h: h.respond(length_error=1)),
            ('changed ETag', lambda h: h.respond(etag='"2"')),
        ):
            with self.subTest(name):
                err = self.download(lambda h: h.respond(sent=DOWNLOAD_CUT), behaviour)
                self.assertIsInstance(err, idf_tools.ContentTooShortError)
                self.assertIn('did not send the rest of the same file', str(err))
                self.assertEqual(len(self.server.ranges), 2)

    def test_resume_without_progress(self):
        err = self.download(lambda h: h.respond(sent=DOWNLOAD_CUT), lambda h: h.send_error(404))
        self.assertIsInstance(err, idf_tools.ContentTooShortError)
        self.assertIn(f'got only {DOWNLOAD_CUT} out of {len(DOWNLOAD_PAYLOAD)} bytes', str(err))
        self.assertIn('404', str(err))
        self.assertEqual(len(self.server.ranges), 1 + idf_tools.DOWNLOAD_RESUME_COUNT)

    @unittest.skipUnless(os.path.exists('/dev/full'), reason='/dev/full is needed to simulate a full disk')
    def test_write_error_not_resumed(self):
        self.destination = '/dev/full'
        err = self.download(DownloadHandler.respond)
        self.assertIsInstance(err, OSError)
        self.assertEqual(err.errno, errno.ENOSPC)
        self.assertEqual(self.server.ranges, [None])

    def test_truncated_constraints_file_not_used(self):
        self.serve(lambda h: h.respond(sent=DOWNLOAD_CUT, resume=False))
        mirror = f'https://dl.espressif.com/dl/esp-idf,http://127.0.0.1:{self.server.server_port}/dl'
        # An invalid IDF_GITHUB_ASSETS from the environment would end get_constraints() before the download.
        with patch.dict(os.environ, {'IDF_MIRROR_PREFIX_MAP': mirror, 'IDF_GITHUB_ASSETS': ''}):
            with patch.object(idf_tools.g, '_idf_tools_path', self.temp_dir):
                with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
                    with self.assertRaises(SystemExit):
                        idf_tools.get_constraints('0.0')
        self.assertFalse(os.path.exists(os.path.join(self.temp_dir, 'espidf.constraints.v0.0.txt')))
        self.assertEqual(len(self.server.ranges), 2 * idf_tools.DOWNLOAD_RETRY_COUNT)


# TestSystemDependencies tests behaviour when system dependencies had been broken, on linux
@unittest.skipUnless(sys.platform == 'linux', reason='Tools for LINUX differ')
@unittest.skipUnless(
    int(os.getenv('IDF_TEST_MAY_BREAK_DEPENDENCIES', 0)) == 1, reason='Do not run destructive tests by default'
)
class TestSystemDependencies(TestUsageBase):
    @classmethod
    def tearDownClass(cls):
        # We won't restore dependencies because `apt-get update` and `apt-get install` require network access and take
        # a lot of time on the runners
        print(
            '\nINFO: System dependencies have been modified for TestSystemDependencies. '
            'Other tests may not work correctly'
        )
        super().tearDownClass()

    def test_commands_when_nodeps(self):
        # Prerequisites
        # (don't go to the setUpClass() because it's a part of this test case)

        self.run_idf_tools_with_action(['install', 'required', 'qemu-xtensa'])
        with open(os.devnull, 'w') as devnull:
            check_call(['dpkg', '--purge', 'libsdl2-2.0-0'], stdout=devnull, stderr=STDOUT)

        # Tests

        _, err_output = self.run_idf_tools_with_error(['list'])
        self.assertIn(
            f'ERROR: tool {QEMU_XTENSA} version {QEMU_XTENSA_VERSION} is installed, but cannot be run', err_output
        )

        _, err_output = self.run_idf_tools_with_error(['export'])
        self.assertIn(
            f'ERROR: tool {QEMU_XTENSA} version {QEMU_XTENSA_VERSION} is installed, but cannot be run', err_output
        )

        _, err_output = self.run_idf_tools_with_error(['check'], assertError=True)
        self.assertIn(
            f'ERROR: tool {QEMU_XTENSA} version {QEMU_XTENSA_VERSION} is installed, but cannot be run', err_output
        )

        _, err_output = self.run_idf_tools_with_error(['install', 'qemu-xtensa'], assertError=True)
        self.assertIn(
            f'ERROR: tool {QEMU_XTENSA} version {QEMU_XTENSA_VERSION} is installed, but getting error', err_output
        )
        self.assertIn('ERROR: Failed to check the tool while installed', err_output)


if __name__ == '__main__':
    unittest.main()
