# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import glob
import os
import subprocess
import typing as t

import __init__  # noqa: F401 # inject the system path
from idf_build_apps import json_list_files_to_apps
from idf_ci import GitlabEnvVars
from idf_ci_local.app import enrich_apps_with_metrics_info

from dynamic_pipelines.report import BuildReportGenerator
from dynamic_pipelines.report import JobReportGenerator
from dynamic_pipelines.report import TargetTestReportGenerator
from dynamic_pipelines.utils import fetch_app_metrics
from dynamic_pipelines.utils import fetch_failed_jobs
from dynamic_pipelines.utils import parse_testcases_from_filepattern


def main() -> None:
    parser: argparse.ArgumentParser = setup_argument_parser()
    args: argparse.Namespace = parser.parse_args()

    report_actions: dict[str, t.Callable[[argparse.Namespace], None]] = {
        'build': generate_build_report,
        'target_test': generate_target_test_report,
        'job': generate_jobs_report,
    }

    report_action = report_actions.get(args.report_type)
    if report_action is None:
        raise ValueError('Unknown report type is requested to be generated.')

    report_action(args)


def setup_argument_parser() -> argparse.ArgumentParser:
    report_type_parser: argparse.ArgumentParser = argparse.ArgumentParser(add_help=False)
    report_type_parser.add_argument(
        '--report-type', choices=['build', 'target_test', 'job'], required=True, help='Type of report to generate'
    )
    report_type_args: argparse.Namespace
    remaining_args: list[str]
    report_type_args, remaining_args = report_type_parser.parse_known_args()

    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description='Update reports in MR pipelines based on the selected report type',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        parents=[report_type_parser],
    )

    common_arguments(parser)
    conditional_arguments(report_type_args, parser)

    return parser


def common_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument('--project-id', type=int, default=os.getenv('CI_PROJECT_ID'), help='Project ID')
    parser.add_argument('--mr-iid', type=int, default=os.getenv('CI_MERGE_REQUEST_IID'), help='Merge Request IID')
    parser.add_argument('--pipeline-id', type=int, default=os.getenv('PARENT_PIPELINE_ID'), help='Pipeline ID')
    parser.add_argument('--job-id', type=int, default=os.getenv('CI_JOB_ID'), help='Job ID')
    parser.add_argument('--commit-id', default=os.getenv('CI_COMMIT_SHA', ''), help='MR merged result commit ID')
    parser.add_argument('--local-commit-id', default=os.getenv('PIPELINE_COMMIT_SHA', ''), help='local dev commit ID')


def conditional_arguments(report_type_args: argparse.Namespace, parser: argparse.ArgumentParser) -> None:
    if report_type_args.report_type == 'build':
        parser.add_argument('--app-list-filepattern', default='app_info*.txt', help='Pattern to match app list files')
    elif report_type_args.report_type == 'target_test':
        parser.add_argument(
            '--junit-report-filepattern', default='XUNIT_RESULT*.xml', help='Pattern to match JUnit report files'
        )


def generate_build_report(args: argparse.Namespace) -> None:
    # generate presigned url for the artifacts
    subprocess.check_output(
        [
            'idf-ci',
            'gitlab',
            'generate-presigned-json',
            '--commit-sha',
            args.local_commit_id,
            '--output',
            'presigned.json',
        ],
    )
    print('generated presigned.json')

    # generate report
    apps = json_list_files_to_apps(glob.glob(args.app_list_filepattern))
    print(f'loaded {len(apps)} apps')
    app_metrics = fetch_app_metrics(
        source_commit_sha=args.commit_id,
        target_commit_sha=os.environ.get('CI_MERGE_REQUEST_TARGET_BRANCH_SHA'),
    )
    apps = enrich_apps_with_metrics_info(app_metrics, apps)
    report_generator = BuildReportGenerator(
        args.project_id, args.mr_iid, args.pipeline_id, args.job_id, args.commit_id, args.local_commit_id, apps=apps
    )
    report_generator.post_report()


def generate_target_test_report(args: argparse.Namespace) -> None:
    test_cases: list[t.Any] = parse_testcases_from_filepattern(args.junit_report_filepattern)
    report_generator = TargetTestReportGenerator(
        args.project_id,
        args.mr_iid,
        args.pipeline_id,
        args.job_id,
        args.commit_id,
        args.local_commit_id,
        test_cases=test_cases,
    )
    report_generator.post_report()

    if GitlabEnvVars().IDF_CI_IS_DEBUG_PIPELINE:
        print('Debug pipeline detected, exit non-zero to fail the pipeline in order to block merge')
        exit(30)


def generate_jobs_report(args: argparse.Namespace) -> None:
    jobs: list[t.Any] = fetch_failed_jobs(args.commit_id)

    if not jobs:
        return

    report_generator = JobReportGenerator(
        args.project_id, args.mr_iid, args.pipeline_id, args.job_id, args.commit_id, args.local_commit_id, jobs=jobs
    )
    report_generator.post_report(print_retry_jobs_message=any(job.is_failed for job in jobs))


if __name__ == '__main__':
    main()
