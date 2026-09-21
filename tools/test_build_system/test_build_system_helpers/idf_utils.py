# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import shutil
import subprocess
import sys
import typing
import uuid
from pathlib import Path

try:
    EXT_IDF_PATH = os.environ['IDF_PATH']  # type: str
except KeyError:
    print('IDF_PATH must be set before running this test', file=sys.stderr)
    exit(1)


EnvDict = dict[str, str]
IdfPyFunc = typing.Callable[..., subprocess.CompletedProcess]

# Session fixture in conftest.py sets this to the pytest --work-dir tree so
# failed-command files survive --cleanup-idf-copy of the app directory.
FAILED_COMMAND_LOG_DIR_ENV = 'IDF_TEST_FAILED_COMMAND_LOG_DIR'


_LOG_ERROR_MARKERS = (
    'CMake Error',
    'FAILED:',
    'fatal error',
    'ninja: build stopped',
    'HINT:',
)


def _shorten_log_line(line: str, max_line_len: int = 200) -> str:
    if len(line) <= max_line_len:
        return line
    return line[:max_line_len] + f'... [{len(line) - max_line_len} chars omitted]'


def _failure_lines(text: str | None, max_lines: int = 20) -> list[str]:
    """Marker lines only. Do not send build tails over the CI live log."""
    if not text:
        return []
    lines: list[str] = []
    for line in text.splitlines():
        if any(marker in line for marker in _LOG_ERROR_MARKERS):
            lines.append(_shorten_log_line(line))
            if len(lines) >= max_lines:
                break
    return lines


def _log_process_failure(
    command_name: str,
    cmd: list[str],
    workdir: Path | str,
    error: subprocess.CalledProcessError,
) -> None:
    """Save the untouched output to files, then log paths and failure lines.

    The files keep the whole output. The live log only names those files and
    repeats a few marker lines: a 12 KB record of clipped stdout still hangs
    Windows CI the same way an unclipped one did.
    """
    env_log_dir = os.environ.get(FAILED_COMMAND_LOG_DIR_ENV)
    log_dir = Path(env_log_dir) if env_log_dir else Path(workdir) / 'failed_command_logs'
    saved_paths: dict[str, Path] = {}
    try:
        log_dir.mkdir(parents=True, exist_ok=True)
        prefix = f'{command_name}_{uuid.uuid4().hex}'
        for stream_name, output in (('stdout', error.stdout), ('stderr', error.stderr)):
            output_path = log_dir / f'{prefix}.{stream_name}.txt'
            output_path.write_text(output or '', encoding='utf-8')
            saved_paths[stream_name] = output_path
    except OSError as write_error:
        logging.error('Full output of the failed command could not be saved: %s', write_error)

    message = [
        f'The following {command_name} command has failed: {" ".join(cmd)}',
        f'Working directory: {workdir}',
    ]
    for stream_name, output_path in saved_paths.items():
        message.append(f'Full {stream_name}: {output_path}')
    failure_lines = _failure_lines(error.stdout) + _failure_lines(error.stderr)
    if failure_lines:
        message.append('Failure lines:')
        message.extend(failure_lines)
    logging.error('\n'.join(message))


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
        _log_process_failure('idf.py', cmd, workdir, e)
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
        _log_process_failure('cmake', cmd, build_dir, e)
        raise


def run_cmake_and_build(*cmake_args: str, env: EnvDict | None = None) -> None:
    """
    Run cmake command with given arguments and build afterwards, raise an exception on failure
    :param cmake_args: arguments to pass cmake
    :param env: environment variables to run the cmake with; if not set, the default environment is used
    """
    run_cmake(*cmake_args, env=env)
    run_cmake('--build', '.')
