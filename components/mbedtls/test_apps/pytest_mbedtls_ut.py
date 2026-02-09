# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_mbedtls(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'perf_esp32',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_mbedtls_esp32_compiler_perf_opt(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'aes_no_hw',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c5'], indirect=['target'])
def test_mbedtls_aes_no_hw(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'psram',
        'psram_all_ext',
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_SPIRAM_SUPPORTED == 1'), indirect=['target'])
def test_mbedtls_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)


@pytest.mark.flash_encryption_psram
@pytest.mark.parametrize(
    'config',
    [
        'psram_all_ext_flash_enc',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_mbedtls_psram_all_ext_flash_enc_esp32(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)


@pytest.mark.flash_encryption
@pytest.mark.parametrize(
    'config',
    [
        'psram_all_ext_flash_enc',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4', 'esp32c5'], indirect=['target'])
def test_mbedtls_psram_all_ext_flash_enc(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)


@pytest.mark.flash_encryption_f4r8
@pytest.mark.parametrize(
    'config',
    [
        'psram_all_ext_flash_enc_f4r8',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_mbedtls_psram_all_ext_flash_enc_s3_f4r8(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)


@pytest.mark.ecdsa_efuse
@pytest.mark.parametrize(
    'config',
    [
        'ecdsa_sign',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32h2'], indirect=['target'])
def test_mbedtls_ecdsa_sign(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='efuse_key')


# TODO: IDF-15012
# @pytest.mark.generic
# @pytest.mark.parametrize(
#     'config',
#     [
#         'rom_impl',
#     ],
#     indirect=True,
# )
# @idf_parametrize('target', ['esp32c2'], indirect=['target'])
# def test_mbedtls_rom_impl_esp32c2(dut: Dut) -> None:
#     dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'ds_rsa',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_mbedtls_ds_rsa(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='ds_rsa')


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'aria',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_mbedtls_aria(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='aria')
