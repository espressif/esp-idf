# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_custom_bootloader_hooks_example(dut: Dut) -> None:
    # Expect to read both hooks messages
    dut.expect_exact('This hook is called BEFORE bootloader initialization')
    dut.expect_exact('This hook is called AFTER bootloader initialization')
