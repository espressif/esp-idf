# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'embed',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32', 'esp32c2', 'esp32c3', 'esp32s2', 'esp32s3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32s31'],
    indirect=['target'],
)
def test_phy_multiple_init_data(dut: Dut) -> None:
    # each case switches the PHY init data type, so start every one of them from a fresh boot
    dut.run_all_single_board_cases(reset=True)
