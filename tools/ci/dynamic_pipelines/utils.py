# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import glob
import os
import re
import typing as t
import xml.etree.ElementTree as ET
from urllib.parse import urlparse

import yaml

from .models import Job
from .models import TestCase


def dump_jobs_to_yaml(
    jobs: t.List[Job], output_filepath: str, extra_include_yml: t.Optional[t.List[str]] = None
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
        with open(known_failures_file) as f:
            file_content = f.read()
        known_cases_list = re.sub(re.compile('#.*\n'), '', file_content).split()
        return {case.strip() for case in known_cases_list}
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
