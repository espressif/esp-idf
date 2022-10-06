#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import unittest

try:
    from esp_coredump.corefile import ESPCoreDumpLoaderError
    from esp_coredump.corefile.elf import ESPCoreDumpElfFile
    from esp_coredump.corefile.loader import ESPCoreDumpFileLoader
except ImportError:
    raise ModuleNotFoundError('No module named "esp_coredump" please install esp_coredump by running '
                              '"python -m pip install esp-coredump"')

SUPPORTED_TARGET = ['esp32', 'esp32s2', 'esp32c3', 'esp32s3', 'esp32c2']


class TestESPCoreDumpElfFile(unittest.TestCase):
    def test_read_elf(self):
        for target in SUPPORTED_TARGET:
            elf = ESPCoreDumpElfFile(os.path.join(target, 'core.elf'))
            assert elf.load_segments
            assert elf.note_segments


class TestESPCoreDumpFileLoader(unittest.TestCase):
    def test_load_wrong_encode_core_bin(self):
        for target in SUPPORTED_TARGET:
            with self.assertRaises(ESPCoreDumpLoaderError):
                ESPCoreDumpFileLoader(path=os.path.join(target, 'coredump.b64'), is_b64=False)

    def test_create_corefile(self):
        for target in SUPPORTED_TARGET:
            loader = ESPCoreDumpFileLoader(path=os.path.join(target, 'coredump.b64'), is_b64=True)
            loader.create_corefile()


if __name__ == '__main__':
    # The purpose of these tests is to increase the code coverage at places which are sensitive to issues related to
    # Python 2&3 compatibility.
    # The espcoredump is not suited for through unit testing. There lot of nested functions, interactive
    # communication with the development board and GDB, ...
    unittest.main()
