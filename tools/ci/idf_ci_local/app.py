# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import subprocess
import sys
import typing as t
from pathlib import Path

from idf_build_apps import CMakeApp
from idf_build_apps.constants import BuildStatus
from idf_build_apps.utils import rmdir
from idf_ci_utils import APP_EXTRA_S3_ARTIFACT_TYPE
from idf_ci_utils import idf_relpath
from metrics.size_metrics import collect_build_metrics

_SIZE_METRICS_CONFIG_PATH = Path(__file__).parent.parent / 'metrics' / 'size_metrics' / 'size_metrics_config.yml'


class IdfCMakeApp(CMakeApp):
    build_system: t.Literal['idf_cmake'] = 'idf_cmake'

    def _initialize_hook(self, **kwargs: t.Any) -> None:
        # ensure this env var exists
        os.environ['IDF_TARGET'] = self.target

        super()._initialize_hook(**kwargs)

    def _post_build(self) -> None:
        super()._post_build()

        # Size metrics are only consumed in CI (aggregated by a downstream job),
        # so skip the extraction entirely for local builds.
        if os.getenv('CI_JOB_ID'):
            # Never fail the build because of size metrics.
            try:
                metrics = collect_build_metrics.build_raw_metrics_for_build_dir(
                    Path(self.build_path),
                    idf_relpath(self.app_dir),
                    self.target,
                    self.config_name,
                    collect_build_metrics.load_size_metrics_config(_SIZE_METRICS_CONFIG_PATH),
                )
                if metrics is not None:
                    collect_build_metrics.write_raw_metrics_artifact(Path(self.build_path), metrics)
            except Exception as e:
                print(f'size-metrics: extraction failed for {self.build_path}: {e}')

            upload_commands = [
                [
                    'idf-ci',
                    'gitlab',
                    'upload-artifacts',
                    self.app_dir,
                    '--build-dir',
                    self.build_dir,
                ],
                [
                    'idf-ci',
                    'gitlab',
                    'upload-artifacts',
                    self.app_dir,
                    '--type',
                    APP_EXTRA_S3_ARTIFACT_TYPE,
                ],
            ]

            for command in upload_commands:
                result = subprocess.run(
                    command,
                    stdout=sys.stdout,
                    stderr=sys.stderr,
                )
                if result.returncode != 0:
                    self.build_status = BuildStatus.FAILED
                    self.build_comment = 'Failed to upload artifacts'
                    break

            rmdir(
                self.build_path,
                exclude_file_patterns=['build_log.txt', 'size*.json', 'build_size_metrics.json'],
            )
