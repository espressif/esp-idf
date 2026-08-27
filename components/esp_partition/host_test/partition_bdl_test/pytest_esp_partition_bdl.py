# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@idf_parametrize('target', ['linux'], indirect=['target'])
def test_esp_partition_bdl(dut: Dut) -> None:
    dut.expect_unity_test_output(timeout=5)
