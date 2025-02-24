# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded.dut import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32c6', 'esp32h2', 'esp32p4', 'esp32c5', 'esp32c61'], indirect=['target'])
def test_temp_sensor_monitor_example(dut: Dut) -> None:
    dut.expect_exact('Install temperature sensor')
    dut.expect_exact('Enable temperature sensor')
    dut.expect_exact('Read temperature')
    temp_value = dut.expect(r'Temperature value (\d+\.\d+) .*', timeout=5)
    # Because the example test only run in the normal temperature environment. So this assert range is meaningful
    assert 0 < float(temp_value.group(1).decode('utf8')) < 50
    temp_value = dut.expect(r'Temperature value (\d+\.\d+) .*', timeout=5)
    assert 0 < float(temp_value.group(1).decode('utf8')) < 50
