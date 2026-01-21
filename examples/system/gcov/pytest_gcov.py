# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os.path
import sys
import time
import typing

import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

if typing.TYPE_CHECKING:
    from conftest import OpenOCD

try:
    from gcov_capture import UartGcovCapture
    from gcov_capture import get_coverage_data
except ImportError:
    idf_path = os.getenv('IDF_PATH')
    if not idf_path:
        raise RuntimeError('IDF_PATH not found. Please run `source $IDF_PATH/export.sh`')
    esp_app_trace_dir = os.path.join(idf_path, 'tools', 'esp_app_trace')
    sys.path.insert(0, esp_app_trace_dir)
    from gcov_capture import UartGcovCapture
    from gcov_capture import get_coverage_data


def get_expected_gcda_paths(dut: IdfDut) -> list:
    """Get list of expected .gcda file paths for this example."""
    return [
        os.path.join(
            dut.app.binary_path, 'esp-idf', 'main', 'CMakeFiles', '__idf_main.dir', 'gcov_example_main.c.gcda'
        ),
        os.path.join(
            dut.app.binary_path, 'esp-idf', 'main', 'CMakeFiles', '__idf_main.dir', 'gcov_example_func.c.gcda'
        ),
        os.path.join(dut.app.binary_path, 'esp-idf', 'sample', 'CMakeFiles', '__idf_sample.dir', 'some_funcs.c.gcda'),
    ]


def prepare_test(dut: IdfDut) -> list:
    """Prepare test environment: create directories and clean up old .gcda files.

    Returns list of expected .gcda file paths.
    """
    # Create the generated .gcda folders
    # Normally created via `idf.py build`, but in CI non-related files aren't preserved
    os.makedirs(os.path.join(dut.app.binary_path, 'esp-idf', 'main', 'CMakeFiles', '__idf_main.dir'), exist_ok=True)
    os.makedirs(os.path.join(dut.app.binary_path, 'esp-idf', 'sample', 'CMakeFiles', '__idf_sample.dir'), exist_ok=True)

    # Get expected paths and clean up old files
    expected_gcda_paths = get_expected_gcda_paths(dut)
    for gcda_path in expected_gcda_paths:
        if os.path.isfile(gcda_path):
            try:
                os.remove(gcda_path)
                print(f'Removed old .gcda file: {os.path.basename(gcda_path)}')
            except OSError as e:
                print(f'Warning: Could not remove {gcda_path}: {e}')

    return expected_gcda_paths


def _test_gcov(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    expected_gcda_paths = prepare_test(dut)

    def expect_counter_output(loop: int, timeout: int = 10) -> None:
        dut.expect_exact(
            [f'blink_dummy_func: Counter = {loop}', f'some_dummy_func: Counter = {loop * 2}'],
            expect_all=True,
            timeout=timeout,
        )

    def dump_coverage(on_the_fly: bool, expected_counts: dict | None = None) -> None:
        response = openocd.gcov_dump(on_the_fly=on_the_fly)

        expect_lines = [
            'Targets connected.',
            'gcov_example_main.c.gcda',
            'gcov_example_func.c.gcda',
            'some_funcs.c.gcda',
            'Targets disconnected.',
        ]

        for line in response.splitlines():
            for expect in expect_lines[:]:
                if expect in line:
                    if expect.endswith('.gcda'):  # check file exists
                        file_path = line.split()[3].strip("'")
                        assert os.path.isfile(file_path)

                    expect_lines.remove(expect)

        assert len(expect_lines) == 0

        # Verify execution counts if expected and coverage data is available
        if expected_counts:
            coverage = get_coverage_data(dut.app.binary_path, expected_gcda_paths)
            if coverage:  # Only verify if detailed data is available
                print(f'Coverage data: {coverage}')
                for func, expected_count in expected_counts.items():
                    actual_count = coverage.get(func)
                    assert actual_count == expected_count, f'Expected {func}={expected_count}, got {actual_count}'
            else:
                # Backup verification: ensure .gcda files exist
                print('Coverage data not available (gcov tool not found)')
                for gcda_path in expected_gcda_paths:
                    assert os.path.isfile(gcda_path), f'Expected .gcda file not found: {gcda_path}'
                print('Basic verification passed (all .gcda files exist)')

    dut.expect_exact('example: Ready for OpenOCD connection', timeout=5)
    with openocd_dut.run() as openocd:
        openocd.write('reset run')
        dut.expect_exact('example: Ready for OpenOCD connection', timeout=5)

        expect_counter_output(0)
        dut.expect('Ready to dump GCOV data...', timeout=5)

        # Test two hard-coded dumps with verification
        dump_coverage(False, {'gcov_example_func.c:blink_dummy_func': 1, 'some_funcs.c:some_dummy_func': 1})
        dut.expect('GCOV data have been dumped.', timeout=5)
        expect_counter_output(1)
        dut.expect('Ready to dump GCOV data...', timeout=5)

        dump_coverage(False, {'gcov_example_func.c:blink_dummy_func': 2, 'some_funcs.c:some_dummy_func': 2})
        dut.expect('GCOV data have been dumped.', timeout=5)

        for i in range(2, 6):
            expect_counter_output(i)

        # Test instant run-time dumps with verification
        expected_runtime_counts = [
            {'gcov_example_func.c:blink_dummy_func': 7, 'some_funcs.c:some_dummy_func': 7},
            {'gcov_example_func.c:blink_dummy_func': 8, 'some_funcs.c:some_dummy_func': 8},
            {'gcov_example_func.c:blink_dummy_func': 10, 'some_funcs.c:some_dummy_func': 10},
        ]

        for expected in expected_runtime_counts:
            time.sleep(1)
            dump_coverage(True, expected)


@pytest.mark.jtag
@idf_parametrize('config', ['gcov_jtag'], indirect=['config'])
@idf_parametrize('target', ['esp32', 'esp32c2', 'esp32s2'], indirect=['target'])
def test_gcov(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_gcov(openocd_dut, dut)


@pytest.mark.usb_serial_jtag
@idf_parametrize('config', ['gcov_jtag'], indirect=['config'])
@idf_parametrize(
    'target', ['esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4'], indirect=['target']
)
def test_gcov_usj(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_gcov(openocd_dut, dut)


def _test_gcov_uart(dut: IdfDut) -> None:
    # Close console port to free up the port for the gcov capture
    dut.serial.close()

    expected_gcda_paths = prepare_test(dut)
    log_file = os.path.join(dut.logdir, 'gcov_uart.log')
    uart_port = dut.serial.port
    baud = dut.app.sdkconfig.get('APPTRACE_UART_BAUDRATE')
    with UartGcovCapture(port=uart_port, baudrate=baud, log_file=log_file, log_level=1) as uart_capture:
        uart_capture.run(background=True)
        time.sleep(0.5)

        # Expected execution counts
        expected_counts = [
            {'gcov_example_func.c:blink_dummy_func': 1, 'some_funcs.c:some_dummy_func': 1},  # First dump
            {'gcov_example_func.c:blink_dummy_func': 2, 'some_funcs.c:some_dummy_func': 2},  # Second dump
        ]

        # Verify each dump
        for dump_num, expected in enumerate(expected_counts, start=1):
            if uart_capture.wait_for_fstop(timeout=10.0):
                coverage = get_coverage_data(dut.app.binary_path, expected_gcda_paths)

                if coverage:  # Only verify details if coverage data is available
                    print(f'Coverage data: {coverage}')
                    for func, expected_count in expected.items():
                        actual_count = coverage.get(func)
                        assert actual_count == expected_count, f'Expected {func}={expected_count}, got {actual_count}'
                else:
                    # Backup verification: ensure .gcda files exist
                    print('Coverage data not available (gcov tool not found)')
                    for gcda_path in expected_gcda_paths:
                        assert os.path.isfile(gcda_path), f'Expected .gcda file not found: {gcda_path}'
                    print('Basic verification passed (all .gcda files exist)')
            else:
                assert False, f'esp_gcov_dump {dump_num} timeout'


@pytest.mark.generic
@idf_parametrize('config', ['gcov_uart'], indirect=['config'])
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_gcov_uart(dut: IdfDut) -> None:
    _test_gcov_uart(dut)
