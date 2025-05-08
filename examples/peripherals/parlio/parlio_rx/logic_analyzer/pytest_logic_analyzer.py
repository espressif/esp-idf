# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'flash_stream',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c5', 'esp32c6', 'esp32h2', 'esp32p4'], indirect=['target'])
def test_logic_analyzer_flash_stream(dut: Dut) -> None:
    dut.expect(r'flash_fat: Probe data partition base addr: \w+ size: \w+')
    dut.expect(r'flash_fat: flash FATFS mounted')
    dut.expect(r'esp_probe: Dump data size reached the max dump size')
    dut.expect(r'example: Probe finished! [0-9]+ \(\w+\) bytes dumped')
