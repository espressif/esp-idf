# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32s31'], indirect=['target'])
@idf_parametrize('config', ['default'], indirect=['config'])
def test_sv32(dut: Dut) -> None:
    dut.run_all_single_board_cases()
