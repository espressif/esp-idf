# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os

import pytest
from artifacts_handler import ArtifactType
from idf_ci_utils import IDF_PATH
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize('config', ['32K'], indirect=True)
@idf_parametrize('target', ['esp32c6', 'esp32h2'], indirect=['target'])
def test_app_mmu_page_size_32k_and_bootloader_mmu_page_size_64k(dut: Dut, app_downloader, config) -> None:  # type: ignore
    dut.expect('App is running')

    assert '32K' in config
    app_config = config.replace('32K', '64K')

    path_to_mmu_page_size_64k_build = os.path.join(dut.app.app_path, f'build_{dut.target}_{app_config}')
    if app_downloader:
        app_downloader.download_app(
            os.path.relpath(path_to_mmu_page_size_64k_build, IDF_PATH), ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES
        )

    dut.serial.bootloader_flash(path_to_mmu_page_size_64k_build)
    dut.expect('MMU page size mismatch')
    dut.expect('App is running')
    dut.expect('Partition test done')
    dut.expect('Image verified successfully', timeout=30)


@pytest.mark.generic
@pytest.mark.parametrize('config', ['64K'], indirect=True)
@idf_parametrize('target', ['esp32c6', 'esp32h2'], indirect=['target'])
def test_app_mmu_page_size_64k_and_bootloader_mmu_page_size_32k(dut: Dut, app_downloader, config) -> None:  # type: ignore
    dut.expect('App is running')

    assert '64K' in config
    app_config = config.replace('64K', '32K')

    path_to_mmu_page_size_32k_build = os.path.join(dut.app.app_path, f'build_{dut.target}_{app_config}')
    if app_downloader:
        app_downloader.download_app(
            os.path.relpath(path_to_mmu_page_size_32k_build, IDF_PATH), ArtifactType.BUILD_DIR_WITHOUT_MAP_AND_ELF_FILES
        )

    dut.serial.bootloader_flash(path_to_mmu_page_size_32k_build)
    dut.expect('MMU page size mismatch')
    dut.expect('App is running')
    dut.expect('Partition test done')
    dut.expect('Image verified successfully', timeout=30)
