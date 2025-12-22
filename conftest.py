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

import json
import logging
import os
import re
import signal
import time
import typing as t
from copy import deepcopy
from urllib.parse import quote

import common_test_methods  # noqa: F401
import gitlab_api
import pexpect
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
from pytest_embedded.utils import to_bytes
from pytest_embedded.utils import to_str
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.unity_tester import CaseTester
from pytest_embedded_jtag._telnetlib.telnetlib import Telnet  # python 3.13 removed telnetlib, use this instead


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
def pipeline_id(request: FixtureRequest) -> str | None:
    return request.config.getoption('pipeline_id', None) or os.getenv('PARENT_PIPELINE_ID', None)  # type: ignore


def get_pipeline_commit_sha_by_pipeline_id(pipeline_id: str) -> str | None:
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
        pipeline_id: str | None = None,
    ) -> None:
        self.commit_sha = commit_sha
        self.pipeline_id = pipeline_id

    def download_app(self, app_build_path: str, artifact_type: str | None = None) -> None:
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


class OpenOCD:
    def __init__(self, dut: 'IdfDut'):
        self.MAX_RETRIES = 3
        self.RETRY_DELAY = 1
        self.TELNET_PORT = 4444
        self.dut = dut
        self.telnet: Telnet | None = None
        self.log_file = os.path.join(self.dut.logdir, 'ocd.txt')
        self.proc: pexpect.spawn | None = None

    def __enter__(self) -> 'OpenOCD':
        return self

    def __exit__(self, exception_type: t.Any, exception_value: t.Any, exception_traceback: t.Any) -> None:
        self.kill()

    def run(self) -> t.Optional['OpenOCD']:
        desc_path = os.path.join(self.dut.app.binary_path, 'project_description.json')

        try:
            with open(desc_path) as f:
                project_desc = json.load(f)
        except FileNotFoundError:
            logging.error('Project description file not found at %s', desc_path)
            raise

        openocd_scripts = os.getenv('OPENOCD_SCRIPTS')
        if not openocd_scripts:
            raise RuntimeError('OPENOCD_SCRIPTS environment variable is not set.')

        debug_args = project_desc.get('debug_arguments_openocd')
        if not debug_args:
            raise KeyError("'debug_arguments_openocd' key is missing in project_description.json")

        # For debug purposes, make the value '4'
        ocd_env = os.environ.copy()
        ocd_env['LIBUSB_DEBUG'] = '1'

        for attempt in range(1, self.MAX_RETRIES + 1):
            try:
                self.proc = pexpect.spawn(
                    command='openocd',
                    args=['-s', openocd_scripts] + debug_args.split(),
                    timeout=5,
                    encoding='utf-8',
                    codec_errors='ignore',
                    env=ocd_env,
                )
                if self.proc and self.proc.isalive():
                    self.proc.expect_exact('Info : Listening on port 3333 for gdb connections', timeout=5)
                    self.connect_telnet()
                    self.write(f'log_output {self.log_file}')
                    return self
            except (pexpect.exceptions.EOF, pexpect.exceptions.TIMEOUT, ConnectionRefusedError) as e:
                logging.error(
                    'OpenOCD connection attempt %d/%d failed. Error: %s', attempt, self.MAX_RETRIES, type(e).__name__
                )
                self.kill()
            time.sleep(self.RETRY_DELAY)

        raise RuntimeError('Failed to run OpenOCD after %d attempts.', self.MAX_RETRIES)

    def connect_telnet(self) -> None:
        for attempt in range(1, self.MAX_RETRIES + 1):
            try:
                self.telnet = Telnet('127.0.0.1', self.TELNET_PORT, 5)
                break
            except ConnectionRefusedError as e:
                logging.error('Error telnet connection: %s in attempt:%d', e, attempt)
                time.sleep(1)
        else:
            raise ConnectionRefusedError

    def write(self, s: str, timeout: int = 30) -> t.Any:
        if self.telnet is None:
            logging.error('Telnet connection is not established.')
            return ''
        resp = self.telnet.read_very_eager()
        self.telnet.write(to_bytes(s, '\n'))
        resp += self.telnet.read_until(b'>', timeout=timeout)
        if not resp.endswith(b'>'):
            return ''
        return to_str(resp)

    def apptrace_wait_stop(self, timeout: int = 30) -> None:
        stopped = False
        end_before = time.time() + timeout
        while not stopped:
            cmd_out = self.write('esp apptrace status')
            for line in cmd_out.splitlines():
                if line.startswith('Tracing is STOPPED.'):
                    stopped = True
                    break
            if not stopped and time.time() > end_before:
                raise pexpect.TIMEOUT('Failed to wait for apptrace stop!')
            time.sleep(1)

    def gcov_dump(self, on_the_fly: bool = True) -> t.Any:
        cmd = 'esp gcov'
        if not on_the_fly:
            cmd += ' dump'
        cmd_out = self.write(cmd)
        if 'Targets connected.' not in cmd_out:
            raise pexpect.TIMEOUT('Failed to start gcov dump!')
        if 'Targets disconnected.' not in cmd_out:
            raise pexpect.TIMEOUT('Failed to stop gcov dump!')
        return cmd_out

    def kill(self) -> None:
        # Check if the process is still running
        if self.proc and self.proc.isalive():
            self.proc.terminate()
            self.proc.kill(signal.SIGKILL)


@pytest.fixture
def openocd_dut(dut: IdfDut) -> OpenOCD:
    if isinstance(dut, tuple):
        raise ValueError('Multi-DUT support is not implemented yet')
    return OpenOCD(dut)


@pytest.fixture(scope='session')
def app_downloader(
    pipeline_id: str | None,
) -> AppDownloader | None:
    if commit_sha := os.getenv('PIPELINE_COMMIT_SHA'):
        logging.debug('pipeline commit sha from CI env is %s', commit_sha)
        return AppDownloader(commit_sha, None)

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
    target: str | None,
    config: str | None,
    app_downloader: AppDownloader | None,
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
    if 'skip_app_downloader' in case.all_markers:
        logging.debug('skip_app_downloader marker found, skip downloading app')
        downloader = None
    else:
        downloader = app_downloader

    if downloader:
        # somehow hardcoded...
        app_build_path = os.path.join(idf_relpath(app_path), f'build_{target}_{config}')
        if requires_elf_or_map(case):
            downloader.download_app(app_build_path)
        else:
            downloader.download_app(app_build_path, 'flash')
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

    def _attach_log_url_to_xml_attribute(log_file_path: str) -> str:
        if not isinstance(log_file_path, str):
            return 'No log URL found'

        ci_pages_url = os.getenv('CI_PAGES_URL')
        logdir_pattern = re.compile(rf'({DEFAULT_LOGDIR}/.*)')
        match = logdir_pattern.search(log_file_path)

        if not match:
            return 'No log URL found'

        if not ci_pages_url:
            return log_file_path

        job_id = os.getenv('CI_JOB_ID', '0')
        modified_ci_pages_url = ci_pages_url.replace('esp-idf', '-/esp-idf')
        log_url = f'{modified_ci_pages_url}/-/jobs/{job_id}/artifacts/{match.group(1)}'

        return log_url

    xml_attribute = []
    if isinstance(_pexpect_logfile, str):
        xml_attribute.append(_attach_log_url_to_xml_attribute(_pexpect_logfile))
    if isinstance(_pexpect_logfile, tuple):
        for i, log_file in enumerate(_pexpect_logfile):
            xml_attribute.append(_attach_log_url_to_xml_attribute(log_file))
    record_xml_attribute('dut_log_url', ';'.join(xml_attribute))


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
            dut_artifacts_url.append(f'{_dut[0].test_case_name}:')
        else:
            logs_files.append(template.format(get_path(_dut.logfile)))
            dut_artifacts_url.append(f'{_dut.test_case_name}:')

        for file in logs_files:
            dut_artifacts_url.append('    - {}'.format(quote(file, safe=':/')))


def pytest_terminal_summary(terminalreporter, exitstatus, config):  # type: ignore
    if dut_artifacts_url:
        terminalreporter.ensure_newline()
        terminalreporter.section('Failed Test Artifacts URL', sep='-', red=True, bold=True)
        terminalreporter.line('\n'.join(dut_artifacts_url))
