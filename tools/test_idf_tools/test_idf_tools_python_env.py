# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import shutil
import subprocess
import sys
import tempfile
import unittest
from typing import List

try:
    import idf_tools
except ImportError:
    sys.path.append('..')
    import idf_tools

IDF_PATH = os.environ.get('IDF_PATH', '../..')
TOOLS_DIR = os.environ.get('IDF_TOOLS_PATH') or os.path.expanduser(idf_tools.IDF_TOOLS_PATH_DEFAULT)
PYTHON_DIR = os.path.join(TOOLS_DIR, 'python_env')
REQ_SATISFIED = 'Python requirements are satisfied'
REQ_CORE = '- {}'.format(os.path.join(IDF_PATH, 'tools', 'requirements', 'requirements.core.txt'))
REQ_GDBGUI = '- {}'.format(os.path.join(IDF_PATH, 'tools', 'requirements', 'requirements.gdbgui.txt'))
CONSTR = 'Constraint file: {}/espidf.constraints'.format(TOOLS_DIR)


class TestPythonInstall(unittest.TestCase):

    def setUp(self):  # type: () -> None
        if os.path.isdir(PYTHON_DIR):
            shutil.rmtree(PYTHON_DIR)
        if os.path.isfile(os.path.join(TOOLS_DIR, 'idf-env.json')):
            os.remove(os.path.join(TOOLS_DIR, 'idf-env.json'))

    def run_idf_tools(self, extra_args):  # type: (List[str]) -> str
        args = [sys.executable, '../idf_tools.py'] + extra_args
        ret = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=300)
        return ret.stdout.decode('utf-8', 'ignore')

    def test_default_arguments(self):  # type: () -> None
        output = self.run_idf_tools(['check-python-dependencies'])
        self.assertNotIn(REQ_SATISFIED, output)
        self.assertIn('bin/python doesn\'t exist', output)

        output = self.run_idf_tools(['install-python-env'])
        self.assertIn(CONSTR, output)
        self.assertIn(REQ_CORE, output)
        self.assertNotIn(REQ_GDBGUI, output)

        output = self.run_idf_tools(['check-python-dependencies'])
        self.assertIn(REQ_SATISFIED, output)

    def test_opt_argument(self):  # type: () -> None
        output = self.run_idf_tools(['install-python-env', '--features', 'gdbgui'])
        self.assertIn(CONSTR, output)
        self.assertIn(REQ_CORE, output)
        self.assertIn(REQ_GDBGUI, output)

        output = self.run_idf_tools(['install-python-env'])
        # The gdbgui should be installed as well because the feature is is stored in the JSON file
        self.assertIn(CONSTR, output)
        self.assertIn(REQ_CORE, output)
        self.assertIn(REQ_GDBGUI, output)

        # Argument that begins with '-' can't stand alone to be parsed as value
        output = self.run_idf_tools(['install-python-env', '--features=-gdbgui'])
        # After removing the gdbgui should not be present
        self.assertIn(CONSTR, output)
        self.assertIn(REQ_CORE, output)
        self.assertNotIn(REQ_GDBGUI, output)

    def test_no_constraints(self):  # type: () -> None
        output = self.run_idf_tools(['install-python-env', '--no-constraints'])
        self.assertNotIn(CONSTR, output)
        self.assertIn(REQ_CORE, output)


class TestCustomPythonPathInstall(TestPythonInstall):

    def setUp(self):  # type: () -> None
        self.CUSTOM_PYTHON_DIR = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, self.CUSTOM_PYTHON_DIR)
        os.environ['IDF_PYTHON_ENV_PATH'] = self.CUSTOM_PYTHON_DIR

    def test_default_arguments(self):  # type: () -> None
        output = self.run_idf_tools(['check-python-dependencies'])
        self.assertIn(f"{self.CUSTOM_PYTHON_DIR}/bin/python doesn't exist", output)
        self.assertNotIn(PYTHON_DIR, output)

        output = self.run_idf_tools(['install-python-env'])
        self.assertIn(self.CUSTOM_PYTHON_DIR, output)
        self.assertNotIn(PYTHON_DIR, output)

        output = self.run_idf_tools(['check-python-dependencies'])
        self.assertIn(self.CUSTOM_PYTHON_DIR, output)


if __name__ == '__main__':
    unittest.main()
