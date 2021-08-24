#!/usr/bin/env python
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import sys
import unittest

try:
    from corefile.elf import ESPCoreDumpElfFile
    from corefile.loader import ESPCoreDumpFileLoader, ESPCoreDumpLoaderError
except ImportError:
    idf_path = os.getenv('IDF_PATH')
    if idf_path:
        sys.path.insert(0, os.path.join(idf_path, 'components', 'espcoredump'))
    else:
        sys.path.insert(0, '..')
    from corefile.elf import ESPCoreDumpElfFile
    from corefile.loader import ESPCoreDumpFileLoader, ESPCoreDumpLoaderError

SUPPORTED_TARGET = ['esp32', 'esp32s2', 'esp32c3']


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
