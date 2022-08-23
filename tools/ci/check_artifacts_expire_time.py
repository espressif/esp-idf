#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# internal use only
# check if expire time is set for all artifacts

import os

import yaml

IDF_PATH = os.getenv('IDF_PATH')
if not IDF_PATH:
    print('Please set IDF_PATH before running this script')
    raise SystemExit(-1)

GITLAB_CONFIG_FILE = os.path.join(IDF_PATH, '.gitlab-ci.yml')


def check_artifacts_expire_time() -> None:
    with open(GITLAB_CONFIG_FILE, 'r') as f:
        config = yaml.load(f, Loader=yaml.FullLoader)

    # load files listed in `include`
    if 'include' in config:
        for _file in config['include']:
            with open(os.path.join(IDF_PATH or '', _file)) as f:
                config.update(yaml.load(f, Loader=yaml.FullLoader))

    print('expire time for jobs:')
    errors = []

    job_names = list(config.keys())
    job_names.sort()

    for job_name in job_names:
        try:
            if 'expire_in' not in config[job_name]['artifacts']:
                errors.append(job_name)
            else:
                print('{}: {}'.format(job_name, config[job_name]['artifacts']['expire_in']))
        except (KeyError, TypeError):
            # this is not job, or the job does not have artifacts
            pass

    if errors:
        print('\n\nThe following jobs did not set expire time for its artifacts')
        for error in errors:
            print(error)
        raise SystemExit(-2)


if __name__ == '__main__':
    check_artifacts_expire_time()
