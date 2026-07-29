# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize('config', ['minimal', 'full'], indirect=True)
@idf_parametrize('target', ['esp32', 'esp32c6'], indirect=['target'])
def test_esp_timer_size_metrics_boots(dut: IdfDut) -> None:
    dut.expect_exact('size_metrics app_main started', timeout=30)
