# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.temp_skip_ci(
    targets=['esp32s2', 'esp32s3'], reason='eFuse for S2 and S3 is similar to the C3 chip, so testing on C3 is enough'
)
@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_efuse(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.qemu
@pytest.mark.host_test
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_efuse_qemu(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_efuse_linux(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=60)
