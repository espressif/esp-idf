# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.octal_psram
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_esp_h264_esp32s3(dut: Dut) -> None:
    dut.expect_exact('H264 process Completed successfully')


@pytest.mark.generic
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_esp_h264_esp32p4(dut: Dut) -> None:
    dut.expect_exact('H264 process Completed successfully')
