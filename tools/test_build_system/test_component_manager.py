# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os.path
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc


@pytest.mark.test_app_copy('examples/get-started/blink')
def test_dependency_lock(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    with open(test_app_copy / 'CMakeLists.txt', 'r+') as fw:
        data = fw.read()
        fw.seek(0)
        fw.write(
            data.replace(
                'project(blink)',
                'idf_build_set_property(DEPENDENCIES_LOCK dependencies.lock.${IDF_TARGET})\nproject(blink)',
            )
        )

    idf_py('fullclean')
    idf_py('reconfigure')
    assert os.path.isfile(test_app_copy / 'dependencies.lock.esp32')
    assert not os.path.isfile(test_app_copy / 'dependencies.lock')
