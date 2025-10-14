# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os
import re

import pytest
from idf_ci_utils import IDF_PATH
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize('config', ['multicore', 'multicore_psram'], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_CPU_CORES_NUM > 1'), indirect=['target'])
def test_multicore_app_and_unicore_bootloader(dut: Dut, app_downloader, config) -> None:  # type: ignore
    dut.expect('Multicore bootloader')
    dut.expect('Multicore app')
    dut.expect('App is running')

    assert 'multicore' in config
    app_config = config.replace('multicore', 'unicore')

    path_to_unicore_build = os.path.join(dut.app.app_path, f'build_{dut.target}_{app_config}')
    if app_downloader:
        app_downloader.download_app(
            os.path.relpath(path_to_unicore_build, IDF_PATH),
            'flash',
        )

    dut.serial.bootloader_flash(path_to_unicore_build)
    dut.expect('Unicore bootloader')
    dut.expect('Multicore app')
    if 'psram' in config:
        dut.expect(re.compile(r'Adding pool of \d+K of PSRAM memory to heap allocator'))
    dut.expect('App is running')
    dut.expect('NVS test done')


@pytest.mark.generic
@pytest.mark.parametrize('config', ['unicore', 'unicore_psram'], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_CPU_CORES_NUM > 1'), indirect=['target'])
def test_unicore_app_and_multicore_bootloader(dut: Dut, app_downloader, config) -> None:  # type: ignore
    dut.expect('Unicore bootloader')
    dut.expect('Unicore app')
    dut.expect('App is running')

    assert 'unicore' in config
    app_config = config.replace('unicore', 'multicore')

    path_to_multicore_build = os.path.join(dut.app.app_path, f'build_{dut.target}_{app_config}')
    if app_downloader:
        app_downloader.download_app(
            os.path.relpath(path_to_multicore_build, IDF_PATH),
            'flash',
        )

    dut.serial.bootloader_flash(path_to_multicore_build)
    dut.expect('Multicore bootloader')
    dut.expect('Unicore app')
    if 'psram' in config:
        dut.expect(re.compile(r'Adding pool of \d+K of PSRAM memory to heap allocator'))
    dut.expect('App is running')
    dut.expect('NVS test done')
