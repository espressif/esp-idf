# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.temp_skip_ci(targets=['esp32c61'], reason='C5 C61 GPSPI same, so testing on C5 is enough')
@pytest.mark.sdcard_spimode
@idf_parametrize('target', ['esp32', 'esp32s3', 'esp32c3', 'esp32p4', 'esp32c5'], indirect=['target'])
def test_sdspi(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(reset=True)
