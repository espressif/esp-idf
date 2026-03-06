# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.sdcard_spimode
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32s3', 'esp32p4'], indirect=['target'])
def test_sdspi(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(reset=True)
