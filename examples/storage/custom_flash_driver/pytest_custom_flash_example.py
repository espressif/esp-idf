# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'default',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='cannot pass')  # TODO: IDF-15601
def test_examples_custom_flash_driver(dut: Dut) -> None:
    dut.expect(r'Using overridden bootloader_flash_unlock', timeout=20)
    dut.expect(r'Using overridden bootloader_flash_qio, the list number is \d+', timeout=20)
