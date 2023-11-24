# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.temp_skip_ci(targets=['esp32s3'], reason='skip due to duplication with test_uart_vfs_psram')
@pytest.mark.generic
@pytest.mark.parametrize('config', [
    'default', 'iram',
], indirect=True)
def test_uart_vfs_default(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32s3
@pytest.mark.quad_psram
@pytest.mark.parametrize('config', [
    'default', 'iram',
], indirect=True)
def test_uart_vfs_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases()
