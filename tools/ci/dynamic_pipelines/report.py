# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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
from gitlab import GitlabUpdateError
from gitlab_api import Gitlab
from idf_build_apps import App
from idf_build_apps.constants import BuildStatus
from idf_ci_local.app import AppWithMetricsInfo
from idf_ci_local.uploader import AppUploader
from prettytable import PrettyTable

from .constants import BINARY_SIZE_METRIC_NAME
from .constants import CI_DASHBOARD_API
from .constants import COMMENT_START_MARKER
from .constants import CSS_STYLES_FILEPATH
from .constants import JS_SCRIPTS_FILEPATH
from .constants import REPORT_TEMPLATE_FILEPATH
from .constants import RETRY_JOB_PICTURE_LINK
from .constants import RETRY_JOB_PICTURE_PATH
from .constants import RETRY_JOB_TITLE
from .constants import SIZE_DIFFERENCE_BYTES_THRESHOLD
from .constants import TEST_RELATED_APPS_DOWNLOAD_URLS_FILENAME
from .constants import TOP_N_APPS_BY_SIZE_DIFF
from .models import GitlabJob
from .models import TestCase
from .utils import fetch_failed_testcases_failure_ratio
from .utils import format_permalink
from .utils import get_artifacts_url
from .utils import get_repository_file_url
from .utils import is_url
from .utils import known_failure_issue_jira_fast_link
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

    @property
    def get_commit_summary(self) -> str:
        """Return a formatted commit summary string."""
        return (
            f'with CI commit SHA: {self.commit_id[:8]}, '
            f'local commit SHA: {os.getenv("CI_MERGE_REQUEST_SOURCE_BRANCH_SHA", "")[:8]}'
        )

    @staticmethod
    def get_download_link_for_url(url: str) -> str:
        if url:
            return f'<a href="{url}">Download</a>'

        return ''

    @staticmethod
    def write_report_to_file(report_str: str, job_id: int, output_filepath: str) -> t.Optional[str]:
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

    @staticmethod
    def _load_file_content(filepath: str) -> str:
        """
        Load the content of a file as string

        :param filepath: Path to the file to load
        :return: Content of the file as string
        """
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                return f.read()
        except (IOError, FileNotFoundError) as e:
            print(f'Warning: Could not read file {filepath}: {e}')
            return ''

    def generate_html_report(self, table_str: str) -> str:
        # we're using bootstrap table
        table_str = table_str.replace(
            '<table>',
            '<table data-toggle="table" data-search-align="left" data-search="true" data-sticky-header="true">',
        )

        template = self._load_file_content(REPORT_TEMPLATE_FILEPATH)
        css_content = self._load_file_content(CSS_STYLES_FILEPATH)
        js_content = self._load_file_content(JS_SCRIPTS_FILEPATH)

        template = template.replace('{{css_content}}', css_content)
        template = template.replace('{{js_content}}', js_content)
        template = template.replace('{{pipeline_id}}', str(self.pipeline_id))
        template = template.replace('{{apiBaseUrl}}', CI_DASHBOARD_API)

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
    def generate_additional_info_section(
        title: str, count: int, report_url: t.Optional[str] = None, add_permalink: bool = True
    ) -> str:
        """
        Generate a section for the additional info string.

        :param title: The title of the section.
        :param count: The count of test cases.
        :param report_url: The URL of the report. If count = 0, only the count will be included.
        :param add_permalink: Whether to include a permalink in the report URL. Defaults to True.
        :return: The formatted additional info section string.
        """
        if count != 0 and report_url:
            if add_permalink:
                return f'- **{title}:** [{count}]({report_url}/#{format_permalink(title)})\n'
            else:
                return f'- **{title}:** [{count}]({report_url})\n'
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
        items: t.List[t.Union[TestCase, GitlabJob, AppWithMetricsInfo]],
        key: t.Union[str, t.Callable[[t.Union[TestCase, GitlabJob, AppWithMetricsInfo]], t.Any]],
        order: str = 'asc',
        sort_function: t.Optional[t.Callable[[t.Any], t.Any]] = None,
    ) -> t.List[t.Union[TestCase, GitlabJob, AppWithMetricsInfo]]:
        """
        Sort items based on a given key, order, and optional custom sorting function.

        :param items: List of items to sort.
        :param key: A string representing the attribute name or a function to extract the sorting key.
        :param order: Order of sorting ('asc' for ascending, 'desc' for descending).
        :param sort_function: A custom function to control sorting logic
            (e.g., prioritizing positive/negative/zero values).
        :return: List of sorted instances.
        """
        key_func = None
        if isinstance(key, str):

            def key_func(item: t.Any) -> t.Any:
                return getattr(item, key)

        sorting_key = sort_function if sort_function is not None else key_func
        try:
            items = sorted(items, key=sorting_key, reverse=(order == 'desc'))
        except TypeError:
            print(f'Comparison for the key {key} is not supported')

        return items

    @abc.abstractmethod
    def _get_report_str(self) -> str:
        raise NotImplementedError

    def _generate_comment(self) -> str:
        # Report in HTML format to avoid exceeding length limits
        comment = f'#### {self.title}\n'
        report_str = self._get_report_str()
        comment += f'{self.additional_info}\n'
        self.write_report_to_file(report_str, self.job_id, self.output_filepath)

        return comment

    def _update_mr_comment(self, comment: str, print_retry_jobs_message: bool) -> None:
        retry_job_picture_comment = (f'{RETRY_JOB_TITLE}\n\n{RETRY_JOB_PICTURE_LINK}').format(
            pic_url=get_repository_file_url(RETRY_JOB_PICTURE_PATH)
        )
        del_retry_job_pic_pattern = re.escape(RETRY_JOB_TITLE) + r'.*?' + re.escape(f'{RETRY_JOB_PICTURE_PATH})')

        new_comment = f'{COMMENT_START_MARKER}\n\n{comment}'
        if print_retry_jobs_message:
            new_comment += retry_job_picture_comment

        for note in self.mr.notes.list(iterator=True):
            if note.body.startswith(COMMENT_START_MARKER):
                updated_str = self._get_updated_comment(note.body, comment)

                # Add retry job message only if any job has failed
                if print_retry_jobs_message:
                    updated_str = re.sub(del_retry_job_pic_pattern, '', updated_str, flags=re.DOTALL)
                    updated_str += retry_job_picture_comment

                note.body = updated_str
                try:
                    note.save()
                except GitlabUpdateError:
                    print('Failed to update MR comment, Creating a new comment')
                    self.mr.notes.create({'body': new_comment})
                break
        else:
            self.mr.notes.create({'body': new_comment})

    def _get_updated_comment(self, existing_comment: str, new_comment: str) -> str:
        updated_str = re.sub(self.REGEX_PATTERN.format(self.title), new_comment, existing_comment)
        if updated_str == existing_comment:
            updated_str = f'{existing_comment.strip()}\n\n{new_comment}'
        return updated_str

    def post_report(self, print_retry_jobs_message: bool = False) -> None:
        comment = self._generate_comment()

        print(comment)

        if self.mr is None:
            print('No MR found, skip posting comment')
            return

        self._update_mr_comment(comment, print_retry_jobs_message=print_retry_jobs_message)


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
        self._uploader = AppUploader(self.pipeline_id)
        self.apps_presigned_url_filepath = TEST_RELATED_APPS_DOWNLOAD_URLS_FILENAME
        self.report_titles_map = {
            'failed_apps': 'Failed Apps',
            'built_test_related_apps': 'Built Apps - Test Related',
            'built_non_test_related_apps': 'Built Apps - Non Test Related',
            'new_test_related_apps': 'New Apps - Test Related',
            'new_non_test_related_apps': 'New Apps - Non Test Related',
            'skipped_apps': 'Skipped Apps',
        }
        self.failed_apps_report_file = 'failed_apps.html'
        self.built_apps_report_file = 'built_apps.html'
        self.skipped_apps_report_file = 'skipped_apps.html'
        self.app_presigned_urls_dict: t.Dict[str, t.Dict[str, str]] = {}

    @staticmethod
    def custom_sort(item: AppWithMetricsInfo) -> t.Tuple[int, t.Any]:
        """
        Custom sort function to:
        1. Push items with zero binary sizes to the end.
        2. Sort other items by absolute size_difference_percentage.
        """
        # Priority: 0 for zero binaries, 1 for non-zero binaries
        zero_binary_priority = (
            1
            if item.metrics[BINARY_SIZE_METRIC_NAME].source_value != 0
            or item.metrics[BINARY_SIZE_METRIC_NAME].target_value != 0
            else 0
        )
        # Secondary sort: Negative absolute size_difference_percentage for descending order
        size_difference_sort = abs(item.metrics[BINARY_SIZE_METRIC_NAME].difference_percentage)
        return zero_binary_priority, size_difference_sort

    def _generate_top_n_apps_by_size_table(self) -> str:
        """
        Generate a markdown table for the top N apps by size difference.
        Only includes apps with size differences greater than 500 bytes.
        """
        filtered_apps = [
            app
            for app in self.apps
            if abs(app.metrics[BINARY_SIZE_METRIC_NAME].difference) > SIZE_DIFFERENCE_BYTES_THRESHOLD
        ]

        top_apps = sorted(
            filtered_apps, key=lambda app: abs(app.metrics[BINARY_SIZE_METRIC_NAME].difference_percentage), reverse=True
        )[:TOP_N_APPS_BY_SIZE_DIFF]

        if not top_apps:
            return ''

        table = (
            f'\n⚠️⚠️⚠️ Top {len(top_apps)} Apps with Binary Size Sorted by Size Difference\n'
            f'Note: Apps with changes of less than {SIZE_DIFFERENCE_BYTES_THRESHOLD} bytes are not shown.\n'
        )
        table += '| App Dir | Build Dir | Size Diff (bytes) | Size Diff (%) |\n'
        table += '|---------|-----------|-------------------|---------------|\n'
        for app in top_apps:
            table += dedent(
                f'| {app.app_dir} | {app.build_dir} | '
                f'{app.metrics[BINARY_SIZE_METRIC_NAME].difference} | '
                f'{app.metrics[BINARY_SIZE_METRIC_NAME].difference_percentage}% |\n'
            )
        table += (
            '\n**For more details, please click on the numbers in the summary above '
            'to view the corresponding report files.** ⬆️⬆️⬆️\n\n'
        )

        return table

    @staticmethod
    def split_new_and_existing_apps(
        apps: t.Iterable[AppWithMetricsInfo],
    ) -> t.Tuple[t.List[AppWithMetricsInfo], t.List[AppWithMetricsInfo]]:
        """
        Splits apps into new apps and existing apps.

        :param apps: Iterable of apps to process.
        :return: A tuple (new_apps, existing_apps).
        """
        new_apps = [app for app in apps if app.is_new_app]
        existing_apps = [app for app in apps if not app.is_new_app]
        return new_apps, existing_apps

    def filter_apps_by_criteria(self, build_status: str, preserve: bool) -> t.List[AppWithMetricsInfo]:
        """
        Filters apps based on build status and preserve criteria.

        :param build_status: Build status to filter by.
        :param preserve: Whether to filter preserved apps.
        :return: Filtered list of apps.
        """
        return [app for app in self.apps if app.build_status == build_status and app.preserve == preserve]

    def get_built_apps_report_parts(self) -> t.List[str]:
        """
        Generates report parts for new and existing apps.

        :return: List of report parts.
        """
        new_test_related_apps, built_test_related_apps = self.split_new_and_existing_apps(
            self.filter_apps_by_criteria(BuildStatus.SUCCESS, True)
        )

        new_non_test_related_apps, built_non_test_related_apps = self.split_new_and_existing_apps(
            self.filter_apps_by_criteria(BuildStatus.SUCCESS, False)
        )

        sections = []

        if new_test_related_apps:
            for app in new_test_related_apps:
                for artifact_type in [ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES, ArtifactType.MAP_AND_ELF_FILES]:
                    url = self._uploader.get_app_presigned_url(app, artifact_type)
                    self.app_presigned_urls_dict.setdefault(app.build_path, {})[artifact_type.value] = url

            new_test_related_apps_table_section = self.create_table_section(
                title=self.report_titles_map['new_test_related_apps'],
                items=new_test_related_apps,
                headers=[
                    'App Dir',
                    'Build Dir',
                    'Bin Files with Build Log (without map and elf)',
                    'Map and Elf Files',
                    'Your Branch App Size',
                ],
                row_attrs=[
                    'app_dir',
                    'build_dir',
                ],
                value_functions=[
                    ('Your Branch App Size', lambda app: str(app.metrics[BINARY_SIZE_METRIC_NAME].source_value)),
                    (
                        'Bin Files with Build Log (without map and elf)',
                        lambda app: self.get_download_link_for_url(
                            self.app_presigned_urls_dict[app.build_path][
                                ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES.value
                            ]
                        ),
                    ),
                    (
                        'Map and Elf Files',
                        lambda app: self.get_download_link_for_url(
                            self.app_presigned_urls_dict[app.build_path][ArtifactType.MAP_AND_ELF_FILES.value]
                        ),
                    ),
                ],
            )
            sections.extend(new_test_related_apps_table_section)

        if built_test_related_apps:
            for app in built_test_related_apps:
                for artifact_type in [ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES, ArtifactType.MAP_AND_ELF_FILES]:
                    url = self._uploader.get_app_presigned_url(app, artifact_type)
                    self.app_presigned_urls_dict.setdefault(app.build_path, {})[artifact_type.value] = url

            built_test_related_apps = self._sort_items(
                built_test_related_apps,
                key='metrics.binary_size.difference_percentage',
                order='desc',
                sort_function=self.custom_sort,
            )

            built_test_related_apps_table_section = self.create_table_section(
                title=self.report_titles_map['built_test_related_apps'],
                items=built_test_related_apps,
                headers=[
                    'App Dir',
                    'Build Dir',
                    'Bin Files with Build Log (without map and elf)',
                    'Map and Elf Files',
                    'Your Branch App Size',
                    'Target Branch App Size',
                    'Size Diff',
                    'Size Diff, %',
                ],
                row_attrs=[
                    'app_dir',
                    'build_dir',
                ],
                value_functions=[
                    ('Your Branch App Size', lambda app: str(app.metrics[BINARY_SIZE_METRIC_NAME].source_value)),
                    ('Target Branch App Size', lambda app: str(app.metrics[BINARY_SIZE_METRIC_NAME].target_value)),
                    ('Size Diff', lambda app: str(app.metrics[BINARY_SIZE_METRIC_NAME].difference)),
                    ('Size Diff, %', lambda app: str(app.metrics[BINARY_SIZE_METRIC_NAME].difference_percentage)),
                    (
                        'Bin Files with Build Log (without map and elf)',
                        lambda app: self.get_download_link_for_url(
                            self.app_presigned_urls_dict[app.build_path][
                                ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES.value
                            ]
                        ),
                    ),
                    (
                        'Map and Elf Files',
                        lambda app: self.get_download_link_for_url(
                            self.app_presigned_urls_dict[app.build_path][ArtifactType.MAP_AND_ELF_FILES.value]
                        ),
                    ),
                ],
            )
            sections.extend(built_test_related_apps_table_section)

        if new_non_test_related_apps:
            new_non_test_related_apps_table_section = self.create_table_section(
                title=self.report_titles_map['new_non_test_related_apps'],
                items=new_non_test_related_apps,
                headers=[
                    'App Dir',
                    'Build Dir',
                    'Build Log',
                    'Your Branch App Size',
                ],
                row_attrs=[
                    'app_dir',
                    'build_dir',
                ],
                value_functions=[
                    ('Your Branch App Size', lambda app: str(app.metrics[BINARY_SIZE_METRIC_NAME].source_value)),
                    (
                        'Build Log',
                        lambda app: self.get_download_link_for_url(
                            self._uploader.get_app_presigned_url(app, ArtifactType.LOGS)
                        ),
                    ),
                ],
            )
            sections.extend(new_non_test_related_apps_table_section)

        if built_non_test_related_apps:
            built_non_test_related_apps = self._sort_items(
                built_non_test_related_apps,
                key='metrics.binary_size.difference_percentage',
                order='desc',
                sort_function=self.custom_sort,
            )
            built_non_test_related_apps_table_section = self.create_table_section(
                title=self.report_titles_map['built_non_test_related_apps'],
                items=built_non_test_related_apps,
                headers=[
                    'App Dir',
                    'Build Dir',
                    'Build Log',
                    'Your Branch App Size',
                    'Target Branch App Size',
                    'Size Diff',
                    'Size Diff, %',
                ],
                row_attrs=[
                    'app_dir',
                    'build_dir',
                ],
                value_functions=[
                    ('Your Branch App Size', lambda app: str(app.metrics[BINARY_SIZE_METRIC_NAME].source_value)),
                    ('Target Branch App Size', lambda app: str(app.metrics[BINARY_SIZE_METRIC_NAME].target_value)),
                    ('Size Diff', lambda app: str(app.metrics[BINARY_SIZE_METRIC_NAME].difference)),
                    ('Size Diff, %', lambda app: str(app.metrics[BINARY_SIZE_METRIC_NAME].difference_percentage)),
                    (
                        'Build Log',
                        lambda app: self.get_download_link_for_url(
                            self._uploader.get_app_presigned_url(app, ArtifactType.LOGS)
                        ),
                    ),
                ],
            )
            sections.extend(built_non_test_related_apps_table_section)

        built_apps_report_url = self.write_report_to_file(
            self.generate_html_report(''.join(sections)),
            self.job_id,
            self.built_apps_report_file,
        )

        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['built_test_related_apps'],
            len(built_test_related_apps),
            built_apps_report_url,
        )
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['built_non_test_related_apps'],
            len(built_non_test_related_apps),
            built_apps_report_url,
        )
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['new_test_related_apps'],
            len(new_test_related_apps),
            built_apps_report_url,
        )
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['new_non_test_related_apps'],
            len(new_non_test_related_apps),
            built_apps_report_url,
        )

        self.additional_info += self._generate_top_n_apps_by_size_table()

        # also generate a yaml file that includes the apps and the presigned urls
        # for helping debugging locally
        with open(self.apps_presigned_url_filepath, 'w') as fw:
            yaml.dump(self.app_presigned_urls_dict, fw)

        return sections

    def get_failed_apps_report_parts(self) -> t.List[str]:
        failed_apps = [app for app in self.apps if app.build_status == BuildStatus.FAILED]
        if not failed_apps:
            return []

        failed_apps_table_section = self.create_table_section(
            title=self.report_titles_map['failed_apps'],
            items=failed_apps,
            headers=['App Dir', 'Build Dir', 'Failed Reason', 'Build Log'],
            row_attrs=['app_dir', 'build_dir', 'build_comment'],
            value_functions=[
                (
                    'Build Log',
                    lambda app: self.get_download_link_for_url(
                        self._uploader.get_app_presigned_url(app, ArtifactType.LOGS)
                    ),
                ),
            ],
        )
        failed_apps_report_url = self.write_report_to_file(
            self.generate_html_report(''.join(failed_apps_table_section)),
            self.job_id,
            self.failed_apps_report_file,
        )
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['failed_apps'], len(failed_apps), failed_apps_report_url
        )
        return failed_apps_table_section

    def get_skipped_apps_report_parts(self) -> t.List[str]:
        skipped_apps = [app for app in self.apps if app.build_status == BuildStatus.SKIPPED]
        if not skipped_apps:
            return []

        skipped_apps_table_section = self.create_table_section(
            title=self.report_titles_map['skipped_apps'],
            items=skipped_apps,
            headers=['App Dir', 'Build Dir', 'Skipped Reason', 'Build Log'],
            row_attrs=['app_dir', 'build_dir', 'build_comment'],
            value_functions=[
                (
                    'Build Log',
                    lambda app: self.get_download_link_for_url(
                        self._uploader.get_app_presigned_url(app, ArtifactType.LOGS)
                    ),
                ),
            ],
        )
        skipped_apps_report_url = self.write_report_to_file(
            self.generate_html_report(''.join(skipped_apps_table_section)),
            self.job_id,
            self.skipped_apps_report_file,
        )
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['skipped_apps'], len(skipped_apps), skipped_apps_report_url
        )
        return skipped_apps_table_section

    def _get_report_str(self) -> str:
        self.additional_info = (
            f'**Build Summary ({self.get_commit_summary}):**\n'
            '\n'
            '> ℹ️ Note: Binary artifacts stored in MinIO are retained for 4 DAYS from their build date\n'
        )
        failed_apps_report_parts = self.get_failed_apps_report_parts()
        skipped_apps_report_parts = self.get_skipped_apps_report_parts()
        built_apps_report_parts = self.get_built_apps_report_parts()

        return self.generate_html_report(
            ''.join(failed_apps_report_parts + built_apps_report_parts + skipped_apps_report_parts)
        )


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
            'failed_yours': 'Testcases failed ONLY on your branch (known failures are excluded)',
            'failed_others': 'Testcases failed on your branch as well as on others (known failures are excluded)',
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
                'Test App Path',
                'Failure Reason',
                'These test cases failed exclusively on your branch in the latest 40 runs',
                'Dut Log URL',
                'Create Known Failure Case Jira',
                'Job URL',
                'Grafana URL',
            ],
            row_attrs=['name', 'app_path', 'failure', 'dut_log_url', 'ci_job_url', 'ci_dashboard_url'],
            value_functions=[
                (
                    'These test cases failed exclusively on your branch in the latest 40 runs',
                    lambda item: f'{getattr(item, "latest_failed_count", "")} / '
                    f'{getattr(item, "latest_total_count", "")}',
                ),
                ('Create Known Failure Case Jira', known_failure_issue_jira_fast_link),
            ],
        )
        other_branch_cases_table_section = self.create_table_section(
            title=self.report_titles_map['failed_others'],
            items=failed_test_cases_other_branch,
            headers=[
                'Test Case',
                'Test App Path',
                'Failure Reason',
                'Cases that failed in other branches as well (40 latest testcases)',
                'Dut Log URL',
                'Create Known Failure Case Jira',
                'Job URL',
                'Grafana URL',
            ],
            row_attrs=['name', 'app_path', 'failure', 'dut_log_url', 'ci_job_url', 'ci_dashboard_url'],
            value_functions=[
                (
                    'Cases that failed in other branches as well (40 latest testcases)',
                    lambda item: f'{getattr(item, "latest_failed_count", "")} '
                    f'/ {getattr(item, "latest_total_count", "")}',
                ),
                ('Create Known Failure Case Jira', known_failure_issue_jira_fast_link),
            ],
        )
        known_failures_cases_table_section = self.create_table_section(
            title=self.report_titles_map['failed_known'],
            items=known_failures,
            headers=['Test Case', 'Test App Path', 'Failure Reason', 'Job URL', 'Grafana URL'],
            row_attrs=['name', 'app_path', 'failure', 'ci_job_url', 'ci_dashboard_url'],
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
            headers=['Test Case', 'Test App Path', 'Skipped Reason', 'Grafana URL'],
            row_attrs=['name', 'app_path', 'skipped', 'ci_dashboard_url'],
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
            headers=['Test Case', 'Test App Path', 'Job URL', 'Grafana URL'],
            row_attrs=['name', 'app_path', 'ci_job_url', 'ci_dashboard_url'],
        )
        succeeded_cases_report_url = self.write_report_to_file(
            self.generate_html_report(''.join(succeeded_cases_table_section)),
            self.job_id,
            self.succeeded_cases_report_file,
        )
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['succeeded'],
            len(succeeded_test_cases),
            succeeded_cases_report_url,
            add_permalink=False,
        )
        self.additional_info += '\n'
        return succeeded_cases_table_section

    def _get_report_str(self) -> str:
        """
        Generate a complete HTML report string by processing test cases.
        :return: Complete HTML report string.
        """
        self.additional_info = f'**Test Case Summary ({self.get_commit_summary}):**\n'
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

        self.additional_info = f'**Job Summary ({self.get_commit_summary}):**\n'
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
                    lambda item: f'{getattr(item, "latest_failed_count", "")} '
                    f'/ {getattr(item, "latest_total_count", "")}',
                )
            ],
        )
        relevant_failed_jobs_report_url = get_artifacts_url(self.job_id, self.failed_jobs_report_file)
        self.additional_info += self.generate_additional_info_section(
            self.report_titles_map['failed_jobs'], len(relevant_failed_jobs), relevant_failed_jobs_report_url
        )

        report_str = self.generate_html_report(''.join(report_sections))
        return report_str
