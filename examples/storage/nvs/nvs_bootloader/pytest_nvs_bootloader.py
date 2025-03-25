# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_nvs_bootloader_example(dut: Dut) -> None:
    # Expect to read hooks messages and data from NVS partition
    dut.expect_exact('Before reading from NVS partition')
    dut.expect_exact('Result data. Return code: ESP_OK')
    dut.expect_exact('u8               U8  255')
    dut.expect_exact('i32              I32 -2147483648')
    dut.expect_exact('i8               I8  -13')
    dut.expect_exact('u16              U16 65535')
    dut.expect_exact('string_10_chars  STR Text_67890')
    dut.expect_exact('string_66_chars  STR Text_67890_Text_67890_Text_67890_0_Text_67890_Text_67890_Text_6789')
    dut.expect_exact('Finished bootloader part')
    dut.expect_exact('User application is loaded and running.')


@pytest.mark.nvs_encr_hmac
@pytest.mark.parametrize('config', ['nvs_enc_hmac'], indirect=True)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_nvs_bootloader_example_nvs_encr_hmac(dut: Dut) -> None:
    test_nvs_bootloader_example(dut)


@pytest.mark.flash_encryption
@pytest.mark.parametrize('config', ['nvs_enc_flash_enc'], indirect=True)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_nvs_bootloader_example_flash_enc(dut: Dut) -> None:
    test_nvs_bootloader_example(dut)
