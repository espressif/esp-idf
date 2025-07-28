# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os.path
import time
import typing

import pytest
from pytest_embedded_idf import IdfDut

if typing.TYPE_CHECKING:
    from conftest import OpenOCD


def _test_gcov(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    # create the generated .gcda folder, otherwise would have error: failed to open file.
    # normally this folder would be created via `idf.py build`. but in CI the non-related files would not be preserved
    os.makedirs(os.path.join(dut.app.binary_path, 'esp-idf', 'main', 'CMakeFiles', '__idf_main.dir'), exist_ok=True)
    os.makedirs(os.path.join(dut.app.binary_path, 'esp-idf', 'sample', 'CMakeFiles', '__idf_sample.dir'), exist_ok=True)

    def expect_counter_output(loop: int, timeout: int = 10) -> None:
        dut.expect_exact(
            [f'blink_dummy_func: Counter = {loop}', f'some_dummy_func: Counter = {loop * 2}'],
            expect_all=True,
            timeout=timeout,
        )

    def dump_coverage(cmd: str) -> None:
        response = openocd.write(cmd)

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

    dut.expect_exact('example: Ready for OpenOCD connection', timeout=5)
    with openocd_dut.run() as openocd:
        openocd.write('reset run')
        dut.expect_exact('example: Ready for OpenOCD connection', timeout=5)

        expect_counter_output(0)
        dut.expect('Ready to dump GCOV data...', timeout=5)

        # Test two hard-coded dumps
        dump_coverage('esp gcov dump')
        dut.expect('GCOV data have been dumped.', timeout=5)
        expect_counter_output(1)
        dut.expect('Ready to dump GCOV data...', timeout=5)
        dump_coverage('esp gcov dump')
        dut.expect('GCOV data have been dumped.', timeout=5)

        for i in range(2, 6):
            expect_counter_output(i)

        for _ in range(3):
            time.sleep(1)
            # Test instant run-time dump
            dump_coverage('esp gcov')


@pytest.mark.jtag
@pytest.mark.esp32
@pytest.mark.esp32c2
@pytest.mark.esp32s2
def test_gcov(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_gcov(openocd_dut, dut)


@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.usb_serial_jtag
def test_gcov_usj(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_gcov(openocd_dut, dut)
