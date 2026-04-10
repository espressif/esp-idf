# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
from urllib.parse import urlparse

import requests

from .constants import CI_DASHBOARD_API
from .constants import CI_JOB_TOKEN
from .constants import CI_MERGE_REQUEST_SOURCE_BRANCH_SHA
from .constants import CI_PAGES_URL
from .constants import CI_PROJECT_URL
from .models import GitlabJob


def is_url(string: str) -> bool:
    """
    Check if the string is a valid URL by parsing it and verifying if it contains both a scheme and a network location.

    :param string: The string to check if it is a URL.
    :return: True if the string is a valid URL, False otherwise.
    """
    parsed = urlparse(string)
    return bool(parsed.scheme) and bool(parsed.netloc)


def fetch_failed_jobs(commit_id: str) -> list[GitlabJob]:
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
        json={
            'job_names': failed_job_names,
            'exclude_branches': [os.getenv('CI_MERGE_REQUEST_SOURCE_BRANCH_NAME', '')],
        },
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


def fetch_app_metrics(
    source_commit_sha: str,
    target_commit_sha: str,
) -> dict:
    """
    Fetches the app metrics for the given source commit SHA and target branch SHA.
    :param source_commit_sha: The source commit SHA.
    :param target_branch_sha: The commit SHA of the branch to compare app sizes against.
    :return: A dict of sizes of built binaries.
    """
    print(f'Fetching bin size info: {source_commit_sha=} {target_commit_sha=}')
    build_info_map = dict()
    response = requests.post(
        f'{CI_DASHBOARD_API}/apps/metrics',
        headers={'CI-Job-Token': CI_JOB_TOKEN},
        json={
            'source_commit_sha': source_commit_sha,
            'target_commit_sha': target_commit_sha,
        },
    )
    if response.status_code != 200:
        print(f'Failed to fetch build info: {response.status_code} - {response.text}')
    else:
        response_data = response.json()
        build_info_map = {
            f'{info["app_path"]}_{info["config_name"]}_{info["target"]}': info for info in response_data.get('data', [])
        }

    return build_info_map


def load_file(file_path: str) -> str:
    """
    Loads the content of a file.

    :param file_path: The path to the file needs to be loaded.
    :return: The content of the file as a string.
    """
    with open(file_path) as file:
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
