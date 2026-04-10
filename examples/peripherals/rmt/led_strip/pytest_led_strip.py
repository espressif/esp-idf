# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_RMT_SUPPORTED == 1'), indirect=['target'])
def test_led_strip_example(dut: Dut) -> None:
    dut.expect_exact('example: Create RMT TX channel')
    dut.expect_exact('example: Install led strip encoder')
    dut.expect_exact('example: Enable RMT TX channel')
    dut.expect_exact('example: Start LED rainbow chase')
