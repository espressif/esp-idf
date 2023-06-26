# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import logging
import os

import pytest
from pytest_embedded_idf.dut import IdfDut

STR_KEY_VAL = ['Lorem ipsum dolor sit amet, consectetur adipiscing elit.',
               'Fusce quis risus justo.',
               'Suspendisse egestas in nisi sit amet auctor.',
               'Pellentesque rhoncus dictum sodales.',
               'In justo erat, viverra at interdum eget, interdum vel dui.']

ENCR_TEXT_ARR = ['fe ff ff ff 00 00 00 00  fe ff ff ff ff ff ff ff',
                 'ca 8b c3 bb 2d c2 33 d6  6b d4 a7 3d 31 0e 9c 36',
                 'bd c1 2a 10 87 44 5e 1c  4b 2c 7c 5d ac 97 48 63']


@pytest.mark.esp32c3
@pytest.mark.nvs_encr_hmac
@pytest.mark.parametrize('config', ['nvs_encr_hmac'], indirect=True)
def test_nvs_flash_encr_keys_hmac(dut: IdfDut) -> None:
    # Logging example binary details
    binary_file = os.path.join(dut.app.binary_path, 'nvs_encryption_hmac.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('nvs_encryption_hmac_bin_size : {}KB'.format(bin_size // 1024))

    # Start test and verify serial output
    dut.expect('NVS partition "nvs" is encrypted.', timeout=30)
    dut.expect('NVS partition "custom_nvs" is encrypted', timeout=30)

    dut.expect('Key: u8_key | Val: 255', timeout=30)
    dut.expect('Key: i8_key | Val: -128', timeout=30)
    dut.expect('Key: u16_key | Val: 65535', timeout=30)
    dut.expect('Key: u32_key | Val: 4294967295', timeout=30)
    dut.expect('Key: i32_key | Val: -2147483648', timeout=30)

    for string in STR_KEY_VAL:
        dut.expect(string, timeout=30)

    for encr_txt in ENCR_TEXT_ARR:
        dut.expect(encr_txt, timeout=30)

    dut.expect('Returned from app_main()', timeout=30)
