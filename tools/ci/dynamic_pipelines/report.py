# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import abc
import html
import re
import typing as t
from textwrap import dedent

from gitlab import GitlabUpdateError
from gitlab_api import Gitlab
from idf_build_apps.constants import BuildStatus
from idf_ci_local.app import AppWithMetricsInfo
from idf_ci_utils import idf_relpath
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
from .constants import TOP_N_APPS_BY_SIZE_DIFF
from .models import GitlabJob
from .models import TestCase
from .utils import format_permalink
from .utils import get_artifacts_url
from .utils import get_repository_file_url
from .utils import is_url


class ReportGenerator:
    REGEX_PATTERN = r'#### {}\n[\s\S]*?(?=\n#### |$)'

    def __init__(
        self,
        project_id: int,
        mr_iid: int,
        pipeline_id: int,
        job_id: int,
        commit_id: str,
        local_commit_id: str,
        *,
        title: str,
    ):
        gl_project = Gitlab(project_id).project
        if mr_iid is not None:
            self.mr = gl_project.mergerequests.get(mr_iid)
        else:
            self.mr = None
        self.pipeline_id = pipeline_id
        self.job_id = job_id
        self.commit_id = commit_id
        self.local_commit_id = local_commit_id

        self.title = title
        self.output_filepath = self.title.lower().replace(' ', '_') + '.html'
        self.additional_info = ''

    @property
    def get_commit_summary(self) -> str:
        return f'with CI commit SHA: {self.commit_id[:8]}, local commit SHA: {self.local_commit_id[:8]}'

    @staticmethod
    def get_download_link_for_url(url: str) -> str:
        if url:
            return f'<a href="{url}">Download</a>'

        return ''

    @staticmethod
    def write_report_to_file(report_str: str, job_id: int, output_filepath: str) -> str | None:
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

        # for example, {URL}/-/esp-idf/-/jobs/{id}/artifacts/app_info_84.txt
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
            with open(filepath, encoding='utf-8') as f:
                return f.read()
        except (OSError, FileNotFoundError) as e:
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
        value_functions: list | None = None,
    ) -> list:
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
        title: str, count: int, report_url: str | None = None, add_permalink: bool = True
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
        items: list[TestCase] | list[GitlabJob],
        headers: list[str],
        row_attrs: list[str],
        value_functions: list[tuple[str, t.Callable[[TestCase | GitlabJob], str]]] | None = None,
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
        items: list[TestCase] | list[GitlabJob], condition: t.Callable[[TestCase | GitlabJob], bool]
    ) -> list[TestCase]:
        """
        Filter items s based on a given condition.

        :param items: List of items to filter by given condition.
        :param condition: A function that evaluates to True or False for each items.
        :return: List of filtered instances.
        """
        return [item for item in items if condition(item)]

    @staticmethod
    def _sort_items(
        items: list[TestCase | GitlabJob | AppWithMetricsInfo],
        key: str | t.Callable[[TestCase | GitlabJob | AppWithMetricsInfo], t.Any],
        order: str = 'asc',
        sort_function: t.Callable[[t.Any], t.Any] | None = None,
    ) -> list[TestCase | GitlabJob | AppWithMetricsInfo]:
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
        local_commit_id: str,
        *,
        title: str = 'Build Report',
        apps: list[AppWithMetricsInfo],
    ) -> None:
        super().__init__(project_id, mr_iid, pipeline_id, job_id, commit_id, local_commit_id, title=title)
        self.apps = apps
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

    @staticmethod
    def custom_sort(item: AppWithMetricsInfo) -> tuple[int, t.Any]:
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
    ) -> tuple[list[AppWithMetricsInfo], list[AppWithMetricsInfo]]:
        """
        Splits apps into new apps and existing apps.

        :param apps: Iterable of apps to process.
        :return: A tuple (new_apps, existing_apps).
        """
        new_apps = [app for app in apps if app.is_new_app]
        existing_apps = [app for app in apps if not app.is_new_app]
        return new_apps, existing_apps

    def filter_apps_by_criteria(self, build_status: str, preserve: bool) -> list[AppWithMetricsInfo]:
        """
        Filters apps based on build status and preserve criteria.

        :param build_status: Build status to filter by.
        :param preserve: Whether to filter preserved apps.
        :return: Filtered list of apps.
        """
        return [app for app in self.apps if app.build_status == build_status and app.preserve == preserve]

    def get_built_apps_report_parts(self) -> list[str]:
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
            new_test_related_apps_table_section = self.create_table_section(
                title=self.report_titles_map['new_test_related_apps'],
                items=new_test_related_apps,
                headers=[
                    'App Dir',
                    'Build Dir',
                    'Download Command',
                    'Your Branch App Size',
                ],
                row_attrs=[
                    'app_dir',
                    'build_dir',
                ],
                value_functions=[
                    ('Your Branch App Size', lambda _app: str(_app.metrics[BINARY_SIZE_METRIC_NAME].source_value)),
                    (
                        'Download Command',
                        lambda _app: f'idf-ci gitlab download-artifacts --pipeline-id {self.pipeline_id} '
                        f'{idf_relpath(_app.build_path)}',
                    ),
                ],
            )
            sections.extend(new_test_related_apps_table_section)

        if built_test_related_apps:
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
                    'Download Command',
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
                        'Download Command',
                        lambda _app: f'idf-ci gitlab download-artifacts --pipeline-id {self.pipeline_id} '
                        f'{idf_relpath(_app.build_path)}',
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
                    'Download Command',
                    'Your Branch App Size',
                ],
                row_attrs=[
                    'app_dir',
                    'build_dir',
                ],
                value_functions=[
                    (
                        'Download Command',
                        lambda _app: f'idf-ci gitlab download-artifacts --pipeline-id {self.pipeline_id} '
                        f'{idf_relpath(_app.build_path)}',
                    ),
                    ('Your Branch App Size', lambda app: str(app.metrics[BINARY_SIZE_METRIC_NAME].source_value)),
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
                    'Download Command',
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
                        'Download Command',
                        lambda _app: f'idf-ci gitlab download-artifacts --pipeline-id {self.pipeline_id} '
                        f'{idf_relpath(_app.build_path)}',
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

        return sections

    def get_failed_apps_report_parts(self) -> list[str]:
        failed_apps = [app for app in self.apps if app.build_status == BuildStatus.FAILED]
        if not failed_apps:
            return []

        failed_apps_table_section = self.create_table_section(
            title=self.report_titles_map['failed_apps'],
            items=failed_apps,
            headers=['App Dir', 'Build Dir', 'Failed Reason', 'Download Command'],
            row_attrs=['app_dir', 'build_dir', 'build_comment'],
            value_functions=[
                (
                    'Download Command',
                    lambda _app: f'idf-ci gitlab download-artifacts --pipeline-id {self.pipeline_id} '
                    f'{idf_relpath(_app.build_path)}',
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

    def get_skipped_apps_report_parts(self) -> list[str]:
        skipped_apps = [app for app in self.apps if app.build_status == BuildStatus.SKIPPED]
        if not skipped_apps:
            return []

        skipped_apps_table_section = self.create_table_section(
            title=self.report_titles_map['skipped_apps'],
            items=skipped_apps,
            headers=['App Dir', 'Build Dir', 'Skipped Reason'],
            row_attrs=['app_dir', 'build_dir', 'build_comment'],
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


class JobReportGenerator(ReportGenerator):
    def __init__(
        self,
        project_id: int,
        mr_iid: int,
        pipeline_id: int,
        job_id: int,
        commit_id: str,
        local_commit_id: str,
        *,
        title: str = 'Job Report',
        jobs: list[GitlabJob],
    ):
        super().__init__(project_id, mr_iid, pipeline_id, job_id, commit_id, local_commit_id, title=title)
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
