# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_RMT_SUPPORTED == 1'), indirect=['target'])
def test_onewire_example(dut: Dut) -> None:
    dut.expect_exact('example: 1-Wire bus installed on GPIO')
    dut.expect_exact('example: Device iterator created, start searching')
    dut.expect_exact('example: Searching done')
