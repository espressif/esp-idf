# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded.dut import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.flash_suspend
@pytest.mark.parametrize(
    'config',
    [
        'flash_suspend',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_flash_suspend_example(dut: Dut) -> None:
    dut.expect_exact('found partition')
    res = dut.expect(r'During Erase, ISR callback function\(in flash\) response time:\s+(\d+(\.\d{1,2})) us')
    response_time = res.group(1).decode('utf8')
    assert 0 <= float(response_time) < 120

    res = dut.expect(r'During Erase, ISR callback function\(in iram\) response time:\s+(\d+(\.\d{1,2})) us')
    response_time = res.group(1).decode('utf8')
    assert 0 <= float(response_time) < 5
