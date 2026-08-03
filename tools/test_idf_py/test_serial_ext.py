# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import pathlib
import sys
from typing import Any
from unittest import mock

import pytest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

from idf_py_actions.tools import PropertyDict  # noqa: E402
from idf_py_actions.tools import get_selected_target  # noqa: E402


@pytest.fixture
def project_dir(tmp_path: pathlib.Path) -> str:
    return str(tmp_path)


@pytest.fixture
def args(project_dir: str, tmp_path: pathlib.Path) -> PropertyDict:
    build_dir = str(tmp_path / 'build')
    os.makedirs(build_dir, exist_ok=True)
    return PropertyDict(
        project_dir=project_dir,
        build_dir=build_dir,
        port=None,
        baud=460800,
        define_cache_entry=[],
        no_hints=False,
    )


class TestGetSelectedTarget:
    def test_returns_none_when_nothing_set(self, args: PropertyDict) -> None:
        with mock.patch.dict(os.environ, {}, clear=True):
            os.environ.pop('IDF_TARGET', None)
            os.environ.pop('SDKCONFIG_DEFAULTS', None)
            assert get_selected_target(args) is None

    def test_returns_target_from_env(self, args: PropertyDict) -> None:
        with mock.patch.dict(os.environ, {'IDF_TARGET': 'esp32s3'}):
            assert get_selected_target(args) == 'esp32s3'

    def test_returns_target_from_define_cache(self, args: PropertyDict) -> None:
        args.define_cache_entry = ['IDF_TARGET=esp32c3']
        with mock.patch.dict(os.environ, {}, clear=True):
            os.environ.pop('IDF_TARGET', None)
            os.environ.pop('SDKCONFIG_DEFAULTS', None)
            assert get_selected_target(args) == 'esp32c3'

    def test_returns_target_from_sdkconfig(self, args: PropertyDict, project_dir: str) -> None:
        sdkconfig_path = os.path.join(project_dir, 'sdkconfig')
        with open(sdkconfig_path, 'w') as f:
            f.write('CONFIG_IDF_TARGET="esp32h2"\n')
        with mock.patch.dict(os.environ, {}, clear=True):
            os.environ.pop('IDF_TARGET', None)
            os.environ.pop('SDKCONFIG_DEFAULTS', None)
            assert get_selected_target(args) == 'esp32h2'

    def test_returns_target_from_sdkconfig_defaults(self, args: PropertyDict, project_dir: str) -> None:
        defaults_path = os.path.join(project_dir, 'sdkconfig.defaults')
        with open(defaults_path, 'w') as f:
            f.write('CONFIG_IDF_TARGET="esp32c6"\n')
        with mock.patch.dict(os.environ, {}, clear=True):
            os.environ.pop('IDF_TARGET', None)
            os.environ.pop('SDKCONFIG_DEFAULTS', None)
            assert get_selected_target(args) == 'esp32c6'

    def test_returns_target_from_cmake_cache(self, args: PropertyDict) -> None:
        cache_path = os.path.join(args.build_dir, 'CMakeCache.txt')
        with open(cache_path, 'w') as f:
            f.write('IDF_TARGET:STRING=esp32s2\n')
        with mock.patch.dict(os.environ, {}, clear=True):
            os.environ.pop('IDF_TARGET', None)
            os.environ.pop('SDKCONFIG_DEFAULTS', None)
            assert get_selected_target(args) == 'esp32s2'

    def test_env_takes_priority_over_cache(self, args: PropertyDict) -> None:
        cache_path = os.path.join(args.build_dir, 'CMakeCache.txt')
        with open(cache_path, 'w') as f:
            f.write('IDF_TARGET:STRING=esp32s2\n')
        with mock.patch.dict(os.environ, {'IDF_TARGET': 'esp32c3'}):
            assert get_selected_target(args) == 'esp32c3'


class TestMonitorPortDetection:
    """Test that the monitor function builds correct args for idf_monitor."""

    @pytest.fixture
    def mock_esp(self) -> Any:
        esp = mock.MagicMock()
        esp.serial_port = '/dev/ttyUSB0'
        esp.CHIP_NAME = 'ESP32-C5'
        return esp

    def _get_monitor_fn(self) -> Any:
        from idf_py_actions.serial_ext import action_extensions

        ext = action_extensions({}, '')
        return ext['actions']['monitor']['callback']

    def _call_monitor(self, args: PropertyDict, port: str | None = None, no_reset: bool = False) -> None:
        args.port = port
        monitor_fn = self._get_monitor_fn()

        ctx = mock.MagicMock()
        ctx.info_name = 'idf.py'
        ctx._parameter_source = {'baud': mock.MagicMock()}

        monitor_fn(
            'monitor',
            ctx,
            args,
            print_filter=None,
            monitor_baud=None,
            encrypted=False,
            no_reset=no_reset,
            timestamps=False,
            timestamp_format=None,
            force_color=False,
            disable_auto_color=False,
        )

    def test_clean_project_autodetects_port_and_target(self, args: PropertyDict, mock_esp: Any) -> None:
        with mock.patch.dict(os.environ, {'IDF_PATH': '/idf'}, clear=False):
            os.environ.pop('IDF_TARGET', None)
            with mock.patch('idf_py_actions.serial_ext.get_default_esp', return_value=mock_esp):
                with mock.patch('idf_py_actions.serial_ext.RunTool') as mock_run:
                    self._call_monitor(args)

        call_args = mock_run.call_args[0][1]
        assert '-p' in call_args
        assert '/dev/ttyUSB0' in call_args
        assert '--target' in call_args
        assert 'esp32c5' in call_args

    def test_clean_project_with_explicit_port(self, args: PropertyDict) -> None:
        with mock.patch.dict(os.environ, {'IDF_PATH': '/idf'}, clear=False):
            os.environ.pop('IDF_TARGET', None)
            with mock.patch('idf_py_actions.serial_ext.get_default_serial_port', return_value='/dev/ttyACM0'):
                with mock.patch('idf_py_actions.serial_ext.RunTool') as mock_run:
                    self._call_monitor(args, port='/dev/ttyACM1')

        call_args = mock_run.call_args[0][1]
        assert '/dev/ttyACM1' in call_args

    def test_clean_project_with_target_set_uses_correct_port(self, args: PropertyDict) -> None:
        with mock.patch.dict(os.environ, {'IDF_PATH': '/idf', 'IDF_TARGET': 'esp32s3'}):
            with mock.patch(
                'idf_py_actions.serial_ext.get_default_serial_port', return_value='/dev/ttyUSB2'
            ) as mock_port:
                with mock.patch('idf_py_actions.serial_ext.RunTool') as mock_run:
                    self._call_monitor(args)

        mock_port.assert_called_with('esp32s3')
        call_args = mock_run.call_args[0][1]
        assert '/dev/ttyUSB2' in call_args
        assert '--target' in call_args
        assert 'esp32s3' in call_args

    def test_clean_project_with_target_set_no_autodetect(self, args: PropertyDict, mock_esp: Any) -> None:
        """When target is already set, get_default_esp should NOT be called."""
        with mock.patch.dict(os.environ, {'IDF_PATH': '/idf', 'IDF_TARGET': 'esp32s3'}):
            with mock.patch('idf_py_actions.serial_ext.get_default_esp', return_value=mock_esp) as mock_detect:
                with mock.patch('idf_py_actions.serial_ext.get_default_serial_port', return_value='/dev/ttyUSB0'):
                    with mock.patch('idf_py_actions.serial_ext.RunTool'):
                        self._call_monitor(args)

        mock_detect.assert_not_called()
