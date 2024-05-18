# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import abc
import fnmatch
import html
import os
import re
import typing as t

import yaml
from artifacts_handler import ArtifactType
from gitlab_api import Gitlab
from idf_build_apps import App
from idf_build_apps.constants import BuildStatus
from idf_ci.uploader import AppUploader
from prettytable import PrettyTable

from .constants import COMMENT_START_MARKER
from .constants import REPORT_TEMPLATE_FILEPATH
from .constants import TEST_RELATED_APPS_DOWNLOAD_URLS_FILENAME
from .models import TestCase
from .utils import is_url
from .utils import load_known_failure_cases


class ReportGenerator:
    REGEX_PATTERN = '#### {}[^####]+'

    def __init__(self, project_id: int, mr_iid: int, pipeline_id: int, *, title: str):
        gl_project = Gitlab(project_id).project
        if mr_iid is not None:
            self.mr = gl_project.mergerequests.get(mr_iid)
        else:
            self.mr = None
        self.pipeline_id = pipeline_id

        self.title = title
        self.output_filepath = self.title.lower().replace(' ', '_') + '.html'
        self.additional_info = ''

    @staticmethod
    def get_download_link_for_url(url: str) -> str:
        if url:
            return f'<a href="{url}">Download</a>'

        return ''

    def generate_html_report(self, table_str: str) -> str:
        # we're using bootstrap table
        table_str = table_str.replace('<table>', '<table data-toggle="table" data-search="true">')

        with open(REPORT_TEMPLATE_FILEPATH) as fr:
            template = fr.read()

        return template.replace('{{title}}', self.title).replace('{{table}}', table_str)

    @staticmethod
    def table_to_html_str(table: PrettyTable) -> str:
        return html.unescape(table.get_html_string())  # type: ignore

    @abc.abstractmethod
    def _get_report_str(self) -> str:
        raise NotImplementedError

    def post_report(self, job_id: int, commit_id: str) -> None:
        # report in html format, otherwise will exceed the limit
        with open(self.output_filepath, 'w') as fw:
            fw.write(self._get_report_str())

        # for example, {URL}/-/esp-idf/-/jobs/{id}/artifacts/list_job_84.txt
        # CI_PAGES_URL is {URL}/esp-idf, which missed one `-`
        url = os.getenv('CI_PAGES_URL', '').replace('esp-idf', '-/esp-idf')

        comment = f'#### {self.title}\n'
        if self.additional_info:
            comment += f'{self.additional_info}\n'

        comment += f"""
Full {self.title} here: {url}/-/jobs/{job_id}/artifacts/{self.output_filepath} (with commit {commit_id})

"""
        print(comment)

        if self.mr is None:
            print('No MR found, skip posting comment')
            return

        for note in self.mr.notes.list(iterator=True):
            if note.body.startswith(COMMENT_START_MARKER):
                updated_str = re.sub(self.REGEX_PATTERN.format(self.title), comment, note.body)
                if updated_str == note.body:  # not updated
                    updated_str = f'{note.body.strip()}\n\n{comment}'

                note.body = updated_str
                note.save()
                break
        else:
            new_comment = f"""{COMMENT_START_MARKER}

{comment}"""
            self.mr.notes.create({'body': new_comment})


class BuildReportGenerator(ReportGenerator):
    def __init__(
        self,
        project_id: int,
        mr_iid: int,
        pipeline_id: int,
        *,
        title: str = 'Build Report',
        apps: t.List[App],
    ):
        super().__init__(project_id, mr_iid, pipeline_id, title=title)
        self.apps = apps

        self.apps_presigned_url_filepath = TEST_RELATED_APPS_DOWNLOAD_URLS_FILENAME

    def _get_report_str(self) -> str:
        if not self.apps:
            print('No apps found, skip generating build report')
            return 'No Apps Built'

        uploader = AppUploader(self.pipeline_id)

        table_str = ''

        failed_apps = [app for app in self.apps if app.build_status == BuildStatus.FAILED]
        if failed_apps:
            table_str += '<h2>Failed Apps</h2>'

            failed_apps_table = PrettyTable()
            failed_apps_table.field_names = [
                'App Dir',
                'Build Dir',
                'Failed Reason',
                'Build Log',
            ]
            for app in failed_apps:
                failed_apps_table.add_row(
                    [
                        app.app_dir,
                        app.build_dir,
                        app.build_comment or '',
                        self.get_download_link_for_url(uploader.get_app_presigned_url(app, ArtifactType.LOGS)),
                    ]
                )

            table_str += self.table_to_html_str(failed_apps_table)

        built_test_related_apps = [app for app in self.apps if app.build_status == BuildStatus.SUCCESS and app.preserve]
        if built_test_related_apps:
            table_str += '<h2>Built Apps (Test Related)</h2>'

            built_apps_table = PrettyTable()
            built_apps_table.field_names = [
                'App Dir',
                'Build Dir',
                'Bin Files with Build Log (without map and elf)',
                'Map and Elf Files',
            ]
            app_presigned_urls_dict: t.Dict[str, t.Dict[str, str]] = {}
            for app in built_test_related_apps:
                _d = {
                    ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES.value: uploader.get_app_presigned_url(
                        app, ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES
                    ),
                    ArtifactType.MAP_AND_ELF_FILES.value: uploader.get_app_presigned_url(
                        app, ArtifactType.MAP_AND_ELF_FILES
                    ),
                }

                built_apps_table.add_row(
                    [
                        app.app_dir,
                        app.build_dir,
                        self.get_download_link_for_url(_d[ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES]),
                        self.get_download_link_for_url(_d[ArtifactType.MAP_AND_ELF_FILES]),
                    ]
                )

                app_presigned_urls_dict[app.build_path] = _d

            # also generate a yaml file that includes the apps and the presigned urls
            # for helping debugging locally
            with open(self.apps_presigned_url_filepath, 'w') as fw:
                yaml.dump(app_presigned_urls_dict, fw)

            table_str += self.table_to_html_str(built_apps_table)

        built_non_test_related_apps = [
            app for app in self.apps if app.build_status == BuildStatus.SUCCESS and not app.preserve
        ]
        if built_non_test_related_apps:
            table_str += '<h2>Built Apps (Non Test Related)</h2>'

            built_apps_table = PrettyTable()
            built_apps_table.field_names = [
                'App Dir',
                'Build Dir',
                'Build Log',
            ]
            for app in built_non_test_related_apps:
                built_apps_table.add_row(
                    [
                        app.app_dir,
                        app.build_dir,
                        self.get_download_link_for_url(uploader.get_app_presigned_url(app, ArtifactType.LOGS)),
                    ]
                )

            table_str += self.table_to_html_str(built_apps_table)

        skipped_apps = [app for app in self.apps if app.build_status == BuildStatus.SKIPPED]
        if skipped_apps:
            table_str += '<h2>Skipped Apps</h2>'

            skipped_apps_table = PrettyTable()
            skipped_apps_table.field_names = ['App Dir', 'Build Dir', 'Skipped Reason', 'Build Log']
            for app in skipped_apps:
                skipped_apps_table.add_row(
                    [
                        app.app_dir,
                        app.build_dir,
                        app.build_comment or '',
                        self.get_download_link_for_url(uploader.get_app_presigned_url(app, ArtifactType.LOGS)),
                    ]
                )

            table_str += self.table_to_html_str(skipped_apps_table)

        return self.generate_html_report(table_str)


class TargetTestReportGenerator(ReportGenerator):
    def __init__(
        self,
        project_id: int,
        mr_iid: int,
        pipeline_id: int,
        *,
        title: str = 'Target Test Report',
        test_cases: t.List[TestCase],
    ):
        super().__init__(project_id, mr_iid, pipeline_id, title=title)

        self.test_cases = test_cases
        self._known_failure_cases_set = None

    @property
    def known_failure_cases_set(self) -> t.Optional[t.Set[str]]:
        if self._known_failure_cases_set is None:
            self._known_failure_cases_set = load_known_failure_cases()

        return self._known_failure_cases_set

    def get_known_failure_cases(self) -> t.List[TestCase]:
        if self.known_failure_cases_set is None:
            return []
        matched_cases = [
            testcase
            for testcase in self.test_cases
            if any(fnmatch.fnmatch(testcase.name, pattern) for pattern in self.known_failure_cases_set)
            and testcase.is_failure
        ]
        return matched_cases

    def _filter_test_cases(self, condition: t.Callable[[TestCase], bool]) -> t.List[TestCase]:
        """
        Filter test cases based on a given condition.  In this scenario, we filter by status,
        however it is possible to filter by other criteria.

        :param condition: A function that evaluates to True or False for each test case.
        :return: List of filtered TestCase instances.
        """
        return [tc for tc in self.test_cases if condition(tc)]

    def _create_table_for_test_cases(
        self, test_cases: t.List[TestCase], headers: t.List[str], row_attrs: t.List[str]
    ) -> str:
        """
        Create a PrettyTable and convert it to an HTML string for the provided test cases.
        :param test_cases: List of TestCase objects to include in the table.
        :param headers: List of strings for the table headers.
        :param row_attrs: List of attributes to include in each row.
        :return: HTML table string.
        """
        table = PrettyTable()
        table.field_names = headers
        for tc in test_cases:
            row = []
            for attr in row_attrs:
                value = getattr(tc, attr, '')
                if is_url(value):
                    link = f'<a href="{value}">link</a>'
                    row.append(link)
                else:
                    row.append(value)
            table.add_row(row)

        return self.table_to_html_str(table)

    def _get_report_str(self) -> str:
        """
        Generate a complete HTML report string by processing test cases.
        :return: Complete HTML report string.
        """
        table_str = ''

        known_failures = self.get_known_failure_cases()
        known_failure_case_names = {case.name for case in known_failures}
        failed_test_cases = self._filter_test_cases(
            lambda tc: tc.is_failure and tc.name not in known_failure_case_names
        )
        skipped_test_cases = self._filter_test_cases(lambda tc: tc.is_skipped)
        successful_test_cases = self._filter_test_cases(lambda tc: tc.is_success)

        if failed_test_cases:
            table_str += '<h2>Failed Test Cases (Excludes Known Failure Cases)</h2>'
            table_str += self._create_table_for_test_cases(
                test_cases=failed_test_cases,
                headers=['Test Case', 'Test Script File Path', 'Failure Reason', 'Job URL', 'Grafana URL'],
                row_attrs=['name', 'file', 'failure', 'ci_job_url', 'ci_dashboard_url'],
            )

        if known_failures:
            table_str += '<h2>Known Failure Cases</h2>'
            table_str += self._create_table_for_test_cases(
                test_cases=known_failures,
                headers=['Test Case', 'Test Script File Path', 'Failure Reason', 'Job URL', 'Grafana URL'],
                row_attrs=['name', 'file', 'failure', 'ci_job_url', 'ci_dashboard_url'],
            )

        if skipped_test_cases:
            table_str += '<h2>Skipped Test Cases</h2>'
            table_str += self._create_table_for_test_cases(
                test_cases=skipped_test_cases,
                headers=['Test Case', 'Test Script File Path', 'Skipped Reason', 'Grafana URL'],
                row_attrs=['name', 'file', 'skipped', 'ci_dashboard_url'],
            )

        if successful_test_cases:
            table_str += '<h2>Succeeded Test Cases</h2>'
            table_str += self._create_table_for_test_cases(
                test_cases=successful_test_cases,
                headers=['Test Case', 'Test Script File Path', 'Job URL', 'Grafana URL'],
                row_attrs=['name', 'file', 'ci_job_url', 'ci_dashboard_url'],
            )
        self.additional_info = (
            '**Test Case Summary:**\n'
            f'- **Failed Test Cases (Excludes Known Failure Cases):** {len(failed_test_cases)}\n'
            f'- **Known Failures:** {len(known_failures)}\n'
            f'- **Skipped Test Cases:** {len(skipped_test_cases)}\n'
            f'- **Succeeded Test Cases:** {len(successful_test_cases)}\n\n'
            f'Please check report below for more information.\n\n'
        )

        return self.generate_html_report(table_str)
