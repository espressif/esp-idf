# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded.dut import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.adc
def test_adc_continuous(dut: Dut) -> None:
    res = dut.expect(r'TASK: ret is 0, ret_num is (\d+)')
    num = res.group(1).decode('utf8')
    assert int(num) == 256
