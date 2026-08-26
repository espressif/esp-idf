# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
    monkeypatch_module.setattr('pytest_embedded_idf.IdfSerial', FlashBootloader)
