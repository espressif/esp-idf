# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import datetime
import logging
import os
import shutil
import subprocess
import typing
from pathlib import Path
from tempfile import mkdtemp

import pytest
from _pytest.fixtures import FixtureRequest
from test_build_system_helpers import EnvDict
from test_build_system_helpers import EXT_IDF_PATH
from test_build_system_helpers import get_idf_build_env
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import run_idf_py


# Pytest hook used to check if the test has passed or failed, from a fixture.
# Based on https://docs.pytest.org/en/latest/example/simple.html#making-test-result-information-available-in-fixtures
@pytest.hookimpl(tryfirst=True, hookwrapper=True)
def pytest_runtest_makereport(item: typing.Any, call: typing.Any) -> typing.Generator[None, pytest.TestReport, None]:  # pylint: disable=unused-argument
    outcome = yield  # Execute all other hooks to obtain the report object
    report = outcome.get_result()
    if report.when == 'call' and report.passed:
        # set an attribute which can be checked using 'should_clean_test_dir' function below
        setattr(item, 'passed', True)


def should_clean_test_dir(request: FixtureRequest) -> bool:
    # Only remove the test directory if the test has passed
    return getattr(request.node, 'passed', False) or request.config.getoption('cleanup_idf_copy', False)


def pytest_addoption(parser: pytest.Parser) -> None:
    parser.addoption(
        '--work-dir', action='store', default=None,
        help='Directory for temporary files. If not specified, an OS-specific '
             'temporary directory will be used.'
    )
    parser.addoption(
        '--cleanup-idf-copy', action='store_true',
        help='Always clean up the IDF copy after the test. By default, the copy is cleaned up only if the test passes.'
    )


@pytest.fixture(scope='session')
def _session_work_dir(request: FixtureRequest) -> typing.Generator[typing.Tuple[Path, bool], None, None]:
    work_dir = request.config.getoption('--work-dir')

    if work_dir:
        work_dir = os.path.join(work_dir, datetime.datetime.utcnow().strftime('%Y-%m-%d_%H-%M-%S'))
        logging.debug(f'using work directory: {work_dir}')
        os.makedirs(work_dir, exist_ok=True)
        clean_dir = None
        is_temp_dir = False
    else:
        work_dir = mkdtemp()
        logging.debug(f'created temporary work directory: {work_dir}')
        clean_dir = work_dir
        is_temp_dir = True

    # resolve allows using relative paths with --work-dir option
    yield Path(work_dir).resolve(), is_temp_dir

    if clean_dir:
        logging.debug(f'cleaning up {clean_dir}')
        shutil.rmtree(clean_dir, ignore_errors=True)


@pytest.fixture(name='func_work_dir', autouse=True)
def work_dir(request: FixtureRequest, _session_work_dir: typing.Tuple[Path, bool]) -> typing.Generator[Path, None, None]:
    session_work_dir, is_temp_dir = _session_work_dir

    if request._pyfuncitem.keywords.get('force_temp_work_dir') and not is_temp_dir:
        work_dir = Path(mkdtemp()).resolve()
        logging.debug('Force using temporary work directory')
        clean_dir = work_dir
    else:
        work_dir = session_work_dir
        clean_dir = None

    # resolve allows using relative paths with --work-dir option
    yield work_dir

    if clean_dir:
        logging.debug(f'cleaning up {clean_dir}')
        shutil.rmtree(clean_dir, ignore_errors=True)


@pytest.fixture
def test_app_copy(func_work_dir: Path, request: FixtureRequest) -> typing.Generator[Path, None, None]:
    # by default, use hello_world app and copy it to a temporary directory with
    # the name resembling that of the test
    copy_from = 'tools/test_build_system/build_test_app'
    copy_to = request.node.name + '_app'

    # allow overriding source and destination via pytest.mark.test_app_copy()
    mark = request.node.get_closest_marker('test_app_copy')
    if mark:
        copy_from = mark.args[0]
        if len(mark.args) > 1:
            copy_to = mark.args[1]

    path_from = Path(os.environ['IDF_PATH']) / copy_from
    path_to = func_work_dir / copy_to

    # if the new directory inside the original directory,
    # make sure not to go into recursion.
    ignore = shutil.ignore_patterns(
        path_to.name,
        # also ignore files which may be present in the work directory
        'build', 'sdkconfig')

    logging.debug(f'copying {path_from} to {path_to}')
    shutil.copytree(path_from, path_to, ignore=ignore, symlinks=True)

    old_cwd = Path.cwd()
    os.chdir(path_to)

    yield Path(path_to)

    os.chdir(old_cwd)

    if should_clean_test_dir(request):
        logging.debug('cleaning up work directory after a successful test: {}'.format(path_to))
        shutil.rmtree(path_to, ignore_errors=True)


@pytest.fixture
def test_git_template_app(func_work_dir: Path, request: FixtureRequest) -> typing.Generator[Path, None, None]:
    copy_to = request.node.name + '_app'
    path_to = func_work_dir / copy_to

    logging.debug(f'cloning git-template app to {path_to}')
    path_to.mkdir()
    # No need to clone full repository, just a single master branch
    subprocess.run(['git', 'clone', '--single-branch', '-b', 'master', '--depth', '1', 'https://github.com/espressif/esp-idf-template.git', '.'],
                   cwd=path_to, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    old_cwd = Path.cwd()
    os.chdir(path_to)

    yield Path(path_to)

    os.chdir(old_cwd)

    if should_clean_test_dir(request):
        logging.debug('cleaning up work directory after a successful test: {}'.format(path_to))
        shutil.rmtree(path_to, ignore_errors=True)


@pytest.fixture
def idf_copy(func_work_dir: Path, request: FixtureRequest) -> typing.Generator[Path, None, None]:
    copy_to = request.node.name + '_idf'

    # allow overriding the destination via pytest.mark.idf_copy()
    mark = request.node.get_closest_marker('idf_copy')
    if mark:
        copy_to = mark.args[0]

    path_from = EXT_IDF_PATH
    path_to = func_work_dir / copy_to

    # if the new directory inside the original directory,
    # make sure not to go into recursion.
    ignore = shutil.ignore_patterns(
        path_to.name,
        # also ignore the build directories which may be quite large
        # plus ignore .git since it is causing trouble when removing on Windows
        '**/build', '.git')

    logging.debug(f'copying {path_from} to {path_to}')
    shutil.copytree(path_from, path_to, ignore=ignore, symlinks=True)

    orig_idf_path = os.environ['IDF_PATH']
    os.environ['IDF_PATH'] = str(path_to)

    yield Path(path_to)

    os.environ['IDF_PATH'] = orig_idf_path

    if should_clean_test_dir(request):
        logging.debug('cleaning up work directory after a successful test: {}'.format(path_to))
        shutil.rmtree(path_to, ignore_errors=True)


@pytest.fixture(name='default_idf_env')
def fixture_default_idf_env() -> EnvDict:
    return get_idf_build_env(os.environ['IDF_PATH'])  # type: ignore


@pytest.fixture
def idf_py(default_idf_env: EnvDict) -> IdfPyFunc:
    def result(*args: str, check: bool = True, input_str: typing.Optional[str] = None) -> subprocess.CompletedProcess:
        return run_idf_py(*args, env=default_idf_env, workdir=os.getcwd(), check=check, input_str=input_str)  # type: ignore
    return result
