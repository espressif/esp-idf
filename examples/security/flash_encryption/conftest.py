# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging

import pytest
from _pytest.fixtures import FixtureRequest
from _pytest.monkeypatch import MonkeyPatch
from pytest_embedded_idf.serial import IdfSerial


# This is a custom Serial Class to add the erase_flash functionality
class FlashEncSerial(IdfSerial):

    @IdfSerial.use_esptool
    def erase_partition(self, partition_name: str) -> None:

        if partition_name is None:
            logging.error('Invalid arguments')
            return

        if not self.app.partition_table:
            logging.error('Partition table not parsed.')
            return

        if partition_name in self.app.partition_table:
            address = self.app.partition_table[partition_name]['offset']
            size = self.app.partition_table[partition_name]['size']
            logging.info('Erasing the partition {0} of size {1} at {2}'.format(partition_name, size, address))
            self.stub.erase_region(address, size)
        else:
            logging.error('partition name {0} not found in app partition table'.format(partition_name))
            return


@pytest.fixture(scope='module')
def monkeypatch_module(request: FixtureRequest) -> MonkeyPatch:
    mp = MonkeyPatch()
    request.addfinalizer(mp.undo)
    return mp


@pytest.fixture(scope='module', autouse=True)
def replace_dut_class(monkeypatch_module: MonkeyPatch) -> None:
    monkeypatch_module.setattr('pytest_embedded_idf.serial.IdfSerial', FlashEncSerial)
