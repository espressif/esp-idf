# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.temp_skip_ci(
    targets=['esp32s2', 'esp32p4'], reason='lack of runners with usb_host_flash_disk tag,p4 rev3 migration, IDF-14832'
)
@pytest.mark.usb_host_flash_disk
@idf_parametrize('target', ['esp32s2', 'esp32s3', 'esp32p4'], indirect=['target'])
def test_usb_host_lib_example(dut: Dut) -> None:
    # register client
    dut.expect_exact('CLASS: Registering Client')

    # expect device descriptor
    dut.expect_exact('CLASS: Getting device descriptor')

    # confirm device descriptor
    dut.expect_exact('*** Device descriptor ***')

    # expect configuration descriptor
    dut.expect_exact('CLASS: Getting config descriptor')

    # confirm configuration descriptor
    dut.expect_exact('*** Configuration descriptor ***')
