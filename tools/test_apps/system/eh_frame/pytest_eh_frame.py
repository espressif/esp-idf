# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c5
@pytest.mark.esp32c6
@pytest.mark.esp32c61
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
def test_eh_frame_wdt(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.confirm_write('"Test task wdt can print backtrace with eh-frame"', expect_str=f'Running')

    # Expect a backtrace which is at least 3 PC-SP pairs deep
    dut.expect(r'Backtrace: (0x[a-fA-F0-9]+:0x[a-fA-F0-9]+\s*){3,}')


@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c5
@pytest.mark.esp32c6
@pytest.mark.esp32c61
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
def test_eh_frame_panic(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.confirm_write('"Test panic can print backtrace with eh-frame"', expect_str=f'Running')

    # Expect a backtrace which is at least 3 PC-SP pairs deep
    dut.expect(r'Backtrace: (0x[a-fA-F0-9]+:0x[a-fA-F0-9]+\s*){3,}')
