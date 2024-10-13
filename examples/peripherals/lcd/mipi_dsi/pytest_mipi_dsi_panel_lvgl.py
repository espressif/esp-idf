# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32p4
@pytest.mark.generic
def test_rgb_lcd_lvgl(dut: Dut) -> None:
    dut.expect_exact('example: Install MIPI DSI LCD control panel')
