# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import logging
from typing import Any, Optional

import esptool
from pytest_embedded_idf.app import IdfApp
from pytest_embedded_serial_esp.serial import EspSerial, EsptoolArgs


class LoadableAppSerial(EspSerial):
    def __init__(
        self,
        app: IdfApp,
        target: Optional[str] = None,
        **kwargs: Any,
    ) -> None:
        self.app = app

        if not hasattr(self.app, 'target'):
            raise ValueError(f'Idf app not parsable. Please check if it\'s valid: {self.app.binary_path}')

        if target and self.app.target and self.app.target != target:
            raise ValueError(f'Targets do not match. App target: {self.app.target}, Cmd target: {target}.')

        super().__init__(
            target=target or app.target,
            **kwargs,
        )

    def _start(self) -> None:
        self.load_ram()

    @EspSerial.use_esptool(hard_reset_after=False, no_stub=True)
    def load_ram(self) -> None:
        if not self.app.bin_file:
            logging.error('No image file detected. Skipping load ram...')
            return

        f_bin_file = open(self.app.bin_file, 'rb')

        default_kwargs = {
            'filename': f_bin_file,
            'chip': self.esp.CHIP_NAME.lower().replace('-', ''),
        }

        load_ram_args = EsptoolArgs(**default_kwargs)

        try:
            self.esp.change_baud(460800)
            esptool.load_ram(self.esp, load_ram_args)
        finally:
            f_bin_file.close()
