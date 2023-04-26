# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize('config', ['default'], indirect=True)
def test_nvs_flash(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()


CONFIGS_NVS_ENCR_KEYS_FLASH_ENC = [
    pytest.param('nvs_encr_keys_flash_enc_esp32',   marks=[pytest.mark.esp32]),
    pytest.param('nvs_encr_keys_flash_enc_esp32c3', marks=[pytest.mark.esp32c3]),
]


@pytest.mark.parametrize('config', CONFIGS_NVS_ENCR_KEYS_FLASH_ENC, indirect=True)
@pytest.mark.flash_encryption
def test_nvs_flash_encr_keys_flash_enc(dut: IdfDut) -> None:
    # Erase the nvs_key partition
    dut.serial.erase_partition('nvs_key')
    dut.run_all_single_board_cases()
