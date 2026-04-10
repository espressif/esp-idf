# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""This file is used for generating the child pipeline for target test jobs.

1. Check the build jobs' artifacts to get the built apps' information.
2. Post the Build Report if it's running in an MR pipeline.
3. Generate the child pipeline for target test jobs.
"""

import argparse
import os
import typing as t

import __init__  # noqa: F401 # inject the system path
import yaml
from idf_ci import get_pytest_cases
from idf_ci.idf_gitlab import test_child_pipeline
from idf_ci.idf_pytest import GroupedPytestCases
from idf_ci.idf_pytest.models import GroupKey
from idf_ci_utils import IDF_PATH
from idf_pytest.constants import TIMEOUT_4H_MARKERS

from dynamic_pipelines.constants import KNOWN_GENERATE_TEST_CHILD_PIPELINE_WARNINGS_FILEPATH

BUILD_ONLY_LABEL = 'For Maintainers: Only Build Tests'
TIMEOUT_4H_TEMPLATE_NAME = '.timeout_4h_template'
TARGET_TEST_CHILD_PIPELINE_FILEPATH = os.path.join(IDF_PATH, 'target_test_child_pipeline.yml')


def main(output_filepath: str) -> None:
    if mr_labels := os.getenv('CI_MERGE_REQUEST_LABELS'):
        print(f'MR labels: {mr_labels}')

        if BUILD_ONLY_LABEL in mr_labels.split(','):
            print('MR has build only label, skip generating target test child pipeline')
            test_child_pipeline(
                output_filepath,
                cases=GroupedPytestCases([]),
            )
            return

    cases = GroupedPytestCases(get_pytest_cases())

    with open(KNOWN_GENERATE_TEST_CHILD_PIPELINE_WARNINGS_FILEPATH) as fr:
        known_warnings_dict = yaml.safe_load(fr) or dict()

    def _process_match_group(runner_tags: str) -> set | None:
        match_group = {_el for _el in runner_tags.split(',') if _el != '*'}
        if len(match_group) == 0:
            print('WARNING: wildcard exclusion requires at least one specific tag — skipped')
            return None
        return match_group

    exclude_runner_tags_set = set()
    exclude_runner_tags_matching = []
    for _tag in known_warnings_dict.get('no_runner_tags', []):
        if '*' not in _tag:
            exclude_runner_tags_set.add(_tag)
        else:
            if res := _process_match_group(_tag):
                exclude_runner_tags_matching.append(res)

    # EXCLUDE_RUNNER_TAGS is a string separated by ';'
    # like 'esp32,generic;esp32c3,wifi'
    # or with wildcard like 'esp32,*; esp32p4,wifi,*'
    if exclude_runner_tags := os.getenv('EXCLUDE_RUNNER_TAGS'):
        for _tag in exclude_runner_tags.split(';'):
            if '*' not in _tag:
                exclude_runner_tags_set.add(_tag)
            else:
                if res := _process_match_group(_tag):
                    exclude_runner_tags_matching.append(res)

    flattened_cases = []
    additional_dict: dict[GroupKey, dict[str, t.Any]] = {}
    for key, grouped_cases in cases.grouped_cases.items():
        # skip test cases with no runner tags
        if ','.join(sorted(key.runner_tags)) in exclude_runner_tags_set:
            print(f'WARNING: excluding test cases with runner tags: {key.runner_tags}')
            continue

        print(f'INFO: key.runner tags: {key.runner_tags}')
        if any(set(key.runner_tags) & group for group in exclude_runner_tags_matching):
            print(f'WARNING: Excluding test cases with runner tags (wildcard match): {key.runner_tags}')
            continue

        flattened_cases.extend(grouped_cases)

        for case in grouped_cases:
            for timeout_4h_marker in TIMEOUT_4H_MARKERS:
                if timeout_4h_marker in case.env_markers:
                    if key not in additional_dict:
                        additional_dict[key] = {
                            'extra_extends': [],
                        }

                    if TIMEOUT_4H_TEMPLATE_NAME not in additional_dict[key]['extra_extends']:
                        additional_dict[key]['extra_extends'].append(TIMEOUT_4H_TEMPLATE_NAME)

    test_child_pipeline(
        output_filepath,
        cases=GroupedPytestCases(
            cases=flattened_cases,
            additional_dict=additional_dict,
        ),
    )


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Generate Target Test Child Pipeline. Update Build Report in MR pipelines',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        '-o',
        '--output',
        default=TARGET_TEST_CHILD_PIPELINE_FILEPATH,
        help='Output child pipeline file path',
    )

    args = parser.parse_args()

    main(args.output)
