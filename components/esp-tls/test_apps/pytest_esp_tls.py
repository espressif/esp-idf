# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('config', ['default'], indirect=['config'])
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_esp_tls(dut: Dut) -> None:
    dut.run_all_single_board_cases()
