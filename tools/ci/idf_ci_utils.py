# internal use only for CI
# some CI related util functions
#
# SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import io
import logging
import os
import subprocess
import sys
from contextlib import redirect_stdout
from typing import TYPE_CHECKING, List

if TYPE_CHECKING:
    from _pytest.nodes import Function

IDF_PATH = os.path.abspath(
    os.getenv('IDF_PATH', os.path.join(os.path.dirname(__file__), '..', '..'))
)


def get_submodule_dirs(full_path: bool = False) -> List:
    """
    To avoid issue could be introduced by multi-os or additional dependency,
    we use python and git to get this output
    :return: List of submodule dirs
    """
    dirs = []
    try:
        lines = (
            subprocess.check_output(
                [
                    'git',
                    'config',
                    '--file',
                    os.path.realpath(os.path.join(IDF_PATH, '.gitmodules')),
                    '--get-regexp',
                    'path',
                ]
            )
            .decode('utf8')
            .strip()
            .split('\n')
        )
        for line in lines:
            _, path = line.split(' ')
            if full_path:
                dirs.append(os.path.join(IDF_PATH, path))
            else:
                dirs.append(path)
    except Exception as e:  # pylint: disable=W0703
        logging.warning(str(e))

    return dirs


def _check_git_filemode(full_path):  # type: (str) -> bool
    try:
        stdout = (
            subprocess.check_output(['git', 'ls-files', '--stage', full_path])
            .strip()
            .decode('utf-8')
        )
    except subprocess.CalledProcessError:
        return True

    mode = stdout.split(' ', 1)[0]  # e.g. 100644 for a rw-r--r--
    if any([int(i, 8) & 1 for i in mode[-3:]]):
        return True
    return False


def is_executable(full_path: str) -> bool:
    """
    os.X_OK will always return true on windows. Use git to check file mode.
    :param full_path: file full path
    :return: True is it's an executable file
    """
    if sys.platform == 'win32':
        return _check_git_filemode(full_path)
    return os.access(full_path, os.X_OK)


def get_git_files(path: str = IDF_PATH, full_path: bool = False) -> List[str]:
    """
    Get the result of git ls-files
    :param path: path to run git ls-files
    :param full_path: return full path if set to True
    :return: list of file paths
    """
    try:
        # this is a workaround when using under worktree
        # if you're using worktree, when running git commit a new environment variable GIT_DIR would be declared,
        # the value should be <origin_repo_path>/.git/worktrees/<worktree name>
        # This would effect the return value of `git ls-files`, unset this would use the `cwd`value or its parent
        # folder if no `.git` folder found in `cwd`.
        workaround_env = os.environ.copy()
        workaround_env.pop('GIT_DIR', None)
        files = (
            subprocess.check_output(['git', 'ls-files'], cwd=path, env=workaround_env)
            .decode('utf8')
            .strip()
            .split('\n')
        )
    except Exception as e:  # pylint: disable=W0703
        logging.warning(str(e))
        files = []
    return [os.path.join(path, f) for f in files] if full_path else files


def is_in_directory(file_path: str, folder: str) -> bool:
    return os.path.realpath(file_path).startswith(os.path.realpath(folder) + os.sep)


class PytestCase:
    def __init__(self, test_path: str, target: str, config: str, case: str):
        self.app_path = os.path.dirname(test_path)
        self.test_path = test_path
        self.target = target
        self.config = config
        self.case = case

    def __repr__(self) -> str:
        return f'{self.test_path}: {self.target}.{self.config}.{self.case}'


class PytestCollectPlugin:
    def __init__(self, target: str) -> None:
        self.target = target
        self.nodes: List[PytestCase] = []

    def pytest_collection_modifyitems(self, items: List['Function']) -> None:
        for item in items:
            try:
                file_path = str(item.path)
            except AttributeError:
                # pytest 6.x
                file_path = item.fspath

            target = self.target
            if hasattr(item, 'callspec'):
                config = item.callspec.params.get('config', 'default')
            else:
                config = 'default'
            case_name = item.originalname

            self.nodes.append(PytestCase(file_path, target, config, case_name))


def get_pytest_cases(folder: str, target: str) -> List[PytestCase]:
    import pytest
    from _pytest.config import ExitCode

    collector = PytestCollectPlugin(target)

    with io.StringIO() as buf:
        with redirect_stdout(buf):
            res = pytest.main(['--collect-only', folder, '-q', '--target', target], plugins=[collector])
        if res.value != ExitCode.OK:
            if res.value == ExitCode.NO_TESTS_COLLECTED:
                print(f'WARNING: no pytest app found for target {target} under folder {folder}')
            else:
                print(buf.getvalue())
                raise RuntimeError('pytest collection failed')

    return collector.nodes


def get_pytest_app_paths(folder: str, target: str) -> List[str]:
    nodes = get_pytest_cases(folder, target)

    return list({node.app_path for node in nodes})
