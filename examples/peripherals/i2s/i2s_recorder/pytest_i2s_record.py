# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32s3', 'esp32p4'], indirect=['target'])
def test_i2s_recorder_generic(dut: Dut) -> None:
    dut.expect('PDM microphone recording example start')
    dut.expect('--------------------------------------')
    dut.expect('I \\(([0-9]+)\\) pdm_rec_example: Initializing SD card')
