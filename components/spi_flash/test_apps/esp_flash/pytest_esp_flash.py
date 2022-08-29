# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c2
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
        'flash_qio',
    ],
    indirect=True,
)
def test_esp_flash(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[esp_flash]')
    dut.expect_unity_test_output()


@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c2
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'rom_impl',
    ],
    indirect=True,
)
def test_esp_flash_rom(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[esp_flash]')
    dut.expect_unity_test_output()


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c2
@pytest.mark.flash_mutli
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
def test_esp_flash_multi(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[esp_flash_multi]')
    dut.expect_unity_test_output()
