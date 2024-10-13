# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os

import urllib3
from minio import Minio

from artifacts_handler import get_minio_client


def getenv(env_var: str) -> str:
    try:
        return os.environ[env_var]
    except KeyError as e:
        raise Exception(f'Environment variable {env_var} not set') from e


if __name__ == '__main__':
    client = get_minio_client()
    file_name = getenv('KNOWN_FAILURE_CASES_FILE_NAME')
    client.fget_object('ignore-test-result-files', file_name, file_name)
