# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import shutil
import subprocess
import sys
import typing
from pathlib import Path

try:
    EXT_IDF_PATH = os.environ['IDF_PATH']  # type: str
except KeyError:
    print('IDF_PATH must be set before running this test', file=sys.stderr)
    exit(1)


EnvDict = dict[str, str]
IdfPyFunc = typing.Callable[..., subprocess.CompletedProcess]


def normalize_output(text: str) -> str:
    """Collapse all whitespace runs to a single space.

    Use for content assertions on messages that include file paths: long paths can
    push lines past COLUMNS=200 and cause Rich to insert a mid-message line break.
    """
    return ' '.join(text.split())


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
    cmd = [sys.executable, os.path.join(idf_path, 'tools', 'idf_tools.py'), 'export', '--format=key-value']
    keys_values = subprocess.check_output(cmd, stderr=subprocess.PIPE).decode()
    idf_tool_py_env = {
        key: os.path.expandvars(value) for key, value in [line.split('=') for line in keys_values.splitlines()]
    }
    env_vars = {}  # type: EnvDict
    env_vars.update(os.environ)
    env_vars.update(idf_tool_py_env)
    # not set by idf_tools.py, normally set by export.sh
    env_vars['IDF_PATH'] = idf_path

    return env_vars


def run_idf_py(
    *args: str,
    env: EnvDict | None = None,
    idf_path: str | Path | None = None,
    workdir: str | None = None,
    check: bool = True,
    python: str | None = None,
    input_str: str | None = None,
) -> subprocess.CompletedProcess:
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

    cmd = [python, os.path.join(idf_path, 'tools', 'idf.py')]
    cmd += args  # type: ignore
    logging.debug('running {} in {}'.format(' '.join(cmd), workdir))
    try:
        return subprocess.run(
            cmd,
            env=env,
            cwd=workdir,
            check=check,
            capture_output=True,
            text=True,
            encoding='utf-8',
            errors='backslashreplace',
            input=input_str,
        )
    except subprocess.CalledProcessError as e:
        logging.error('The following idf.py command has failed: {}'.format(' '.join(cmd)))
        logging.error(f'Working directory: {workdir}')
        logging.error(f'Stdout: {e.stdout}')
        logging.error(f'Stderr: {e.stderr}')
        raise


def run_cmake(
    *cmake_args: str,
    env: EnvDict | None = None,
    check: bool = True,
    workdir: Path | str | None = None,
) -> subprocess.CompletedProcess:
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
        build_dir = Path(os.getcwd()) / 'build'

    build_dir.mkdir(parents=True, exist_ok=True)

    cmd = ['cmake'] + list(cmake_args)

    logging.debug('running {} in {}'.format(' '.join(cmd), build_dir))
    try:
        return subprocess.run(
            cmd,
            env=env,
            cwd=build_dir,
            check=check,
            capture_output=True,
            text=True,
            encoding='utf-8',
            errors='backslashreplace',
        )
    except subprocess.CalledProcessError as e:
        logging.error('The following cmake command has failed: {}'.format(' '.join(cmd)))
        logging.error(f'Working directory: {workdir}')
        logging.error(f'Stdout: {e.stdout}')
        logging.error(f'Stderr: {e.stderr}')
        raise


def run_cmake_and_build(*cmake_args: str, env: EnvDict | None = None) -> None:
    """
    Run cmake command with given arguments and build afterwards, raise an exception on failure
    :param cmake_args: arguments to pass cmake
    :param env: environment variables to run the cmake with; if not set, the default environment is used
    """
    run_cmake(*cmake_args, env=env)
    run_cmake('--build', '.')
