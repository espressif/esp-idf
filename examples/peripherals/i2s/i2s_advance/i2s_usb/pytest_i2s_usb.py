# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32s2', 'esp32s3', 'esp32p4'], indirect=['target'])
def test_i2s_usb_example(dut: Dut) -> None:
    dut.expect(r'initial MCLK: [0-9]+ Hz')
