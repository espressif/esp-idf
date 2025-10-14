# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'dyn_buffers',
        'no_dyn_buffers',
    ],
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_fatfs_flash_dyn_buffers(config: str, dut: Dut) -> None:
    dut.expect('Mounting FATFS')
    dut.expect('Mounted')
    dut.expect('Unmounting FATFS')
    dut.expect('Unmounted')
    dut.expect('Done')
