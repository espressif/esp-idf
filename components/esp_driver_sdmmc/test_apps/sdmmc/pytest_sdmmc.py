# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded_idf import IdfDut


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.sdcard
def test_sdmmc(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()
