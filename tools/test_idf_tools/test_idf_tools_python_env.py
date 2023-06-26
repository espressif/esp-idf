# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# NOTE: unittest is by default sorting tests based on their names,
# so the order if which the tests are started may be different from
# the order in which they are defined. Please make sure all tests
# are entirely self contained and don't have affect on other tests,
# for example by changing some global state, like system environment.
# If test needs to change global state, it should return it to the
# original state after it's finished. For more information please see
# https://docs.python.org/3/library/unittest.html#organizing-test-code

import inspect
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

# Set default global paths for idf_tools. If some test needs to
# use functions from idf_tools with custom paths, it should
# set it in setUp() and change them back to defaults in tearDown().
idf_tools.global_idf_path = IDF_PATH
idf_tools.global_idf_tools_path = TOOLS_DIR


class BasePythonInstall(unittest.TestCase):
    def run_tool(self, cmd):  # type: (List[str]) -> str
        ret = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=300)
        decoded_output = ret.stdout.decode('utf-8', 'ignore')
        with open(os.path.join(IDF_PATH, 'tools', 'test_idf_tools', 'test_python_env_logs.txt'), 'a+') as w:
            # stack() returns list of callers frame records. [1] represent caller of this function
            w.write('============================= ' + inspect.stack()[1].function + ' =============================\n')
            w.write(decoded_output)
        return decoded_output

    def run_idf_tools(self, args):  # type: (List[str]) -> str
        cmd = [sys.executable, '../idf_tools.py'] + args
        return self.run_tool(cmd)

    def run_in_venv(self, args):  # type: (List[str]) -> str
        _, _, python_venv, _ = idf_tools.get_python_env_path()
        cmd = [python_venv] + args
        return self.run_tool(cmd)

    def dump_foopackage(self):  # type: () -> str
        # Wheel for foopackage-0.99-py3-none-any.whl
        # This is dummy package for testing purposes created with
        # python -m build --wheel for the following package
        '''
        ├── foopackage
        │   └── __init__.py
        └── setup.py

        setup.py
        from setuptools import setup

        setup(
            name="foopackage",
            version="0.99",
        )

        __init__.py
        if __name__ == '__main__':
            return
        '''

        whl = (b'PK\x03\x04\x14\x00\x00\x00\x08\x00\x07fqVz|E\t&\x00\x00\x00&\x00\x00\x00\x16\x00\x00\x00'
               b'foopackage/__init__.py\xcbLS\x88\x8f\xcfK\xccM\x8d\x8fW\xb0\xb5UP\x8f\x8f\xcfM\xcc\xcc\x8b\x8fW'
               b'\xb7\xe2R\x00\x82\xa2\xd4\x92\xd2\xa2<.\x00PK\x03\x04\x14\x00\x00\x00\x08\x00%fqV\x8d\x90\x81\x05'
               b'1\x00\x00\x006\x00\x00\x00"\x00\x00\x00foopackage-0.99.dist-info/METADATA\xf3M-ILI,I\xd4\rK-*\xce'
               b'\xcc\xcf\xb3R0\xd23\xe4\xf2K\xccM\xb5RH\xcb\xcf/HL\xceNLO\xe5\x82\xcb\x1a\xe8YZrq\x01\x00PK\x03\x04'
               b'\x14\x00\x00\x00\x08\x00%fqVI\xa2!\xcb\\\x00\x00\x00\\\x00\x00\x00\x1f\x00\x00\x00foopackage-0.99'
               b'.dist-info/WHEEL\x0b\xcfHM\xcd\xd1\rK-*\xce\xcc\xcf\xb3R0\xd43\xe0rO\xcdK-J,\xc9/\xb2RHJ\xc9,.\x89/'
               b'\x07\xa9Q\xd00\xd031\xd03\xd0\xe4\n\xca\xcf/\xd1\xf5,\xd6\r(-J\xcd\xc9L\xb2R()*M\xe5\nIL\xb7R(\xa84'
               b'\xd6\xcd\xcb\xcfK\xd5M\xcc\xab\xe4\xe2\x02\x00PK\x03\x04\x14\x00\x00\x00\x08\x00%fqVI*\x9e\xa7\r\x00'
               b'\x00\x00\x0b\x00\x00\x00\'\x00\x00\x00foopackage-0.99.dist-info/top_level.txtK\xcb\xcf/HL\xceNLO\xe5'
               b'\x02\x00PK\x03\x04\x14\x00\x00\x00\x08\x00%fqV&\xdc\x9b\x88\xfd\x00\x00\x00}\x01\x00\x00 \x00\x00\x00'
               b'foopackage-0.99.dist-info/RECORD}\xcc;\x92\x820\x00\x00\xd0\xde\xb3\x04\xe4#\xbfb\x8b\xac\xb0\x0b,'
               b'\xa8\x83\x02#M&\x08\x81\x80\x02c\x02\x82\xa7\xb7rK\xdf\x01\x1e\xe9\xfb\x01_Z\\\x95k\x84hG9B\xe2\xb0'
               b'\x00VcE\xd3\xbf\xf4\xe6\xe1\t6a2\xc3\x16N\x06]1Bm\xb7\x17\xc2Z\xef\xaa\xed\xf6\x9c\xdaQ \xd0\xf6\xc6'
               b':\xec\x00\xd5\\\x91\xffL\x90D\xcb\x12\x0b\xca\xb8@;\xd2\xafC\xe7\x04mx\x82\xef\xb8\xf2\xc6"\xd9\xdd'
               b'\r\x18\xe4\xcd\xef=\xf7\n7\x9eg4?\xa7\x04V*gXI\xff\xcanD\xc1\xf1\xc0\x80\xb6\xf9\x10\xa7\xae\xe3\x04'
               b'\xefuh/<;?\xe3\xe3\x06\x9e\x93N/|\xc1Puc\xefgt\xfaQJ3\x82V\x8e\xb2\xef\x86\x12\xd9\x04\x96\xf2a\xe5'
               b'\xfd\x80\xae\xe5T^E>\xf3\xf7\x1eW\x122\xe4\x91\xfbi\x1f\xd6\xeem\x99\xd4\xec\x11Ju\x9d\'R\xc83R\x19>'
               b'jbO:\xb8\x8b\td\xf9\xc3\x1e9\xdb}d\x03\xb0z\x01PK\x01\x02\x14\x03\x14\x00\x00\x00\x08\x00\x07fqVz|E\t'
               b'&\x00\x00\x00&\x00\x00\x00\x16\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xa4\x81\x00\x00\x00\x00'
               b'foopackage/__init__.pyPK\x01\x02\x14\x03\x14\x00\x00\x00\x08\x00%fqV\x8d\x90\x81\x051\x00\x00\x006\x00'
               b'\x00\x00"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xa4\x81Z\x00\x00\x00foopackage-0.99.dist-info'
               b'/METADATAPK\x01\x02\x14\x03\x14\x00\x00\x00\x08\x00%fqVI\xa2!\xcb\\\x00\x00\x00\\\x00\x00\x00\x1f\x00'
               b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xa4\x81\xcb\x00\x00\x00foopackage-0.99.dist-info/WHEELPK\x01'
               b'\x02\x14\x03\x14\x00\x00\x00\x08\x00%fqVI*\x9e\xa7\r\x00\x00\x00\x0b\x00\x00\x00\'\x00\x00\x00\x00\x00'
               b'\x00\x00\x00\x00\x00\x00\xa4\x81d\x01\x00\x00foopackage-0.99.dist-info/top_level.txtPK\x01\x02\x14\x03'
               b'\x14\x00\x00\x00\x08\x00%fqV&\xdc\x9b\x88\xfd\x00\x00\x00}\x01\x00\x00 \x00\x00\x00\x00\x00\x00\x00'
               b'\x00\x00\x00\x00\xb4\x81\xb6\x01\x00\x00foopackage-0.99.dist-info/RECORDPK\x05\x06\x00\x00\x00\x00\x05'
               b'\x00\x05\x00\x84\x01\x00\x00\xf1\x02\x00\x00\x00\x00')

        tmpdir = tempfile.mkdtemp()
        foopackage_fn = os.path.join(tmpdir, 'foopackage-0.99-py3-none-any.whl')
        with open(foopackage_fn, 'wb') as fd:
            fd.write(whl)

        self.addCleanup(shutil.rmtree, tmpdir)
        return foopackage_fn


class TestPythonInstall(BasePythonInstall):

    def setUp(self):  # type: () -> None
        if os.path.isdir(PYTHON_DIR):
            shutil.rmtree(PYTHON_DIR)
        if os.path.isfile(os.path.join(TOOLS_DIR, 'idf-env.json')):
            os.remove(os.path.join(TOOLS_DIR, 'idf-env.json'))

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


class TestCustomPythonPathInstall(BasePythonInstall):

    def setUp(self):  # type: () -> None
        self.CUSTOM_PYTHON_DIR = tempfile.mkdtemp()
        self.environ_old = os.environ.copy()
        os.environ['IDF_PYTHON_ENV_PATH'] = self.CUSTOM_PYTHON_DIR

    def tearDown(self):  # type: () -> None
        os.environ.clear()
        os.environ.update(self.environ_old)
        shutil.rmtree(self.CUSTOM_PYTHON_DIR)

    def test_default_arguments(self):  # type: () -> None
        output = self.run_idf_tools(['check-python-dependencies'])
        self.assertIn(f"{self.CUSTOM_PYTHON_DIR}/bin/python doesn't exist", output)
        self.assertNotIn(PYTHON_DIR, output)

        output = self.run_idf_tools(['install-python-env'])
        self.assertIn(self.CUSTOM_PYTHON_DIR, output)
        self.assertNotIn(PYTHON_DIR, output)

        output = self.run_idf_tools(['check-python-dependencies'])
        self.assertIn(self.CUSTOM_PYTHON_DIR, output)


class TestCheckPythonDependencies(BasePythonInstall):
    def setUp(self):  # type: () -> None
        if os.path.isdir(PYTHON_DIR):
            shutil.rmtree(PYTHON_DIR)

    def tearDown(self):  # type: () -> None
        if os.path.isdir(PYTHON_DIR):
            shutil.rmtree(PYTHON_DIR)

    def test_check_python_dependencies(self):  # type: () -> None
        # Prepare artificial constraints file containing packages from
        # requirements.core.txt, which are also reported in pip-freeze output
        # for virtual env. The constraints file requires package versions higher
        # than currently installed in venv, so check_python_dependencies
        # should fail for all of them.
        self.run_idf_tools(['install-python-env'])
        freeze_output = self.run_in_venv(['-m', 'pip', 'freeze', '--all'])

        req_fn = os.path.join(IDF_PATH, 'tools', 'requirements', 'requirements.core.txt')
        with open(req_fn) as fd:
            req_list = [i for i in fd.read().splitlines() if i and i[0] != '#']

        # Create constrains list for packages in requirements.core.txt which
        # are also present in the freeze list.
        con_list = [r.replace('==', '>') for r in freeze_output.splitlines() if r.split('==')[0] in req_list]

        con_fn = idf_tools.get_constraints(idf_tools.get_idf_version(), online=False)
        # delete modified constraints file after this test is finished
        self.addCleanup(os.remove, con_fn)

        # Write the created constraints list into existing constraints file.
        # It will not be overwritten by subsequent idf_tools.py run, because
        # there is timestamp check.
        with open(con_fn, 'w') as fd:
            fd.write(os.linesep.join(con_list))

        # Test that check_python_dependencies reports that requirements are not satisfied for
        # all packages in the artificially created constrains file.
        output = self.run_idf_tools(['check-python-dependencies'])
        for con in [c.split('>')[0] for c in con_list]:
            self.assertIn(con, output)

    def test_check_required_packages_only(self):  # type: () -> None
        # Test for espressif/esp-idf/-/merge_requests/17917
        # Install python env with core requirements, plus foopackage.
        # Add foopackage to constraints file requiring higher version
        # than currently installed. Since foopackage is not a direct
        # requirement, the dependency check should ignore it and should
        # not fail.
        self.run_idf_tools(['install-python-env'])
        foo_pkg = self.dump_foopackage()
        self.run_in_venv(['-m', 'pip', 'install', foo_pkg])

        con_fn = idf_tools.get_constraints(idf_tools.get_idf_version(), online=False)
        # delete modified constraints file after this test is finished
        self.addCleanup(os.remove, con_fn)

        # append foopackage constraint to the existing constraints file
        with open(con_fn, 'a') as fd:
            fd.write('foopackage>0.99')

        # check-python-dependencies should not complain about dummy_package
        output = self.run_idf_tools(['check-python-dependencies'])
        self.assertIn(REQ_SATISFIED, output)


if __name__ == '__main__':
    unittest.main()
