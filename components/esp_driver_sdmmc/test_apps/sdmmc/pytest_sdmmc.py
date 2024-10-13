# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.sdcard
def test_sdmmc(dut: IdfDut) -> None:
    # SDMMC driver can't be reinitialized if the test fails,
    # so we need to reset the board between tests to avoid failing
    # all the tests after the first one fails.
    dut.run_all_single_board_cases(reset=True)
