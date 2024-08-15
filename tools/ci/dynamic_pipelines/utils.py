# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import glob
import os
import re
import typing as t
import xml.etree.ElementTree as ET
from urllib.parse import urlparse

import requests
import yaml

from .constants import CI_DASHBOARD_API
from .constants import CI_JOB_TOKEN
from .constants import CI_MERGE_REQUEST_SOURCE_BRANCH_SHA
from .constants import CI_PAGES_URL
from .constants import CI_PROJECT_URL
from .models import GitlabJob
from .models import Job
from .models import TestCase


def dump_jobs_to_yaml(
    jobs: t.List[Job],
    output_filepath: str,
    pipeline_name: str,
    extra_include_yml: t.Optional[t.List[str]] = None,
) -> None:
    yaml_dict = {}
    for job in jobs:
        yaml_dict.update(job.to_dict())

    # global stuffs
    yaml_dict.update(
        {
            'include': [
                'tools/ci/dynamic_pipelines/templates/.dynamic_jobs.yml',
                '.gitlab/ci/common.yml',
            ],
            'workflow': {
                'name': pipeline_name,
                'rules': [
                    # always run the child pipeline, if they are created
                    {'when': 'always'},
                ],
            },
        }
    )
    yaml_dict['include'].extend(extra_include_yml or [])

    with open(output_filepath, 'w') as fw:
        yaml.dump(yaml_dict, fw, indent=2)


def parse_testcases_from_filepattern(junit_report_filepattern: str) -> t.List[TestCase]:
    """
    Parses test cases from XML files matching the provided file pattern.

    >>> test_cases = parse_testcases_from_filepattern("path/to/your/junit/reports/*.xml")

    :param junit_report_filepattern: The file pattern to match XML files containing JUnit test reports.
    :return: List[TestCase]: A list of TestCase objects parsed from the XML files.
    """

    test_cases = []
    for f in glob.glob(junit_report_filepattern):
        root = ET.parse(f).getroot()
        for tc in root.findall('.//testcase'):
            test_cases.append(TestCase.from_test_case_node(tc))
    return test_cases


def load_known_failure_cases() -> t.Optional[t.Set[str]]:
    known_failures_file = os.getenv('KNOWN_FAILURE_CASES_FILE_NAME', '')
    if not known_failures_file:
        return None
    try:
        with open(known_failures_file, 'r') as f:
            file_content = f.read()

        pattern = re.compile(r'^(.*?)\s+#\s+([A-Z]+)-\d+', re.MULTILINE)
        matches = pattern.findall(file_content)

        known_cases_list = [match[0].strip() for match in matches]
        return set(known_cases_list)
    except FileNotFoundError:
        return None


def is_url(string: str) -> bool:
    """
    Check if the string is a valid URL by parsing it and verifying if it contains both a scheme and a network location.

    :param string: The string to check if it is a URL.
    :return: True if the string is a valid URL, False otherwise.
    """
    parsed = urlparse(string)
    return bool(parsed.scheme) and bool(parsed.netloc)


def fetch_failed_jobs(commit_id: str) -> t.List[GitlabJob]:
    """
    Fetches a list of jobs from the specified commit_id using an API request to ci-dashboard-api.
    :param commit_id: The commit ID for which to fetch jobs.
    :return: A list of jobs if the request is successful, otherwise an empty list.
    """
    response = requests.get(
        f'{CI_DASHBOARD_API}/commits/{commit_id}/jobs',
        headers={'CI-Job-Token': CI_JOB_TOKEN},
    )
    if response.status_code != 200:
        print(f'Failed to fetch jobs data: {response.status_code} with error: {response.text}')
        return []

    data = response.json()
    jobs = data.get('jobs', [])

    if not jobs:
        return []

    failed_job_names = [job['name'] for job in jobs if job['status'] == 'failed']
    response = requests.post(
        f'{CI_DASHBOARD_API}/jobs/failure_ratio',
        headers={'CI-Job-Token': CI_JOB_TOKEN},
        json={'job_names': failed_job_names, 'exclude_branches': [os.getenv('CI_MERGE_REQUEST_SOURCE_BRANCH_NAME', '')]},
    )
    if response.status_code != 200:
        print(f'Failed to fetch jobs failure rate data: {response.status_code} with error: {response.text}')
        return []

    failure_rate_data = response.json()
    failure_rates = {item['name']: item for item in failure_rate_data.get('jobs', [])}

    combined_jobs = []
    for job in jobs:
        failure_data = failure_rates.get(job['name'], {})
        combined_jobs.append(GitlabJob.from_json_data(job, failure_data))

    return combined_jobs


def fetch_failed_testcases_failure_ratio(failed_testcases: t.List[TestCase], branches_filter: dict) -> t.List[TestCase]:
    """
    Fetches info about failure rates of testcases using an API request to ci-dashboard-api.
    :param failed_testcases: The list of failed testcases models.
    :param branches_filter: The filter to filter testcases by branch names.
    :return: A list of testcases with enriched with failure rates data.
    """
    req_json = {'testcase_names': list(set([testcase.name for testcase in failed_testcases])), **branches_filter}
    response = requests.post(
        f'{CI_DASHBOARD_API}/testcases/failure_ratio',
        headers={'CI-Job-Token': CI_JOB_TOKEN},
        json=req_json,
    )
    if response.status_code != 200:
        print(f'Failed to fetch testcases failure rate data: {response.status_code} with error: {response.text}')
        return []

    failure_rate_data = response.json()
    failure_rates = {item['name']: item for item in failure_rate_data.get('testcases', [])}

    for testcase in failed_testcases:
        testcase.latest_total_count = failure_rates.get(testcase.name, {}).get('total_count', 0)
        testcase.latest_failed_count = failure_rates.get(testcase.name, {}).get('failed_count', 0)

    return failed_testcases


def load_file(file_path: str) -> str:
    """
    Loads the content of a file.

    :param file_path: The path to the file needs to be loaded.
    :return: The content of the file as a string.
    """
    with open(file_path, 'r') as file:
        return file.read()


def format_permalink(s: str) -> str:
    """
    Formats a given string into a permalink.

    :param s: The string to be formatted into a permalink.
    :return: The formatted permalink as a string.
    """
    end_index = s.find('(')

    if end_index != -1:
        trimmed_string = s[:end_index].strip()
    else:
        trimmed_string = s.strip()

    formatted_string = trimmed_string.lower().replace(' ', '-')

    return formatted_string


def get_artifacts_url(job_id: int, output_filepath: str) -> str:
    """
    Generates the url of the path where the artifact will be stored in the job's artifacts .

    :param job_id: The job identifier used to construct the URL.
    :param output_filepath: The path to the output file.
    :return: The modified URL pointing to the job's artifacts.
    """
    url = CI_PAGES_URL.replace('esp-idf', '-/esp-idf')
    return f'{url}/-/jobs/{job_id}/artifacts/{output_filepath}'


def get_repository_file_url(file_path: str) -> str:
    """
    Generates the url of the file path inside the repository.

    :param file_path: The file path where the file is stored.
    :return: The modified URL pointing to the file's path in the repository.
    """
    return f'{CI_PROJECT_URL}/-/raw/{CI_MERGE_REQUEST_SOURCE_BRANCH_SHA}/{file_path}'
