# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.sdcard
@idf_parametrize('target', ['esp32', 'esp32s3', 'esp32p4'], indirect=['target'])
def test_sdmmc(dut: IdfDut) -> None:
    # SDMMC driver can't be reinitialized if the test fails,
    # so we need to reset the board between tests to avoid failing
    # all the tests after the first one fails.
    dut.run_all_single_board_cases(reset=True)
