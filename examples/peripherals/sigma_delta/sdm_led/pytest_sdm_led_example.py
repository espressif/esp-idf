# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'target',
    ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4'],
    indirect=['target'],
)
def test_sdm_led_example(dut: Dut) -> None:
    dut.expect_exact('sdm_led: Install sigma delta channel')
    dut.expect_exact('sdm_led: Enable sigma delta channel')
    dut.expect_exact('sdm_led: Change duty cycle continuously')
