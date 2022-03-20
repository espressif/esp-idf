#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0


'''
Download artifacts from all test jobs

CI_PROJECT_DIR
└── TEST_RESULTS
    ├── <job_id>
    │   └── XUNIT_RESULT.xml
    ├── <job_id>
    │   └── XUNIT_RESULT.xml
    ├── <job_id>
    │   └── TEST_LOGS

'''

import argparse
import json
import os
import shutil

import gitlab_api

CI_PROJECT_ID = int(os.getenv('CI_PROJECT_ID', ''))
CI_PIPELINE_ID = int(os.getenv('CI_PIPELINE_ID', ''))
IDF_PATH = os.getenv('IDF_PATH', '.')
TEST_RESULT_PATH = os.path.join(IDF_PATH, 'TEST_RESULTS')


def download_all_test_results(result_path: str, include_retried: bool = False) -> None:
    if os.path.exists(result_path):
        shutil.rmtree(result_path)
    os.makedirs(result_path, exist_ok=True)

    gitlab_inst = gitlab_api.Gitlab(CI_PROJECT_ID)
    pipelines = gitlab_inst.project.pipelines.get(CI_PIPELINE_ID)
    _include_retried = 'true' if include_retried else 'false'
    jobs = pipelines.jobs.list(all=True, per_page=100, include_retried=_include_retried)

    job_info_list = []
    for job in jobs:
        if job.stage in ['target_test', 'host_test']:
            log_path = ''
            if job.status not in ['success', 'failed']:
                print('Job {}({}) is not finished'.format(job.id, job.name))
            elif not hasattr(job, 'artifacts_file'):
                print('Job {}({}) has no artifacts.'.format(job.id, job.name))
            else:
                log_path = os.path.join(result_path, 'job_{}'.format(job.id))
                print('Downloading artifacts from: {}'.format(job.name))
                os.makedirs(log_path, exist_ok=True)
                gitlab_inst.download_artifacts(job.id, log_path)

            job_info = {
                'id': job.id,
                'name': job.name,
                'tag_list': job.tag_list,
                'status': job.status,
                'stage': job.stage,
                'web_url': job.web_url,
                'commit_url': job.commit['web_url'],
                'log_path': log_path,
            }
            job_info_list.append(job_info)

    with open(os.path.join(result_path, 'index.json'), 'w') as f:
        f.write(json.dumps({'jobs': job_info_list}, indent=1, sort_keys=True))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('--path', '-p', default=TEST_RESULT_PATH, help='Path to save result files.')
    parser.add_argument('--include_retried', action='store_true', help='Including retried jobs.')
    args = parser.parse_args()

    download_all_test_results(args.path, args.include_retried)


if __name__ == '__main__':
    main()
