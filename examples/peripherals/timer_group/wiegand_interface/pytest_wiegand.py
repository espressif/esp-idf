# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'target',
    ['esp32', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32s2', 'esp32s3', 'esp32p4'],
    indirect=['target'],
)
def test_gptimer_wiegand(dut: Dut) -> None:
    dut.expect_exact('example: Configure wiegand interface')
    dut.expect_exact('example: send wiegand data')
