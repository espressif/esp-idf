# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_perfmon(dut: Dut) -> None:

    dut.expect('example: Start')
    dut.expect('example: Start test with printing all available statistic')
    dut.expect('example: Start test with user defined statistic')
    dut.expect('example: The End')
