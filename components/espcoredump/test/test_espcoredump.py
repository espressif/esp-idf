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

import sys
import os
import unittest

try:
    import espcoredump
except ImportError:
    idf_path = os.getenv('IDF_PATH')
    if idf_path:
        sys.path.insert(0, os.path.join(idf_path, 'components', 'espcoredump'))
    import espcoredump


class TestESPCoreDumpFileLoader(unittest.TestCase):
    def setUp(self):
        self.tmp_file = 'tmp'
        self.dloader = espcoredump.ESPCoreDumpFileLoader(path='coredump.b64', b64=True)
        self.assertIsInstance(self.dloader, espcoredump.ESPCoreDumpFileLoader)

    def tearDown(self):
        self.dloader.cleanup()

    def testESPCoreDumpFileLoaderWithoutB64(self):
        t = espcoredump.ESPCoreDumpFileLoader(path='coredump.b64', b64=False)
        self.assertIsInstance(t, espcoredump.ESPCoreDumpFileLoader)  # invoke for coverage of open()
        t.cleanup()

    def test_cannot_remove_dir(self):
        self.dloader.remove_tmp_file(fname='.')  # silent failure (but covers exception inside)

    def test_create_corefile(self):
        self.assertEqual(self.dloader.create_corefile(core_fname=self.tmp_file, off=0, rom_elf=None), self.tmp_file)


if __name__ == '__main__':
    # The purpose of these tests is to increase the code coverage at places which are sensitive to issues related to
    # Python 2&3 compatibility.
    # The espcoredump is not suited for through unit testting. There lot of nested functions, interactive
    # communication with the developement board and GDB, ...
    unittest.main()
