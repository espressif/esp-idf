# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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
@idf_parametrize('target', soc_filtered_targets('SOC_DMA2D_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_dma2d(dut: Dut) -> None:
    dut.run_all_single_board_cases()
