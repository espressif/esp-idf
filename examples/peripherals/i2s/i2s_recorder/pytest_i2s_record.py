# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.generic
def test_i2s_es8311_example_generic(dut: Dut) -> None:
    dut.expect('PDM microphone recording example start', timeout=30)
    dut.expect('--------------------------------------', timeout=30)
    dut.expect('I \\(([0-9]+)\\) pdm_rec_example: Initializing SD card', timeout=30)
