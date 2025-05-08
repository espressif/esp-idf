#!/usr/bin/env python
# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import os.path
import sys
import unittest
from unittest.mock import MagicMock
from unittest.mock import patch

sys.path.insert(0, os.path.join(f'{os.environ.get("IDF_PATH")}', 'tools', 'ci', 'python_packages'))
sys.path.insert(0, os.path.join(f'{os.environ.get("IDF_PATH")}', 'tools', 'ci'))

from idf_build_apps.constants import BuildStatus  # noqa: E402
from idf_ci_local.app import enrich_apps_with_metrics_info  # noqa: E402
from idf_ci_local.app import import_apps_from_txt  # noqa: E402

from dynamic_pipelines.models import GitlabJob  # noqa: E402
from dynamic_pipelines.report import BuildReportGenerator  # noqa: E402
from dynamic_pipelines.report import JobReportGenerator  # noqa: E402
from dynamic_pipelines.report import TargetTestReportGenerator  # noqa: E402
from dynamic_pipelines.utils import load_file  # noqa: E402
from dynamic_pipelines.utils import parse_testcases_from_filepattern  # noqa: E402


class TestReportGeneration(unittest.TestCase):
    def setUp(self) -> None:
        self.reports_sample_data_path = os.path.join(
            os.environ.get('IDF_PATH', ''),
            'tools',
            'ci',
            'dynamic_pipelines',
            'tests',
            'test_report_generator',
            'reports_sample_data',
        )
        self.setup_patches()
        self.load_test_and_job_reports()
        self.create_report_generators()

    def setup_patches(self) -> None:
        self.gitlab_patcher = patch('dynamic_pipelines.report.Gitlab')
        self.uploader_patcher = patch('dynamic_pipelines.report.AppUploader')
        self.failure_rate_patcher = patch('dynamic_pipelines.report.fetch_failed_testcases_failure_ratio')
        self.env_patcher = patch.dict(
            'os.environ',
            {
                'CI_DASHBOARD_HOST': 'https://test_dashboard_host',
                'CI_PAGES_URL': 'https://artifacts_path',
                'CI_JOB_ID': '1',
                'JIRA_SERVER': 'https://jira.com',
            },
        )
        self.yaml_dump_patcher = patch('dynamic_pipelines.report.yaml.dump')

        self.MockGitlab = self.gitlab_patcher.start()
        self.MockUploader = self.uploader_patcher.start()
        self.test_cases_failure_rate = self.failure_rate_patcher.start()
        self.env_patcher.start()
        self.yaml_dump_patcher.start()

        self.mock_project = MagicMock()
        self.mock_mr = MagicMock()
        self.MockGitlab.return_value.project = self.mock_project
        self.mock_project.mergerequests.get.return_value = self.mock_mr
        self.MockUploader.return_value.get_app_presigned_url.return_value = 'https://example.com/presigned-url'

        self.addCleanup(self.gitlab_patcher.stop)
        self.addCleanup(self.uploader_patcher.stop)
        self.addCleanup(self.failure_rate_patcher.stop)
        self.addCleanup(self.env_patcher.stop)
        self.addCleanup(self.yaml_dump_patcher.stop)
        self.addCleanup(self.cleanup_files)

    def cleanup_files(self) -> None:
        files_to_delete = [
            self.target_test_report_generator.skipped_test_cases_report_file,
            self.target_test_report_generator.succeeded_cases_report_file,
            self.target_test_report_generator.failed_cases_report_file,
            self.build_report_generator.failed_apps_report_file,
            self.build_report_generator.built_apps_report_file,
            self.build_report_generator.skipped_apps_report_file,
            self.build_report_generator.apps_presigned_url_filepath,
        ]
        for file_path in files_to_delete:
            if os.path.exists(file_path):
                os.remove(file_path)

    def load_test_and_job_reports(self) -> None:
        self.expected_target_test_report_html = load_file(
            os.path.join(self.reports_sample_data_path, 'expected_target_test_report.html')
        )
        self.expected_job_report_html = load_file(
            os.path.join(self.reports_sample_data_path, 'expected_job_report.html')
        )
        self.expected_build_report_html = load_file(
            os.path.join(self.reports_sample_data_path, 'expected_build_report.html')
        )

    def create_report_generators(self) -> None:
        jobs_response_raw = load_file(os.path.join(self.reports_sample_data_path, 'jobs_api_response.json'))
        failure_rate_jobs_response = load_file(
            os.path.join(self.reports_sample_data_path, 'failure_rate_jobs_response.json')
        )
        built_apps_size_info_response = json.loads(
            load_file(os.path.join(self.reports_sample_data_path, 'apps_size_info_api_response.json'))
        )
        failure_rates = {item['name']: item for item in json.loads(failure_rate_jobs_response).get('jobs', [])}
        jobs = [
            GitlabJob.from_json_data(job_json, failure_rates.get(job_json['name'], {}))
            for job_json in json.loads(jobs_response_raw)['jobs']
        ]
        test_cases = parse_testcases_from_filepattern(os.path.join(self.reports_sample_data_path, 'XUNIT_*.xml'))
        apps = enrich_apps_with_metrics_info(
            built_apps_size_info_response, import_apps_from_txt(os.path.join(self.reports_sample_data_path, 'apps'))
        )
        self.target_test_report_generator = TargetTestReportGenerator(
            project_id=123,
            mr_iid=1,
            pipeline_id=456,
            job_id=0,
            commit_id='cccc',
            title='Test Report',
            test_cases=test_cases,
        )
        self.job_report_generator = JobReportGenerator(
            project_id=123, mr_iid=1, pipeline_id=456, job_id=0, commit_id='cccc', title='Job Report', jobs=jobs
        )
        self.build_report_generator = BuildReportGenerator(
            project_id=123, mr_iid=1, pipeline_id=456, job_id=0, commit_id='cccc', title='Build Report', apps=apps
        )
        self.target_test_report_generator._known_failure_cases_set = {
            '*.test_wpa_supplicant_ut',
            'esp32c3.release.test_esp_timer',
            '*.512safe.test_wear_levelling',
        }
        test_cases_failed = [tc for tc in test_cases if tc.is_failure]
        for index, tc in enumerate(test_cases_failed):
            tc.latest_total_count = 40
            if index % 3 == 0:
                tc.latest_failed_count = 0
            else:
                tc.latest_failed_count = 3
        self.test_cases_failure_rate.return_value = test_cases_failed

    def test_known_failure_cases(self) -> None:
        known_failure_cases = self.target_test_report_generator.get_known_failure_cases()
        self.assertEqual(len(known_failure_cases), 4)

    def test_failed_cases_in_target_test_report(self) -> None:
        known_failures = self.target_test_report_generator.get_known_failure_cases()
        known_failure_case_names = {case.name for case in known_failures}
        failed_testcases = self.target_test_report_generator._filter_items(
            self.target_test_report_generator.test_cases,
            lambda tc: tc.is_failure and tc.name not in known_failure_case_names,
        )
        self.assertEqual(len(failed_testcases), 3)

    def test_skipped_cases_in_target_test_report(self) -> None:
        skipped_testcases = self.target_test_report_generator._filter_items(
            self.target_test_report_generator.test_cases, lambda tc: tc.is_skipped
        )
        self.assertEqual(len(skipped_testcases), 1)

    def test_successful_cases_in_target_test_report(self) -> None:
        succeeded_testcases = self.target_test_report_generator._filter_items(
            self.target_test_report_generator.test_cases, lambda tc: tc.is_success
        )
        self.assertEqual(len(succeeded_testcases), 9)

    def test_target_test_report_html_structure(self) -> None:
        report = self.target_test_report_generator._get_report_str()
        self.assertEqual(report, self.expected_target_test_report_html)

    def test_failed_jobs_in_job_report(self) -> None:
        failed_jobs = self.job_report_generator._filter_items(self.job_report_generator.jobs, lambda job: job.is_failed)
        self.assertEqual(len(failed_jobs), 3)

    def test_successful_jobs_in_job_report(self) -> None:
        succeeded_jobs = self.job_report_generator._filter_items(
            self.job_report_generator.jobs, lambda job: job.is_success
        )
        self.assertEqual(len(succeeded_jobs), 13)

    def test_job_report_html_structure(self) -> None:
        report = self.job_report_generator._get_report_str()
        self.assertEqual(report, self.expected_job_report_html)

    def test_generate_top_n_apps_by_size_table(self) -> None:
        apps_with_size_diff = [
            MagicMock(
                app_dir=f'app_dir_{i}',
                build_dir=f'build_dir_{i}',
                build_status=BuildStatus.SUCCESS,
                metrics={
                    'binary_size': MagicMock(
                        source=i * 10000,
                        target=i * 10000 + i * 1000,
                        difference=i * 1000,
                        difference_percentage=i * 0.5,
                    )
                },
            )
            for i in range(1, 6)
        ]
        build_report_generator = BuildReportGenerator(
            project_id=123,
            mr_iid=1,
            pipeline_id=456,
            job_id=0,
            commit_id='cccc',
            title='Build Report',
            apps=apps_with_size_diff,
        )

        top_apps_table = build_report_generator._generate_top_n_apps_by_size_table()

        self.assertIn('| App Dir | Build Dir | Size Diff (bytes) | Size Diff (%) |', top_apps_table)
        self.assertIn('| app_dir_5 | build_dir_5 | 5000 | 2.5% |', top_apps_table)
        self.assertIn('| app_dir_1 | build_dir_1 | 1000 | 0.5% |', top_apps_table)

    def test_get_built_apps_report_parts(self) -> None:
        apps = [
            MagicMock(
                app_dir='test_app_1',
                build_dir='build_dir_1',
                size_difference=1000,
                size_difference_percentage=1.0,
                build_status=BuildStatus.SUCCESS,
                preserve=True,
                metrics={'binary_size': MagicMock(difference=1000, difference_percentage=1.0)},
            ),
            MagicMock(
                app_dir='test_app_2',
                build_dir='build_dir_2',
                size_difference=2000,
                size_difference_percentage=2.0,
                build_status=BuildStatus.SUCCESS,
                preserve=False,
                metrics={'binary_size': MagicMock(difference=2000, difference_percentage=2.0)},
            ),
        ]

        build_report_generator = BuildReportGenerator(
            project_id=123, mr_iid=1, pipeline_id=456, job_id=0, commit_id='cccc', title='Build Report', apps=apps
        )

        built_apps_report_parts = build_report_generator.get_built_apps_report_parts()

        self.assertGreater(len(built_apps_report_parts), 0)
        self.assertIn('test_app_1', ''.join(built_apps_report_parts))
        self.assertIn('test_app_2', ''.join(built_apps_report_parts))

    def test_get_failed_apps_report_parts(self) -> None:
        failed_apps = [
            MagicMock(
                app_dir='failed_app_1',
                build_dir='build_dir_1',
                build_comment='Compilation error',
                build_status=BuildStatus.FAILED,
                metrics={'binary_size': MagicMock(difference=None, difference_percentage=None)},
            ),
            MagicMock(
                app_dir='failed_app_2',
                build_dir='build_dir_2',
                build_comment='Linker error',
                build_status=BuildStatus.FAILED,
                metrics={'binary_size': MagicMock(difference=None, difference_percentage=None)},
            ),
        ]

        build_report_generator = BuildReportGenerator(
            project_id=123,
            mr_iid=1,
            pipeline_id=456,
            job_id=0,
            commit_id='cccc',
            title='Build Report',
            apps=failed_apps,
        )

        failed_apps_report_parts = build_report_generator.get_failed_apps_report_parts()

        self.assertGreater(len(failed_apps_report_parts), 0)
        self.assertIn('failed_app_1', ''.join(failed_apps_report_parts))
        self.assertIn('failed_app_2', ''.join(failed_apps_report_parts))

    def test_get_skipped_apps_report_parts(self) -> None:
        skipped_apps = [
            MagicMock(
                app_dir='skipped_app_1',
                build_dir='build_dir_1',
                build_comment='Dependencies unmet',
                build_status=BuildStatus.SKIPPED,
                metrics={'binary_size': MagicMock(difference=None, difference_percentage=None)},
            ),
            MagicMock(
                app_dir='skipped_app_2',
                build_dir='build_dir_2',
                build_comment='Feature flag disabled',
                build_status=BuildStatus.SKIPPED,
                metrics={'binary_size': MagicMock(difference=None, difference_percentage=None)},
            ),
        ]

        build_report_generator = BuildReportGenerator(
            project_id=123,
            mr_iid=1,
            pipeline_id=456,
            job_id=0,
            commit_id='cccc',
            title='Build Report',
            apps=skipped_apps,
        )

        skipped_apps_report_parts = build_report_generator.get_skipped_apps_report_parts()

        self.assertGreater(len(skipped_apps_report_parts), 0)
        self.assertIn('skipped_app_1', ''.join(skipped_apps_report_parts))
        self.assertIn('skipped_app_2', ''.join(skipped_apps_report_parts))

    def test_build_report_html_structure(self) -> None:
        report = self.build_report_generator._get_report_str()
        self.assertEqual(report, self.expected_build_report_html)


if __name__ == '__main__':
    unittest.main()
