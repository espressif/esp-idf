# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# pylint: disable=W0621  # redefined-outer-name

import collections
import os
import tempfile
import time

import espefuse
import espsecure
import pytest
import serial
from _pytest.fixtures import FixtureRequest
from _pytest.monkeypatch import MonkeyPatch
from pytest_embedded_idf.app import FlashFile
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.serial import IdfSerial
from pytest_embedded_serial_esp.serial import EspSerial

efuse_reset_port = os.getenv('EFUSEPORT')


# This is a custom Serial Class for the FPGA
class FpgaSerial(IdfSerial):
    def __init__(self, *args, **kwargs) -> None:  # type: ignore
        super().__init__(*args, **kwargs)
        self.efuse_reset_port = efuse_reset_port
        if self.efuse_reset_port is None:
            raise RuntimeError('EFUSEPORT not specified')

        self.secure_boot_en = self.app.sdkconfig.get('CONFIG_SECURE_BOOT') and \
            not self.app.sdkconfig.get('CONFIG_EFUSE_VIRTUAL')

        self.efuses = None
        self.efuse_operations = None

    @EspSerial.use_esptool(hard_reset_after=False, no_stub=True)
    def enable_efuses(self) -> None:
        # We use an extra COM port to reset the efuses on FPGA.
        # Connect DTR pin of the COM port to the efuse reset pin on daughter board
        # Set EFUSEPORT env variable to the extra COM port
        if self.efuse_reset_port is None:
            raise RuntimeError('EFUSEPORT not specified')

        self.efuses, self.efuse_operations = espefuse.get_efuses(self.esp, False, False, True)

    @EspSerial.use_esptool(hard_reset_after=False, no_stub=True)
    def bootloader_flash(self, bootloader_path: str) -> None:
        """
        Flash bootloader.

        :return: None
        """
        offs = int(self.app.sdkconfig.get('BOOTLOADER_OFFSET_IN_FLASH', 0))
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
        self.app.flash_settings['no_stub'] = True
        self.app.flash_settings['force'] = True
        self.flash()
        # Restore self.app.flash files to original value
        self.app.flash_files = prev_flash_files

    @EspSerial.use_esptool(hard_reset_after=False, no_stub=True)
    def app_flash(self, app_path: str) -> None:
        """
        Flash App.

        :return: None
        """
        offs = self.app.flash_args['app']['offset']
        prev_flash_files = self.app.flash_files
        flash_files = []
        flash_files.append(
            FlashFile(
                offs,
                app_path,
                False,
            )
        )
        self.app.flash_files = flash_files
        self.app.flash_settings['encrypt'] = False
        self.app.flash_settings['no_stub'] = True
        self.flash()
        # Restore self.app.flash files to original value
        self.app.flash_files = prev_flash_files

    @EspSerial.use_esptool(hard_reset_after=True, no_stub=True)
    def burn_efuse_key_digest(self, key: str, purpose: str, block: str) -> None:
        if self.efuse_operations is None:
            self.enable_efuses()
        BurnDigestArgs = collections.namedtuple('BurnDigestArgs',
                                                ['keyfile', 'keypurpose', 'block',
                                                 'force_write_always', 'no_write_protect', 'no_read_protect'])
        args = BurnDigestArgs([open(key, 'rb')],
                              [purpose],
                              [block],
                              False, False, True)

        self.efuse_operations.burn_key_digest(self.esp, self.efuses, args)   # type: ignore

    @EspSerial.use_esptool(hard_reset_after=False, no_stub=True)
    def burn_efuse(self, field: str, val: int) -> None:
        if self.efuse_operations is None:
            self.enable_efuses()
        BurnEfuseArgs = collections.namedtuple('BurnEfuseArgs', ['do_not_confirm', 'name_value_pairs'])
        args = BurnEfuseArgs(True, {field: val})

        self.efuse_operations.burn_efuse(self.esp, self.efuses, args)   # type: ignore

    @EspSerial.use_esptool(hard_reset_after=False, no_stub=True)
    def burn_efuse_key(self, key: str, purpose: str, block: str) -> None:
        if self.efuse_operations is None:
            self.enable_efuses()
        BurnKeyArgs = collections.namedtuple('BurnKeyArgs',
                                             ['keyfile', 'keypurpose', 'block',
                                              'force_write_always', 'no_write_protect', 'no_read_protect'])
        args = BurnKeyArgs([key],
                           [purpose],
                           [block],
                           False, False, False)

        self.efuse_operations.burn_key(self.esp, self.efuses, args)   # type: ignore

    def reset_efuses(self) -> None:
        if self.efuse_reset_port is None:
            raise RuntimeError('EFUSEPORT not specified')
        with serial.Serial(self.efuse_reset_port) as efuseport:
            print('Resetting efuses')
            efuseport.dtr = 0
            self.proc.setRTS(0)
            time.sleep(1)
            efuseport.dtr = 1
            self.proc.setRTS(1)
            time.sleep(1)
            self.proc.setRTS(0)
            efuseport.dtr = 0

            self.efuse_operations = None
            self.efuses = None


class FpgaDut(IdfDut):
    ERASE_NVS = True
    FLASH_ENCRYPT_SCHEME = None             # type: str
    FLASH_ENCRYPT_CNT_KEY = None            # type: str
    FLASH_ENCRYPT_CNT_VAL = 0
    FLASH_ENCRYPT_PURPOSE = None            # type: str
    SECURE_BOOT_EN_KEY = None               # type: str
    SECURE_BOOT_EN_VAL = 0
    FLASH_SECTOR_SIZE = 4096

    def __init__(self, *args, **kwargs) -> None:  # type: ignore
        super().__init__(*args, **kwargs)
        self.efuses = None
        self.efuse_operations = None
        self.efuse_reset_port = efuse_reset_port

    def sign_data(self, data_file: str, key_files: str, version: str, append_signature: int = 0) -> bytes:
        SignDataArgs = collections.namedtuple('SignDataArgs',
                                              ['datafile','keyfile','output', 'version', 'append_signatures'])
        with tempfile.NamedTemporaryFile() as outfile:
            args = SignDataArgs(data_file, key_files, outfile.name, str(version), append_signature)
            espsecure.sign_data(args)
            outfile.seek(0)
            return outfile.read()


class Esp32c3FpgaDut(FpgaDut):
    FLASH_ENCRYPT_SCHEME = 'AES-XTS'
    FLASH_ENCRYPT_CNT_KEY = 'SPI_BOOT_CRYPT_CNT'
    FLASH_ENCRYPT_CNT_VAL = 1
    FLASH_ENCRYPT_PURPOSE = 'XTS_AES_128_KEY'
    SECURE_BOOT_EN_KEY = 'SECURE_BOOT_EN'
    SECURE_BOOT_EN_VAL = 1
    WAFER_VERSION = 'WAFER_VERSION_MINOR_LO'
    WAFER_VERSION_VAL = 3

    def burn_wafer_version(self) -> None:
        self.serial.burn_efuse(self.WAFER_VERSION, self.WAFER_VERSION_VAL)

    def flash_encrypt_burn_cnt(self) -> None:
        self.serial.burn_efuse(self.FLASH_ENCRYPT_CNT_KEY, self.FLASH_ENCRYPT_CNT_VAL)

    def flash_encrypt_burn_key(self, key: str, block: int = 0) -> None:
        self.serial.burn_efuse_key(key, self.FLASH_ENCRYPT_PURPOSE, 'BLOCK_KEY%d' % block)

    def flash_encrypt_get_scheme(self) -> str:
        return self.FLASH_ENCRYPT_SCHEME

    def secure_boot_burn_en_bit(self) -> None:
        self.serial.burn_efuse(self.SECURE_BOOT_EN_KEY, self.SECURE_BOOT_EN_VAL)

    def secure_boot_burn_digest(self, digest: str, key_index: int = 0, block: int = 0) -> None:
        self.serial.burn_efuse_key_digest(digest, 'SECURE_BOOT_DIGEST%d' % key_index, 'BLOCK_KEY%d' % block)


class Esp32s3FpgaDut(FpgaDut):
    FLASH_ENCRYPT_SCHEME = 'AES-XTS'
    FLASH_ENCRYPT_CNT_KEY = 'SPI_BOOT_CRYPT_CNT'
    FLASH_ENCRYPT_CNT_VAL = 1
    FLASH_ENCRYPT_PURPOSE = 'XTS_AES_128_KEY'
    SECURE_BOOT_EN_KEY = 'SECURE_BOOT_EN'
    SECURE_BOOT_EN_VAL = 1
    WAFER_VERSION = 'WAFER_VERSION_MINOR_LO'
    WAFER_VERSION_VAL = 1

    def burn_wafer_version(self) -> None:
        self.serial.burn_efuse(self.WAFER_VERSION, self.WAFER_VERSION_VAL)

    def flash_encrypt_burn_cnt(self) -> None:
        self.serial.burn_efuse(self.FLASH_ENCRYPT_CNT_KEY, self.FLASH_ENCRYPT_CNT_VAL)

    def flash_encrypt_burn_key(self, key: str, block: int = 0) -> None:
        self.serial.burn_efuse_key(key, self.FLASH_ENCRYPT_PURPOSE, 'BLOCK_KEY%d' % block)

    def flash_encrypt_get_scheme(self) -> str:
        return self.FLASH_ENCRYPT_SCHEME

    def secure_boot_burn_en_bit(self) -> None:
        self.serial.burn_efuse(self.SECURE_BOOT_EN_KEY, self.SECURE_BOOT_EN_VAL)

    def secure_boot_burn_digest(self, digest: str, key_index: int = 0, block: int = 0) -> None:
        self.serial.burn_efuse_key_digest(digest, 'SECURE_BOOT_DIGEST%d' % key_index, 'BLOCK_KEY%d' % block)


@pytest.fixture(scope='module')
def monkeypatch_module(request: FixtureRequest) -> MonkeyPatch:
    mp = MonkeyPatch()
    request.addfinalizer(mp.undo)
    return mp


@pytest.fixture(scope='module', autouse=True)
def replace_dut_class(monkeypatch_module: MonkeyPatch, pytestconfig: pytest.Config) -> None:
    target = pytestconfig.getoption('target')
    if target == 'esp32c3':
        monkeypatch_module.setattr('pytest_embedded_idf.IdfDut', Esp32c3FpgaDut)
    elif target == 'esp32s3':
        monkeypatch_module.setattr('pytest_embedded_idf.IdfDut', Esp32s3FpgaDut)

    monkeypatch_module.setattr('pytest_embedded_idf.IdfSerial', FpgaSerial)
