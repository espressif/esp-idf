# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
@pytest.mark.parametrize('config', ['default'], indirect=True)
def test_nvs_bootloader_support(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='!nvs_encrypted_bootloader', timeout=120)


@pytest.mark.esp32c3
@pytest.mark.nvs_encr_hmac
@pytest.mark.parametrize('config', ['nvs_enc_hmac'], indirect=True)
def test_nvs_bootloader_support_encr_hmac(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.flash_encryption
@pytest.mark.parametrize('config', ['nvs_enc_flash_enc'], indirect=True)
def test_nvs_bootloader_support_encr_flash_enc(dut: IdfDut) -> None:
    # Erase the nvs_key partition
    dut.serial.erase_partition('nvs_key')
    dut.run_all_single_board_cases()


@pytest.mark.esp32c3
@pytest.mark.nvs_encr_hmac
@pytest.mark.parametrize('config', ['nvs_enc_hmac_no_cfg'], indirect=True)
def test_nvs_bootloader_support_encr_hmac_no_cfg(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()
