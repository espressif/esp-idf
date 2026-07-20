# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Tests for unchanged local-header mirroring into .compressed_srcs."""

import json
import os
import shutil
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

import test_utils  # noqa: F401 — must be first to set up sys.path
from ble_log_compress import LogCompressor
from test_utils import BLUEDROID_SCRIPT
from test_utils import BLUEDROID_TAGS
from test_utils import write_yaml_config


class _HeaderMirrorTestCase(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = Path(tempfile.mkdtemp())
        self.code_base = self.tmp / 'code_base'
        self.mirror = self.tmp / 'build' / 'ble_log' / '.compressed_srcs'
        self.code_base.mkdir(parents=True)
        self.mirror.mkdir(parents=True)

        self.compressor = LogCompressor()
        self.compressor.code_base_path = self.code_base
        self.compressor.bt_compressed_srcs_path = self.mirror
        self.compressor.module_info = {'BLE_HOST': {'code_path': ['module']}}

    def tearDown(self) -> None:
        shutil.rmtree(self.tmp, ignore_errors=True)

    def write_source(self, relative: str, content: str) -> Path:
        path = self.code_base / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content)
        return path


class TestHeaderDiscoveryAndCopy(_HeaderMirrorTestCase):
    def test_recursively_mirrors_h_and_inc_only(self) -> None:
        private_h = self.write_source('module/src/private.h', '#define PRIVATE_VALUE 7\n')
        config_inc = self.write_source('module/shared/config.inc', '#define CONFIG_VALUE 9\n')
        self.write_source('module/src/ignored.c', 'int ignored;\n')
        self.write_source('module/CMakeLists.txt', 'set(ignored yes)\n')

        self.compressor.mirror_local_includes()

        mirrored_h = self.mirror / private_h.relative_to(self.code_base)
        mirrored_inc = self.mirror / config_inc.relative_to(self.code_base)
        self.assertEqual(mirrored_h.read_bytes(), private_h.read_bytes())
        self.assertEqual(mirrored_inc.read_bytes(), config_inc.read_bytes())
        self.assertEqual(mirrored_h.stat().st_mtime_ns, private_h.stat().st_mtime_ns)
        self.assertFalse((self.mirror / 'module/src/ignored.c').exists())
        self.assertFalse((self.mirror / 'module/CMakeLists.txt').exists())

        manifest = json.loads((self.mirror / '.mirrored_includes.json').read_text())
        self.assertEqual(
            manifest,
            {
                'version': 1,
                'files': ['module/shared/config.inc', 'module/src/private.h'],
            },
        )

    def test_overlapping_module_roots_copy_each_header_once(self) -> None:
        self.write_source('module/src/private.h', '#define PRIVATE_VALUE 7\n')
        self.compressor.module_info = {
            'BLE_HOST': {'code_path': ['module']},
            'BLE_ISO': {'code_path': ['module/src']},
        }

        with mock.patch('ble_log_compress.shutil.copy2', wraps=shutil.copy2) as copy2:
            self.compressor.mirror_local_includes()

        self.assertEqual(copy2.call_count, 1)

    def test_rejects_missing_and_non_directory_module_roots(self) -> None:
        cases = ('missing', 'not_a_directory')
        self.write_source('not_a_directory', 'not a directory\n')

        for code_path in cases:
            with self.subTest(code_path=code_path):
                self.compressor.module_info = {'BLE_HOST': {'code_path': [code_path]}}
                with self.assertRaisesRegex(ValueError, 'Invalid module code path'):
                    self.compressor.mirror_local_includes()

    def test_rejects_module_root_outside_code_base(self) -> None:
        outside = self.tmp / 'outside'
        outside.mkdir()
        link = self.code_base / 'module'
        try:
            link.symlink_to(outside, target_is_directory=True)
        except OSError as error:
            self.skipTest(f'Directory symlinks unavailable: {error}')

        with self.assertRaisesRegex(ValueError, 'escapes CODE_BASE_PATH'):
            self.compressor.mirror_local_includes()


class TestHeaderMirrorSynchronization(_HeaderMirrorTestCase):
    def test_modified_header_updates_content_and_timestamp(self) -> None:
        source = self.write_source('module/src/private.h', '#define PRIVATE_VALUE 7\n')
        self.compressor.mirror_local_includes()
        destination = self.mirror / 'module/src/private.h'

        updated_mtime = source.stat().st_mtime_ns + 2_000_000_000
        source.write_text('#define PRIVATE_VALUE 11\n')
        os.utime(source, ns=(updated_mtime, updated_mtime))
        self.compressor.mirror_local_includes()

        self.assertEqual(destination.read_bytes(), source.read_bytes())
        self.assertEqual(destination.stat().st_mtime_ns, source.stat().st_mtime_ns)

    def test_deleted_header_is_removed_without_touching_unmanaged_outputs(self) -> None:
        source = self.write_source('module/src/private.h', '#define PRIVATE_VALUE 7\n')
        self.compressor.mirror_local_includes()
        generated_c = self.mirror / 'module/src/generated.c'
        unmanaged_h = self.mirror / 'module/src/unmanaged.h'
        generated_c.write_text('int generated;\n')
        unmanaged_h.write_text('#define UNMANAGED 1\n')

        source.unlink()
        self.compressor.mirror_local_includes()

        self.assertFalse((self.mirror / 'module/src/private.h').exists())
        self.assertEqual(generated_c.read_text(), 'int generated;\n')
        self.assertEqual(unmanaged_h.read_text(), '#define UNMANAGED 1\n')
        manifest = json.loads((self.mirror / '.mirrored_includes.json').read_text())
        self.assertEqual(manifest, {'version': 1, 'files': []})

    def test_rejects_malformed_manifest_before_copying(self) -> None:
        source = self.write_source('module/src/private.h', '#define PRIVATE_VALUE 7\n')
        (self.mirror / '.mirrored_includes.json').write_text('{not-json')

        with self.assertRaisesRegex(ValueError, 'Invalid mirrored include manifest'):
            self.compressor.mirror_local_includes()

        self.assertFalse((self.mirror / source.relative_to(self.code_base)).exists())

    def test_rejects_unsafe_manifest_path_without_deleting_outside_file(self) -> None:
        outside = self.mirror.parent / 'outside.h'
        outside.write_text('#define OUTSIDE 1\n')
        manifest = {
            'version': 1,
            'files': ['../outside.h'],
        }
        (self.mirror / '.mirrored_includes.json').write_text(json.dumps(manifest))

        with self.assertRaisesRegex(ValueError, 'Invalid mirrored include path'):
            self.compressor.mirror_local_includes()

        self.assertEqual(outside.read_text(), '#define OUTSIDE 1\n')

    def test_manifest_replace_failure_preserves_previous_manifest(self) -> None:
        self.write_source('module/src/private.h', '#define PRIVATE_VALUE 7\n')
        self.compressor.mirror_local_includes()
        manifest_path = self.mirror / '.mirrored_includes.json'
        previous_manifest = manifest_path.read_bytes()
        second = self.write_source('module/src/second.h', '#define SECOND_VALUE 2\n')

        with mock.patch('ble_log_compress.os.replace', side_effect=OSError('replace failed')):
            with self.assertRaisesRegex(OSError, 'replace failed'):
                self.compressor.mirror_local_includes()

        self.assertEqual(manifest_path.read_bytes(), previous_manifest)
        second.unlink()
        self.compressor.mirror_local_includes()
        self.assertFalse((self.mirror / second.relative_to(self.code_base)).exists())

    def test_rejects_header_symlink_leaving_module_root(self) -> None:
        outside = self.write_source('outside.h', '#define OUTSIDE 1\n')
        link = self.code_base / 'module/src/external.h'
        link.parent.mkdir(parents=True, exist_ok=True)
        try:
            link.symlink_to(outside)
        except OSError as error:
            self.skipTest(f'File symlinks unavailable: {error}')

        with self.assertRaisesRegex(ValueError, 'Local include source escapes allowed root'):
            self.compressor.mirror_local_includes()


class TestHeaderMirrorPipeline(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = Path(tempfile.mkdtemp())

    def tearDown(self) -> None:
        shutil.rmtree(self.tmp, ignore_errors=True)

    def test_main_mirrors_same_directory_and_parent_relative_includes(self) -> None:
        code_base = self.tmp / 'code_base'
        build_dir = self.tmp / 'build'
        compressed_srcs = build_dir / 'ble_log' / '.compressed_srcs'
        source_dir = code_base / 'test_src' / 'src'
        shared_dir = code_base / 'test_src' / 'shared'
        source_dir.mkdir(parents=True)
        shared_dir.mkdir(parents=True)
        source = source_dir / 'local_include.c'
        private_h = source_dir / 'private.h'
        config_inc = shared_dir / 'config.inc'
        source.write_text(
            '#include "private.h"\n'
            '#include "../shared/config.inc"\n'
            'void local_include(void) { APPL_TRACE_DEBUG("value %d", PRIVATE_VALUE); }\n'
        )
        private_h.write_text('#define PRIVATE_VALUE 7\n')
        config_inc.write_text('#define CONFIG_VALUE 9\n')
        write_yaml_config(
            str(self.tmp),
            tags=BLUEDROID_TAGS,
            script_path=BLUEDROID_SCRIPT,
        )

        argv = [
            'ble_log_compress.py',
            'compress',
            '--srcs',
            'test_src/src/local_include.c',
            '--code_base_path',
            str(code_base),
            '--module',
            'BLE_HOST',
            '--build_path',
            str(build_dir),
            '--compressed_srcs_path',
            str(compressed_srcs),
        ]
        with mock.patch.object(sys, 'argv', argv):
            result = LogCompressor().main()

        self.assertEqual(result, 0)
        generated_c = compressed_srcs / 'test_src/src/local_include.c'
        mirrored_h = generated_c.parent / 'private.h'
        mirrored_inc = generated_c.parent / '../shared/config.inc'
        self.assertTrue(generated_c.exists())
        self.assertEqual(mirrored_h.read_bytes(), private_h.read_bytes())
        self.assertEqual(mirrored_inc.resolve().read_bytes(), config_inc.read_bytes())

    def test_cmake_does_not_add_nimble_local_header_directories(self) -> None:
        cmake_path = Path(__file__).resolve().parent.parent / 'CMakeLists.txt'
        cmake = cmake_path.read_text()
        self.assertNotIn('"host/nimble/nimble/nimble/host/src"', cmake)
        self.assertNotIn('"host/nimble/nimble/nimble/host/store/config/src"', cmake)


if __name__ == '__main__':
    unittest.main()
