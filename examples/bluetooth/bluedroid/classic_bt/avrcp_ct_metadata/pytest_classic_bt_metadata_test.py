# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_bt_avrcp_metadata(dut: Dut) -> None:
    dut.expect(r'AVRCP (CT|TG) STATE: Init Complete', timeout=30)
    dut.expect(r'AVRCP (CT|TG) STATE: Init Complete', timeout=30)
