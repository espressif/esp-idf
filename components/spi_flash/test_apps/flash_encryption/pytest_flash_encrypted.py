# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.flash_encryption
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
def test_flash_encryption(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output()


@pytest.mark.esp32s3
@pytest.mark.flash_encryption_f4r8
@pytest.mark.parametrize(
    'config',
    [
        'release_f4r8',
    ],
    indirect=True,
)
def test_flash_encryption_f4r8(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output()


@pytest.mark.esp32s3
@pytest.mark.flash_encryption_f8r8
@pytest.mark.parametrize(
    'config',
    [
        'release_f8r8',
    ],
    indirect=True,
)
def test_flash_encryption_f8r8(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output()
