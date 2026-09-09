# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'phy_multiple_init_data',
        'phy_multiple_init_data_embed',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32', 'esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32s2', 'esp32s3'],
    indirect=['target'],
)
def test_phy_multi_init_data_bin(dut: IdfDut, config: str) -> None:
    # Where the bin was loaded from is only visible in the log, and app_main loads it
    # once before handing over to the unity runner.
    if config == 'phy_multiple_init_data_embed':
        dut.expect_exact('loading embedded multiple PHY init data')
    dut.expect_exact('Support multiple PHY init data bins')

    # the unity menu parser consumes the "Press ENTER" prompt itself, so do not expect it here
    # each case switches the PHY init data type, so start every one of them from a fresh boot
    dut.run_all_single_board_cases(reset=True)
