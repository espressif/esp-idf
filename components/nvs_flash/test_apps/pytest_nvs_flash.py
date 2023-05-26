# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded_idf.dut import IdfDut

CONFIGS_NVS_ENCR_FLASH_ENC = [
    pytest.param('nvs_encr_flash_enc_esp32',   marks=[pytest.mark.esp32]),
    pytest.param('nvs_encr_flash_enc_esp32c3', marks=[pytest.mark.esp32c3]),
]


@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize('config', ['default'], indirect=True)
def test_nvs_flash(dut: IdfDut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('![nvs_encr_hmac]')
    dut.expect_unity_test_output(timeout=120)


@pytest.mark.esp32c3
@pytest.mark.nvs_encr_hmac
@pytest.mark.parametrize('config', ['nvs_encr_hmac_esp32c3'], indirect=True)
def test_nvs_flash_encr_hmac(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.flash_encryption
@pytest.mark.parametrize('config', CONFIGS_NVS_ENCR_FLASH_ENC, indirect=True)
def test_nvs_flash_encr_flash_enc(dut: IdfDut) -> None:
    # Erase the nvs_key partition
    dut.serial.erase_partition('nvs_key')
    dut.run_all_single_board_cases()
