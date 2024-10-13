# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'no_rom_impl_components',
        'rom_impl_components',
    ],
    indirect=True,
)
def test_esp_rom_impl_components(dut: Dut) -> None:
    dut.run_all_single_board_cases()
