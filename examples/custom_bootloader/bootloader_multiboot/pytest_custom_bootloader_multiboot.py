# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.app import IdfApp
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32c3', 'esp32s3'], indirect=['target'])
def test_custom_bootloader_multiboot_example(app: IdfApp, dut: Dut) -> None:
    # Wait for the bootloader menu before matching entry names so flashing logs do not
    # satisfy the expectations prematurely.
    dut.expect_exact('Please select a partition to boot.')
    dut.expect_exact('FOUND 3 BOOT PARTITIONS:')
    dut.expect_exact('main')
    dut.expect_exact('hello_world')
    dut.expect_exact('console')
    # Send "down arrow" signal to select the second image, which should be `hello_world`
    dut.write('\x1b[B')
    # Send Enter to validate and start the image
    dut.write('\n')
    # Make sure the example booted properly
    dut.expect('Hello world!')
