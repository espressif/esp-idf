# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_nvs_bootloader_example(dut: Dut) -> None:
    # Expect to read hooks messages and data from NVS partition
    dut.expect_exact('Before reading from NVS partition')
    dut.expect_exact('Finished bootloader part')
    dut.expect_exact('User application is loaded and running.')
