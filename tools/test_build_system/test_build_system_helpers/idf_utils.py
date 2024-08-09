# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import shutil
import subprocess
import sys
import typing
from pathlib import Path
from typing import Union

try:
    EXT_IDF_PATH = os.environ['IDF_PATH']  # type: str
except KeyError:
    print('IDF_PATH must be set before running this test', file=sys.stderr)
    exit(1)


EnvDict = typing.Dict[str, str]
IdfPyFunc = typing.Callable[..., subprocess.CompletedProcess]


def find_python(path_var: str) -> str:
    """
    Find python interpreter in the paths specified in the given PATH variable.
    Returns the full path to the interpreter.
    """
    res = shutil.which('python', path=path_var)
    if res is None:
        raise ValueError('python not found')
    return res


def get_idf_build_env(idf_path: str) -> EnvDict:
    """
    Get environment variables (as set by export.sh) for the specific IDF copy
    :param idf_path: path of the IDF copy to use
    :return: dictionary of environment variables and their values
    """
    cmd = [
        sys.executable,
        os.path.join(idf_path, 'tools', 'idf_tools.py'),
        'export',
        '--format=key-value'
    ]
    keys_values = subprocess.check_output(cmd, stderr=subprocess.PIPE).decode()
    idf_tool_py_env = {key: os.path.expandvars(value) for key, value in
                       [line.split('=') for line in keys_values.splitlines()]}
    env_vars = {}  # type: EnvDict
    env_vars.update(os.environ)
    env_vars.update(idf_tool_py_env)
    # not set by idf_tools.py, normally set by export.sh
    env_vars['IDF_PATH'] = idf_path

    return env_vars


def run_idf_py(*args: str,
               env: typing.Optional[EnvDict] = None,
               idf_path: typing.Optional[typing.Union[str,Path]] = None,
               workdir: typing.Optional[str] = None,
               check: bool = True,
               python: typing.Optional[str] = None,
               input_str: typing.Optional[str] = None) -> subprocess.CompletedProcess:
    """
    Run idf.py command with given arguments, raise an exception on failure
    :param args: arguments to pass to idf.py
    :param env: environment variables to run the build with; if not set, the default environment is used
    :param idf_path: path to the IDF copy to use; if not set, IDF_PATH from the 'env' argument is used
    :param workdir: directory where to run the build; if not set, the current directory is used
    :param check: check process exits with a zero exit code, if false all retvals are accepted without failing the test
    :param python: absolute path to python interpreter
    :param input_str: input to idf.py
    """
    if not env:
        env = dict(**os.environ)
    if not workdir:
        workdir = os.getcwd()
    # order: function argument -> value in env dictionary -> system environment
    if idf_path is None:
        idf_path = env.get('IDF_PATH')
        if not idf_path:
            raise ValueError('IDF_PATH must be set in the env array if idf_path argument is not set')
    if python is None:
        python = find_python(env['PATH'])

    cmd = [
        python,
        os.path.join(idf_path, 'tools', 'idf.py')
    ]
    cmd += args  # type: ignore
    logging.debug('running {} in {}'.format(' '.join(cmd), workdir))
    return subprocess.run(
        cmd, env=env, cwd=workdir,
        check=check, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        text=True, encoding='utf-8', errors='backslashreplace', input=input_str)


def run_cmake(*cmake_args: str,
              env: typing.Optional[EnvDict] = None,
              check: bool = True,
              workdir: typing.Optional[Union[Path,str]] = None) -> subprocess.CompletedProcess:
    """
    Run cmake command with given arguments, raise an exception on failure
    :param cmake_args: arguments to pass cmake
    :param env: environment variables to run the cmake with; if not set, the default environment is used
    :param check: check process exits with a zero exit code, if false all retvals are accepted without failing the test
    :param workdir: directory where to run cmake; if not set, the current directory is used
    """
    if not env:
        env = dict(**os.environ)

    if workdir:
        build_dir = Path(workdir, 'build')
    else:
        build_dir = (Path(os.getcwd()) / 'build')

    build_dir.mkdir(parents=True, exist_ok=True)

    cmd = ['cmake'] + list(cmake_args)

    logging.debug('running {} in {}'.format(' '.join(cmd), build_dir))
    return subprocess.run(
        cmd, env=env, cwd=build_dir,
        check=check, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        text=True, encoding='utf-8', errors='backslashreplace')


def run_cmake_and_build(*cmake_args: str, env: typing.Optional[EnvDict] = None) -> None:
    """
    Run cmake command with given arguments and build afterwards, raise an exception on failure
    :param cmake_args: arguments to pass cmake
    :param env: environment variables to run the cmake with; if not set, the default environment is used
    """
    run_cmake(*cmake_args, env=env)
    run_cmake('--build', '.')
