# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded.dut import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.adc
@idf_parametrize(
    'target', ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32c5'], indirect=['target']
)
def test_adc_continuous(dut: Dut) -> None:
    res = dut.expect(r'TASK: ret is 0, ret_num is (\d+) bytes')
    num = res.group(1).decode('utf8')
    assert int(num) == 256
