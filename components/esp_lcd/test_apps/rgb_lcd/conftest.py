# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from _pytest.fixtures import FixtureRequest
from _pytest.monkeypatch import MonkeyPatch
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


@pytest.fixture(scope='module')
def monkeypatch_module(request: FixtureRequest) -> MonkeyPatch:
    mp = MonkeyPatch()
    request.addfinalizer(mp.undo)
    return mp


@pytest.fixture(scope='module', autouse=True)
def replace_dut_class(monkeypatch_module: MonkeyPatch) -> None:
    monkeypatch_module.setattr('pytest_embedded_idf.IdfSerial', EfuseFlashEncSerial)
