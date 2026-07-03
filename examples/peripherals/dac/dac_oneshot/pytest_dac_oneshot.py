# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32s2'], indirect=['target'])
def test_dac_oneshot_example(dut: Dut) -> None:
    res = []
    for _ in range(10):
        res.append(dut.expect(r'DAC channel 0 value:( +)(\d+)(.*)DAC channel 1 value:( +)(\d+)', timeout=10))

    avg1_ch0 = sum(int(val.group(2)) for val in res[0:5]) / 5
    avg1_ch1 = sum(int(val.group(5)) for val in res[0:5]) / 5
    avg2_ch0 = sum(int(val.group(2)) for val in res[5:10]) / 5
    avg2_ch1 = sum(int(val.group(5)) for val in res[5:10]) / 5

    assert avg2_ch0 > avg1_ch0
    # On ESP32-S2 CI runners, GPIO18 (DAC ch1) has an LED attached. The voltage is clamped.
    if dut.target != 'esp32s2':
        assert avg2_ch1 > avg1_ch1
