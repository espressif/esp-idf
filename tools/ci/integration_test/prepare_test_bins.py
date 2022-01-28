#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import os

import gitlab
import gitlab_api
from AutoTestScript.RunnerConfigs.Config import Config

SSC_BUILD_JOB_MAP = {
    'ESP32': 'build_ssc_esp32',
    'ESP32C3': 'build_ssc_esp32c3',
}
NEEDED_FILES = [
    'flasher_args.json',
    'bootloader/bootloader.bin',
    'partition_table/partition-table.bin',
    'ssc.bin',
    'ssc.elf',
]
IDF_PATH = os.environ.get('IDF_PATH')


def try_to_download_artifacts(bin_path: str) -> None:
    '''
    bin_path: "SSC/ssc_bin/ESP32[C3]/SSC[_APP]"
    '''
    project_id = os.getenv('CI_PROJECT_ID')
    pipeline_id = os.getenv('CI_PIPELINE_ID')
    gitlab_inst = gitlab_api.Gitlab(project_id)
    build_job_name = SSC_BUILD_JOB_MAP[bin_path.split('/')[-2]]
    job_list = gitlab_inst.find_job_id(build_job_name, pipeline_id=pipeline_id)
    files_to_download = [os.path.join(bin_path, f) for f in NEEDED_FILES]
    for job_info in job_list:
        try:
            gitlab_inst.download_artifact(job_info['id'], files_to_download, IDF_PATH)
            print('Downloaded {} from {}'.format(bin_path, job_info['id']))
            break
        except gitlab.exceptions.GitlabError as e:
            if e.response_code == 404:
                continue
            raise


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        'test_config_file',
        help='The test config file to be used.'
    )
    args = parser.parse_args()

    configs = Config.parse(args.test_config_file)
    test_bin_paths = configs.get_bin_paths()

    for _path in test_bin_paths:
        if os.path.exists(_path):
            continue
        relative_path = os.path.relpath(_path, IDF_PATH)
        try_to_download_artifacts(relative_path)


if __name__ == '__main__':
    main()
