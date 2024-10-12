# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut


@pytest.mark.temp_skip_ci(targets=['esp32c61'], reason='C5 C61 GPSPI same, so testing on C5 is enough')
@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32p4
@pytest.mark.esp32c5
@pytest.mark.sdcard_spimode
def test_sdspi(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(reset=True)
