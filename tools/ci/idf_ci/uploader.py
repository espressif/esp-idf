# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import abc
import glob
import os
import typing as t
from datetime import timedelta
from zipfile import ZIP_DEFLATED
from zipfile import ZipFile

import minio
from artifacts_handler import ArtifactType
from artifacts_handler import get_minio_client
from artifacts_handler import getenv
from idf_build_apps import App
from idf_build_apps.utils import rmdir
from idf_ci_utils import IDF_PATH
from idf_pytest.constants import DEFAULT_BUILD_LOG_FILENAME


class AppDownloader:
    ALL_ARTIFACT_TYPES = [ArtifactType.MAP_AND_ELF_FILES, ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES]

    @abc.abstractmethod
    def _download_app(self, app_build_path: str, artifact_type: ArtifactType) -> None:
        pass

    def download_app(self, app_build_path: str, artifact_type: t.Optional[ArtifactType] = None) -> None:
        """
        Download the app
        :param app_build_path: the path to the build directory
        :param artifact_type: if not specify, download all types of artifacts
        :return: None
        """
        if not artifact_type:
            for _artifact_type in self.ALL_ARTIFACT_TYPES:
                self._download_app(app_build_path, _artifact_type)
        else:
            self._download_app(app_build_path, artifact_type)


class AppUploader(AppDownloader):
    TYPE_PATTERNS_DICT = {
        ArtifactType.MAP_AND_ELF_FILES: [
            'bootloader/*.map',
            'bootloader/*.elf',
            '*.map',
            '*.elf',
        ],
        ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES: [
            '*.bin',
            'bootloader/*.bin',
            'partition_table/*.bin',
            'flasher_args.json',
            'flash_project_args',
            'config/sdkconfig.json',
            'sdkconfig',
            'project_description.json',
        ],
        ArtifactType.LOGS: [
            DEFAULT_BUILD_LOG_FILENAME,
        ],
        ArtifactType.SIZE_REPORTS: [
            'size.json',
        ],
    }

    def __init__(self, pipeline_id: t.Union[str, int, None] = None) -> None:
        self.pipeline_id = str(pipeline_id or '1')

        self._client = get_minio_client()

    def get_app_object_name(self, app_path: str, zip_name: str, artifact_type: ArtifactType) -> str:
        return f'{self.pipeline_id}/{artifact_type.value}/{app_path}/{zip_name}'

    def _upload_app(self, app_build_path: str, artifact_type: ArtifactType) -> bool:
        app_path, build_dir = os.path.split(app_build_path)
        zip_filename = f'{build_dir}.zip'

        has_file = False
        with ZipFile(
            zip_filename,
            'w',
            compression=ZIP_DEFLATED,
            # 1 is the fastest compression level
            # the size differs not much between 1 and 9
            compresslevel=1,
        ) as zw:
            for pattern in self.TYPE_PATTERNS_DICT[artifact_type]:
                for file in glob.glob(os.path.join(app_build_path, pattern), recursive=True):
                    zw.write(file)
                    has_file = True

        uploaded = False
        try:
            if has_file:
                obj_name = self.get_app_object_name(app_path, zip_filename, artifact_type)
                self._client.fput_object(getenv('IDF_S3_BUCKET'), obj_name, zip_filename)
                uploaded = True
        finally:
            os.remove(zip_filename)
            return uploaded

    def upload_app(self, app_build_path: str, artifact_type: t.Optional[ArtifactType] = None) -> None:
        uploaded = False
        if not artifact_type:
            upload_types: t.Iterable[ArtifactType] = self.TYPE_PATTERNS_DICT.keys()
        else:
            upload_types = [artifact_type]

        print(f'Uploading {app_build_path} {[k.value for k in upload_types]} to minio server')
        for upload_type in upload_types:
            uploaded |= self._upload_app(app_build_path, upload_type)

        if uploaded:
            rmdir(app_build_path, exclude_file_patterns=DEFAULT_BUILD_LOG_FILENAME)

    def _download_app(self, app_build_path: str, artifact_type: ArtifactType) -> None:
        app_path, build_dir = os.path.split(app_build_path)
        zip_filename = f'{build_dir}.zip'

        # path are relative to IDF_PATH
        current_dir = os.getcwd()
        os.chdir(IDF_PATH)
        try:
            obj_name = self.get_app_object_name(app_path, zip_filename, artifact_type)
            print(f'Downloading {obj_name}')
            try:
                try:
                    self._client.stat_object(getenv('IDF_S3_BUCKET'), obj_name)
                except minio.error.S3Error as e:
                    raise RuntimeError(
                        f'No such file on minio server: {obj_name}. '
                        f'Probably the build failed or the artifacts got expired. '
                        f'Full error message: {str(e)}'
                    )
                else:
                    self._client.fget_object(getenv('IDF_S3_BUCKET'), obj_name, zip_filename)
                    print(f'Downloaded to {zip_filename}')
            except minio.error.S3Error as e:
                raise RuntimeError('Shouldn\'t happen, please report this bug in the CI channel' + str(e))

            with ZipFile(zip_filename, 'r') as zr:
                zr.extractall()

            os.remove(zip_filename)
        finally:
            os.chdir(current_dir)

    def get_app_presigned_url(self, app: App, artifact_type: ArtifactType) -> str:
        obj_name = self.get_app_object_name(app.app_dir, f'{app.build_dir}.zip', artifact_type)
        try:
            self._client.stat_object(
                getenv('IDF_S3_BUCKET'),
                obj_name,
            )
        except minio.error.S3Error:
            return ''
        else:
            return self._client.get_presigned_url(  # type: ignore
                'GET', getenv('IDF_S3_BUCKET'), obj_name, expires=timedelta(days=4)
            )


def get_app_uploader() -> t.Optional['AppUploader']:
    if parent_pipeline_id := os.getenv('PARENT_PIPELINE_ID'):
        return AppUploader(parent_pipeline_id)

    return None
