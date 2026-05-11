# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# pylint: disable=W0621  # redefined-outer-name
import base64
import csv
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Any

import espsecure
import esptool
import pytest
from _pytest.fixtures import FixtureRequest
from _pytest.monkeypatch import MonkeyPatch
from pytest_embedded_idf.serial import IdfSerial
from pytest_embedded_serial_esp.serial import EspSerial

# fmt: off
esp_tee_empty_bin = {
    'esp32c6': [
        0xE9, 0x04, 0x02, 0x10, 0x00, 0x00, 0x80, 0x40, 0xEE, 0x00, 0x00, 0x00,
        0x0D, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        # DROM segment
        0x20, 0x00, 0x00, 0x42, 0x00, 0x02, 0x00, 0x00,
        # esp_app_desc structure
        0x32, 0x54, 0xCD, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x31, 0x2E, 0x30, 0x2E, 0x30, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x65, 0x73, 0x70, 0x5F, 0x74, 0x65, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x30, 0x30, 0x3A, 0x30, 0x30, 0x3A, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x4E, 0x6F, 0x76, 0x20, 0x31, 0x31, 0x20, 0x32,
        0x30, 0x32, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x35, 0x2E, 0x35,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x2D, 0x63, 0x66, 0x8B, 0x75, 0xFA, 0x59, 0x05,
        0x53, 0x34, 0x91, 0x71, 0x51, 0x33, 0x91, 0xDD, 0xF8, 0xB1, 0xFE, 0x83,
        0x06, 0xEB, 0x03, 0x80, 0x45, 0xC9, 0x18, 0x20, 0x83, 0x7E, 0x2E, 0x43,
        *([0x00] * 0x50),
        # Padding
        *([0x00] * 0x100),
        # IRAM segment
        0x00, 0x00, 0x80, 0x40, 0x20, 0x00, 0x00, 0x00,
        *([0x00] * 0x20),
        # PADDING segment
        0x00, 0x00, 0x00, 0x00, 0xC8, 0x7D, 0x00, 0x00,
        *([0x00] * 0x7DC8),
        # IROM segment
        0x20, 0x80, 0x00, 0x42, 0x00, 0x01, 0x00, 0x00,
        *([0x00] * 0x100),
        # Padding
        *([0x00] * 0x0F),
        # CRC8 checksum
        0x3F,
        # Image SHA256
        0x2B, 0xF4, 0x49, 0x65, 0x93, 0xDD, 0xA6, 0x65, 0xE4, 0x31, 0xEA, 0xC5,
        0x42, 0x40, 0xA0, 0x96, 0x8B, 0x70, 0x48, 0xFE, 0xD9, 0x7E, 0x6F, 0x21,
        0x0B, 0xC1, 0xA5, 0xD4, 0x43, 0x87, 0x52, 0x2A
    ],
    'esp32h2': [
        0xE9, 0x04, 0x02, 0x1F, 0x00, 0x00, 0x80, 0x40, 0xEE, 0x00, 0x00, 0x00,
        0x10, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        # DROM segment
        0x20, 0x00, 0x00, 0x42, 0x00, 0x02, 0x00, 0x00,
        # esp_app_desc structure
        0x32, 0x54, 0xCD, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x31, 0x2E, 0x30, 0x2E, 0x30, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x65, 0x73, 0x70, 0x5F, 0x74, 0x65, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x30, 0x30, 0x3A, 0x30, 0x30, 0x3A, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x4E, 0x6F, 0x76, 0x20, 0x31, 0x31, 0x20, 0x32,
        0x30, 0x32, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x35, 0x2E, 0x35,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x2D, 0x63, 0x66, 0x8B, 0x75, 0xFA, 0x59, 0x05,
        0x53, 0x34, 0x91, 0x71, 0x51, 0x33, 0x91, 0xDD, 0xF8, 0xB1, 0xFE, 0x83,
        0x06, 0xEB, 0x03, 0x80, 0x45, 0xC9, 0x18, 0x20, 0x83, 0x7E, 0x2E, 0x43,
        *([0x00] * 0x50),
        # Padding
        *([0x00] * 0x100),
        # IRAM segment
        0x00, 0x00, 0x80, 0x40, 0x20, 0x00, 0x00, 0x00,
        *([0x00] * 0x20),
        # PADDING segment
        0x00, 0x00, 0x00, 0x00, 0xC8, 0x7D, 0x00, 0x00,
        *([0x00] * 0x7DC8),
        # IROM segment
        0x20, 0x80, 0x00, 0x42, 0x00, 0x01, 0x00, 0x00,
        *([0x00] * 0x100),
        # Padding
        *([0x00] * 0x0F),
        # CRC8 checksum
        0x3F,
        # Image SHA256
        0x9B, 0x83, 0x45, 0x5A, 0xCD, 0x07, 0xC2, 0x4D, 0x54, 0x29, 0xA9, 0x2B,
        0xA3, 0xB0, 0xFA, 0x97, 0x62, 0x95, 0xDA, 0x5C, 0xAD, 0xEF, 0xAC, 0x9D,
        0xB6, 0xF7, 0x09, 0x95, 0xC2, 0x33, 0x8E, 0x36
    ],
    'esp32c5': [
        0xE9, 0x04, 0x02, 0x1F, 0x00, 0x00, 0x80, 0x40, 0xEE, 0x00, 0x00, 0x00,
        0x17, 0x00, 0x00, 0x64, 0x00, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        # DROM segment
        0x20, 0x00, 0x00, 0x42, 0x00, 0x02, 0x00, 0x00,
        # esp_app_desc structure
        0x32, 0x54, 0xCD, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x31, 0x2E, 0x30, 0x2E, 0x30, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x65, 0x73, 0x70, 0x5F, 0x74, 0x65, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x30, 0x30, 0x3A, 0x30, 0x30, 0x3A, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x4E, 0x6F, 0x76, 0x20, 0x31, 0x31, 0x20, 0x32,
        0x30, 0x32, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x35, 0x2E, 0x35,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x2D, 0x63, 0x66, 0x8B, 0x75, 0xFA, 0x59, 0x05,
        0x53, 0x34, 0x91, 0x71, 0x51, 0x33, 0x91, 0xDD, 0xF8, 0xB1, 0xFE, 0x83,
        0x06, 0xEB, 0x03, 0x80, 0x45, 0xC9, 0x18, 0x20, 0x83, 0x7E, 0x2E, 0x43,
        *([0x00] * 0x50),
        # Padding
        *([0x00] * 0x100),
        # IRAM segment
        0x00, 0x00, 0x80, 0x40, 0x20, 0x00, 0x00, 0x00,
        *([0x00] * 0x20),
        # PADDING segment
        0x00, 0x00, 0x00, 0x00, 0xC8, 0x7D, 0x00, 0x00,
        *([0x00] * 0x7DC8),
        # IROM segment
        0x20, 0x80, 0x00, 0x42, 0x00, 0x01, 0x00, 0x00,
        *([0x00] * 0x100),
        # Padding
        *([0x00] * 0x0F),
        # CRC8 checksum
        0x3F,
        # Image SHA256
        0x1C, 0x25, 0x32, 0xD0, 0x99, 0xBD, 0xA4, 0xE8, 0x20, 0x8E, 0x43, 0x61,
        0xDA, 0xCD, 0x47, 0x3B, 0x2A, 0x1C, 0xCE, 0x38, 0x0F, 0x74, 0xA5, 0x7E,
        0xAA, 0x05, 0xA3, 0x8F, 0x6A, 0x22, 0xAE, 0x0B
    ],
    'esp32c61': [
        0xE9, 0x04, 0x02, 0x1F, 0x00, 0x00, 0x80, 0x40, 0xEE, 0x00, 0x00, 0x00,
        0x14, 0x00, 0x00, 0x64, 0x00, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        # DROM segment
        0x20, 0x00, 0x00, 0x42, 0x00, 0x02, 0x00, 0x00,
        # esp_app_desc structure
        0x32, 0x54, 0xCD, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x31, 0x2E, 0x30, 0x2E, 0x30, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x65, 0x73, 0x70, 0x5F, 0x74, 0x65, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x30, 0x30, 0x3A, 0x30, 0x30, 0x3A, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x4E, 0x6F, 0x76, 0x20, 0x31, 0x31, 0x20, 0x32,
        0x30, 0x32, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x35, 0x2E, 0x35,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x2D, 0x63, 0x66, 0x8B, 0x75, 0xFA, 0x59, 0x05,
        0x53, 0x34, 0x91, 0x71, 0x51, 0x33, 0x91, 0xDD, 0xF8, 0xB1, 0xFE, 0x83,
        0x06, 0xEB, 0x03, 0x80, 0x45, 0xC9, 0x18, 0x20, 0x83, 0x7E, 0x2E, 0x43,
        *([0x00] * 0x50),
        # Padding
        *([0x00] * 0x100),
        # IRAM segment
        0x00, 0x00, 0x80, 0x40, 0x20, 0x00, 0x00, 0x00,
        *([0x00] * 0x20),
        # PADDING segment
        0x00, 0x00, 0x00, 0x00, 0xC8, 0x7D, 0x00, 0x00,
        *([0x00] * 0x7DC8),
        # IROM segment
        0x20, 0x80, 0x00, 0x42, 0x00, 0x01, 0x00, 0x00,
        *([0x00] * 0x100),
        # Padding
        *([0x00] * 0x0F),
        # CRC8 checksum
        0x3F,
        # Image SHA256
        0x3F, 0xD4, 0x9C, 0xF4, 0xBC, 0x38, 0xA3, 0x6A, 0xB9, 0xCA, 0x09, 0x3A,
        0x79, 0xFD, 0x9C, 0x20, 0xE2, 0x74, 0x89, 0xA5, 0x04, 0xAD, 0x7C, 0x45,
        0xB7, 0xB7, 0x56, 0xCD, 0x55, 0x8F, 0xD5, 0x6D
    ],
}
# fmt: on


# This is a custom IdfSerial class to support custom functionality
# which is required only for this test
class TEESerial(IdfSerial):
    def __init__(self, *args, **kwargs) -> None:  # type: ignore
        super().__init__(*args, **kwargs)

    def _get_flash_size(self) -> Any:
        return self.app.sdkconfig.get('ESPTOOLPY_FLASHSIZE', '')

    @EspSerial.use_esptool()
    def custom_erase_partition(self, partition: str) -> None:
        if self.app.sdkconfig.get('SECURE_ENABLE_SECURE_ROM_DL_MODE'):
            with tempfile.NamedTemporaryFile(delete=True) as temp_file:
                offs = self.app.partition_table[partition]['offset']
                size = self.app.partition_table[partition]['size']
                flash_size = self._get_flash_size()

                binstr = b'\xff' * int(size)
                temp_file.write(binstr)
                temp_file.flush()

                esptool.main(
                    f'--no-stub write-flash {offs} {temp_file.name} --flash-size {flash_size}'.split(), esp=self.esp
                )
        else:
            self.erase_partition(partition)

    @EspSerial.use_esptool()
    def custom_write_partition(self, partition: str, bin_path: str, encrypt: bool = False) -> None:
        offs = self.app.partition_table[partition]['offset']
        no_stub = '--no-stub' if self.app.sdkconfig.get('SECURE_ENABLE_SECURE_ROM_DL_MODE') else ''
        flash_size = self._get_flash_size()
        flash_file = bin_path

        args = f'{no_stub} write-flash {offs} {flash_file}'.split()

        if encrypt:
            args.append('--encrypt')
        args += f'--flash-size {flash_size}'.split()

        esptool.main(args, esp=self.esp)

    @EspSerial.use_esptool()
    def copy_test_tee_img(self, partition: str, is_rollback: bool = False) -> None:
        flash_file = os.path.join(self.app.binary_path, 'esp_tee', 'esp_tee.bin')
        encrypt = self.app.sdkconfig.get('SECURE_FLASH_ENC_ENABLED', False)

        if is_rollback:
            datafile = 'esp_tee_empty.bin'
            datafile_signed = 'esp_tee_empty_signed.bin'
            flash_file = datafile

            with open(datafile, 'wb') as data_file:
                bin_data = esp_tee_empty_bin.get(self.app.sdkconfig.get('IDF_TARGET'), None)
                if bin_data is not None:
                    data_file.write(bytes(bin_data))
                    data_file.flush()

            if self.app.sdkconfig.get('SECURE_BOOT'):
                keyfile = self.app.sdkconfig.get('SECURE_BOOT_SIGNING_KEY')
                espsecure.main(
                    [
                        'sign-data',
                        '--version',
                        '2',
                        '--append-signatures',
                        '--keyfile',
                        keyfile,
                        '--output',
                        datafile_signed,
                        datafile,
                    ]
                )
                flash_file = datafile_signed

        self.custom_write_partition(partition, flash_file, encrypt=encrypt)

        if is_rollback:
            for file in [datafile, datafile_signed]:
                if os.path.exists(file):
                    os.remove(file)

    @EspSerial.use_esptool()
    def custom_flash_w_test_tee_img_gen(self) -> None:
        self.flash()
        self.copy_test_tee_img('ota_1', False)

    @EspSerial.use_esptool()
    def custom_flash_w_test_tee_img_rb(self) -> None:
        self.flash()
        self.copy_test_tee_img('ota_1', True)

    @EspSerial.use_esptool()
    def custom_flash_with_empty_sec_stg(self) -> None:
        self.flash()
        self.custom_erase_partition('secure_storage')

    KEY_DEFS_ENCRYPTION_TEST: list[str] = [
        'aes256_key0',
        'aes256_key1',
        'attest_key',
        'ecdsa_p256_key0',
    ]

    # TEE Secure Storage Development mode
    # NVS XTS-AES keys: E-key=0x33*32 || T-key=0xCC*32
    NVS_KEYS_DEV_B64 = 'MzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzPMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzMzA=='

    @property
    def nvs_partition_gen(self) -> str:
        return str(
            Path(os.environ['IDF_PATH'])
            / 'components'
            / 'nvs_flash'
            / 'nvs_partition_generator'
            / 'nvs_partition_gen.py'
        )

    def derive_sec_stg_nvs_keys(self, out_path: Path) -> None:
        out_path.parent.mkdir(parents=True, exist_ok=True)
        if self.app.sdkconfig.get('SECURE_TEE_SEC_STG_MODE_RELEASE'):
            hmac_key_src = self.TEST_KEYS_DIR / 'tee_sec_stg_hmac_key.bin'
            self.run_command(
                [
                    sys.executable,
                    self.nvs_partition_gen,
                    'generate-key',
                    '--key_protect_hmac',
                    '--kp_hmac_inputkey',
                    str(hmac_key_src),
                    '--keyfile',
                    out_path.name,
                    '--outdir',
                    str(out_path.parent),
                ]
            )
            (out_path.parent / 'keys' / out_path.name).replace(out_path)
        else:
            self.write_keys_to_file(self.NVS_KEYS_DEV_B64, out_path)

    def decrypt_sec_stg_partition(self, dump_path: Path, keys_path: Path, decrypted_path: Path) -> None:
        self.run_command(
            [sys.executable, self.nvs_partition_gen, 'decrypt', str(dump_path), str(keys_path), str(decrypted_path)]
        )

    _SEC_STG_HEX_LINE_RE = re.compile(
        rb'test_esp_tee_sec_storage:\s+((?:[0-9a-f]{2} ){0,15}[0-9a-f]{2})',
    )
    SEC_STG_DUMP_SZ = 4096

    def capture_sec_stg_partition_dump(self, dut: Any, timeout: float = 60) -> bytes:
        dut.expect_exact('SEC_STG_DUMP_BEGIN', timeout=timeout)
        blob = dut.expect_exact('SEC_STG_DUMP_END', timeout=timeout, return_what_before_match=True)

        raw = bytearray()
        for match in self._SEC_STG_HEX_LINE_RE.finditer(blob):
            for tok in match.group(1).split():
                raw.append(int(tok, 16))

        if len(raw) != self.SEC_STG_DUMP_SZ:
            raise RuntimeError(
                f'Hex dump parse mismatch: got {len(raw)} bytes, expected {self.SEC_STG_DUMP_SZ}.\n'
                f'Blob (first 256 bytes): {blob[:256]!r}'
            )

        # Make sure the Unity case actually passed before we trust the dump.
        m = dut.expect(re.compile(rb'(\d+) Tests (\d+) Failures (\d+) Ignored'), timeout=timeout)
        if int(m.group(2)) != 0:
            raise RuntimeError(f'Unity reported {m.group(2).decode()} failures while running encryption test')

        return bytes(raw)

    def _run_nvs_tool_minimal(self, partition_file: Path) -> subprocess.CompletedProcess:
        nvs_tool = Path(os.environ['IDF_PATH']) / 'components' / 'nvs_flash' / 'nvs_partition_tool' / 'nvs_tool.py'
        return subprocess.run(
            [sys.executable, str(nvs_tool), '-d', 'minimal', '--color', 'never', str(partition_file)],
            capture_output=True,
            text=True,
        )

    def verify_tee_sec_stg_encryption(self, dut: Any) -> None:
        tmp_dir = self.TMP_DIR / 'sec_stg_encryption'
        tmp_dir.mkdir(parents=True, exist_ok=True)
        raw_path = tmp_dir / 'tee_sec_stg_dump.bin'
        keys_path = tmp_dir / self.NVS_KEYS_FILE
        decrypted_path = tmp_dir / 'tee_sec_stg_decr.bin'
        expected_key_ids = self.KEY_DEFS_ENCRYPTION_TEST

        print('Verifying TEE Secure Storage NVS partition encryption (XTS-AES-512: 256-bit AES, 512-bit total key)')
        try:
            raw_bytes = self.capture_sec_stg_partition_dump(dut)
            raw_path.write_bytes(raw_bytes)

            self.derive_sec_stg_nvs_keys(keys_path)
            self.decrypt_sec_stg_partition(raw_path, keys_path, decrypted_path)

            print('Confirming key IDs are NOT present in the raw (encrypted) NVS dump')
            raw_parse = self._run_nvs_tool_minimal(raw_path)
            for key_id in expected_key_ids:
                assert key_id not in raw_parse.stdout, f'{key_id!r} surfaced in raw dump (not encrypted)'

            print('Confirming key IDs ARE present after XTS-AES decrypt with the derived NVS keys')
            decrypted_parse = self._run_nvs_tool_minimal(decrypted_path)
            assert decrypted_parse.returncode == 0, f'nvs_tool exit {decrypted_parse.returncode} on decrypted dump'
            for key_id in expected_key_ids:
                assert key_id in decrypted_parse.stdout, f'{key_id!r} missing after decrypt (wrong XTS-AES key?)'
        finally:
            shutil.rmtree(tmp_dir, ignore_errors=True)

    KEY_DEFS: list[dict[str, Any]] = [
        {'key': 'aes256_key0', 'type': 'aes256', 'input': None, 'write_once': True},
        {
            'key': 'aes256_key1',
            'type': 'aes256',
            'input': 'aes256_key.bin',
            'write_once': False,
            'b64': 'qZxftt2T8mOpLxALIfsDqI65srqPxrJtCVnDU8wrKXY=',
        },
        {'key': 'p256_key0', 'type': 'ecdsa_p256', 'input': None, 'write_once': False},
        {
            'key': 'attest_key',
            'type': 'ecdsa_p256',
            'input': 'ecdsa_p256_key.pem',
            'write_once': True,
            'b64': (
                'LS0tLS1CRUdJTiBFQyBQUklWQVRFIEtFWS0tLS0tCk1IY0NBUUVFSUlNU1VpUktHaVZjSTIvbUZFekI3eXRIOVJj'
                'd0wyUThkNDhONHNFUHFYc0RvQW9HQ0NxR1NNNDkKQXdFSG9VUURRZ0FFSkYxYXRZQUxrdnB4cCt4N3c1dmVPQ1Vj'
                'RUhFRTY5azkvcFB5eFdTbEZkbW5wMnBmbVJpZwp5NnRTMDNaM2tnN2hYcitTQmNLbmRCV2RlZW81Vm9XV29nPT0K'
                'LS0tLS1FTkQgRUMgUFJJVkFURSBLRVktLS0tLQo='
            ),
        },
        {
            'key': 'p384_key0',
            'type': 'ecdsa_p384',
            'input': 'ecdsa_p384_key.pem',
            'write_once': True,
            'b64': (
                'LS0tLS1CRUdJTiBFQyBQUklWQVRFIEtFWS0tLS0tCk1JR2tBZ0VCQkRDQUIreENyckkrOWJObWlzVlVsSG1R'
                'T3dZMHIyNWVIL09DalVMUVY4NHZtZU0wWDVLMnYrRE4KZkdSUkRZd0NheGlnQndZRks0RUVBQ0toWkFOaUFB'
                'UVFWcHFObVY0Qm1aemw5VWZBcm0veVpiak9QZTJoUEtDRQp0RERZLy9FS0lOZExvY1dvMUgwWWhzYTBuU2xj'
                'cGk2SDM3eXlSVHNxUnFueFBOeXFYZDdqbXo1WkIySWZHWHhBCmptZnZLVmJIcVNJV0JDRUx0WUxwS2t3UHNCO'
                'XV4WW89Ci0tLS0tRU5EIEVDIFBSSVZBVEUgS0VZLS0tLS0='
            ),
        },
    ]

    TEST_KEYS_DIR = Path(__file__).parent / 'test_keys'
    TMP_DIR = Path('tmp')
    NVS_CSV_PATH = TMP_DIR / 'tee_sec_stg_val.csv'
    NVS_BIN_PATH = TMP_DIR / 'tee_sec_stg_nvs.bin'
    NVS_KEYS_FILE = 'tee_sec_stg_nvs_keys.bin'

    def run_command(self, command: list[str]) -> None:
        try:
            subprocess.check_call(command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        except subprocess.CalledProcessError:
            print(f'Command failed: {" ".join(command)}')
            raise

    def write_keys_to_file(self, b64_data: str, path: Path) -> None:
        path.write_bytes(base64.b64decode(b64_data))

    def create_tee_sec_stg_csv(self, tmp_dir: Path) -> Path:
        csv_path = self.NVS_CSV_PATH
        rows: list[list[str]] = [
            ['key', 'type', 'encoding', 'value'],
            ['tee_sec_stg_ns', 'namespace', '', ''],
        ]
        rows += [[entry['key'], 'file', 'binary', str(tmp_dir / f'{entry["key"]}.bin')] for entry in self.KEY_DEFS]
        with csv_path.open('w', newline='') as f:
            csv.writer(f).writerows(rows)
        return csv_path

    def custom_flash_with_host_gen_sec_stg_img(self) -> None:
        tmp_dir = self.TMP_DIR
        tmp_dir.mkdir(parents=True, exist_ok=True)

        for entry in self.KEY_DEFS:
            if entry['input']:
                input_path = tmp_dir / entry['input']
                self.write_keys_to_file(entry['b64'], input_path)
                entry['input'] = str(input_path)

        ESP_TEE_SEC_STG_KEYGEN = os.path.join(
            os.environ['IDF_PATH'],
            'components',
            'esp_tee',
            'scripts',
            'esp_tee_sec_stg_keygen',
            'esp_tee_sec_stg_keygen.py',
        )

        nvs_keys = tmp_dir / self.NVS_KEYS_FILE
        self.derive_sec_stg_nvs_keys(nvs_keys)

        cmds = [
            [sys.executable, ESP_TEE_SEC_STG_KEYGEN, '-k', entry['type'], '-o', str(tmp_dir / f'{entry["key"]}.bin')]
            + (['-i', entry['input']] if entry['input'] else [])
            + (['--write-once'] if entry['write_once'] else [])
            for entry in self.KEY_DEFS
        ]

        csv_path = self.create_tee_sec_stg_csv(tmp_dir)
        nvs_bin = self.NVS_BIN_PATH
        size = self.app.partition_table['secure_storage']['size']

        cmds.append(
            [
                sys.executable,
                self.nvs_partition_gen,
                'encrypt',
                str(csv_path),
                str(nvs_bin),
                str(size),
                '--inputkey',
                str(nvs_keys),
            ]
        )

        try:
            for cmd in cmds:
                self.run_command(cmd)

            self.flash()
            self.custom_erase_partition('secure_storage')
            self.custom_write_partition('secure_storage', str(nvs_bin))

        finally:
            shutil.rmtree(tmp_dir)


@pytest.fixture(scope='module')
def monkeypatch_module(request: FixtureRequest) -> MonkeyPatch:
    mp = MonkeyPatch()
    request.addfinalizer(mp.undo)
    return mp


@pytest.fixture(scope='module', autouse=True)
def replace_dut_class(monkeypatch_module: MonkeyPatch) -> None:
    monkeypatch_module.setattr('pytest_embedded_idf.IdfSerial', TEESerial)
