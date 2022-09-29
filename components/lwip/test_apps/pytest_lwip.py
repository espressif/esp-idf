# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
def test_lwip(dut: Dut) -> None:
    dut.expect_unity_test_output()
