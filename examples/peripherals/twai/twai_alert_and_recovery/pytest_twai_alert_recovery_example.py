# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import time

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


def esp_enter_flash_mode(dut: Dut) -> None:
    ser = dut.serial.proc
    ser.setRTS(True)  # EN Low
    time.sleep(0.5)
    ser.setDTR(True)  # GPIO0 Low
    ser.setRTS(False)  # EN High
    dut.expect('waiting for download', timeout=2)
    ser.setDTR(False)  # Back RTS/DTR to 1/1 to avoid affect to esptool


@pytest.mark.twai_adapter
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_twai_alert_recovery_example(dut: Dut) -> None:
    try:
        dut.expect_exact('TWAI Alert and Recovery: Driver installed')
        dut.expect_exact('TWAI Alert and Recovery: Driver uninstalled')
    finally:
        esp_enter_flash_mode(dut)
