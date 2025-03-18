# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.flash_suspend
@idf_parametrize('config', ['defaults'], indirect=['config'])
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_flash_auto_suspend_minimize_iram_usage(dut: Dut) -> None:
    dut.expect_exact('Hello World!', timeout=30)
