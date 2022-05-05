# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded.dut import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.generic
def test_temp_sensor_example(dut: Dut) -> None:
    dut.expect_exact('Initializing Temperature sensor')
    dut.expect_exact('Temperature sensor started')
    temp_value = dut.expect(r'Temperature out celsius (\d+\.\d+)', timeout=30)
    # Because the example test only run in the normal temperature environment. So this assert range is meaningful
    assert 0 < float(temp_value.group(1)) < 45
