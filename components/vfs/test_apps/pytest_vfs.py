# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.parametrize('config', [
    'default', 'iram',
], indirect=True)
def test_vfs_default(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32
@pytest.mark.parametrize('config', [
    'ccomp',
], indirect=True)
def test_vfs_ccomp(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32s3
@pytest.mark.quad_psram
@pytest.mark.parametrize('config', [
    'psram',
], indirect=True)
def test_vfs_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases()
