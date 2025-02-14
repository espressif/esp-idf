# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s3
@pytest.mark.temp_skip_ci(targets=['esp32s3'], reason='only to run locally')
def test_usb_enum(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='mock_enum_device', timeout=250)  # some tests take more than default timeout
