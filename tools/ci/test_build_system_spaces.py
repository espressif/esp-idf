#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import shutil
import subprocess
import sys
import typing
import unittest

try:
    IDF_PATH = os.environ['IDF_PATH']  # type: str
except KeyError:
    print('IDF_PATH must be set before running this test', file=sys.stderr)
    exit(1)

# Use the current directory for the builds
TEST_PATH = os.getcwd()


##################
# Helper functions
##################

def find_python(path_var: str) -> str:
    """
    Find python interpreter in the paths specified in the given PATH variable.
    Returns the full path to the interpreter.
    """
    res = shutil.which('python', path=path_var)
    if res is None:
        raise ValueError('python not found')
    return res


def get_idf_build_env(idf_path: typing.Optional[str]) -> typing.Dict[str, str]:
    """
    Get environment variables (as set by export.sh) for the specific IDF copy
    :param idf_path: if set, path of the IDF copy to use; otherwise, IDF_PATH from environment is used
    :return: dictionary of environment variables and their values
    """
    if idf_path is None:
        idf_path = IDF_PATH
    cmd = [
        sys.executable,
        os.path.join(idf_path, 'tools', 'idf_tools.py'),
        'export',
        '--format=key-value'
    ]
    keys_values = subprocess.check_output(cmd).decode()
    env_vars = {key: os.path.expandvars(value) for key, value in
                [line.split('=') for line in keys_values.splitlines()]}
    # not set by idf_tools.py, normally set by export.sh
    env_vars['IDF_PATH'] = idf_path

    return env_vars


def idf_py(*args: str, env_vars: typing.Optional[typing.Dict[str, str]] = None,
           idf_path: typing.Optional[str] = None,
           workdir: typing.Optional[str] = None) -> None:
    """
    Run idf.py command with given arguments, raise an exception on failure
    :param args: arguments to pass to idf.py
    :param env_vars: environment variables to run the build with; if not set, the default environment is used
    :param idf_path: path to the IDF copy to use; if not set, IDF_PATH from the environment is used
    :param workdir: directory where to run the build; if not set, the current directory is used
    """
    env = dict(**os.environ)
    if env_vars is not None:
        env.update(env_vars)
    if not workdir:
        workdir = os.getcwd()
    # order: function argument -> value in env dictionary -> system environment
    if idf_path is not None:
        inferred_idf_path = idf_path
    else:
        inferred_idf_path = str(env.get('IDF_PATH', IDF_PATH))

    python = find_python(env['PATH'])

    cmd = [
        python,
        os.path.join(inferred_idf_path, 'tools', 'idf.py')
    ]
    cmd += args  # type: ignore
    subprocess.check_call(cmd, env=env, cwd=workdir)


############
# Test cases
############

class PathsWithSpaces(unittest.TestCase):
    IDF_PATH_WITH_SPACES = ''
    DO_CLEANUP = True
    ENV: typing.Dict[str, str] = dict()

    @classmethod
    def setUpClass(cls) -> None:
        if ' ' in IDF_PATH:
            print('IDF_PATH already contains spaces, not making a copy')
            cls.IDF_PATH_WITH_SPACES = IDF_PATH
            cls.DO_CLEANUP = False
        else:
            # Make a copy of ESP-IDF directory, with a space in its name
            cls.IDF_PATH_WITH_SPACES = os.path.join(TEST_PATH, 'esp idf')
            dest = cls.IDF_PATH_WITH_SPACES
            print('Copying esp-idf from {} to {}'.format(IDF_PATH, dest))
            shutil.copytree(IDF_PATH, dest,
                            # if the CWD is inside the original esp-idf directory, make sure not to go into recursion.
                            ignore=shutil.ignore_patterns(os.path.basename(dest)))

        cls.ENV = get_idf_build_env(cls.IDF_PATH_WITH_SPACES)

    @classmethod
    def tearDownClass(cls) -> None:
        if cls.DO_CLEANUP and os.path.exists(cls.IDF_PATH_WITH_SPACES):
            shutil.rmtree(cls.IDF_PATH_WITH_SPACES, ignore_errors=True)

    def test_install_export(self) -> None:
        env = dict(**os.environ)
        del env['IDF_PATH']
        if os.name == 'nt':
            install_cmd = 'install.bat esp32'
            export_cmd = 'export.bat'
        else:
            install_cmd = './install.sh esp32'
            export_cmd = '. ./export.sh'

        subprocess.check_call(install_cmd, env=env, shell=True, cwd=self.IDF_PATH_WITH_SPACES)

        if os.name == 'nt':
            subprocess.check_call(export_cmd, env=env, shell=True, cwd=self.IDF_PATH_WITH_SPACES)
        else:
            # The default shell used by subprocess.Popen on POSIX platforms is '/bin/sh',
            # which in esp-env Docker image is 'dash'. The export script doesn't support
            # IDF_PATH detection when used in dash, so we have to override the shell here.
            subprocess.check_call(export_cmd, env=env, shell=True, cwd=self.IDF_PATH_WITH_SPACES, executable='/bin/bash')

    def _copy_app_to(self, app_path: str, dest_dir_name: str) -> str:
        """
        Copy given app to a different directory, setting up cleanup hook.
        Destination directory is first deleted if it already exists.
        Returns the full path of the destination directory.
        app_path: path relative to esp-idf directory
        dest_dir_name: name of the destination directory, relative to the directory where the test is running
        """
        dest_dir = os.path.join(TEST_PATH, dest_dir_name)
        if os.path.exists(dest_dir) and os.path.isdir(dest_dir):
            shutil.rmtree(dest_dir, ignore_errors=True)
        src_dir = os.path.join(self.IDF_PATH_WITH_SPACES, app_path)
        shutil.copytree(os.path.join(src_dir), dest_dir)
        build_dir = os.path.join(dest_dir, 'build')
        if os.path.exists(build_dir):
            shutil.rmtree(build_dir, ignore_errors=True)
        self.addCleanup(shutil.rmtree, dest_dir, ignore_errors=True)
        return dest_dir

    # The tests below build different ESP-IDF apps (examples or test apps) to cover different parts
    # of the build system and related scripts.
    # In each test, IDF_PATH and app path both contain spaces.

    def test_build(self) -> None:
        build_path = self._copy_app_to(os.path.join('examples', 'get-started', 'hello_world'), 'test app')
        idf_py('build', env_vars=self.ENV, idf_path=self.IDF_PATH_WITH_SPACES, workdir=build_path)

    def test_build_ulp_fsm(self) -> None:
        build_path = self._copy_app_to(os.path.join('examples', 'system', 'ulp_fsm', 'ulp'), 'test app')
        idf_py('build', env_vars=self.ENV, idf_path=self.IDF_PATH_WITH_SPACES, workdir=build_path)

    def test_build_ulp_riscv(self) -> None:
        build_path = self._copy_app_to(os.path.join('examples', 'system', 'ulp_riscv', 'gpio'), 'test app')
        idf_py('-DIDF_TARGET=esp32s2', 'build', env_vars=self.ENV, idf_path=self.IDF_PATH_WITH_SPACES,
               workdir=build_path)

    def test_spiffsgen(self) -> None:
        build_path = self._copy_app_to(os.path.join('examples', 'storage', 'spiffsgen'), 'test app')
        idf_py('build', env_vars=self.ENV, idf_path=self.IDF_PATH_WITH_SPACES, workdir=build_path)

    def test_flash_encryption(self) -> None:
        build_path = self._copy_app_to(os.path.join('examples', 'security', 'flash_encryption'), 'test app')
        idf_py('build', env_vars=self.ENV, idf_path=self.IDF_PATH_WITH_SPACES, workdir=build_path)

    def test_secure_boot_v1(self) -> None:
        build_path = self._copy_app_to(os.path.join('tools', 'test_apps', 'security', 'secure_boot'), 'test app')
        idf_py('-DSDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.ci.01', 'build', env_vars=self.ENV,
               idf_path=self.IDF_PATH_WITH_SPACES, workdir=build_path)

    def test_secure_boot_v2(self) -> None:
        build_path = self._copy_app_to(os.path.join('tools', 'test_apps', 'security', 'secure_boot'), 'test app')
        idf_py('-DSDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.ci.00', 'build', env_vars=self.ENV,
               idf_path=self.IDF_PATH_WITH_SPACES, workdir=build_path)

    def test_app_signing(self) -> None:
        build_path = self._copy_app_to(os.path.join('tools', 'test_apps', 'security', 'secure_boot'), 'test app')
        idf_py('-DSDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.ci.02', 'build', env_vars=self.ENV,
               idf_path=self.IDF_PATH_WITH_SPACES, workdir=build_path)

    def test_secure_boot_release_mode(self) -> None:
        build_path = self._copy_app_to(os.path.join('tools', 'test_apps', 'security', 'secure_boot'), 'test app')
        idf_py('-DSDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.ci.04', '-DIDF_TARGET=esp32s2', 'build',
               env_vars=self.ENV, idf_path=self.IDF_PATH_WITH_SPACES, workdir=build_path)

    def test_x509_cert_bundle(self) -> None:
        build_path = self._copy_app_to(os.path.join('examples', 'protocols', 'https_x509_bundle'), 'test app')
        idf_py('build', env_vars=self.ENV, idf_path=self.IDF_PATH_WITH_SPACES, workdir=build_path)

    def test_dfu(self) -> None:
        build_path = self._copy_app_to(os.path.join('examples', 'get-started', 'hello_world'), 'test app')
        idf_py('-DIDF_TARGET=esp32s2', 'dfu', env_vars=self.ENV, idf_path=self.IDF_PATH_WITH_SPACES,
               workdir=build_path)

    def test_uf2(self) -> None:
        build_path = self._copy_app_to(os.path.join('examples', 'get-started', 'hello_world'), 'test app')
        idf_py('uf2', env_vars=self.ENV, idf_path=self.IDF_PATH_WITH_SPACES, workdir=build_path)


if __name__ == '__main__':
    unittest.main()
