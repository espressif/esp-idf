# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.flaky(reruns=2, reruns_delay=5)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_nvs_bootloader_example(dut: Dut) -> None:
    # Expect to read hooks messages and data from NVS partition
    dut.expect_exact('Before reading from NVS partition')
    dut.expect_exact('Finished bootloader part')
    dut.expect_exact('User application is loaded and running.')
