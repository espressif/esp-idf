# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from .boot_sector import BootSector
from .utils import read_filesystem


class FATFSParser:

    def __init__(self, image_file_path: str, wl_support: bool = False) -> None:
        if wl_support:
            raise NotImplementedError('Parser is not implemented for WL yet.')
        self.fatfs = read_filesystem(image_file_path)

        # when wl is not supported we expect boot sector to be the first
        self.parsed_header = BootSector.BOOT_SECTOR_HEADER.parse(self.fatfs[:BootSector.BOOT_HEADER_SIZE])
        print(BootSector)
