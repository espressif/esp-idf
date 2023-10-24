# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_mbedtls(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'perf_esp32',
    ],
    indirect=True,
)
def test_mbedtls_esp32_compiler_perf_opt(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'aes_no_hw',
    ],
    indirect=True,
)
def test_mbedtls_aes_no_hw(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ],
    indirect=True,
)
def test_mbedtls_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'psram_esp32',
        'psram_all_ext',
    ],
    indirect=True,
)
def test_mbedtls_psram_esp32(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32h2
@pytest.mark.ecdsa_efuse
@pytest.mark.parametrize('config', ['ecdsa_sign',], indirect=True)
def test_mbedtls_ecdsa_sign(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='efuse_key')


@pytest.mark.esp32c2
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'rom_impl',
    ],
    indirect=True,
)
def test_mbedtls_rom_impl_esp32c2(dut: Dut) -> None:
    dut.run_all_single_board_cases()
