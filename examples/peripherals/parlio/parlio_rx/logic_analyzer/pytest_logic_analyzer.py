# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32c5
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'flash_stream',
    ],
    indirect=True,
)
def test_logic_analyzer_flash_stream(dut: Dut) -> None:
    dut.expect(r'flash_fat: Probe data partition base addr: \w+ size: \w+')
    dut.expect(r'flash_fat: flash FATFS mounted')
    dut.expect(r'esp_probe: Dump data size reached the max dump size')
    dut.expect(r'example: Probe finished! [0-9]+ \(\w+\) bytes dumped')
