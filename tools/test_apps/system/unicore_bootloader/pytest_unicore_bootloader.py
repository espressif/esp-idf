# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os

import pytest
from artifacts_handler import ArtifactType
from idf_ci_utils import IDF_PATH
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize('config', ['multicore'], indirect=True)
def test_multicore_app_and_unicore_bootloader(dut: Dut, app_downloader) -> None:  # type: ignore
    dut.expect('Multicore bootloader')
    dut.expect('Multicore app')
    dut.expect('App is running')

    path_to_unicore_build = os.path.join(dut.app.app_path, f'build_{dut.target}_unicore')
    if app_downloader:
        app_downloader.download_app(
            os.path.relpath(path_to_unicore_build, IDF_PATH), ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES
        )

    dut.serial.bootloader_flash(path_to_unicore_build)
    dut.expect('Unicore bootloader')
    dut.expect('Multicore app')
    dut.expect('App is running')


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize('config', ['unicore'], indirect=True)
def test_unicore_app_and_multicore_bootloader(dut: Dut, app_downloader) -> None:  # type: ignore
    dut.expect('Unicore bootloader')
    dut.expect('Unicore app')
    dut.expect('App is running')

    path_to_multicore_build = os.path.join(dut.app.app_path, f'build_{dut.target}_multicore')
    if app_downloader:
        app_downloader.download_app(
            os.path.relpath(path_to_multicore_build, IDF_PATH), ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES
        )

    dut.serial.bootloader_flash(path_to_multicore_build)
    dut.expect('Multicore bootloader')
    dut.expect('Unicore app')
    dut.expect('App is running')
