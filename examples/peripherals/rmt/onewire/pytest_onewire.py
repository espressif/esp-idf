# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
def test_onewire_example(dut: Dut) -> None:
    dut.expect_exact('example: 1-Wire bus installed on GPIO')
    dut.expect_exact('example: Device iterator created, start searching')
    dut.expect_exact('example: Searching done')
