# SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.sdcard
@pytest.mark.flaky(reruns=2, reruns_delay=5)
@idf_parametrize('config', ['default'], indirect=['config'])
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_sdmmc_extra(dut: Dut) -> None:
    dut.run_all_single_board_cases()
