# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32s2', 'esp32s3'], indirect=['target'])
def test_perfmon(dut: Dut) -> None:
    dut.expect('example: Start')
    dut.expect('example: Start test with printing all available statistic')
    dut.expect('example: Start test with user defined statistic')
    dut.expect('example: The End')
