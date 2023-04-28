# internal use only for CI
# some CI related util functions
#
# SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import io
import logging
import os
import subprocess
import sys
from contextlib import redirect_stdout
from dataclasses import dataclass
from typing import TYPE_CHECKING, Any, List, Optional, Set

if TYPE_CHECKING:
    from _pytest.python import Function

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


def to_list(s: Any) -> List[Any]:
    if isinstance(s, (set, tuple)):
        return list(s)

    if isinstance(s, list):
        return s

    return [s]


@dataclass
class PytestApp:
    path: str
    target: str
    config: str

    def __hash__(self) -> int:
        return hash((self.path, self.target, self.config))


@dataclass
class PytestCase:
    path: str
    name: str
    apps: Set[PytestApp]

    def __hash__(self) -> int:
        return hash((self.path, self.name, self.apps))


class PytestCollectPlugin:
    def __init__(self, target: str) -> None:
        self.target = target
        self.cases: List[PytestCase] = []

    @staticmethod
    def get_param(item: 'Function', key: str, default: Any = None) -> Any:
        if not hasattr(item, 'callspec'):
            raise ValueError(f'Function {item} does not have params')

        return item.callspec.params.get(key, default) or default

    def pytest_report_collectionfinish(self, items: List['Function']) -> None:
        from pytest_embedded.plugin import parse_multi_dut_args

        for item in items:
            count = 1
            case_path = str(item.path)
            case_name = item.originalname
            target = self.target
            # funcargs is not calculated while collection
            if hasattr(item, 'callspec'):
                count = item.callspec.params.get('count', 1)
                app_paths = to_list(
                    parse_multi_dut_args(
                        count,
                        self.get_param(item, 'app_path', os.path.dirname(case_path)),
                    )
                )
                configs = to_list(
                    parse_multi_dut_args(
                        count, self.get_param(item, 'config', 'default')
                    )
                )
                targets = to_list(
                    parse_multi_dut_args(count, self.get_param(item, 'target', target))
                )
            else:
                app_paths = [os.path.dirname(case_path)]
                configs = ['default']
                targets = [target]

            case_apps = set()
            for i in range(count):
                case_apps.add(PytestApp(app_paths[i], targets[i], configs[i]))

            self.cases.append(PytestCase(case_path, case_name, case_apps))


def get_pytest_cases(
    folder: str, target: str, marker_expr: Optional[str] = None
) -> List[PytestCase]:
    import pytest
    from _pytest.config import ExitCode

    collector = PytestCollectPlugin(target)
    if marker_expr:
        marker_expr = f'{target} and ({marker_expr})'
    else:
        marker_expr = target  # target is also a marker

    with io.StringIO() as buf:
        with redirect_stdout(buf):
            res = pytest.main(
                ['--collect-only', folder, '-q', '-m', marker_expr], plugins=[collector]
            )
        if res.value != ExitCode.OK:
            if res.value == ExitCode.NO_TESTS_COLLECTED:
                print(
                    f'WARNING: no pytest app found for target {target} under folder {folder}'
                )
            else:
                print(buf.getvalue())
                raise RuntimeError('pytest collection failed')

    return collector.cases


def get_pytest_app_paths(
    folder: str, target: str, marker_expr: Optional[str] = None
) -> Set[str]:
    cases = get_pytest_cases(folder, target, marker_expr)

    return set({app.path for case in cases for app in case.apps})
