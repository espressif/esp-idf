#!/usr/bin/env python
# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os.path
import sys
import unittest
from unittest.mock import MagicMock
from unittest.mock import patch

sys.path.insert(0, os.path.join(f'{os.environ.get("IDF_PATH")}', 'tools', 'ci', 'python_packages'))
sys.path.insert(0, os.path.join(f'{os.environ.get("IDF_PATH")}', 'tools', 'ci'))


from dynamic_pipelines.report import TargetTestReportGenerator  # noqa: E402
from dynamic_pipelines.utils import parse_testcases_from_filepattern  # noqa: E402


class TestReportGeneration(unittest.TestCase):
    @classmethod
    def load_expected_report(cls, file_path: str) -> str:
        """
        Loads the content of an expected report HTML file.

        :param file_path: The path to the file containing the expected HTML report.
        :return: The content of the file as a string.
        """
        with open(file_path, 'r') as file:
            return file.read()

    def setUp(self) -> None:
        patcher = patch('dynamic_pipelines.report.Gitlab')
        env_patcher = patch.dict('os.environ', {
            'CI_DASHBOARD_HOST': 'https://test_dashboard_host',
            'CI_PAGES_URL': 'https://artifacts_path',
            'CI_JOB_ID': '1',
        })
        env_patcher.start()
        self.MockGitlab = patcher.start()
        self.addCleanup(patcher.stop)
        self.addCleanup(env_patcher.stop)
        self.reports_sample_data_path = os.path.join(
            os.environ.get('IDF_PATH', ''),  # type: ignore
            'tools',
            'ci',
            'dynamic_pipelines',
            'tests',
            'test_report_generator',
            'reports_sample_data'
        )
        self.mock_project = MagicMock()
        self.mock_mr = MagicMock()

        self.MockGitlab.return_value.project = self.mock_project
        self.mock_project.mergerequests.get.return_value = self.mock_mr

        self.expected_report_html = self.load_expected_report(
            os.path.join(self.reports_sample_data_path, 'expected_target_test_report.html')
        )

        test_cases = parse_testcases_from_filepattern(os.path.join(self.reports_sample_data_path, 'XUNIT_*.xml'))
        self.report_generator = TargetTestReportGenerator(
            project_id=123, mr_iid=1, pipeline_id=456, title='Test Report', test_cases=test_cases
        )
        self.report_generator._known_failure_cases_set = {
            '*.test_wpa_supplicant_ut',
            'esp32c3.release.test_esp_timer',
            '*.512safe.test_wear_levelling',
        }

    def test_known_failure_cases(self) -> None:
        known_failure_cases = self.report_generator.get_known_failure_cases()
        self.assertEqual(len(known_failure_cases), 4)

    def test_failed_cases_in_report(self) -> None:
        known_failures = self.report_generator.get_known_failure_cases()
        known_failure_case_names = {case.name for case in known_failures}
        failed_testcases = self.report_generator._filter_test_cases(
            lambda tc: tc.is_failure and tc.name not in known_failure_case_names
        )
        self.assertEqual(len(failed_testcases), 3)

    def test_skipped_cases_in_report(self) -> None:
        skipped_testcases = self.report_generator._filter_test_cases(lambda tc: tc.is_skipped)
        self.assertEqual(len(skipped_testcases), 1)

    def test_successful_cases_in_report(self) -> None:
        succeeded_testcases = self.report_generator._filter_test_cases(lambda tc: tc.is_success)
        self.assertEqual(len(succeeded_testcases), 9)

    def test_complete_html_structure(self) -> None:
        report = self.report_generator._get_report_str()
        self.assertEqual(report, self.expected_report_html)


if __name__ == '__main__':
    unittest.main()
