# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
def test_i2s_es8311_example_generic(dut: Dut) -> None:
    dut.expect('i2s es8311 codec example start')
    dut.expect('-----------------------------')
    dut.expect('i2s_es8311: i2s driver init success')


@pytest.mark.esp32s3
@pytest.mark.es8311
@pytest.mark.parametrize(
    'config',
    [
        's3_korvo2_v3',
    ],
    indirect=True,
)
def test_i2s_es8311_example_korvo2_v3(dut: Dut) -> None:
    dut.expect(r'i2s es8311 codec example start')
    dut.expect(r'-----------------------------')
    dut.expect(r'i2s_es8311: i2s driver init success')
    dut.expect(r'i2s_es8311: es8311 codec init success')
    dut.expect(r'Returned from app_main\(\)')
