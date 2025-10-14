# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_USB_OTG_SUPPORTED == 1'), indirect=['target'])
def test_usb_phy(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='phy')
