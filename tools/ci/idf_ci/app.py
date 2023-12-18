# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import sys
import typing as t
from typing import Literal

from idf_build_apps import App, CMakeApp, json_to_app
from idf_ci.uploader import AppUploader, get_app_uploader


class IdfCMakeApp(CMakeApp):
    uploader: t.ClassVar[t.Optional['AppUploader']] = get_app_uploader()
    build_system: Literal['idf_cmake'] = 'idf_cmake'

    def _post_build(self) -> None:
        super()._post_build()

        if self.uploader:
            self.uploader.upload_app(self.build_path)


def dump_apps_to_txt(apps: t.List[App], output_filepath: str) -> None:
    with open(output_filepath, 'w') as fw:
        for app in apps:
            fw.write(app.model_dump_json() + '\n')


def import_apps_from_txt(input_filepath: str) -> t.List[App]:
    apps: t.List[App] = []
    with open(input_filepath) as fr:
        for line in fr:
            if line := line.strip():
                try:
                    apps.append(json_to_app(line, extra_classes=[IdfCMakeApp]))
                except Exception:  # noqa
                    print('Failed to deserialize app from line: %s' % line)
                    sys.exit(1)

    return apps
