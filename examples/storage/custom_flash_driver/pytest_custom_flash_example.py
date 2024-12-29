# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'default',
    ],
    indirect=True,
)
def test_examples_custom_flash_driver(dut: Dut) -> None:
    dut.expect(r'Using overridden bootloader_flash_unlock', timeout=20)
    dut.expect(r'Using overridden bootloader_flash_qio, the list number is \d+', timeout=20)
