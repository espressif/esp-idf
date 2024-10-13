# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.temp_skip_ci(targets=['esp32s2'], reason='lack of runners with usb_host_flash_disk tag')
@pytest.mark.usb_host_flash_disk
def test_usb_host_msc_example(dut: Dut) -> None:
    # Get wMaxPacketSize to get USB device speed
    max_packet_size = int(dut.expect(r'wMaxPacketSize (\d{2,3})')[1].decode())

    # Check result of file_operations()
    dut.expect_exact("example: Read from file '/usb/esp/test.txt': 'Hello World!'")

    # Check result of speed_test()
    write_throughput = float(dut.expect(r'example: Write speed ([0-9]*[.]?[0-9]+) MiB')[1].decode())
    read_throughput = float(dut.expect(r'example: Read speed ([0-9]*[.]?[0-9]+) MiB')[1].decode())

    # Set write and read throughput limits
    if (max_packet_size == 512):        # wMaxPacketSize = 512 for HS
        write_throughput_limit = 4.9
        read_throughput_limit = 11.5
    else:                               # wMaxPacketSize = 64 for FS
        write_throughput_limit = 0.9
        read_throughput_limit = 1.0

    # These values should be updated for HS targets
    if write_throughput > write_throughput_limit:
        print('Write throughput put OK')
    else:
        print('write throughput too slow!')
    if read_throughput > read_throughput_limit:
        print('Read throughput put OK')
    else:
        print('Read throughput too slow!')
