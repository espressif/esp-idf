# internal use only for CI
# some CI related util functions
#
# SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import contextlib
import logging
import os
import subprocess
import sys
from typing import Any, List, Optional, Set, Union

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
