# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.generic
def test_dac_oneshot_example(dut: Dut) -> None:
    res = []
    for _ in range(10):
        res.append(dut.expect(r'DAC channel 0 value:( +)(\d+)(.*)DAC channel 1 value:( +)(\d+)', timeout=10))

    avg1_ch1 = 0
    avg1_ch2 = 0
    avg2_ch1 = 0
    avg2_ch2 = 0

    for val in res[0:5]:
        avg1_ch1 = avg1_ch1 + int(val.group(2))
        avg1_ch2 = avg1_ch2 + int(val.group(5))
    for val in res[5:10]:
        avg2_ch1 = avg1_ch1 + int(val.group(2))
        avg2_ch2 = avg1_ch2 + int(val.group(5))

    avg1_ch1 = int(avg1_ch1 / 5)
    avg1_ch2 = int(avg1_ch2 / 5)
    avg2_ch1 = int(avg2_ch1 / 5)
    avg2_ch2 = int(avg2_ch2 / 5)

    assert avg2_ch1 > avg1_ch1
    assert avg2_ch2 > avg1_ch2
