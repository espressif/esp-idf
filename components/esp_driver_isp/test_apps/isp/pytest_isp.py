# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.camera
@pytest.mark.ov5647
@idf_parametrize('target', soc_filtered_targets('SOC_ISP_SUPPORTED == 1'), indirect=['target'])
def test_isp(dut: Dut) -> None:
    dut.run_all_single_board_cases()
