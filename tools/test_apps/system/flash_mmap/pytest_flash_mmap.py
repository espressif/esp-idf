# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import os
import pathlib

import pytest
from pytest_embedded import Dut

# For F4R8 board (Quad Flash and Octal PSRAM)
MSPI_F4R8_configs = [p.name.replace('sdkconfig.ci.', '') for p in pathlib.Path(os.path.dirname(__file__)).glob('sdkconfig.ci.f4r8*')]


@pytest.mark.esp32s3
@pytest.mark.MSPI_F4R8
@pytest.mark.parametrize('config', MSPI_F4R8_configs, indirect=True)
def test_mmap_flash4_psram8(dut: Dut) -> None:
    dut.expect_exact('flash mmap test success', timeout=300)


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.parametrize('config', [
    'psram',
], indirect=True)
def test_mmap_psram(dut: Dut) -> None:
    dut.expect_exact('flash mmap test success', timeout=300)


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.parametrize('config', [
    'release',
], indirect=True)
def test_mmap(dut: Dut) -> None:
    dut.expect_exact('flash mmap test success', timeout=300)
