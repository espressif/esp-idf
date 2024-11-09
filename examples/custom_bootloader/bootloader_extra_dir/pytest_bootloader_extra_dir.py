# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_custom_bootloader_extra_component(dut: Dut) -> None:
    dut.expect_exact('This function is called from an extra component')
