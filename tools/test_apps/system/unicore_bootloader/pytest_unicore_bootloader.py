# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import os
import re

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize('config', ['multicore', 'multicore_psram'], indirect=True)
def test_multicore_app_and_unicore_bootloader(dut: Dut, config) -> None:  # type: ignore
    dut.expect('Multicore bootloader')
    dut.expect('Multicore app')
    dut.expect('App is running')

    assert 'multicore' in config
    app_config = config.replace('multicore', 'unicore')

    path_to_unicore_build = os.path.join(dut.app.app_path, f'build_{dut.target}_{app_config}')

    dut.serial.bootloader_flash(path_to_unicore_build)
    dut.expect('Unicore bootloader')
    dut.expect('Multicore app')
    if 'psram' in config:
        dut.expect(re.compile(rb'Adding pool of \d+K of PSRAM memory to heap allocator'))
    dut.expect('App is running')
    dut.expect('NVS test done\n')


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize('config', ['unicore', 'unicore_psram'], indirect=True)
def test_unicore_app_and_multicore_bootloader(dut: Dut, config) -> None:  # type: ignore
    dut.expect('Unicore bootloader')
    dut.expect('Unicore app')
    dut.expect('App is running')

    assert 'unicore' in config
    app_config = config.replace('unicore', 'multicore')

    path_to_multicore_build = os.path.join(dut.app.app_path, f'build_{dut.target}_{app_config}')

    dut.serial.bootloader_flash(path_to_multicore_build)
    dut.expect('Multicore bootloader')
    dut.expect('Unicore app')
    if 'psram' in config:
        dut.expect(re.compile(rb'Adding pool of \d+K of PSRAM memory to heap allocator'))
    dut.expect('App is running')
    dut.expect('NVS test done\n')
