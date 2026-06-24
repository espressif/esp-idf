# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_PPA_SUPPORTED == 1'), indirect=['target'])
def test_ppa(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.esp32p4_rev1
@pytest.mark.parametrize(
    'config',
    [
        ('esp32p4_rev1'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_ppa_esp32p4_rev1(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.flash_encryption
@pytest.mark.parametrize(
    'config',
    [
        'flash_enc',
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_PPA_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='TODO: IDFCI-10377 no runner yet')
def test_ppa_flash_encryption(dut: Dut) -> None:
    dut.run_all_single_board_cases()
