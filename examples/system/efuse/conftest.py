# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
import os
import tempfile
from typing import Any

import espefuse
import esptool
import pytest
from _pytest.fixtures import FixtureRequest
from _pytest.monkeypatch import MonkeyPatch
from pytest_embedded_idf.serial import IdfSerial
from serial import SerialException


# This is a custom IdfSerial class to support custom functionality
# which is required only for this test
class EfuseFlashEncSerial(IdfSerial):
    def _flash_without_reset(self, offset: int, file_path: str) -> None:
        # The caller owns the UART through use_esptool for the whole operation.
        # Keep the bootloader from consuming the intermediate virtual eFuse state.
        try:
            esptool.main(
                [
                    '--after',
                    'no_reset',
                    '--baud',
                    os.getenv('ESPBAUD', '921600'),
                    'write_flash',
                    hex(offset),
                    file_path,
                    *self.app.flash_args['write_flash_args'],
                    *self._force_flag(),
                ],
                esp=self.esp,
            )
        except (esptool.FatalError, SerialException) as error:
            logging.error(
                'esptool write_flash failed on %s (offset=%#x, file=%s): %s: %s',
                self.proc.port,
                offset,
                file_path,
                type(error).__name__,
                error,
            )
            raise

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

    @IdfSerial.use_esptool()
    def bootloader_flash(self) -> None:
        """
        Flash bootloader.

        :return: None
        """
        logging.info('Flashing bootloader')
        bootloader_path = os.path.join(self.app.binary_path, 'bootloader', 'bootloader.bin')
        offs = int(self.app.sdkconfig.get('BOOTLOADER_OFFSET_IN_FLASH', 0))
        logging.info(f'bootloader offset is {hex(offs)}')
        self._flash_without_reset(offs, bootloader_path)

    def erase_field_on_emul_efuse_by_name(self, efuse_names: list) -> None:
        pos_of_bits = []
        for name in efuse_names:
            pos_of_bits.append(self.get_efuse_offset(name))
        self.erase_field_on_emul_efuse(pos_of_bits)

    def _read_emul_efuse(self, offset: int, size: int) -> bytearray:
        with tempfile.TemporaryDirectory() as temp_dir:
            dump_path = os.path.join(temp_dir, 'emul_efuse.bin')
            try:
                esptool.main(
                    ['--after', 'no_reset_stub', 'read_flash', str(offset), str(size), dump_path],
                    esp=self.esp,
                )
            except (esptool.FatalError, SerialException) as error:
                logging.error(
                    'esptool read_flash failed on %s (offset=%#x, size=%d); '
                    'aborting virtual eFuse update before writing: %s: %s',
                    self.proc.port,
                    offset,
                    size,
                    type(error).__name__,
                    error,
                )
                raise
            with open(dump_path, 'rb') as f:
                data = bytearray(f.read())
            if len(data) != size:
                raise esptool.FatalError(f'Incomplete virtual eFuse dump: expected {size} bytes, got {len(data)}')
        return data

    @IdfSerial.use_esptool()
    def erase_field_on_emul_efuse(self, pos_of_bits: list) -> None:
        partition = self.app.partition_table['emul_efuse']
        offs = partition['offset']
        size = partition['size']
        emul_efuse_bin_path = os.path.join(self.app.binary_path, 'emul_efuse.bin')
        data = self._read_emul_efuse(offs, size)
        logging.info('Erasing field on emulated efuse')

        for pos_of_bit in sorted(pos_of_bits):
            nbytes, nbits = divmod(pos_of_bit, 8)
            data[nbytes] &= ~(1 << nbits)
        with open(emul_efuse_bin_path, 'wb') as f:
            f.write(data)

        # Synchronize with the running stub so the next command reuses it without a reset.
        no_reset = 'no-reset' if int(esptool.__version__.split('.')[0]) >= 5 else 'no_reset'
        self.esp.connect(mode=no_reset)
        logging.info(f'emul efuse offset is {hex(offs)}')
        self._flash_without_reset(offs, emul_efuse_bin_path)

    def get_efuse_offset(self, efuse_name: str) -> Any:
        with tempfile.NamedTemporaryFile(suffix='.json') as temp_file:
            temp_file_path = temp_file.name
            espefuse.main(f'--virt -c {self.target} summary --format json --file {temp_file_path}'.split())
            with open(temp_file_path, encoding='utf-8') as file:
                efuse_summary = json.load(file)
                if efuse_name in efuse_summary:
                    data = efuse_summary[efuse_name]
                    offset = int(data['word'] * 32) + data['pos']
                    print(f'{efuse_name} offset = {offset}')
                    return offset
                else:
                    raise ValueError(f"eFuse '{efuse_name}' not found in the summary.")


@pytest.fixture(scope='module')
def monkeypatch_module(request: FixtureRequest) -> MonkeyPatch:
    mp = MonkeyPatch()
    request.addfinalizer(mp.undo)
    return mp


@pytest.fixture(scope='module', autouse=True)
def replace_dut_class(monkeypatch_module: MonkeyPatch) -> None:
    monkeypatch_module.setattr('pytest_embedded_idf.IdfSerial', EfuseFlashEncSerial)
