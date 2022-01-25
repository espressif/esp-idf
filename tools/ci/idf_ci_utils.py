# internal use only for CI
# some CI related util functions
#
# SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import logging
import os
import subprocess
import sys
from typing import List

IDF_PATH = os.path.abspath(os.getenv('IDF_PATH', os.path.join(os.path.dirname(__file__), '..', '..')))


def get_submodule_dirs(full_path: bool = False) -> List:
    """
    To avoid issue could be introduced by multi-os or additional dependency,
    we use python and git to get this output
    :return: List of submodule dirs
    """
    dirs = []
    try:
        lines = subprocess.check_output(
            ['git', 'config', '--file', os.path.realpath(os.path.join(IDF_PATH, '.gitmodules')),
             '--get-regexp', 'path']).decode('utf8').strip().split('\n')
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
        stdout = subprocess.check_output(['git', 'ls-files', '--stage', full_path]).strip().decode('utf-8')
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
        files = subprocess.check_output(['git', 'ls-files'], cwd=path, env=workaround_env) \
            .decode('utf8').strip().split('\n')
    except Exception as e:  # pylint: disable=W0703
        logging.warning(str(e))
        files = []
    return [os.path.join(path, f) for f in files] if full_path else files


def is_in_directory(file_path: str, folder: str) -> bool:
    return os.path.realpath(file_path).startswith(os.path.realpath(folder) + os.sep)


def get_pytest_dirs(folder: str) -> List[str]:
    from io import StringIO

    import pytest
    from _pytest.nodes import Item

    class CollectPlugin:
        def __init__(self) -> None:
            self.nodes: List[Item] = []

        def pytest_collection_modifyitems(self, items: List[Item]) -> None:
            for item in items:
                self.nodes.append(item)

    collector = CollectPlugin()

    sys_stdout = sys.stdout
    sys.stdout = StringIO()  # swallow the output
    pytest.main(['--collect-only', folder], plugins=[collector])
    sys.stdout = sys_stdout  # restore sys.stdout

    test_file_paths = set(node.fspath for node in collector.nodes)

    return [os.path.dirname(file) for file in test_file_paths]
