# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os

import __init__  # noqa: F401 # inject the system path
from dynamic_pipelines.report import TargetTestReportGenerator
from dynamic_pipelines.utils import parse_testcases_from_filepattern


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Update Build Report in MR pipelines',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        '--project-id',
        type=int,
        default=os.getenv('CI_PROJECT_ID'),
        help='Project ID',
    )
    parser.add_argument(
        '--mr-iid',
        type=int,
        default=os.getenv('CI_MERGE_REQUEST_IID'),
        help='Merge Request IID',
    )
    parser.add_argument(
        '--pipeline-id',
        type=int,
        default=os.getenv('PARENT_PIPELINE_ID'),
        help='Pipeline ID',
    )
    parser.add_argument(
        '--job-id',
        type=int,
        default=os.getenv('CI_JOB_ID'),
        help='Job ID',
    )
    parser.add_argument(
        '--commit-id',
        default=os.getenv('CI_COMMIT_SHORT_SHA'),
        help='MR commit ID',
    )
    parser.add_argument(
        '--junit-report-filepattern',
        default='XUNIT_RESULT*.xml',
        help='Junit Report file pattern',
    )

    args = parser.parse_args()

    test_cases = parse_testcases_from_filepattern(args.junit_report_filepattern)
    report_generator = TargetTestReportGenerator(args.project_id, args.mr_iid, args.pipeline_id, test_cases=test_cases)
    report_generator.post_report(args.job_id, args.commit_id)
