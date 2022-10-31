# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os

import pytest
from _pytest.fixtures import FixtureRequest
from _pytest.monkeypatch import MonkeyPatch
from pytest_embedded_idf.app import FlashFile
from pytest_embedded_idf.serial import IdfSerial


# This is a custom IdfSerial class to support custom functionality
# which is required only for this test
class EfuseFlashEncSerial(IdfSerial):

    @IdfSerial.use_esptool()
    def write_flash_no_enc(self) -> None:
        self.app.flash_settings['encrypt'] = False
        flash_files = []
        for file in self.app.flash_files:
            # Set encrypted flag to false for each file.
            flash_files.append(file._replace(encrypted=False))
        # Replace the original tuple with modified tuple with all the files marked as unencrypted.
        self.app.flash_files = tuple(flash_files)
        # Now flash the files
        self.flash()

    def bootloader_flash(self) -> None:
        """
        Flash bootloader.

        :return: None
        """
        logging.info('Flashing bootloader')
        bootloader_path = os.path.join(self.app.binary_path, 'bootloader', 'bootloader.bin')
        offs = int(self.app.sdkconfig.get('BOOTLOADER_OFFSET_IN_FLASH', 0))
        logging.info('bootloader offset is {0}'.format(hex(offs)))
        prev_flash_files = self.app.flash_files
        flash_files = []
        flash_files.append(
            FlashFile(
                offs,
                bootloader_path,
                False,
            )
        )
        self.app.flash_files = flash_files
        self.app.flash_settings['encrypt'] = False
        self.flash()
        # Restore self.app.flash files to original value
        self.app.flash_files = prev_flash_files

    def erase_field_on_emul_efuse(self, pos_of_bits: list) -> None:
        emul_efuse_bin_path = os.path.join(self.app.binary_path, 'emul_efuse.bin')
        self.dump_flash(output=emul_efuse_bin_path, partition='emul_efuse')
        logging.info('Erasing field on emulated efuse')

        def erase_bit(pos_of_bit: int) -> None:
            nbytes, nbits = divmod(pos_of_bit, 8)
            with open(emul_efuse_bin_path, 'r+b') as f:
                f.seek(nbytes)
                data = ord(f.read(1))
                data &= ~(1 << nbits)
                f.seek(-1, os.SEEK_CUR)
                f.write(bytes([data]))

        for pos_of_bit in sorted(pos_of_bits):
            erase_bit(pos_of_bit)

        offs = self.app.partition_table['emul_efuse']['offset']
        logging.info('emul efuse offset is {0}'.format(hex(offs)))
        prev_flash_files = self.app.flash_files

        flash_files = []
        flash_files.append(
            FlashFile(
                offs,
                emul_efuse_bin_path,
                False,
            )
        )
        self.app.flash_files = flash_files
        self.app.flash_settings['encrypt'] = False
        self.flash()
        self.app.flash_files = prev_flash_files


@pytest.fixture(scope='module')
def monkeypatch_module(request: FixtureRequest) -> MonkeyPatch:
    mp = MonkeyPatch()
    request.addfinalizer(mp.undo)
    return mp


@pytest.fixture(scope='module', autouse=True)
def replace_dut_class(monkeypatch_module: MonkeyPatch) -> None:
    monkeypatch_module.setattr('pytest_embedded_idf.IdfSerial', EfuseFlashEncSerial)
