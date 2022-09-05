# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
from typing import Tuple

import pytest
from pytest_embedded import Dut


def run_multi_device_case(master: Dut, slave: Dut, case_name: str) -> None:
    master.write(case_name)
    slave.write(case_name)

    slave.expect(r'\t\(2\)\s+\".+\"')
    slave.write('2')
    master.expect(r'\t\(2\)\s+\".+\"')
    master.write('1')

    master.expect_unity_test_output()
    slave.expect_unity_test_output()

    master.expect_exact("Enter next test, or 'enter' to see menu")
    slave.expect_exact("Enter next test, or 'enter' to see menu")


@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [
    2,
], indirect=True)
def test_i2s_tdm_full_duplex(dut: Tuple[Dut, Dut]) -> None:
    master = dut[0]
    slave = dut[1]
    master.expect_exact('Press ENTER to see the list of tests')
    slave.expect_exact('Press ENTER to see the list of tests')

    run_multi_device_case(master, slave, '"I2S TDM full duplex multiple device test (48k, 32bits, 4slots)"')
    run_multi_device_case(master, slave, '"I2S TDM full duplex multiple device test (48k, 16bits, 4slots)"')
    run_multi_device_case(master, slave, '"I2S TDM full duplex multiple device test (48k, 8bits, 4slots)"')
    run_multi_device_case(master, slave, '"I2S TDM full duplex multiple device test (48k, 16bits, 8slots)"')
    run_multi_device_case(master, slave, '"I2S TDM full duplex multiple device test (96k, 16bits, 4slots)"')
