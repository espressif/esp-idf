# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.generic
def test_custom_bootloader_hooks_example(dut: Dut) -> None:
    # Expect to read both hooks messages
    dut.expect_exact('This hook is called BEFORE bootloader initialization')
    dut.expect_exact('This hook is called AFTER bootloader initialization')
