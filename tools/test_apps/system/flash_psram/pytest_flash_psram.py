# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import os
import pathlib

import pytest
from pytest_embedded import Dut

MSPI_F8R8_configs = [p.name.replace('sdkconfig.ci.', '') for p in pathlib.Path(os.path.dirname(__file__)).glob('sdkconfig.ci.f8r8*')]


@pytest.mark.esp32s3
@pytest.mark.MSPI_F8R8
@pytest.mark.parametrize('config', MSPI_F8R8_configs, indirect=True)
def test_flash8_psram8(dut: Dut) -> None:
    dut.expect_exact('flash psram test success')


# For F4R8 board (Quad Flash and Octal PSRAM)
MSPI_F4R8_configs = [p.name.replace('sdkconfig.ci.', '') for p in pathlib.Path(os.path.dirname(__file__)).glob('sdkconfig.ci.f4r8*')]


@pytest.mark.esp32s3
@pytest.mark.MSPI_F4R8
@pytest.mark.parametrize('config', MSPI_F4R8_configs, indirect=True)
def test_flash4_psram8(dut: Dut) -> None:
    dut.expect_exact('flash psram test success')


# For F4R4 board (Quad Flash and Quad PSRAM)
MSPI_F4R4_configs = [p.name.replace('sdkconfig.ci.', '') for p in pathlib.Path(os.path.dirname(__file__)).glob('sdkconfig.ci.f4r4*')]


@pytest.mark.esp32s3
@pytest.mark.MSPI_F4R4
@pytest.mark.parametrize('config', MSPI_F4R4_configs, indirect=True)
def test_flash4_psram4(dut: Dut) -> None:
    dut.expect_exact('flash psram test success')
