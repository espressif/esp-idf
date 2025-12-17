# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.sdcard
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_sdmmc_extra(dut: Dut) -> None:
    dut.expect_unity_test_output()
