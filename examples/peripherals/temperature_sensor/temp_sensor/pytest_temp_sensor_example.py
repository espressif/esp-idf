# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded.dut import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.esp32c2
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.esp32c5
@pytest.mark.generic
def test_temp_sensor_example(dut: Dut) -> None:
    dut.expect_exact('Install temperature sensor')
    dut.expect_exact('Enable temperature sensor')
    dut.expect_exact('Read temperature')
    temp_value = dut.expect(r'Temperature value (\d+\.\d+) .*', timeout=5)
    # Because the example test only run in the normal temperature environment. So this assert range is meaningful
    assert 0 < float(temp_value.group(1).decode('utf8')) < 50
    temp_value = dut.expect(r'Temperature value (\d+\.\d+) .*', timeout=5)
    assert 0 < float(temp_value.group(1).decode('utf8')) < 50
