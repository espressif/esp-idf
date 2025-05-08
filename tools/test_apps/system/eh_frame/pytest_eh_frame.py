# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'target', ['esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4'], indirect=['target']
)
def test_eh_frame_wdt(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.confirm_write('"Test task wdt can print backtrace with eh-frame"', expect_str=f'Running')

    # Expect a backtrace which is at least 3 PC-SP pairs deep
    dut.expect(r'Backtrace: (0x[a-fA-F0-9]+:0x[a-fA-F0-9]+\s*){3,}')


@pytest.mark.generic
@idf_parametrize(
    'target', ['esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4'], indirect=['target']
)
def test_eh_frame_panic(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.confirm_write('"Test panic can print backtrace with eh-frame"', expect_str=f'Running')

    # Expect a backtrace which is at least 3 PC-SP pairs deep
    dut.expect(r'Backtrace: (0x[a-fA-F0-9]+:0x[a-fA-F0-9]+\s*){3,}')
