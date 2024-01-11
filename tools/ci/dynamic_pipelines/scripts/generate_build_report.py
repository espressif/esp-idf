# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import glob
import os

import __init__  # noqa: F401 # inject the system path
from dynamic_pipelines.report import BuildReportGenerator
from idf_ci.app import import_apps_from_txt

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
        '--app-list-filepattern',
        default='list_job_*.txt',
        help='App list file pattern',
    )

    args = parser.parse_args()

    apps = []
    for f in glob.glob(args.app_list_filepattern):
        apps.extend(import_apps_from_txt(f))

    report_generator = BuildReportGenerator(args.project_id, args.mr_iid, args.pipeline_id, apps=apps)
    report_generator.post_report(args.job_id, args.commit_id)
