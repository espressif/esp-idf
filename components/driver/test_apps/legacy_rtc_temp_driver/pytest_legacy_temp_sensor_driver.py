# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target', ['esp32s2', 'esp32c3', 'esp32s3', 'esp32c2', 'esp32c6', 'esp32h2', 'esp32p4'], indirect=['target']
)
def test_legacy_temp_sensor_driver(dut: Dut) -> None:
    dut.expect('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output(timeout=120)
