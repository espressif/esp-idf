# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import os
import shutil
import tempfile
import typing as t
from itertools import zip_longest

import pytest
from pytest_embedded_idf import IdfDut

TEMP_DIR = tempfile.mkdtemp()
HOST_FILE_NAME = 'host_file.txt'
HOST_FILE_PATH = os.path.join(os.path.dirname(__file__), 'data', HOST_FILE_NAME)


@pytest.fixture(autouse=True)
def prepare() -> t.Generator[None, None, None]:
    shutil.copyfile(HOST_FILE_PATH, os.path.join(TEMP_DIR, HOST_FILE_NAME))

    yield

    shutil.rmtree(TEMP_DIR, ignore_errors=True)


@pytest.mark.jtag
@pytest.mark.parametrize(
    'embedded_services, no_gdb, openocd_cli_args',
    [
        pytest.param(
            'esp,idf,jtag',
            'y',
            f'-c \'set ESP_SEMIHOST_BASEDIR "{TEMP_DIR}"\' -f board/esp32-wrover-kit-3.3v.cfg',
            marks=[pytest.mark.esp32],
        ),
    ], ids=[
        'esp32',
    ],
    indirect=True,
)
def test_semihost_vfs(dut: IdfDut) -> None:
    dut.expect_exact('example: Switch to semihosted stdout')
    dut.expect_exact('example: Switched back to UART stdout')
    dut.expect_exact('example: Wrote 2798 bytes')
    dut.expect_exact('====================== HOST DATA START =========================')

    with open(HOST_FILE_PATH) as f:
        for line in f:
            if line.strip():
                dut.expect_exact(line.strip())

    dut.expect_exact('====================== HOST DATA END =========================')
    dut.expect_exact('example: Read 6121 bytes')

    with open(os.path.join(TEMP_DIR, 'esp32_stdout.txt')) as f:

        def expected_content() -> t.Iterator[str]:
            yield 'example: Switched to semihosted stdout'
            for i in range(100):
                yield 'Semihosted stdout write {}'.format(i)
            yield 'example: Switch to UART stdout'

        for actual, expected in zip_longest(f, expected_content(), fillvalue='-'):
            if expected not in actual:  # "in" used because of the printed ASCII color codes
                raise RuntimeError('"{}" != "{}"'.format(expected, actual.strip()))
