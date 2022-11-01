# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.twai_transceiver
def test_twai_alert_recovery_example(dut: Dut) -> None:
    dut.expect_exact('TWAI Alert and Recovery: Driver installed')
    dut.expect_exact('TWAI Alert and Recovery: Driver uninstalled')
