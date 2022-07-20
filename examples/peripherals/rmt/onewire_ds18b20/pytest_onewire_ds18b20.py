# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.generic
def test_onewire_ds18b20_example(dut: Dut) -> None:
    dut.expect_exact('onewire_rmt: RMT Tx channel created for 1-wire bus')
    dut.expect_exact('onewire_rmt: RMT Rx channel created for 1-wire bus')
    dut.expect_exact('example: 1-wire bus installed')
    dut.expect_exact('example: 1-wire bus deleted')
