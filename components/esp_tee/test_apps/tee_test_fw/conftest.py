# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# pylint: disable=W0621  # redefined-outer-name
import os
import tempfile
from typing import Any

import espsecure
import esptool
import pytest
from _pytest.fixtures import FixtureRequest
from _pytest.monkeypatch import MonkeyPatch
from pytest_embedded_idf.serial import IdfSerial
from pytest_embedded_serial_esp.serial import EspSerial

esp_tee_empty_bin = {
    'esp32c6': [
        0xE9, 0x04, 0x02, 0x10, 0x00, 0x00, 0x80, 0x40, 0xEE, 0x00, 0x00, 0x00,
        0x0D, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        # DROM segment
        0x20, 0x00, 0x00, 0x42, 0x00, 0x02, 0x00, 0x00,
        # esp_app_desc structure
        0x32, 0x54, 0xCD, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x76, 0x35, 0x2E, 0x35, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x65, 0x73, 0x70, 0x5F, 0x74, 0x65, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x30, 0x30, 0x3A, 0x30, 0x30, 0x3A, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x4E, 0x6F, 0x76, 0x20, 0x31, 0x31, 0x20, 0x32,
        0x30, 0x32, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x35, 0x2E, 0x35,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x2D, 0x63, 0x66, 0x8B, 0x75, 0xFA, 0x59, 0x05,
        0x53, 0x34, 0x91, 0x71, 0x51, 0x33, 0x91, 0xDD, 0xF8, 0xB1, 0xFE, 0x83,
        0x06, 0xEB, 0x03, 0x80, 0x45, 0xC9, 0x18, 0x20, 0x83, 0x7E, 0x2E, 0x43,
        *([0x00] * 0x58),
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
        0x56,
        # Image SHA256
        0xF4, 0xA4, 0xCF, 0x06, 0xAE, 0x94, 0x75, 0x47, 0xBC, 0x88, 0xA2, 0xCA,
        0x52, 0x97, 0x7A, 0x5C, 0x55, 0x43, 0xD9, 0xF5, 0xD3, 0x45, 0xD1, 0x34,
        0xFC, 0x74, 0xB2, 0xB9, 0x34, 0x72, 0xC3, 0x00
    ]
}


# This is a custom IdfSerial class to support custom functionality
# which is required only for this test
class TEESerial(IdfSerial):
    def __init__(self, *args, **kwargs) -> None:  # type: ignore
        super().__init__(*args, **kwargs)

    def _get_flash_size(self) -> Any:
        return self.app.sdkconfig.get('ESPTOOLPY_FLASHSIZE', '')

    @EspSerial.use_esptool()
    def bootloader_force_flash_if_req(self) -> None:
        # Forcefully flash the bootloader only if security features are enabled
        if any((
            self.app.sdkconfig.get('SECURE_BOOT', True),
            self.app.sdkconfig.get('SECURE_FLASH_ENC_ENABLED', True),
        )):
            offs = int(self.app.sdkconfig.get('BOOTLOADER_OFFSET_IN_FLASH', 0))
            bootloader_path = os.path.join(self.app.binary_path, 'bootloader', 'bootloader.bin')
            encrypt = '--encrypt' if self.app.sdkconfig.get('SECURE_FLASH_ENC_ENABLED') else ''
            flash_size = self._get_flash_size()

            esptool.main(
                f'--no-stub write_flash {offs} {bootloader_path} --force {encrypt} --flash_size {flash_size}'.split(),
                esp=self.esp
            )

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
                    f'--no-stub write_flash {offs} {temp_file.name} --flash_size {flash_size}'.split(),
                    esp=self.esp
                )
        else:
            self.erase_partition(partition)

    @EspSerial.use_esptool()
    def copy_test_tee_img(self, partition: str, is_rollback: bool = False) -> None:
        offs = self.app.partition_table[partition]['offset']
        no_stub = '--no-stub' if self.app.sdkconfig.get('SECURE_ENABLE_SECURE_ROM_DL_MODE') else ''
        encrypt = '--encrypt' if self.app.sdkconfig.get('SECURE_FLASH_ENC_ENABLED') else ''
        flash_size = self._get_flash_size()

        flash_file = os.path.join(self.app.binary_path, 'esp_tee', 'esp_tee.bin')

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
                # Signing the image with espsecure
                espsecure.main(
                    f'sign_data --version 2 --append_signatures --keyfile {keyfile} --output {datafile_signed} {datafile}'.split()
                )
                flash_file = datafile_signed

        esptool.main(
            f'{no_stub} write_flash {offs} {flash_file} {encrypt} --flash_size {flash_size}'.split(),
            esp=self.esp
        )

        if is_rollback:
            if os.path.exists(datafile):
                os.remove(datafile)
            if os.path.exists(datafile_signed):
                os.remove(datafile_signed)

    @EspSerial.use_esptool()
    def custom_flash(self) -> None:
        self.bootloader_force_flash_if_req()
        self.flash()

    @EspSerial.use_esptool()
    def custom_flash_w_test_tee_img_gen(self) -> None:
        self.bootloader_force_flash_if_req()
        self.flash()
        self.copy_test_tee_img('ota_1', False)

    @EspSerial.use_esptool()
    def custom_flash_w_test_tee_img_rb(self) -> None:
        self.bootloader_force_flash_if_req()
        self.flash()
        self.copy_test_tee_img('ota_1', True)

    @EspSerial.use_esptool()
    def custom_flash_with_empty_sec_stg(self) -> None:
        self.bootloader_force_flash_if_req()
        self.flash()
        self.custom_erase_partition('secure_storage')


@pytest.fixture(scope='module')
def monkeypatch_module(request: FixtureRequest) -> MonkeyPatch:
    mp = MonkeyPatch()
    request.addfinalizer(mp.undo)
    return mp


@pytest.fixture(scope='module', autouse=True)
def replace_dut_class(monkeypatch_module: MonkeyPatch) -> None:
    monkeypatch_module.setattr('pytest_embedded_idf.IdfSerial', TEESerial)
