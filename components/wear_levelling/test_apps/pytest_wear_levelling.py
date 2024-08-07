# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
@pytest.mark.parametrize('config', [
    '4k',
    '512perf',
    '512safe',
    'release',
], indirect=True)
def test_wear_levelling(dut: Dut) -> None:
    dut.expect_unity_test_output()
