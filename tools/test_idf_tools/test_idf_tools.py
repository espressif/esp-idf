#!/usr/bin/env python
#
# Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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
import tempfile
import shutil

try:
    from contextlib import redirect_stdout
except ImportError:
    import contextlib

    @contextlib.contextmanager
    def redirect_stdout(target):
        original = sys.stdout
        sys.stdout = target
        yield
        sys.stdout = original

try:
    from cStringIO import StringIO
except ImportError:
    from io import StringIO

# Need to do this before importing idf_tools.py
os.environ['IDF_MAINTAINER'] = '1'

try:
    import idf_tools
except ImportError:
    sys.path.append('..')
    import idf_tools


class TestUsage(unittest.TestCase):

    def test_usage_basic(self):
        old_tools_dir = os.environ.get('IDF_TOOLS_PATH') or os.path.expanduser(idf_tools.IDF_TOOLS_PATH_DEFAULT)

        mirror_prefix_map = None
        if os.path.exists(old_tools_dir):
            mirror_prefix_map = 'https://dl.espressif.com/dl,file://' + os.path.join(old_tools_dir, 'dist')
            mirror_prefix_map += ';https://github.com/espressif/.*/releases/download/.*/,file://' + os.path.join(old_tools_dir, 'dist', '')
        if mirror_prefix_map:
            print('Using IDF_MIRROR_PREFIX_MAP={}'.format(mirror_prefix_map))
            os.environ['IDF_MIRROR_PREFIX_MAP'] = mirror_prefix_map

        temp_tools_dir = tempfile.mkdtemp(prefix='idf_tools_tmp')
        print('Using IDF_TOOLS_PATH={}'.format(temp_tools_dir))
        os.environ['IDF_TOOLS_PATH'] = temp_tools_dir

        self.addCleanup(shutil.rmtree, temp_tools_dir)

        output_stream = StringIO()
        with redirect_stdout(output_stream):
            idf_tools.main(['list'])
        output = output_stream.getvalue()

        xtensa_esp32_elf_version = 'esp-2019r2-8.2.0'
        esp32ulp_version = '2.28.51.20170517'

        self.assertIn('* xtensa-esp32-elf:', output)
        self.assertIn('- %s (recommended)' % xtensa_esp32_elf_version, output)
        self.assertIn('* esp32ulp-elf', output)
        self.assertIn('- %s (recommended)' % esp32ulp_version, output)

        output_stream = StringIO()
        with redirect_stdout(output_stream):
            idf_tools.main(['install'])
        output = output_stream.getvalue()

        self.assertIn('Installing esp32ulp-elf@' + esp32ulp_version, output)
        self.assertIn('Downloading binutils-esp32ulp', output)
        self.assertIn('Installing xtensa-esp32-elf@' + xtensa_esp32_elf_version, output)
        self.assertIn('Downloading xtensa-esp32-elf', output)
        self.assertIn('to ' + os.path.join(temp_tools_dir, 'dist'), output)

        output_stream = StringIO()
        with redirect_stdout(output_stream):
            idf_tools.main(['check'])
        output = output_stream.getvalue()

        self.assertIn('version installed in tools directory: ' + esp32ulp_version, output)
        self.assertIn('version installed in tools directory: ' + xtensa_esp32_elf_version, output)

        output_stream = StringIO()
        with redirect_stdout(output_stream):
            idf_tools.main(['export'])
        output = output_stream.getvalue()

        self.assertIn('%s/tools/esp32ulp-elf/%s/esp32ulp-elf-binutils/bin' %
                      (temp_tools_dir, esp32ulp_version), output)
        self.assertIn('%s/tools/xtensa-esp32-elf/%s/xtensa-esp32-elf/bin' %
                      (temp_tools_dir, xtensa_esp32_elf_version), output)


class TestMaintainer(unittest.TestCase):

    def test_validation(self):
        idf_tools.main(['validate'])

    def test_json_rewrite(self):
        idf_tools.main(['rewrite'])
        idf_path = os.getenv('IDF_PATH')
        if not idf_path:
            self.fail('IDF_PATH needs to be set to run this test')
        with open(os.path.join(idf_path, 'tools/tools.json'), 'r') as f:
            json_old = f.read()
        with open(os.path.join(idf_path, 'tools/tools.new.json'), 'r') as f:
            json_new = f.read()
        self.assertEqual(json_old, json_new)


if __name__ == '__main__':
    unittest.main()
