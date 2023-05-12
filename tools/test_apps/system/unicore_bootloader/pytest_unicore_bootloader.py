# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import logging
import os

import pytest
from _pytest.fixtures import FixtureRequest
from _pytest.monkeypatch import MonkeyPatch
from pytest_embedded import Dut
from pytest_embedded_idf.app import FlashFile
from pytest_embedded_idf.serial import IdfSerial


# This is a custom IdfSerial class to support custom functionality
# which is required only for this test
class FlashBootloader(IdfSerial):
    def bootloader_flash(self, binary_path: str) -> None:
        """
        Flash bootloader.

        :return: None
        """
        logging.info('Flashing bootloader')
        bootloader_path = os.path.join(binary_path, 'bootloader', 'bootloader.bin')
        logging.info(bootloader_path)
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
        self.flash()
        # Restore self.app.flash files to original value
        self.app.flash_files = prev_flash_files


@pytest.fixture(scope='module')
def monkeypatch_module(request: FixtureRequest) -> MonkeyPatch:
    mp = MonkeyPatch()
    request.addfinalizer(mp.undo)
    return mp


@pytest.fixture(scope='module', autouse=True)
def replace_dut_class(monkeypatch_module: MonkeyPatch) -> None:
    monkeypatch_module.setattr('pytest_embedded_idf.serial.IdfSerial', FlashBootloader)


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize('config', ['multicore'], indirect=True)
def test_multicore_app_and_unicore_bootloader(dut: Dut) -> None:
    dut.expect('Multicore bootloader')
    dut.expect('Multicore app')
    dut.expect('App is running')

    path_to_unicore_build = os.path.join(dut.app.app_path, f'build_{dut.target}_unicore')
    dut.serial.bootloader_flash(path_to_unicore_build)
    dut.expect('Unicore bootloader')
    dut.expect('Multicore app')
    dut.expect('App is running')


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize('config', ['unicore'], indirect=True)
def test_unicore_app_and_multicore_bootloader(dut: Dut) -> None:
    dut.expect('Unicore bootloader')
    dut.expect('Unicore app')
    dut.expect('App is running')

    path_to_unicore_build = os.path.join(dut.app.app_path, f'build_{dut.target}_multicore')
    dut.serial.bootloader_flash(path_to_unicore_build)
    dut.expect('Multicore bootloader')
    dut.expect('Unicore app')
    dut.expect('App is running')
