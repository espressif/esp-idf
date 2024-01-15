# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""This file is used for generating the child pipeline for target test jobs.

1. Check the build jobs' artifacts to get the built apps' information.
2. Post the Build Report if it's running in an MR pipeline.
3. Generate the child pipeline for target test jobs.
"""

import argparse
import glob
import os
import typing as t
from collections import Counter, defaultdict

import __init__  # noqa: F401 # inject the system path
from dynamic_pipelines.constants import (BUILD_ONLY_LABEL, DEFAULT_CASES_TEST_PER_JOB,
                                         DEFAULT_TARGET_TEST_CHILD_PIPELINE_FILEPATH, DEFAULT_TEST_PATHS)
from dynamic_pipelines.models import EmptyJob, Job, TargetTestJob
from dynamic_pipelines.utils import dump_jobs_to_yaml
from gitlab.v4.objects import Project
from gitlab_api import Gitlab
from idf_build_apps import App
from idf_ci.app import import_apps_from_txt
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


def get_target_test_jobs(project: Project, paths: str, apps: t.List[App]) -> t.Tuple[t.List[Job], t.List[str]]:
    """
    Return the target test jobs and the extra yaml files to include
    """
    if mr_labels := os.getenv('CI_MERGE_REQUEST_LABELS'):
        print(f'MR labels: {mr_labels}')

        if BUILD_ONLY_LABEL in mr_labels.split(','):
            print('MR has build only label, skip generating target test child pipeline')
            return [EmptyJob()], []

    pytest_cases = get_pytest_cases(
        paths,
        apps=apps,
        marker_expr='not host_test',  # since it's generating target-test child pipeline
    )

    res = defaultdict(list)
    for case in pytest_cases:
        if not case.env_markers:
            print(f'No env markers found for {case.item.originalname} in {case.path}. Ignoring...')
            continue

        res[(case.target_selector, tuple(sorted(case.env_markers)))].append(case)

    target_test_jobs: t.List[Job] = []
    for (target_selector, env_markers), cases in res.items():
        runner_tags = get_tags_with_amount(target_selector) + list(env_markers)
        # we don't need to get all runner, as long as we get one runner, it's fine
        runner_list = project.runners.list(status='online', tag_list=','.join(runner_tags), get_all=False)
        if not runner_list:
            print(f'WARNING: No runner found with tag {",".join(runner_tags)}, ignoring the following test cases:')
            for case in cases:
                print(f'  - {case.name}')
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

    return target_test_jobs, extra_include_yml


def generate_target_test_child_pipeline(project: Project, paths: str, apps: t.List[App], output_filepath: str) -> None:
    target_test_jobs, extra_include_yml = get_target_test_jobs(project, paths, apps)
    dump_jobs_to_yaml(target_test_jobs, output_filepath, extra_include_yml)
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

    args = parser.parse_args()

    app_list_filepattern = 'list_job_*.txt'
    apps = []
    for f in glob.glob(app_list_filepattern):
        apps.extend(import_apps_from_txt(f))

    gl_project = Gitlab(args.project_id).project
    generate_target_test_child_pipeline(gl_project, args.paths, apps, args.output)
