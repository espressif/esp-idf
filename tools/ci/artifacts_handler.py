# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import fnmatch
import glob
import os
import typing as t
import zipfile
from enum import Enum
from pathlib import Path
from zipfile import ZipFile

import urllib3
from idf_ci_utils import sanitize_job_name
from idf_pytest.constants import DEFAULT_BUILD_LOG_FILENAME
from minio import Minio


class ArtifactType(str, Enum):
    MAP_AND_ELF_FILES = 'map_and_elf_files'
    BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES = 'build_dir_without_map_and_elf_files'

    LOGS = 'logs'
    SIZE_REPORTS = 'size_reports'
    JUNIT_REPORTS = 'junit_reports'
    MODIFIED_FILES_AND_COMPONENTS_REPORT = 'modified_files_and_components_report'


TYPE_PATTERNS_DICT = {
    ArtifactType.MAP_AND_ELF_FILES: [
        '**/build*/bootloader/*.map',
        '**/build*/bootloader/*.elf',
        '**/build*/*.map',
        '**/build*/*.elf',
    ],
    ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES: [
        f'**/build*/{DEFAULT_BUILD_LOG_FILENAME}',
        '**/build*/*.bin',
        '**/build*/bootloader/*.bin',
        '**/build*/partition_table/*.bin',
        '**/build*/flasher_args.json',
        '**/build*/flash_project_args',
        '**/build*/config/sdkconfig.json',
        '**/build*/sdkconfig',
        '**/build*/project_description.json',
        'list_job*.txt',
    ],
    ArtifactType.LOGS: [
        f'**/build*/{DEFAULT_BUILD_LOG_FILENAME}',
    ],
    ArtifactType.SIZE_REPORTS: [
        '**/build*/size.json',
        'size_info.txt',
    ],
    ArtifactType.JUNIT_REPORTS: [
        'XUNIT_RESULT*.xml',
        'build_summary*.xml',
    ],
    ArtifactType.MODIFIED_FILES_AND_COMPONENTS_REPORT: [
        'pipeline.env',
    ],
}


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


def _download_files(
    pipeline_id: int,
    *,
    artifact_type: t.Optional[ArtifactType] = None,
    job_name: t.Optional[str] = None,
    job_id: t.Optional[int] = None,
) -> None:
    if artifact_type:
        prefix = f'{pipeline_id}/{artifact_type.value}/'
    else:
        prefix = f'{pipeline_id}/'

    for obj in client.list_objects(getenv('IDF_S3_BUCKET'), prefix=prefix, recursive=True):
        obj_name = obj.object_name
        obj_p = Path(obj_name)
        # <pipeline_id>/<action_type>/<job_name>/<job_id>.zip
        if len(obj_p.parts) != 4:
            print(f'Invalid object name: {obj_name}')
            continue

        if job_name:
            # could be a pattern
            if not fnmatch.fnmatch(obj_p.parts[2], job_name):
                print(f'Job name {job_name} does not match {obj_p.parts[2]}')
                continue

        if job_id:
            if obj_p.parts[3] != f'{job_id}.zip':
                print(f'Job ID {job_id} does not match {obj_p.parts[3]}')
                continue

        client.fget_object(getenv('IDF_S3_BUCKET'), obj_name, obj_name)
        print(f'Downloaded {obj_name}')

        if obj_name.endswith('.zip'):
            with ZipFile(obj_name, 'r') as zr:
                zr.extractall()
            print(f'Extracted {obj_name}')

        os.remove(obj_name)


def _upload_files(
    pipeline_id: int,
    *,
    artifact_type: ArtifactType,
    job_name: str,
    job_id: str,
) -> None:
    has_file = False
    with ZipFile(
        f'{job_id}.zip',
        'w',
        compression=zipfile.ZIP_DEFLATED,
        # 1 is the fastest compression level
        # the size differs not much between 1 and 9
        compresslevel=1,
    ) as zw:
        for pattern in TYPE_PATTERNS_DICT[artifact_type]:
            for file in glob.glob(pattern, recursive=True):
                zw.write(file)
                has_file = True

    try:
        if has_file:
            obj_name = f'{pipeline_id}/{artifact_type.value}/{sanitize_job_name(job_name)}/{job_id}.zip'
            print(f'Created archive file: {job_id}.zip, uploading as {obj_name}')

            client.fput_object(getenv('IDF_S3_BUCKET'), obj_name, f'{job_id}.zip')
            url = client.get_presigned_url('GET', getenv('IDF_S3_BUCKET'), obj_name)
            print(f'Please download the archive file which includes {artifact_type.value} from {url}')
    finally:
        os.remove(f'{job_id}.zip')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Download or upload files from/to S3, the object name would be '
        '[PIPELINE_ID]/[ACTION_TYPE]/[JOB_NAME]/[JOB_ID].zip.'
        '\n'
        'For example: 123456/binaries/build_pytest_examples_esp32/123456789.zip',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )

    common_args = argparse.ArgumentParser(add_help=False, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    common_args.add_argument('--pipeline-id', type=int, help='Pipeline ID')
    common_args.add_argument(
        '--type', type=str, nargs='+', choices=[a.value for a in ArtifactType], help='Types of files to download'
    )

    action = parser.add_subparsers(dest='action', help='Download or Upload')
    download = action.add_parser('download', help='Download files from S3', parents=[common_args])
    upload = action.add_parser('upload', help='Upload files to S3', parents=[common_args])

    download.add_argument('--job-name', type=str, help='Job name pattern')
    download.add_argument('--job-id', type=int, help='Job ID')

    upload.add_argument('--job-name', type=str, help='Job name')
    upload.add_argument('--job-id', type=int, help='Job ID')

    args = parser.parse_args()

    client = get_minio_client()

    ci_pipeline_id = args.pipeline_id or getenv('CI_PIPELINE_ID')  # required
    if args.action == 'download':
        method = _download_files
        ci_job_name = args.job_name  # optional
        ci_job_id = args.job_id  # optional
    else:
        method = _upload_files  # type: ignore
        ci_job_name = args.job_name or getenv('CI_JOB_NAME')  # required
        ci_job_id = args.job_id or getenv('CI_JOB_ID')  # required

    if args.type:
        types = [ArtifactType(t) for t in args.type]
    else:
        types = list(ArtifactType)

    print(f'{"Pipeline ID":15}: {ci_pipeline_id}')
    if ci_job_name:
        print(f'{"Job name":15}: {ci_job_name}')
    if ci_job_id:
        print(f'{"Job ID":15}: {ci_job_id}')

    for _t in types:
        method(ci_pipeline_id, artifact_type=_t, job_name=ci_job_name, job_id=ci_job_id)  # type: ignore
