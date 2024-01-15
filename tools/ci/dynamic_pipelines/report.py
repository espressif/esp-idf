# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import abc
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

from .constants import COMMENT_START_MARKER, REPORT_TEMPLATE_FILEPATH, TEST_RELATED_APPS_DOWNLOAD_URLS_FILENAME
from .models import TestCase


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

        comment = f'''#### {self.title}

Full {self.title} here: {url}/-/jobs/{job_id}/artifacts/{self.output_filepath} (with commit {commit_id})

'''
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
            new_comment = f'''{COMMENT_START_MARKER}

{comment}'''
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

    def _get_report_str(self) -> str:
        table_str = ''

        failed_test_cases = [tc for tc in self.test_cases if tc.is_failure]
        if failed_test_cases:
            table_str += '<h2>Failed Test Cases</h2>'

            failed_test_cases_table = PrettyTable()
            failed_test_cases_table.field_names = ['Test Case', 'Test Script File Path', 'Failure Reason', 'Job URL']
            for tc in failed_test_cases:
                failed_test_cases_table.add_row([tc.name, tc.file, tc.failure, tc.ci_job_url])

            table_str += self.table_to_html_str(failed_test_cases_table)

        skipped_test_cases = [tc for tc in self.test_cases if tc.is_skipped]
        if skipped_test_cases:
            table_str += '<h2>Skipped Test Cases</h2>'

            skipped_test_cases_table = PrettyTable()
            skipped_test_cases_table.field_names = ['Test Case', 'Test Script File Path', 'Skipped Reason']
            for tc in skipped_test_cases:
                skipped_test_cases_table.add_row([tc.name, tc.file, tc.skipped])

            table_str += self.table_to_html_str(skipped_test_cases_table)

        successful_test_cases = [tc for tc in self.test_cases if tc.is_success]
        if successful_test_cases:
            table_str += '<h2>Succeeded Test Cases</h2>'

            successful_test_cases_table = PrettyTable()
            successful_test_cases_table.field_names = ['Test Case', 'Test Script File Path', 'Job URL']
            for tc in successful_test_cases:
                successful_test_cases_table.add_row([tc.name, tc.file, tc.ci_job_url])

            table_str += self.table_to_html_str(successful_test_cases_table)

        return self.generate_html_report(table_str)
