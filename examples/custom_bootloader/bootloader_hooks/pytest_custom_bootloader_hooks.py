# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'default',
        # log_v2 routes ESP_LOG through esp_rom_output_to_channels() -> _putc2. This config
        # verifies that logging works correctly in bootloader hooks (before BSS is cleared),
        # where _putc2 must be initialized to a safe noop rather than left as NULL/garbage.
        'log_v2',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3', 'esp32c3'], indirect=['target'])
def test_custom_bootloader_hooks_example(dut: Dut) -> None:
    # Expect to read both hooks messages
    dut.expect_exact('This hook is called BEFORE bootloader initialization')
    dut.expect_exact('This hook is called AFTER bootloader initialization')
