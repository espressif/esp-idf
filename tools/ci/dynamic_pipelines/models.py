# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import typing as t
import urllib.parse
from dataclasses import dataclass
from xml.etree.ElementTree import Element

from idf_ci_utils import IDF_PATH


@dataclass
class TestCase:
    name: str
    file: str
    time: float
    app_path: t.Optional[str] = None
    failure: t.Optional[str] = None
    skipped: t.Optional[str] = None
    ci_job_url: t.Optional[str] = None
    ci_dashboard_url: t.Optional[str] = None
    dut_log_url: t.Optional[str] = None
    latest_total_count: int = 0
    latest_failed_count: int = 0

    @property
    def is_failure(self) -> bool:
        return self.failure is not None

    @property
    def is_skipped(self) -> bool:
        return self.skipped is not None

    @property
    def is_success(self) -> bool:
        return not self.is_failure and not self.is_skipped

    @classmethod
    def _get_idf_rel_path(cls, path: str) -> str:
        if path.startswith(IDF_PATH):
            return os.path.relpath(path, IDF_PATH)
        else:
            return path

    @classmethod
    def from_test_case_node(cls, node: Element) -> t.Optional['TestCase']:
        if 'name' not in node.attrib:
            print('WARNING: Node Invalid: ', node)
            return None

        # url to test cases dashboard
        grafana_base_url = urllib.parse.urljoin(os.getenv('CI_DASHBOARD_HOST', ''), '/d/Ucg477Fnz/case-list')
        encoded_params = urllib.parse.urlencode({'var-case_id': node.attrib['name']}, quote_via=urllib.parse.quote)

        kwargs = {
            'name': node.attrib['name'],
            'file': node.attrib.get('file'),
            'app_path': '|'.join(
                cls._get_idf_rel_path(path) for path in node.attrib.get('app_path', 'unknown').split('|')
            ),
            'time': float(node.attrib.get('time') or 0),
            'ci_job_url': node.attrib.get('ci_job_url') or 'Not found',
            'ci_dashboard_url': f'{grafana_base_url}?{encoded_params}',
            'dut_log_url': node.attrib.get('dut_log_url') or 'Not found',
        }

        failure_node = node.find('failure')
        # bool(failure_node) is False, so compare with None
        if failure_node is None:
            failure_node = node.find('error')
        if failure_node is not None:
            message = failure_node.attrib.get('message', '')
            kwargs['failure'] = message

        skipped_node = node.find('skipped')
        if skipped_node is not None:
            kwargs['skipped'] = skipped_node.attrib['message']

        return cls(**kwargs)  # type: ignore


@dataclass
class GitlabJob:
    id: int
    name: str
    stage: str
    status: str
    url: str
    ci_dashboard_url: str
    failure_reason: t.Optional[str] = None
    failure_log: t.Optional[str] = None
    latest_total_count: int = 0
    latest_failed_count: int = 0

    @property
    def is_failed(self) -> bool:
        return self.status == 'failed'

    @property
    def is_success(self) -> bool:
        return self.status == 'success'

    @classmethod
    def from_json_data(cls, job_data: dict, failure_data: dict) -> t.Optional['GitlabJob']:
        grafana_base_url = urllib.parse.urljoin(os.getenv('CI_DASHBOARD_HOST', ''), '/d/LoUa-qLWz/job-list')
        encoded_params = urllib.parse.urlencode({'var-job_name': job_data['name']}, quote_via=urllib.parse.quote)

        kwargs = {
            'id': job_data['id'],
            'name': job_data['name'],
            'stage': job_data['stage'],
            'status': job_data['status'],
            'url': job_data['url'],
            'ci_dashboard_url': f'{grafana_base_url}?{encoded_params}',
            'failure_reason': job_data['failure_reason'],
            'failure_log': job_data['failure_log'],
            'latest_total_count': failure_data.get('total_count', 0),
            'latest_failed_count': failure_data.get('failed_count', 0),
        }

        return cls(**kwargs)  # type: ignore
