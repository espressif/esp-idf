# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

CONFIGS_NVS_ENCR_FLASH_ENC = [
    pytest.param('nvs_encr_flash_enc_esp32', marks=[pytest.mark.esp32]),
    pytest.param('nvs_encr_flash_enc_esp32c3', marks=[pytest.mark.esp32c3]),
]


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default'], indirect=True)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_nvs_flash(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='!nvs_encr_hmac', timeout=120)


@pytest.mark.nvs_encr_hmac
@pytest.mark.parametrize('config', ['nvs_encr_hmac_esp32c3'], indirect=True)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_nvs_flash_encr_hmac(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.nvs_encr_hmac
@pytest.mark.parametrize('config', ['nvs_encr_hmac_no_cfg_esp32c3'], indirect=True)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_nvs_flash_encr_hmac_no_cfg(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='nvs_encr_hmac', timeout=120)


@pytest.mark.flash_encryption
@idf_parametrize(
    'config,target',
    [('nvs_encr_flash_enc_esp32', 'esp32'), ('nvs_encr_flash_enc_esp32c3', 'esp32c3')],
    indirect=['config', 'target'],
)
def test_nvs_flash_encr_flash_enc(dut: IdfDut) -> None:
    # Erase the nvs_key partition
    dut.serial.erase_partition('nvs_key')
    dut.run_all_single_board_cases()


@pytest.mark.psram
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_nvs_flash_ram(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='nvs_ram')
