# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import subprocess
import time

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.temp_skip_ci(targets=['esp32s3'], reason='lack of runners with usb_device tag')
@pytest.mark.usb_device
@idf_parametrize('target', ['esp32s2', 'esp32s3'], indirect=['target'])
def test_usb_device_ncm_example(dut: Dut) -> None:
    netif_mac = dut.expect(
        r'Network interface HW address: '
        r'([0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:'
        r'[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2})'
    )
    netif_mac = netif_mac.group(1).decode('utf-8')
    dut.expect_exact('USB NCM and WiFi initialized and started')
    dut.expect_exact('Returned from app_main()')
    time.sleep(2)  # Wait 2s for the network interface to appear
    out_bytes = subprocess.check_output('ifconfig', shell=True, timeout=5)
    out_str = out_bytes.decode('utf-8')
    print('expected network interface HW address: ', netif_mac)
    print('ifconfig command output:\n', out_str)
    if netif_mac in out_str:
        print("NCM device's MAC address {} was found in system network interfaces".format(netif_mac))
    else:
        raise AssertionError('NCM device not found in network interface list')
