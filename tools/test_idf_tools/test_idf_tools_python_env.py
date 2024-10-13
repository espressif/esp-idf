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
PYTHON_DIR_BACKUP = tempfile.mkdtemp()
PYTHON_BINARY = os.path.join('Scripts', 'python.exe') if sys.platform == 'win32' else os.path.join('bin', 'python')
REQ_SATISFIED = 'Python requirements are satisfied'
# Python 3.8 and 3.9 has a different error message that does not include the "No package metadata was found for" part
REQ_MISSING = r'Package was not found and is required by the application: (No package metadata was found for )?{}'
REQ_CORE = '- {}'.format(os.path.join(IDF_PATH, 'tools', 'requirements', 'requirements.core.txt'))
REQ_GDBGUI = '- {}'.format(os.path.join(IDF_PATH, 'tools', 'requirements', 'requirements.gdbgui.txt'))
CONSTR = 'Constraint file: {}'.format(os.path.join(TOOLS_DIR, 'espidf.constraints'))

# Set default global paths for idf_tools. If some test needs to
# use functions from idf_tools with custom paths, it should
# set it in setUp() and change them back to defaults in tearDown().
idf_tools.g.idf_path = IDF_PATH
idf_tools.g.idf_tools_path = TOOLS_DIR


def setUpModule():  # type: () -> None
    shutil.rmtree(PYTHON_DIR_BACKUP)
    shutil.move(PYTHON_DIR, PYTHON_DIR_BACKUP)


def tearDownModule():  # type: () -> None
    if os.path.isdir(PYTHON_DIR):
        shutil.rmtree(PYTHON_DIR)
    shutil.move(PYTHON_DIR_BACKUP, PYTHON_DIR)


class BasePythonInstall(unittest.TestCase):
    def run_tool(self, cmd):  # type: (List[str]) -> str
        ret = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=600)
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

    def dump_package(self, whl, name):  # type: (bytes, str) -> str
        tmpdir = tempfile.mkdtemp()
        foopackage_fn = os.path.join(tmpdir, name)
        with open(foopackage_fn, 'wb') as fd:
            fd.write(whl)

        self.addCleanup(shutil.rmtree, tmpdir)
        return foopackage_fn

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

        return self.dump_package(whl, 'foopackage-0.99-py3-none-any.whl')

    def dump_foopackage_dev(self):  # type: () -> str
        # similar to dump_foopackage, but using dev release version

        whl = (b'PK\x03\x04\x14\x00\x00\x00\x08\x00\nl\x03W !Z\xfc%\x00\x00\x00%\x00\x00\x00\x16\x00\x00\x00'
               b'foopackage/__init__.py\xcbLS\x88\x8f\xcfK\xccM\x8d\x8fW\xb0\xb5UP\x8f\x8f\xcfM\xcc\xcc\x8b\x8fW\xb7'
               b'\xe2R\x00\x82\xa2\xd4\x92\xd2\xa2<\x00PK\x03\x04\x14\x00\x00\x00\x08\x00Jl\x03W\xb4wO\x876\x00\x00'
               b'\x00;\x00\x00\x00\'\x00\x00\x00foopackage-0.99.dev0.dist-info/METADATA\xf3M-ILI,I\xd4\rK-*\xce\xcc'
               b'\xcf\xb3R0\xd23\xe4\xf2K\xccM\xb5RH\xcb\xcf/HL\xceNLO\xe5\x82\xcb\x1a\xe8YZ\xea\xa5\xa4\x96\x19pq'
               b'\x01\x00PK\x03\x04\x14\x00\x00\x00\x08\x00Jl\x03W\xda9\xe8\xb4[\x00\x00\x00\\\x00\x00\x00$\x00\x00'
               b'\x00foopackage-0.99.dev0.dist-info/WHEEL\x0b\xcfHM\xcd\xd1\rK-*\xce\xcc\xcf\xb3R0\xd43\xe0rO\xcdK-J,'
               b'\xc9/\xb2RHJ\xc9,.\x89/\x07\xa9Q\xd00\xd03\x01Jkr\x05\xe5\xe7\x97\xe8z\x16\xeb\x06\x94\x16\xa5\xe6'
               b'd&Y)\x94\x14\x95\xa6r\x85$\xa6[)\x14T\x1a\xeb\xe6\xe5\xe7\xa5\xea&\xe6Urq\x01\x00PK\x03\x04\x14\x00'
               b'\x00\x00\x08\x00Jl\x03WI*\x9e\xa7\r\x00\x00\x00\x0b\x00\x00\x00,\x00\x00\x00foopackage-0.99.dev0'
               b'.dist-info/top_level.txtK\xcb\xcf/HL\xceNLO\xe5\x02\x00PK\x03\x04\x14\x00\x00\x00\x08\x00Jl\x03W'
               b'\x1e\xbaW\xb5\x00\x01\x00\x00\x91\x01\x00\x00%\x00\x00\x00foopackage-0.99.dev0.dist-info/RECORD\x85'
               b'\xcd\xbbv\x820\x00\x00\xd0\xddo\t\x18\xe4\x08d\xe8\x80\x88"\xf2\xb0T\xe4\xb1\xe4\x08\x06B\xa1\x064F'
               b'\xe8\xd7w\xb2\xab?po\xc5X\x7f.\xdbsM\xe6\x187\xd7\x86c,\xf7\x13\xb8\xd3\xf3b\xa9}d\x98\x90\xc1\n\xbc'
               b'[m\xea\x0fI\x848\xda\xb1\x80)\xf5-D\xc7&\xcc\x9d\xe8\xa1\x1f\nj\x97\xbdZ\x02U\x9fU\xff\x98\x04e\x84'
               b'\xe4\x0b\x11P\xbe4w.5\xd7\x8a\xcd}\xfbh\xae\xcd\xa3\xf9\xd2]\xb1jQ4$^?\xe6\xd9\xe4C\xb6\xdfdE3\x89'
               b'\xb1m\x8dt0\xb2.6s[B\xbb_-\x03K\xf4NO\x1c\xdb\xf6^\xb4\xc9W[\xed+\xf5\xd4\xfd\x06\x0b\x18\x8c^\x05'
               b'\t\x9dN!\x85%\xeb.\x92[\xb8Y\x1al\xd9\xcd\xd2>\x01Z\xbc\xa39\xebqG\x04\xe9d>\xf2W\x11\xd7\x10\xeb'
               b'\xca\x83\xbb\t\xf3\xa9\xf33\t5\x7f\xfa\x90\xd2\xe2\x04}\x9eW\xb5\xee\xe2\xefx\x07\x0f\xced\x00EyWD'
               b'\xb6\x15Fk\x00f\x7fPK\x01\x02\x14\x03\x14\x00\x00\x00\x08\x00\nl\x03W !Z\xfc%\x00\x00\x00%\x00\x00'
               b'\x00\x16\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xa4\x81\x00\x00\x00\x00foopackage/__init__.py'
               b'PK\x01\x02\x14\x03\x14\x00\x00\x00\x08\x00Jl\x03W\xb4wO\x876\x00\x00\x00;\x00\x00\x00\'\x00\x00\x00'
               b'\x00\x00\x00\x00\x00\x00\x00\x00\xa4\x81Y\x00\x00\x00foopackage-0.99.dev0.dist-info/METADATAPK\x01'
               b'\x02\x14\x03\x14\x00\x00\x00\x08\x00Jl\x03W\xda9\xe8\xb4[\x00\x00\x00\\\x00\x00\x00$\x00\x00\x00\x00'
               b'\x00\x00\x00\x00\x00\x00\x00\xa4\x81\xd4\x00\x00\x00foopackage-0.99.dev0.dist-info/WHEELPK\x01\x02'
               b'\x14\x03\x14\x00\x00\x00\x08\x00Jl\x03WI*\x9e\xa7\r\x00\x00\x00\x0b\x00\x00\x00,\x00\x00\x00\x00'
               b'\x00\x00\x00\x00\x00\x00\x00\xa4\x81q\x01\x00\x00foopackage-0.99.dev0.dist-info/top_level.txtPK\x01'
               b'\x02\x14\x03\x14\x00\x00\x00\x08\x00Jl\x03W\x1e\xbaW\xb5\x00\x01\x00\x00\x91\x01\x00\x00%\x00\x00'
               b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\xb4\x81\xc8\x01\x00\x00foopackage-0.99.dev0.dist-info/RECORDPK'
               b'\x05\x06\x00\x00\x00\x00\x05\x00\x05\x00\x98\x01\x00\x00\x0b\x03\x00\x00\x00\x00')

        return self.dump_package(whl, 'foopackage-0.99.dev0-py3-none-any.whl')


class TestPythonInstall(BasePythonInstall):

    def setUp(self):  # type: () -> None
        if os.path.isdir(PYTHON_DIR):
            shutil.rmtree(PYTHON_DIR)
        if os.path.isfile(os.path.join(TOOLS_DIR, 'idf-env.json')):
            os.remove(os.path.join(TOOLS_DIR, 'idf-env.json'))

    def test_default_arguments(self):  # type: () -> None
        output = self.run_idf_tools(['check-python-dependencies'])
        self.assertNotIn(REQ_SATISFIED, output)
        self.assertIn(f'{PYTHON_BINARY} doesn\'t exist', output)

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
        self.assertIn(f"{os.path.join(self.CUSTOM_PYTHON_DIR, PYTHON_BINARY)} doesn't exist", output)
        self.assertNotIn(PYTHON_DIR, output)

        output = self.run_idf_tools(['install-python-env'])
        self.assertIn(self.CUSTOM_PYTHON_DIR, output)
        self.assertNotIn(PYTHON_DIR, output)

        output = self.run_idf_tools(['check-python-dependencies'])
        self.assertIn(self.CUSTOM_PYTHON_DIR, output)


class TestCheckPythonDependencies(BasePythonInstall):

    """
    The constraint file name is available as the constraint_file attribute. The content of the file is changed by these
    tests. The backup_constraint_file is a temporary file with the content of the original constraint file. This is
    kept in order to restore the original content of the constraint file. Keeping the original constraint file is
    important for consequent tests which should not download a new one especially when the test was run with a custom
    constraint file different from the one on dl.espressif.com.
    """
    constraint_file: str
    backup_constraint_file: str

    # similar to constraint files (see above) - creating a backup and restoring it as part of test teardown
    requirement_core_file: str
    backup_requirement_core_file: str

    @classmethod
    def setUpClass(cls):  # type: () -> None
        cls.constraint_file = idf_tools.get_constraints(idf_tools.get_idf_version(), online=False)
        cls.requirement_core_file = os.path.join(IDF_PATH, 'tools', 'requirements', 'requirements.core.txt')
        for file_path_var in ['constraint_file', 'requirement_core_file']:
            with tempfile.NamedTemporaryFile() as f:
                setattr(cls, f'backup_{file_path_var}', f.name)
            shutil.copyfile(getattr(cls, file_path_var), getattr(cls, f'backup_{file_path_var}'))

    @classmethod
    def tearDownClass(cls):  # type: () -> None
        try:
            os.remove(cls.backup_constraint_file)
            os.remove(cls.backup_requirement_core_file)
        except OSError:
            pass

    def setUp(self):  # type: () -> None
        if os.path.isdir(PYTHON_DIR):
            shutil.rmtree(PYTHON_DIR)

    def tearDown(self):  # type: () -> None
        shutil.copyfile(self.backup_constraint_file, self.constraint_file)
        shutil.copyfile(self.backup_requirement_core_file, self.requirement_core_file)

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

        # Write the created constraints list into existing constraints file.
        # It will not be overwritten by subsequent idf_tools.py run, because
        # there is timestamp check.
        with open(self.constraint_file, 'w') as fd:
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

        # append foopackage constraint to the existing constraints file
        with open(self.constraint_file, 'a') as fd:
            fd.write('foopackage>0.99')

        # check-python-dependencies should not complain about dummy_package
        output = self.run_idf_tools(['check-python-dependencies'])
        self.assertIn(REQ_SATISFIED, output)

    def test_missing_requirement(self):  # type: () -> None
        # Install python env and then append foopackage to the requirements
        # Make sure that dependency check has failed and complained about missing foopackage
        self.run_idf_tools(['install-python-env'])

        # append foopackage requirement to the existing requirements file
        with open(self.requirement_core_file, 'a') as fd:
            fd.write('foopackage')

        # append foopackage constraint to the existing constraints file
        with open(self.constraint_file, 'a') as fd:
            fd.write('foopackage>0.99')

        # check-python-dependencies should fail as the package was not installed yet
        output = self.run_idf_tools(['check-python-dependencies'])
        self.assertRegex(output, REQ_MISSING.format('foopackage'))
        self.assertNotIn(REQ_SATISFIED, output)

    def test_dev_version(self):  # type: () -> None
        # Install python env with core requirements, plus foopackage in dev version.
        # Add foopackage to constraints file meeting requirement
        # Dependency check should pass as the requirement was met
        # Change dependency to require dev version
        # Dependency check should pass again
        self.run_idf_tools(['install-python-env'])
        foo_pkg = self.dump_foopackage_dev()
        self.run_in_venv(['-m', 'pip', 'install', foo_pkg])

        # append foopackage requirement to the existing requirements file
        with open(self.requirement_core_file, 'a') as fd:
            fd.write('foopackage')

        # append foopackage constraint to the existing constraints file
        with open(self.constraint_file, 'r+') as fd:
            con_lines = fd.readlines()
            fd.write('foopackage~=0.98')

        output = self.run_idf_tools(['check-python-dependencies'])
        self.assertIn(REQ_SATISFIED, output)

        # append foopackage dev version constraint to the existing constraints file
        with open(self.constraint_file, 'r+') as fd:
            fd.writelines(con_lines + ['foopackage==0.99.dev0'])

        output = self.run_idf_tools(['check-python-dependencies'])
        self.assertIn(REQ_SATISFIED, output)


if __name__ == '__main__':
    unittest.main()
