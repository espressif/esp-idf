# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import abc
import copy
import fnmatch
import html
import os
import re
import typing as t
from textwrap import dedent

import yaml
from artifacts_handler import ArtifactType
from gitlab_api import Gitlab
from idf_build_apps import App
from idf_build_apps.constants import BuildStatus
from idf_ci.uploader import AppUploader
from prettytable import PrettyTable

from .constants import COMMENT_START_MARKER
from .constants import REPORT_TEMPLATE_FILEPATH
from .constants import RETRY_JOB_PICTURE_LINK
from .constants import RETRY_JOB_PICTURE_PATH
from .constants import RETRY_JOB_TITLE
from .constants import TEST_RELATED_APPS_DOWNLOAD_URLS_FILENAME
from .models import GitlabJob
from .models import TestCase
from .utils import fetch_failed_testcases_failure_ratio
from .utils import format_permalink
from .utils import get_artifacts_url
from .utils import get_repository_file_url
from .utils import is_url
from .utils import load_known_failure_cases


class ReportGenerator:
    REGEX_PATTERN = r'#### {}\n[\s\S]*?(?=\n#### |$)'

    def __init__(self, project_id: int, mr_iid: int, pipeline_id: int, job_id: int, commit_id: str, *, title: str):
        gl_project = Gitlab(project_id).project
        if mr_iid is not None:
            self.mr = gl_project.mergerequests.get(mr_iid)
        else:
            self.mr = None
        self.pipeline_id = pipeline_id
        self.job_id = job_id
        self.commit_id = commit_id

        self.title = title
        self.output_filepath = self.title.lower().replace(' ', '_') + '.html'
        self.additional_info = ''

    @staticmethod
    def get_download_link_for_url(url: str) -> str:
        if url:
            return f'<a href="{url}">Download</a>'

        return ''

    def write_report_to_file(self, report_str: str, job_id: int, output_filepath: str) -> t.Optional[str]:
        """
        Writes the report to a file and constructs a modified URL based on environment settings.

        :param report_str: The report content to be written to the file.
        :param job_id: The job identifier used to construct the URL.
        :param output_filepath: The path to the output file.
        :return: The modified URL pointing to the job's artifacts.
        """
        if not report_str:
            return None
        with open(output_filepath, 'w') as file:
            file.write(report_str)

        # for example, {URL}/-/esp-idf/-/jobs/{id}/artifacts/list_job_84.txt
        # CI_PAGES_URL is {URL}/esp-idf, which missed one `-`
        report_url: str = get_artifacts_url(job_id, output_filepath)
        return report_url

    def generate_html_report(self, table_str: str) -> str:
        # we're using bootstrap table
        table_str = table_str.replace(
            '<table>',
            '<table data-toggle="table" data-search-align="left" data-search="true" data-sticky-header="true">',
        )
        with open(REPORT_TEMPLATE_FILEPATH) as fr:
            template = fr.read()

        return template.replace('{{title}}', self.title).replace('{{table}}', table_str)

    @staticmethod
    def table_to_html_str(table: PrettyTable) -> str:
        return html.unescape(table.get_html_string())  # type: ignore

    def create_table_section(
        self,
        title: str,
        items: list,
        headers: list,
        row_attrs: list,
        value_functions: t.Optional[list] = None,
    ) -> t.List:
        """
        Appends a formatted section to a report based on the provided items. This section includes
        a header and a table constructed from the items list with specified headers and attributes.

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

        :return: List with appended HTML sections.
        """
        if not items:
            return []

        report_sections = [
            f"""<h2 id="{format_permalink(title)}">{title}<i class="fas fa-link copy-link-icon"
        onclick="copyPermalink('#{format_permalink(title)}')"></i></h2>""",
            self._create_table_for_items(
                items=items, headers=headers, row_attrs=row_attrs, value_functions=value_functions or []
            ),
        ]
        return report_sections

    @staticmethod
    def generate_additional_info_section(title: str, count: int, report_url: t.Optional[str] = None) -> str:
        """
        Generate a section for the additional info string.

        :param title: The title of the section.
        :param count: The count of test cases.
        :param report_url: The URL of the report. If count = 0, only the count will be included.
        :return: The formatted additional info section string.
        """
        if count != 0 and report_url:
            return f'- **{title}:** [{count}]({report_url}/#{format_permalink(title)})\n'
        else:
            return f'- **{title}:** {count}\n'

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

    def post_report(self, print_report_path: bool = True) -> None:
        # report in html format, otherwise will exceed the limit
        comment = f'#### {self.title}\n'

        report_str = self._get_report_str()

        if self.additional_info:
            comment += f'{self.additional_info}\n'

        report_url_path = self.write_report_to_file(report_str, self.job_id, self.output_filepath)
        if print_report_path and report_url_path:
            comment += dedent(f"""
            Full {self.title} here: {report_url_path} (with commit {self.commit_id[:8]}

            """)
        print(comment)

        if self.mr is None:
            print('No MR found, skip posting comment')
            return
        retry_job_picture_comment = (f'{RETRY_JOB_TITLE}\n\n'
                                     f'{RETRY_JOB_PICTURE_LINK}').format(pic_url=get_repository_file_url(RETRY_JOB_PICTURE_PATH))
        del_retry_job_pic_pattern = re.escape(RETRY_JOB_TITLE) + r'.*?' + re.escape(f'{RETRY_JOB_PICTURE_PATH})')
        for note in self.mr.notes.list(iterator=True):
            if note.body.startswith(COMMENT_START_MARKER):
                updated_str = re.sub(self.REGEX_PATTERN.format(self.title), comment, note.body)
                if updated_str == note.body:  # not updated
                    updated_str = f'{note.body.strip()}\n\n{comment}'

                updated_str = re.sub(del_retry_job_pic_pattern, '', updated_str, flags=re.DOTALL)
                note.body = updated_str + retry_job_picture_comment
                note.save()
                break
        else:
            new_comment = f"""{COMMENT_START_MARKER}

{comment}{retry_job_picture_comment}"""
            self.mr.notes.create({'body': new_comment})


class BuildReportGenerator(ReportGenerator):
    def __init__(
        self,
        project_id: int,
        mr_iid: int,
        pipeline_id: int,
        job_id: int,
        commit_id: str,
        *,
        title: str = 'Build Report',
        apps: t.List[App],
    ):
        super().__init__(project_id, mr_iid, pipeline_id, job_id, commit_id, title=title)
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
        job_id: int,
        commit_id: str,
        *,
        title: str = 'Target Test Report',
        test_cases: t.List[TestCase],
    ):
        super().__init__(project_id, mr_iid, pipeline_id, job_id, commit_id, title=title)

        self.test_cases = test_cases
        self._known_failure_cases_set = None
        self.report_titles_map = {
            'failed_yours': 'Failed Test Cases on Your branch (Excludes Known Failure Cases)',
            'failed_others': 'Failed Test Cases on Other branches (Excludes Known Failure Cases)',
            'failed_known': 'Known Failure Cases',
            'skipped': 'Skipped Test Cases',
            'succeeded': 'Succeeded Test Cases',
        }
        self.skipped_test_cases_report_file = 'skipped_cases.html'
        self.succeeded_cases_report_file = 'succeeded_cases.html'
        self.failed_cases_report_file = 'failed_cases.html'

    @property
    def known_failure_cases_set(self) -> t.Optional[t.Set[str]]:
        if self._known_failure_cases_set is None:
            self._known_failure_cases_set = load_known_failure_cases()

        return self._known_failure_cases_set

    def get_known_failure_cases(self) -> t.List[TestCase]:
        """
        Retrieve the known failure test cases.
        :return: A list of known failure test cases.
        """
        if self.known_failure_cases_set is None:
            return []
        matched_cases = [
            testcase
            for testcase in self.test_cases
            if any(fnmatch.fnmatch(testcase.name, pattern) for pattern in self.known_failure_cases_set)
            and testcase.is_failure
        ]
        return matched_cases

    @staticmethod
    def filter_test_cases(
        cur_branch_failures: t.List[TestCase],
        other_branch_failures: t.List[TestCase],
    ) -> t.Tuple[t.List[TestCase], t.List[TestCase]]:
        """
        Filter the test cases into current branch failures and other branch failures.

        :param cur_branch_failures: List of failed test cases on the current branch.
        :param other_branch_failures: List of failed test cases on other branches.
        :return: A tuple containing two lists:
            - failed_test_cases_cur_branch_only: Test cases that have failed only on the current branch.
            - failed_test_cases_other_branch_exclude_cur_branch: Test cases that have failed on other branches
        excluding the current branch.
        """
        cur_branch_unique_failures = []
        other_branch_failure_map = {tc.name: tc for tc in other_branch_failures}

        for cur_tc in cur_branch_failures:
            if cur_tc.latest_failed_count > 0 and (
                cur_tc.name not in other_branch_failure_map
                or other_branch_failure_map[cur_tc.name].latest_failed_count == 0
            ):
                cur_branch_unique_failures.append(cur_tc)
        uniq_fail_names = {cur_tc.name for cur_tc in cur_branch_unique_failures}
        other_branch_exclusive_failures = [tc for tc in other_branch_failures if tc.name not in uniq_fail_names]

        return cur_branch_unique_failures, other_branch_exclusive_failures

    def get_failed_cases_report_parts(self) -> t.List[str]:
        """
        Generate the report parts for failed test cases and update the additional info section.
        :return: A list of strings representing the table sections for the failed test cases.
        """
        known_failures = self.get_known_failure_cases()
        failed_test_cases = self._filter_items(
            self.test_cases, lambda tc: tc.is_failure and tc.name not in {case.name for case in known_failures}
        )
        failed_test_cases_cur_branch = self._sort_items(
            fetch_failed_testcases_failure_ratio(
                copy.deepcopy(failed_test_cases),
                branches_filter={'include_branches': [os.getenv('CI_MERGE_REQUEST_SOURCE_BRANCH_NAME', '')]},
            ),
            key='latest_failed_count',
        )
        failed_test_cases_other_branch = self._sort_items(
            fetch_failed_testcases_failure_ratio(
                copy.deepcopy(failed_test_cases),
                branches_filter={'exclude_branches': [os.getenv('CI_MERGE_REQUEST_SOURCE_BRANCH_NAME', '')]},
            ),
            key='latest_failed_count',
        )
        failed_test_cases_cur_branch, failed_test_cases_other_branch = self.filter_test_cases(
            failed_test_cases_cur_branch, failed_test_cases_other_branch
        )
        cur_branch_cases_table_section = self.create_table_section(
            title=self.report_titles_map['failed_yours'],
            items=failed_test_cases_cur_branch,
            headers=[
                'Test Case',
                'Test Script File Path',
                'Failure Reason',
                f'Failures on your branch (40 latest testcases)',
                'Dut Log URL',
                'Job URL',
                'Grafana URL',
            ],
            row_attrs=['name', 'file', 'failure', 'dut_log_url', 'ci_job_url', 'ci_dashboard_url'],
            value_functions=[
                (
                    'Failures on your branch (40 latest testcases)',
                    lambda item: f"{getattr(item, 'latest_failed_count', '')} / {getattr(item, 'latest_total_count', '')}",
                )
            ],
        )
        other_branch_cases_table_section = self.create_table_section(
            title=self.report_titles_map['failed_others'],
            items=failed_test_cases_other_branch,
            headers=[
                'Test Case',
                'Test Script File Path',
                'Failure Reason',
                'Cases that failed in other branches as well (40 latest testcases)',
                'Dut Log URL',
                'Job URL',
                'Grafana URL',
            ],
            row_attrs=['name', 'file', 'failure', 'dut_log_url', 'ci_job_url', 'ci_dashboard_url'],
            value_functions=[
                (
                    'Cases that failed in other branches as well (40 latest testcases)',
                    lambda item: f"{getattr(item, 'latest_failed_count', '')} / {getattr(item, 'latest_total_count', '')}",
                )
            ],
        )
        known_failures_cases_table_section = self.create_table_section(
            title=self.report_titles_map['failed_known'],
            items=known_failures,
            headers=['Test Case', 'Test Script File Path', 'Failure Reason', 'Job URL', 'Grafana URL'],
            row_attrs=['name', 'file', 'failure', 'ci_job_url', 'ci_dashboard_url'],
        )
        failed_cases_report_url = self.write_report_to_file(
            self.generate_html_report(
                ''.join(
                    cur_branch_cases_table_section
                    + other_branch_cases_table_section
                    + known_failures_cases_table_section
                )
            ),
            self.job_id,
            self.failed_cases_report_file,
        )
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['failed_yours'], len(failed_test_cases_cur_branch), failed_cases_report_url
        )
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['failed_others'], len(failed_test_cases_other_branch), failed_cases_report_url
        )
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['failed_known'], len(known_failures), failed_cases_report_url
        )
        return cur_branch_cases_table_section + other_branch_cases_table_section + known_failures_cases_table_section

    def get_skipped_cases_report_parts(self) -> t.List[str]:
        """
        Generate the report parts for skipped test cases and update the additional info section.
        :return: A list of strings representing the table sections for the skipped test cases.
        """
        skipped_test_cases = self._filter_items(self.test_cases, lambda tc: tc.is_skipped)
        skipped_cases_table_section = self.create_table_section(
            title=self.report_titles_map['skipped'],
            items=skipped_test_cases,
            headers=['Test Case', 'Test Script File Path', 'Skipped Reason', 'Grafana URL'],
            row_attrs=['name', 'file', 'skipped', 'ci_dashboard_url'],
        )
        skipped_cases_report_url = self.write_report_to_file(
            self.generate_html_report(''.join(skipped_cases_table_section)),
            self.job_id,
            self.skipped_test_cases_report_file,
        )
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['skipped'], len(skipped_test_cases), skipped_cases_report_url
        )
        return skipped_cases_table_section

    def get_succeeded_cases_report_parts(self) -> t.List[str]:
        """
        Generate the report parts for succeeded test cases and update the additional info section.
        :return: A list of strings representing the table sections for the succeeded test cases.
        """
        succeeded_test_cases = self._filter_items(self.test_cases, lambda tc: tc.is_success)
        succeeded_cases_table_section = self.create_table_section(
            title=self.report_titles_map['succeeded'],
            items=succeeded_test_cases,
            headers=['Test Case', 'Test Script File Path', 'Job URL', 'Grafana URL'],
            row_attrs=['name', 'file', 'ci_job_url', 'ci_dashboard_url'],
        )
        succeeded_cases_report_url = self.write_report_to_file(
            self.generate_html_report(''.join(succeeded_cases_table_section)),
            self.job_id,
            self.succeeded_cases_report_file,
        )
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['succeeded'], len(succeeded_test_cases), succeeded_cases_report_url
        )
        self.additional_info += '\n'
        return succeeded_cases_table_section

    def _get_report_str(self) -> str:
        """
        Generate a complete HTML report string by processing test cases.
        :return: Complete HTML report string.
        """
        self.additional_info = f'**Test Case Summary (with commit {self.commit_id[:8]}):**\n'
        failed_cases_report_parts = self.get_failed_cases_report_parts()
        skipped_cases_report_parts = self.get_skipped_cases_report_parts()
        succeeded_cases_report_parts = self.get_succeeded_cases_report_parts()

        return self.generate_html_report(
            ''.join(failed_cases_report_parts + skipped_cases_report_parts + succeeded_cases_report_parts)
        )


class JobReportGenerator(ReportGenerator):
    def __init__(
        self,
        project_id: int,
        mr_iid: int,
        pipeline_id: int,
        job_id: int,
        commit_id: str,
        *,
        title: str = 'Job Report',
        jobs: t.List[GitlabJob],
    ):
        super().__init__(project_id, mr_iid, pipeline_id, job_id, commit_id, title=title)
        self.jobs = jobs
        self.report_titles_map = {
            'failed_jobs': 'Failed Jobs (Excludes "integration_test" and "target_test" jobs)',
            'succeeded': 'Succeeded Jobs',
        }
        self.failed_jobs_report_file = 'job_report.html'

    def _get_report_str(self) -> str:
        """
        Generate a complete HTML report string by processing jobs.
        :return: Complete HTML report string.
        """
        report_str: str = ''

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

        self.additional_info = f'**Job Summary (with commit {self.commit_id[:8]}):**\n'
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['succeeded'], len(succeeded_jobs)
        )

        if not relevant_failed_jobs:
            self.additional_info += self.generate_additional_info_section(
                self.report_titles_map['failed_jobs'], len(relevant_failed_jobs)
            )
            return report_str

        report_sections = self.create_table_section(
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
        relevant_failed_jobs_report_url = get_artifacts_url(self.job_id, self.failed_jobs_report_file)
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['failed_jobs'], len(relevant_failed_jobs), relevant_failed_jobs_report_url
        )

        report_str = self.generate_html_report(''.join(report_sections))
        return report_str
