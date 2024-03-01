# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os

import urllib3
from minio import Minio


def getenv(env_var: str) -> str:
    try:
        return os.environ[env_var]
    except KeyError as e:
        raise Exception(f'Environment variable {env_var} not set') from e


def get_minio_client() -> Minio:
    return Minio(
        getenv('IDF_S3_SERVER').replace('https://', ''),
        access_key=getenv('IDF_S3_ACCESS_KEY'),
        secret_key=getenv('IDF_S3_SECRET_KEY'),
        http_client=urllib3.PoolManager(
            num_pools=10,
            timeout=urllib3.Timeout.DEFAULT_TIMEOUT,
            retries=urllib3.Retry(
                total=5,
                backoff_factor=0.2,
                status_forcelist=[500, 502, 503, 504],
            ),
        ),
    )


if __name__ == '__main__':
    client = get_minio_client()
    file_name = getenv('KNOWN_FAILURE_CASES_FILE_NAME')
    client.fget_object('ignore-test-result-files', file_name, file_name)
