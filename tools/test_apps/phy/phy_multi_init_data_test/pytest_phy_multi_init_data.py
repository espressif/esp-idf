# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize('config', [
    'phy_multiple_init_data',
    'phy_multiple_init_data_embed',
], indirect=True)
def test_phy_multi_init_data_bin(dut: IdfDut, config: str) -> None:
    if config == 'phy_multiple_init_data':
        dut.expect_exact('Support multiple PHY init data bins')
    else:
        dut.expect_exact('load embedded multi phy init data')
    dut.expect_exact('wifi_init finished')
