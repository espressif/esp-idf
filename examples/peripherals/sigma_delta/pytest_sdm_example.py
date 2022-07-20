# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.generic
def test_sdm_example(dut: Dut) -> None:
    dut.expect_exact('example: Install sigma delta channel')
    dut.expect_exact('example: Enable sigma delta channel')
    dut.expect_exact('example: Change duty cycle continuously')
