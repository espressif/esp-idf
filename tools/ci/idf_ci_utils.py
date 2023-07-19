# internal use only for CI
# some CI related util functions
#
# SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import contextlib
import io
import logging
import os
import subprocess
import sys
from contextlib import redirect_stdout
from dataclasses import dataclass
from pathlib import Path
from typing import TYPE_CHECKING, Any, List, Optional, Set, Union

try:
    from idf_py_actions.constants import PREVIEW_TARGETS, SUPPORTED_TARGETS
except ImportError:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

    from idf_py_actions.constants import PREVIEW_TARGETS, SUPPORTED_TARGETS

if TYPE_CHECKING:
    from _pytest.python import Function

IDF_PATH = os.path.abspath(os.getenv('IDF_PATH', os.path.join(os.path.dirname(__file__), '..', '..')))


def get_submodule_dirs(full_path: bool = False) -> List[str]:
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


def _check_git_filemode(full_path: str) -> bool:
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
        # This would affect the return value of `git ls-files`, unset this would use the `cwd`value or its parent
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


####################
# Pytest Utilities #
####################
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

    nightly_run: bool

    def __hash__(self) -> int:
        return hash((self.path, self.name, self.apps, self.nightly_run))


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
                configs = to_list(parse_multi_dut_args(count, self.get_param(item, 'config', 'default')))
                targets = to_list(parse_multi_dut_args(count, self.get_param(item, 'target', target)))
            else:
                app_paths = [os.path.dirname(case_path)]
                configs = ['default']
                targets = [target]

            case_apps = set()
            for i in range(count):
                case_apps.add(PytestApp(app_paths[i], targets[i], configs[i]))

            self.cases.append(
                PytestCase(
                    case_path,
                    case_name,
                    case_apps,
                    'nightly_run' in [marker.name for marker in item.iter_markers()],
                )
            )


def get_pytest_files(paths: List[str]) -> List[str]:
    # this is a workaround to solve pytest collector super slow issue
    # benchmark with
    # - time pytest -m esp32 --collect-only
    #   user=15.57s system=1.35s cpu=95% total=17.741
    # - time { find -name 'pytest_*.py'; } | xargs pytest -m esp32 --collect-only
    #   user=0.11s system=0.63s cpu=36% total=2.044
    #   user=1.76s system=0.22s cpu=43% total=4.539
    # use glob.glob would also save a bunch of time
    pytest_scripts: Set[str] = set()
    for p in paths:
        path = Path(p)
        pytest_scripts.update(str(_p) for _p in path.glob('**/pytest_*.py') if 'managed_components' not in _p.parts)

    return list(pytest_scripts)


def get_pytest_cases(
    paths: Union[str, List[str]],
    target: str = 'all',
    marker_expr: Optional[str] = None,
    filter_expr: Optional[str] = None,
) -> List[PytestCase]:
    import pytest
    from _pytest.config import ExitCode

    if target == 'all':
        targets = SUPPORTED_TARGETS + PREVIEW_TARGETS
    else:
        targets = [target]

    paths = to_list(paths)

    origin_include_nightly_run_env = os.getenv('INCLUDE_NIGHTLY_RUN')
    origin_nightly_run_env = os.getenv('NIGHTLY_RUN')

    # disable the env vars to get all test cases
    if 'INCLUDE_NIGHTLY_RUN' in os.environ:
        os.environ.pop('INCLUDE_NIGHTLY_RUN')

    if 'NIGHTLY_RUN' in os.environ:
        os.environ.pop('NIGHTLY_RUN')

    # collect all cases
    os.environ['INCLUDE_NIGHTLY_RUN'] = '1'

    cases = []  # type: List[PytestCase]
    pytest_scripts = get_pytest_files(paths)
    if not pytest_scripts:
        print(f'WARNING: no pytest scripts found for target {target} under paths {", ".join(paths)}')
        return cases

    for target in targets:
        collector = PytestCollectPlugin(target)

        with io.StringIO() as buf:
            with redirect_stdout(buf):
                cmd = ['--collect-only', *pytest_scripts, '--target', target, '-q']
                if marker_expr:
                    cmd.extend(['-m', marker_expr])
                if filter_expr:
                    cmd.extend(['-k', filter_expr])
                res = pytest.main(cmd, plugins=[collector])
            if res.value != ExitCode.OK:
                if res.value == ExitCode.NO_TESTS_COLLECTED:
                    print(f'WARNING: no pytest app found for target {target} under paths {", ".join(paths)}')
                else:
                    print(buf.getvalue())
                    raise RuntimeError(
                        f'pytest collection failed at {", ".join(paths)} with command \"{" ".join(cmd)}\"'
                    )

        cases.extend(collector.cases)

    # revert back the env vars
    if origin_include_nightly_run_env is not None:
        os.environ['INCLUDE_NIGHTLY_RUN'] = origin_include_nightly_run_env

    if origin_nightly_run_env is not None:
        os.environ['NIGHTLY_RUN'] = origin_nightly_run_env

    return cases


##################
# TTFW Utilities #
##################
def get_ttfw_cases(paths: Union[str, List[str]]) -> List[Any]:
    """
    Get the test cases from ttfw_idf under the given paths

    :param paths: list of paths to search
    """
    try:
        from ttfw_idf.IDFAssignTest import IDFAssignTest
    except ImportError:
        sys.path.append(os.path.join(IDF_PATH, 'tools', 'ci', 'python_packages'))

        from ttfw_idf.IDFAssignTest import IDFAssignTest

    # mock CI_JOB_ID if not exists
    if not os.environ.get('CI_JOB_ID'):
        os.environ['CI_JOB_ID'] = '1'

    cases = []
    for path in to_list(paths):
        assign = IDFAssignTest(path, os.path.join(IDF_PATH, '.gitlab', 'ci', 'target-test.yml'))
        with contextlib.redirect_stdout(None):  # swallow stdout
            try:
                cases += assign.search_cases()
            except ImportError as e:
                logging.error(str(e))

    return cases


def get_ttfw_app_paths(paths: Union[str, List[str]], target: Optional[str] = None) -> Set[str]:
    """
    Get the app paths from ttfw_idf under the given paths
    """
    from idf_build_apps import CMakeApp

    cases = get_ttfw_cases(paths)
    res: Set[str] = set()
    for case in cases:
        if not target or target == case.case_info['target'].lower():
            # ttfw has no good way to detect the app path for master-slave tests
            # the apps real location may be the sub folder of the test script path
            # check if the current folder is an app, if it's not, add all its subfolders if they are apps
            # only one level down
            _app_dir = case.case_info['app_dir']
            if CMakeApp.is_app(_app_dir):
                res.add(_app_dir)
            else:
                for child in os.listdir(_app_dir):
                    sub_path = os.path.join(_app_dir, child)
                    if os.path.isdir(sub_path) and CMakeApp.is_app(sub_path):
                        res.add(sub_path)

    return res
