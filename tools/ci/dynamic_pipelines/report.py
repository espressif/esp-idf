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
from .models import GitlabJob
from .models import TestCase
from .utils import fetch_failed_testcases_failure_ratio
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

    def create_table_section(
        self,
        report_sections: list,
        title: str,
        items: list,
        headers: list,
        row_attrs: list,
        value_functions: t.Optional[list] = None,
    ) -> None:
        """
        Appends a formatted section to a report based on the provided items. This section includes
        a header and a table constructed from the items list with specified headers and attributes.

        :param report_sections: List where the HTML report sections are collected. This list is
                                modified in-place by appending new sections.
        :param title: Title for the report section. This title is used as a header above the table.
        :param items: List of item objects to include in the table. Each item should have attributes
                      that correspond to the row_attrs and value_functions specified.
        :param headers: List of strings that will serve as the column headers in the generated table.
        :param row_attrs: List of attributes to include from each item for the table rows. These
                          should be attributes or keys that exist on the items in the 'items' list.
        :param value_functions: Optional list of tuples containing additional header and corresponding
                                value function. Each tuple should specify a header (as a string) and
                                a function that takes an item and returns a string. This is used for
                                generating dynamic columns based on item data.

        :return: None. The function modifies the 'report_sections' list by appending new HTML sections.
        """
        if not items:
            return

        report_sections.append(f'<h2>{title}</h2>')
        report_sections.append(
            self._create_table_for_items(
                items=items, headers=headers, row_attrs=row_attrs, value_functions=value_functions or []
            )
        )

    def _create_table_for_items(
        self,
        items: t.Union[t.List[TestCase], t.List[GitlabJob]],
        headers: t.List[str],
        row_attrs: t.List[str],
        value_functions: t.Optional[t.List[t.Tuple[str, t.Callable[[t.Union[TestCase, GitlabJob]], str]]]] = None,
    ) -> str:
        """
        Create a PrettyTable and convert it to an HTML string for the provided test cases.
        :param items: List of item objects to include in the table.
        :param headers: List of strings for the table headers.
        :param row_attrs: List of attributes to include in each row.
        :param value_functions: List of tuples containing additional header and corresponding value function.
        :return: HTML table string.
        """
        table = PrettyTable()
        table.field_names = headers

        # Create a mapping of header names to their corresponding index in the headers list
        header_index_map = {header: i for i, header in enumerate(headers)}

        for item in items:
            row = []
            for attr in row_attrs:
                value = str(getattr(item, attr, ''))
                if is_url(value):
                    link = f'<a href="{value}">link</a>'
                    row.append(link)
                else:
                    row.append(value)

            # Insert values computed by value functions at the correct column position based on their headers
            if value_functions:
                for header, func in value_functions:
                    index = header_index_map.get(header)
                    if index is not None:
                        computed_value = func(item)
                        row.insert(index, computed_value)

            table.add_row(row)

        return self.table_to_html_str(table)

    @staticmethod
    def _filter_items(
        items: t.Union[t.List[TestCase], t.List[GitlabJob]], condition: t.Callable[[t.Union[TestCase, GitlabJob]], bool]
    ) -> t.List[TestCase]:
        """
        Filter items s based on a given condition.

        :param items: List of items to filter by given condition.
        :param condition: A function that evaluates to True or False for each items.
        :return: List of filtered instances.
        """
        return [item for item in items if condition(item)]

    @staticmethod
    def _sort_items(
        items: t.List[t.Union[TestCase, GitlabJob]],
        key: t.Union[str, t.Callable[[t.Union[TestCase, GitlabJob]], t.Any]],
        order: str = 'asc',
    ) -> t.List[t.Union[TestCase, GitlabJob]]:
        """
        Sort items based on a given key and order.

        :param items: List of items to sort.
        :param key: A string representing the attribute name or a function to extract the sorting key.
        :param order: Order of sorting ('asc' for ascending, 'desc' for descending).
        :return: List of sorted instances.
        """
        key_func = None
        if isinstance(key, str):

            def key_func(item: t.Any) -> t.Any:
                return getattr(item, key)

        if key_func is not None:
            try:
                items = sorted(items, key=key_func, reverse=(order == 'desc'))
            except TypeError:
                print(f'Comparison for the key {key} is not supported')
        return items

    @abc.abstractmethod
    def _get_report_str(self) -> str:
        raise NotImplementedError

    def post_report(self, job_id: int, commit_id: str) -> None:
        # report in html format, otherwise will exceed the limit
        comment = f'#### {self.title}\n'

        report_str = self._get_report_str()

        if self.additional_info:
            comment += f'{self.additional_info}\n'

        if report_str:
            with open(self.output_filepath, 'w') as fw:
                fw.write(report_str)

            # for example, {URL}/-/esp-idf/-/jobs/{id}/artifacts/list_job_84.txt
            # CI_PAGES_URL is {URL}/esp-idf, which missed one `-`
            url = os.getenv('CI_PAGES_URL', '').replace('esp-idf', '-/esp-idf')

            comment += f"""
Full {self.title} here: {url}/-/jobs/{job_id}/artifacts/{self.output_filepath} (with commit {commit_id[:8]})

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

    def _get_report_str(self) -> str:
        """
        Generate a complete HTML report string by processing test cases.
        :return: Complete HTML report string.
        """
        report_parts: list = []

        known_failures = self.get_known_failure_cases()
        known_failure_case_names = {case.name for case in known_failures}
        failed_test_cases = self._filter_items(
            self.test_cases, lambda tc: tc.is_failure and tc.name not in known_failure_case_names
        )
        failed_test_cases_with_ratio = self._sort_items(
            fetch_failed_testcases_failure_ratio(failed_test_cases), key='latest_failed_count'
        )
        skipped_test_cases = self._filter_items(self.test_cases, lambda tc: tc.is_skipped)
        successful_test_cases = self._filter_items(self.test_cases, lambda tc: tc.is_success)

        current_branch_failures = self._sort_items(
            self._filter_items(failed_test_cases_with_ratio, lambda tc: tc.latest_failed_count == 0),
            key='latest_failed_count',
        )
        other_branch_failures = self._sort_items(
            self._filter_items(
                failed_test_cases_with_ratio, lambda tc: tc.name not in [t.name for t in current_branch_failures]
            ),
            key='latest_failed_count',
        )

        self.create_table_section(
            report_sections=report_parts,
            title='Failed Test Cases on Your branch (Excludes Known Failure Cases)',
            items=current_branch_failures,
            headers=[
                'Test Case',
                'Test Script File Path',
                'Failure Reason',
                'Failures across all other branches (20 latest testcases)',
                'Job URL',
                'Grafana URL',
            ],
            row_attrs=['name', 'file', 'failure', 'ci_job_url', 'ci_dashboard_url'],
            value_functions=[
                (
                    'Failures across all other branches (20 latest testcases)',
                    lambda item: f"{getattr(item, 'latest_failed_count', '')} / {getattr(item, 'latest_total_count', '')}",
                )
            ],
        )
        self.create_table_section(
            report_sections=report_parts,
            title='Failed Test Cases on Other branches (Excludes Known Failure Cases)',
            items=other_branch_failures,
            headers=[
                'Test Case',
                'Test Script File Path',
                'Failure Reason',
                'Failures across all other branches (20 latest testcases)',
                'Job URL',
                'Grafana URL',
            ],
            row_attrs=['name', 'file', 'failure', 'ci_job_url', 'ci_dashboard_url'],
            value_functions=[
                (
                    'Failures across all other branches (20 latest testcases)',
                    lambda item: f"{getattr(item, 'latest_failed_count', '')} / {getattr(item, 'latest_total_count', '')}",
                )
            ],
        )

        self.create_table_section(
            report_sections=report_parts,
            title='Known Failure Cases',
            items=known_failures,
            headers=['Test Case', 'Test Script File Path', 'Failure Reason', 'Job URL', 'Grafana URL'],
            row_attrs=['name', 'file', 'failure', 'ci_job_url', 'ci_dashboard_url'],
        )
        self.create_table_section(
            report_sections=report_parts,
            title='Skipped Test Cases',
            items=skipped_test_cases,
            headers=['Test Case', 'Test Script File Path', 'Skipped Reason', 'Grafana URL'],
            row_attrs=['name', 'file', 'skipped', 'ci_dashboard_url'],
        )
        self.create_table_section(
            report_sections=report_parts,
            title='Succeeded Test Cases',
            items=successful_test_cases,
            headers=['Test Case', 'Test Script File Path', 'Job URL', 'Grafana URL'],
            row_attrs=['name', 'file', 'ci_job_url', 'ci_dashboard_url'],
        )

        self.additional_info = (
            '**Test Case Summary:**\n'
            f'- **Failed Test Cases on Your Branch (Excludes Known Failure Cases):** {len(current_branch_failures)}.\n'
            f'- **Failed Test Cases on Other Branches (Excludes Known Failure Cases):** {len(other_branch_failures)}.\n'
            f'- **Known Failures:** {len(known_failures)}\n'
            f'- **Skipped Test Cases:** {len(skipped_test_cases)}\n'
            f'- **Succeeded Test Cases:** {len(successful_test_cases)}\n\n'
            'Please check report below for more information.\n\n'
        )

        return self.generate_html_report(''.join(report_parts))


class JobReportGenerator(ReportGenerator):
    def __init__(
        self,
        project_id: int,
        mr_iid: int,
        pipeline_id: int,
        *,
        title: str = 'Job Report',
        jobs: t.List[GitlabJob],
    ):
        super().__init__(project_id, mr_iid, pipeline_id, title=title)
        self.jobs = jobs

    def _get_report_str(self) -> str:
        """
        Generate a complete HTML report string by processing jobs.
        :return: Complete HTML report string.
        """
        report_str: str = ''
        report_parts: list = []

        if not self.jobs:
            print('No jobs found, skip generating job report')
            return 'No Job Found'

        relevant_failed_jobs = self._sort_items(
            self._filter_items(
                self.jobs, lambda job: job.is_failed and job.stage not in ['integration_test', 'target_test']
            ),
            key='latest_failed_count',
        )
        succeeded_jobs = self._filter_items(self.jobs, lambda job: job.is_success)

        self.additional_info = (
            '**Job Summary:**\n'
            f'- **Failed Jobs (Excludes "integration_test" and "target_test" jobs):** {len(relevant_failed_jobs)}\n'
            f'- **Succeeded Jobs:** {len(succeeded_jobs)}\n\n'
        )

        if relevant_failed_jobs:
            self.create_table_section(
                report_sections=report_parts,
                title='Failed Jobs (Excludes "integration_test" and "target_test" jobs)',
                items=relevant_failed_jobs,
                headers=[
                    'Job Name',
                    'Failure Reason',
                    'Failure Log',
                    'Failures across all other branches (10 latest jobs)',
                    'URL',
                    'CI Dashboard URL',
                ],
                row_attrs=['name', 'failure_reason', 'failure_log', 'url', 'ci_dashboard_url'],
                value_functions=[
                    (
                        'Failures across all other branches (10 latest jobs)',
                        lambda item: f"{getattr(item, 'latest_failed_count', '')} / {getattr(item, 'latest_total_count', '')}",
                    )
                ],
            )
            self.additional_info += f'Please check report below for more information.\n\n'
            report_str = self.generate_html_report(''.join(report_parts))

        return report_str
