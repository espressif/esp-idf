# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32c61'], reason='Support for ESP32C61 is yet to be added.')  # TODO: [ESP32C61] IDF-10987
@pytest.mark.parametrize(
    'config',
    [
        pytest.param('default', marks=[pytest.mark.supported_targets]),
    ],
    indirect=True,
)
def test_crypto_drivers(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)
