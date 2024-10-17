# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""This file is used for generating the child pipeline for target test jobs.

1. Check the build jobs' artifacts to get the built apps' information.
2. Post the Build Report if it's running in an MR pipeline.
3. Generate the child pipeline for target test jobs.
"""
import argparse
import glob
import logging
import os
import typing as t
from collections import Counter
from collections import defaultdict

import __init__  # noqa: F401 # inject the system path
import yaml
from dynamic_pipelines.constants import BUILD_ONLY_LABEL
from dynamic_pipelines.constants import DEFAULT_CASES_TEST_PER_JOB
from dynamic_pipelines.constants import DEFAULT_TARGET_TEST_CHILD_PIPELINE_FILEPATH
from dynamic_pipelines.constants import DEFAULT_TARGET_TEST_CHILD_PIPELINE_NAME
from dynamic_pipelines.constants import DEFAULT_TEST_PATHS
from dynamic_pipelines.constants import KNOWN_GENERATE_TEST_CHILD_PIPELINE_WARNINGS_FILEPATH
from dynamic_pipelines.models import EmptyJob
from dynamic_pipelines.models import Job
from dynamic_pipelines.models import TargetTestJob
from dynamic_pipelines.utils import dump_jobs_to_yaml
from gitlab.v4.objects import Project
from gitlab_api import Gitlab
from idf_build_apps import App
from idf_ci.app import import_apps_from_txt
from idf_ci_utils import IDF_PATH
from idf_pytest.script import get_pytest_cases


def get_tags_with_amount(s: str) -> t.List[str]:
    c: Counter = Counter()
    for _t in s.split(','):
        c[_t] += 1

    res = set()
    for target, amount in c.items():
        if amount > 1:
            res.add(f'{target}_{amount}')
        else:
            res.add(target)

    return sorted(res)


def get_target_test_jobs(
    project: Project, paths: str, apps: t.List[App]
) -> t.Tuple[t.List[Job], t.List[str], t.Dict[str, t.List[str]]]:
    """
    Return the target test jobs and the extra yaml files to include
    """
    issues: t.Dict[str, t.List[str]] = {
        'no_env_marker_test_cases': [],
        'no_runner_tags': [],
    }

    if mr_labels := os.getenv('CI_MERGE_REQUEST_LABELS'):
        print(f'MR labels: {mr_labels}')

        if BUILD_ONLY_LABEL in mr_labels.split(','):
            print('MR has build only label, skip generating target test child pipeline')
            return [EmptyJob()], [], issues

    pytest_cases = get_pytest_cases(
        paths,
        apps=apps,
        marker_expr='not host_test',  # since it's generating target-test child pipeline
    )

    res = defaultdict(list)
    for case in pytest_cases:
        if not case.env_markers:
            issues['no_env_marker_test_cases'].append(case.item.nodeid)
            continue

        res[(case.target_selector, tuple(sorted(case.env_markers)))].append(case)

    target_test_jobs: t.List[Job] = []
    for (target_selector, env_markers), cases in res.items():
        runner_tags = get_tags_with_amount(target_selector) + list(env_markers)
        # we don't need to get all runner, as long as we get one runner, it's fine
        runner_list = project.runners.list(status='online', tag_list=','.join(runner_tags), get_all=False)
        if not runner_list:
            issues['no_runner_tags'].append(','.join(runner_tags))
            logging.warning(f'No runner found for {",".join(runner_tags)}, required by cases:')
            for case in cases:
                logging.warning(f'  - {case.item.nodeid}')
            continue

        target_test_job = TargetTestJob(
            name=f'{target_selector} - {",".join(env_markers)}',
            tags=runner_tags,
            parallel=len(cases) // DEFAULT_CASES_TEST_PER_JOB + 1,
        )
        target_test_job.set_variable('TARGET_SELECTOR', f"'{target_selector}'")
        target_test_job.set_variable('ENV_MARKERS', "'" + ' and '.join(env_markers) + "'")
        target_test_job.set_variable('PYTEST_NODES', ' '.join([f"'{case.item.nodeid}'" for case in cases]))

        target_test_jobs.append(target_test_job)

    extra_include_yml: t.List[str] = []
    if not target_test_jobs:
        print('No target test cases required, create one empty job instead')
        target_test_jobs.append(EmptyJob())
    else:
        extra_include_yml = ['tools/ci/dynamic_pipelines/templates/generate_target_test_report.yml']

    fast_pipeline_flag = int(os.getenv('REPORT_EXIT_CODE', 0)) == 30
    if fast_pipeline_flag:
        extra_include_yml = ['tools/ci/dynamic_pipelines/templates/fast_pipeline.yml']

    issues['no_env_marker_test_cases'] = sorted(issues['no_env_marker_test_cases'])
    issues['no_runner_tags'] = sorted(issues['no_runner_tags'])

    return target_test_jobs, extra_include_yml, issues


def generate_target_test_child_pipeline(
    project: Project,
    paths: str,
    apps: t.List[App],
    output_filepath: str,
) -> None:
    target_test_jobs, extra_include_yml, issues = get_target_test_jobs(project, paths, apps)

    with open(KNOWN_GENERATE_TEST_CHILD_PIPELINE_WARNINGS_FILEPATH) as fr:
        known_warnings_dict = yaml.safe_load(fr) or dict()

    known_no_env_marker_test_cases = set(known_warnings_dict.get('no_env_marker_test_cases', []))
    no_env_marker_test_cases = set(issues['no_env_marker_test_cases'])

    no_env_marker_test_cases_fail = False
    if no_env_marker_test_cases - known_no_env_marker_test_cases:
        print('ERROR: NEW "no_env_marker_test_cases" detected:')
        for case in no_env_marker_test_cases - known_no_env_marker_test_cases:
            print(f'  - {case}')
        no_env_marker_test_cases_fail = True

        print('Please add at least one environment markers to the test cases listed above. '
              'You may check all the env markers here: tools/ci/idf_pytest/constants.py')

    known_no_runner_tags = set(known_warnings_dict.get('no_runner_tags', []))
    no_runner_tags = set(issues['no_runner_tags'])

    no_runner_tags_fail = False
    if no_runner_tags - known_no_runner_tags:
        print('ERROR: NEW "no_runner_tags" detected:')
        for tag in no_runner_tags - known_no_runner_tags:
            print(f'  - {tag}')
        no_runner_tags_fail = True

        print(
            '- If you\'re the owner of the missing runners, '
            'please make sure the runners are online and have the required tags.\n'
            '- If you\'re the owner of the test cases that require the missing tags, '
            'please add at least one runner with the required tags.\n'
            '- For other users, please contact the runner owner first, '
            'or report this issue in our internal CI channel.\n'
            'If the issue cannot be solved in a short time, '
            'please add the missing tags to the "no_runner_tags" section '
            'under the file inside ESP-IDF repo: '
            f'{os.path.relpath(KNOWN_GENERATE_TEST_CHILD_PIPELINE_WARNINGS_FILEPATH, IDF_PATH)}.'
        )

    if no_env_marker_test_cases_fail or no_runner_tags_fail:
        raise SystemExit('Failed to generate target test child pipeline.')

    dump_jobs_to_yaml(target_test_jobs, output_filepath, DEFAULT_TARGET_TEST_CHILD_PIPELINE_NAME, extra_include_yml)
    print(f'Generate child pipeline yaml file {output_filepath} with {sum(j.parallel for j in target_test_jobs)} jobs')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Generate Target Test Child Pipeline. Update Build Report in MR pipelines',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        '-p',
        '--paths',
        nargs='+',
        default=DEFAULT_TEST_PATHS,
        help='Paths to the apps to build.',
    )
    parser.add_argument(
        '--project-id',
        type=int,
        default=os.getenv('CI_PROJECT_ID'),
        help='Project ID',
    )
    parser.add_argument(
        '--pipeline-id',
        type=int,
        default=os.getenv('PARENT_PIPELINE_ID'),
        help='Pipeline ID',
    )
    parser.add_argument(
        '-o',
        '--output',
        default=DEFAULT_TARGET_TEST_CHILD_PIPELINE_FILEPATH,
        help='Output child pipeline file path',
    )
    parser.add_argument(
        '--app-info-filepattern',
        default='list_job_*.txt',
        help='glob pattern to specify the files that include built app info generated by '
        '`idf-build-apps --collect-app-info ...`. will not raise ValueError when binary '
        'paths not exist in local file system if not listed recorded in the app info.',
    )

    args = parser.parse_args()

    gl_project = Gitlab(args.project_id).project

    apps = []
    for f in glob.glob(args.app_info_filepattern):
        apps.extend(import_apps_from_txt(f))

    generate_target_test_child_pipeline(
        gl_project,
        args.paths,
        apps,
        args.output,
    )
