# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# pylint: disable=W0621  # redefined-outer-name
#
# IDF is using [pytest](https://github.com/pytest-dev/pytest) and
# [pytest-embedded plugin](https://github.com/espressif/pytest-embedded) as its test framework.
#
# if you found any bug or have any question,
# please report to https://github.com/espressif/pytest-embedded/issues
# or discuss at https://github.com/espressif/pytest-embedded/discussions
import os
import subprocess
import sys

if os.path.join(os.path.dirname(__file__), 'tools', 'ci') not in sys.path:
    sys.path.append(os.path.join(os.path.dirname(__file__), 'tools', 'ci'))

if os.path.join(os.path.dirname(__file__), 'tools', 'ci', 'python_packages') not in sys.path:
    sys.path.append(os.path.join(os.path.dirname(__file__), 'tools', 'ci', 'python_packages'))

import logging
import os
import re
import typing as t
from copy import deepcopy
from urllib.parse import quote

import common_test_methods  # noqa: F401
import gitlab_api
import pytest
from _pytest.config import Config
from _pytest.fixtures import FixtureRequest
from idf_ci import PytestCase
from idf_ci.idf_pytest import IDF_CI_PYTEST_CASE_KEY
from idf_ci_utils import idf_relpath
from idf_pytest.constants import DEFAULT_LOGDIR
from idf_pytest.plugin import IDF_LOCAL_PLUGIN_KEY
from idf_pytest.plugin import IdfLocalPlugin
from idf_pytest.plugin import requires_elf_or_map
from idf_pytest.utils import format_case_id
from pytest_embedded.plugin import multi_dut_fixture
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.unity_tester import CaseTester


############
# Fixtures #
############
@pytest.fixture(scope='session')
def idf_path() -> str:
    return os.path.dirname(__file__)


@pytest.fixture(scope='session')
def session_root_logdir(idf_path: str) -> str:
    """Session scoped log dir for pytest-embedded"""
    return idf_path


@pytest.fixture
def case_tester(unity_tester: CaseTester) -> CaseTester:
    return unity_tester


@pytest.fixture
def test_func_name(request: FixtureRequest) -> str:
    return request.node.function.__name__  # type: ignore


@pytest.fixture
def test_case_name(request: FixtureRequest, target: str, config: str) -> str:
    is_qemu = request._pyfuncitem.get_closest_marker('qemu') is not None
    if hasattr(request._pyfuncitem, 'callspec'):
        params = deepcopy(request._pyfuncitem.callspec.params)  # type: ignore
    else:
        params = {}

    filtered_params = {}
    for k, v in params.items():
        if k not in request.session._fixturemanager._arg2fixturedefs:  # type: ignore
            filtered_params[k] = v  # not fixture ones

    return format_case_id(target, config, request.node.originalname, is_qemu=is_qemu, params=filtered_params)  # type: ignore


@pytest.fixture(scope='session')
def pipeline_id(request: FixtureRequest) -> t.Optional[str]:
    return request.config.getoption('pipeline_id', None) or os.getenv('PARENT_PIPELINE_ID', None)  # type: ignore


def get_pipeline_commit_sha_by_pipeline_id(pipeline_id: str) -> t.Optional[str]:
    gl = gitlab_api.Gitlab(os.getenv('CI_PROJECT_ID', 'espressif/esp-idf'))
    pipeline = gl.project.pipelines.get(pipeline_id)
    if not pipeline:
        return None

    commit = gl.project.commits.get(pipeline.sha)
    if not commit or not commit.parent_ids:
        return None

    if len(commit.parent_ids) == 1:
        return commit.parent_ids[0]  # type: ignore

    for parent_id in commit.parent_ids:
        parent_commit = gl.project.commits.get(parent_id)
        if parent_commit.parent_ids and len(parent_commit.parent_ids) == 1:
            return parent_id  # type: ignore

    return None


class AppDownloader:
    def __init__(
        self,
        commit_sha: str,
        pipeline_id: t.Optional[str] = None,
    ) -> None:
        self.commit_sha = commit_sha
        self.pipeline_id = pipeline_id

    def download_app(self, app_build_path: str, artifact_type: t.Optional[str] = None) -> None:
        args = [
            'idf-ci',
            'gitlab',
            'download-artifacts',
            '--commit-sha',
            self.commit_sha,
        ]
        if artifact_type:
            args.extend(['--type', artifact_type])
        if self.pipeline_id:
            args.extend(['--pipeline-id', self.pipeline_id])
        args.append(app_build_path)

        subprocess.run(
            args,
            stdout=sys.stdout,
            stderr=sys.stderr,
        )


PRESIGNED_JSON = 'presigned.json'


@pytest.fixture(scope='session')
def app_downloader(
    pipeline_id: t.Optional[str],
) -> t.Optional[AppDownloader]:
    if not pipeline_id:
        return None

    commit_sha = get_pipeline_commit_sha_by_pipeline_id(pipeline_id)
    if not commit_sha:
        raise ValueError(
            'commit sha cannot be found for pipeline id %s. Please check the pipeline id. '
            'If you think this is a bug, please report it to CI team',
        )
    logging.debug('pipeline commit sha of pipeline %s is %s', pipeline_id, commit_sha)

    return AppDownloader(commit_sha, pipeline_id)


@pytest.fixture
@multi_dut_fixture
def build_dir(
    request: FixtureRequest,
    app_path: str,
    target: t.Optional[str],
    config: t.Optional[str],
    app_downloader: t.Optional[AppDownloader],
) -> str:
    """
    Check local build dir with the following priority:

    1. build_<target>_<config>
    2. build_<target>
    3. build_<config>
    4. build

    Returns:
        valid build directory
    """
    # download from minio on CI
    case: PytestCase = request.node.stash[IDF_CI_PYTEST_CASE_KEY]
    if app_downloader:
        # somehow hardcoded...
        app_build_path = os.path.join(idf_relpath(app_path), f'build_{target}_{config}')
        if requires_elf_or_map(case):
            app_downloader.download_app(app_build_path)
        else:
            app_downloader.download_app(app_build_path, 'flash')
        check_dirs = [f'build_{target}_{config}']
    else:
        check_dirs = []
        build_dir_arg = request.config.getoption('build_dir', None)
        if build_dir_arg:
            check_dirs.append(build_dir_arg)
        if target is not None and config is not None:
            check_dirs.append(f'build_{target}_{config}')
        if target is not None:
            check_dirs.append(f'build_{target}')
        if config is not None:
            check_dirs.append(f'build_{config}')
        check_dirs.append('build')

    for check_dir in check_dirs:
        binary_path = os.path.join(app_path, check_dir)
        if os.path.isdir(binary_path):
            logging.info(f'found valid binary path: {binary_path}')
            return check_dir

        logging.warning('checking binary path: %s... missing... try another place', binary_path)

    raise ValueError(
        f'no build dir valid. Please build the binary via "idf.py -B {check_dirs[0]} build" and run pytest again'
    )


@pytest.fixture(autouse=True)
@multi_dut_fixture
def junit_properties(test_case_name: str, record_xml_attribute: t.Callable[[str, object], None]) -> None:
    """
    This fixture is autoused and will modify the junit report test case name to <target>.<config>.<case_name>
    """
    record_xml_attribute('name', test_case_name)


@pytest.fixture(autouse=True)
@multi_dut_fixture
def ci_job_url(record_xml_attribute: t.Callable[[str, object], None]) -> None:
    if ci_job_url := os.getenv('CI_JOB_URL'):
        record_xml_attribute('ci_job_url', ci_job_url)


@pytest.fixture(autouse=True)
def set_test_case_name(request: FixtureRequest, test_case_name: str) -> None:
    request.node.funcargs['test_case_name'] = test_case_name


@pytest.fixture(autouse=True)
def set_dut_log_url(record_xml_attribute: t.Callable[[str, object], None], _pexpect_logfile: str) -> t.Generator:
    # Record the "dut_log_url" attribute in the XML report once test execution finished
    yield

    if not isinstance(_pexpect_logfile, str):
        record_xml_attribute('dut_log_url', 'No log URL found')
        return

    ci_pages_url = os.getenv('CI_PAGES_URL')
    logdir_pattern = re.compile(rf'({DEFAULT_LOGDIR}/.*)')
    match = logdir_pattern.search(_pexpect_logfile)

    if not match:
        record_xml_attribute('dut_log_url', 'No log URL found')
        return

    if not ci_pages_url:
        record_xml_attribute('dut_log_url', _pexpect_logfile)
        return

    job_id = os.getenv('CI_JOB_ID', '0')
    modified_ci_pages_url = ci_pages_url.replace('esp-idf', '-/esp-idf')
    log_url = f'{modified_ci_pages_url}/-/jobs/{job_id}/artifacts/{match.group(1)}'

    record_xml_attribute('dut_log_url', log_url)


######################
# Log Util Functions #
######################
@pytest.fixture
def log_performance(record_property: t.Callable[[str, object], None]) -> t.Callable[[str, str], None]:
    """
    log performance item with pre-defined format to the console
    and record it under the ``properties`` tag in the junit report if available.
    """

    def real_func(item: str, value: str) -> None:
        """
        :param item: performance item name
        :param value: performance value
        """
        logging.info('[Performance][%s]: %s', item, value)
        record_property(item, value)

    return real_func


@pytest.fixture
def check_performance(idf_path: str) -> t.Callable[[str, float, str], None]:
    """
    check if the given performance item meets the passing standard or not
    """

    def real_func(item: str, value: float, target: str) -> None:
        """
        :param item: performance item name
        :param value: performance item value
        :param target: target chip
        :raise: AssertionError: if check fails
        """

        def _find_perf_item(operator: str, path: str) -> float:
            with open(path, encoding='utf-8') as f:
                data = f.read()
            match = re.search(rf'#define\s+IDF_PERFORMANCE_{operator}_{item.upper()}\s+([\d.]+)', data)
            return float(match.group(1))  # type: ignore

        def _check_perf(operator: str, standard_value: float) -> None:
            if operator == 'MAX':
                ret = value <= standard_value
            else:
                ret = value >= standard_value
            if not ret:
                raise AssertionError(
                    f"[Performance] {item} value is {value}, doesn't meet pass standard {standard_value}"
                )

        path_prefix = os.path.join(idf_path, 'components', 'idf_test', 'include')
        performance_files = (
            os.path.join(path_prefix, target, 'idf_performance_target.h'),
            os.path.join(path_prefix, 'idf_performance.h'),
        )

        found_item = False
        for op in ['MIN', 'MAX']:
            for performance_file in performance_files:
                try:
                    standard = _find_perf_item(op, performance_file)
                except (OSError, AttributeError):
                    # performance file doesn't exist or match is not found in it
                    continue

                _check_perf(op, standard)
                found_item = True
                break

        if not found_item:
            raise AssertionError(f'Failed to get performance standard for {item}')

    return real_func


@pytest.fixture
def log_minimum_free_heap_size(dut: IdfDut, config: str, idf_path: str) -> t.Callable[..., None]:
    def real_func() -> None:
        res = dut.expect(r'Minimum free heap size: (\d+) bytes')
        logging.info(
            '\n------ heap size info ------\n'
            '[app_path] {}\n'
            '[app_name] {}\n'
            '[config_name] {}\n'
            '[target] {}\n'
            '[minimum_free_heap_size] {} Bytes\n'
            '------ heap size end ------'.format(
                dut.app.app_path.replace(idf_path, '').lstrip('/\\'),
                os.path.basename(dut.app.app_path),
                config,
                dut.target,
                res.group(1).decode('utf8'),
            )
        )

    return real_func


@pytest.fixture(scope='session')
def dev_password(request: FixtureRequest) -> str:
    return request.config.getoption('dev_passwd') or ''


@pytest.fixture(scope='session')
def dev_user(request: FixtureRequest) -> str:
    return request.config.getoption('dev_user') or ''


##################
# Hook functions #
##################
def pytest_addoption(parser: pytest.Parser) -> None:
    idf_group = parser.getgroup('idf')
    idf_group.addoption(
        '--dev-user',
        help='user name associated with some specific device/service used during the test execution',
    )
    idf_group.addoption(
        '--dev-passwd',
        help='password associated with some specific device/service used during the test execution',
    )
    idf_group.addoption(
        '--pipeline-id',
        help='For users without s3 access. main pipeline id, not the child pipeline id. '
        'Specify this option to download the artifacts from the minio server for debugging purpose.',
    )


def pytest_configure(config: Config) -> None:
    from idf_pytest.constants import PREVIEW_TARGETS
    from idf_pytest.constants import SUPPORTED_TARGETS
    from pytest_embedded_idf.utils import preview_targets
    from pytest_embedded_idf.utils import supported_targets

    supported_targets.set(SUPPORTED_TARGETS)
    preview_targets.set(PREVIEW_TARGETS)

    config.stash[IDF_LOCAL_PLUGIN_KEY] = IdfLocalPlugin()
    config.pluginmanager.register(config.stash[IDF_LOCAL_PLUGIN_KEY])


def pytest_unconfigure(config: Config) -> None:
    idf_local_plugin = config.stash.get(IDF_LOCAL_PLUGIN_KEY, None)
    if idf_local_plugin:
        del config.stash[IDF_LOCAL_PLUGIN_KEY]
        config.pluginmanager.unregister(idf_local_plugin)


dut_artifacts_url = []


@pytest.hookimpl(hookwrapper=True)
def pytest_runtest_makereport(item, call):  # type: ignore
    outcome = yield
    report = outcome.get_result()
    report.sections = []
    if report.failed:
        _dut = item.funcargs.get('dut')
        if not _dut:
            return

        job_id = os.getenv('CI_JOB_ID', 0)
        url = os.getenv('CI_PAGES_URL', '').replace('esp-idf', '-/esp-idf')
        template = f'{url}/-/jobs/{job_id}/artifacts/{DEFAULT_LOGDIR}/{{}}'
        logs_files = []

        def get_path(x: str) -> str:
            return x.split(f'{DEFAULT_LOGDIR}/', 1)[1]

        if isinstance(_dut, list):
            logs_files.extend([template.format(get_path(d.logfile)) for d in _dut])
            dut_artifacts_url.append('{}:'.format(_dut[0].test_case_name))
        else:
            logs_files.append(template.format(get_path(_dut.logfile)))
            dut_artifacts_url.append('{}:'.format(_dut.test_case_name))

        for file in logs_files:
            dut_artifacts_url.append('    - {}'.format(quote(file, safe=':/')))


def pytest_terminal_summary(terminalreporter, exitstatus, config):  # type: ignore
    if dut_artifacts_url:
        terminalreporter.ensure_newline()
        terminalreporter.section('Failed Test Artifacts URL', sep='-', red=True, bold=True)
        terminalreporter.line('\n'.join(dut_artifacts_url))
