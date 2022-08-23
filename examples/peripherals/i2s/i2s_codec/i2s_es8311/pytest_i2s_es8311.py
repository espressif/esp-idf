# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.generic
def test_i2s_es8311_example_generic(dut: Dut) -> None:
    dut.expect('i2s es8311 codec example start')
    dut.expect('-----------------------------')
    dut.expect('I \\(([0-9]+)\\) i2s_es8311: i2s driver init success')
