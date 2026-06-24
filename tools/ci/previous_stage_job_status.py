# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import sys

import requests

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('--stage', type=str, help='Stage name for check jobs status')
    args = parser.parse_args()

    GITLAB_TOKEN = os.getenv('ESPCI_TOKEN')
    GITLAB_HTTP_SERVER = os.getenv('GITLAB_HTTP_SERVER')
    CI_PROJECT_ID = os.getenv('CI_PROJECT_ID')
    CI_PIPELINE_ID = os.getenv('CI_PIPELINE_ID')

    api_path = f'projects/{CI_PROJECT_ID}/pipelines/{CI_PIPELINE_ID}/jobs?scope[]=failed&per_page=100'

    page = 0
    while True:
        response = requests.get(
            f'{GITLAB_HTTP_SERVER}/api/v4/{api_path}&page={page}',
            headers={'PRIVATE-TOKEN': GITLAB_TOKEN}
        )
        jobs = response.json()
        if not jobs:
            break

        for job in jobs:
            if job['stage'] == args.stage:
                print(f'Jobs from the previous stage {args.stage} should pass; otherwise, this job will fail.')
                sys.exit(1)
        page += 1
