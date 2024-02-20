# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.temp_skip_ci(targets=['esp32s2', 'esp32p4'], reason='lack of runners with usb_host_flash_disk tag')
@pytest.mark.usb_host_flash_disk
def test_usb_host_msc_example(dut: Dut) -> None:
    # Check result of file_operations()
    dut.expect_exact("example: Read from file '/usb/esp/test.txt': 'Hello World!'")

    # Check result of speed_test()
    write_throughput = float(dut.expect(r'example: Write speed ([0-9]*[.]?[0-9]+) MiB')[1].decode())
    read_throughput = float(dut.expect(r'example: Read speed ([0-9]*[.]?[0-9]+) MiB')[1].decode())

    # These values should be updated for HS targets
    if write_throughput > 0.9:
        print('Write throughput put OK')
    else:
        print('write throughput too slow!')
    if read_throughput > 1.0:
        print('Read throughput put OK')
    else:
        print('Read throughput too slow!')
