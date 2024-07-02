#!/usr/bin/env python
# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import os.path
import sys
import unittest
from unittest.mock import MagicMock
from unittest.mock import patch

sys.path.insert(0, os.path.join(f'{os.environ.get("IDF_PATH")}', 'tools', 'ci', 'python_packages'))
sys.path.insert(0, os.path.join(f'{os.environ.get("IDF_PATH")}', 'tools', 'ci'))

from dynamic_pipelines.models import GitlabJob  # noqa: E402
from dynamic_pipelines.report import JobReportGenerator, TargetTestReportGenerator  # noqa: E402
from dynamic_pipelines.utils import load_file, parse_testcases_from_filepattern  # noqa: E402


class TestReportGeneration(unittest.TestCase):
    def setUp(self) -> None:
        self.reports_sample_data_path = os.path.join(
            os.environ.get('IDF_PATH', ''), 'tools', 'ci', 'dynamic_pipelines', 'tests', 'test_report_generator', 'reports_sample_data'
        )
        self.setup_patches()
        self.load_test_and_job_reports()
        self.create_report_generators()

    def setup_patches(self) -> None:
        self.gitlab_patcher = patch('dynamic_pipelines.report.Gitlab')
        self.failure_rate_patcher = patch('dynamic_pipelines.report.fetch_failed_testcases_failure_ratio')
        self.env_patcher = patch.dict('os.environ', {
            'CI_DASHBOARD_HOST': 'https://test_dashboard_host',
            'CI_PAGES_URL': 'https://artifacts_path',
            'CI_JOB_ID': '1',
        })

        self.MockGitlab = self.gitlab_patcher.start()
        self.test_cases_failure_rate = self.failure_rate_patcher.start()
        self.env_patcher.start()

        self.mock_project = MagicMock()
        self.mock_mr = MagicMock()
        self.MockGitlab.return_value.project = self.mock_project
        self.mock_project.mergerequests.get.return_value = self.mock_mr

        self.addCleanup(self.gitlab_patcher.stop)
        self.addCleanup(self.failure_rate_patcher.stop)
        self.addCleanup(self.env_patcher.stop)
        self.addCleanup(self.cleanup_files)

    def cleanup_files(self) -> None:
        files_to_delete = [
            self.target_test_report_generator.skipped_test_cases_report_file,
            self.target_test_report_generator.succeeded_cases_report_file,
            self.target_test_report_generator.failed_cases_report_file,
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

    def create_report_generators(self) -> None:
        jobs_response_raw = load_file(os.path.join(self.reports_sample_data_path, 'jobs_api_response.json'))
        failure_rate_jobs_response = load_file(os.path.join(self.reports_sample_data_path, 'failure_rate_jobs_response.json'))
        failure_rates = {item['name']: item for item in json.loads(failure_rate_jobs_response).get('jobs', [])}
        jobs = [GitlabJob.from_json_data(job_json, failure_rates.get(job_json['name'], {})) for job_json in json.loads(jobs_response_raw)['jobs']]
        test_cases = parse_testcases_from_filepattern(os.path.join(self.reports_sample_data_path, 'XUNIT_*.xml'))
        self.target_test_report_generator = TargetTestReportGenerator(
            project_id=123,
            mr_iid=1,
            pipeline_id=456,
            job_id=0,
            commit_id='cccc',
            title='Test Report',
            test_cases=test_cases
        )
        self.job_report_generator = JobReportGenerator(
            project_id=123,
            mr_iid=1,
            pipeline_id=456,
            job_id=0,
            commit_id='cccc',
            title='Job Report',
            jobs=jobs
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


if __name__ == '__main__':
    unittest.main()
