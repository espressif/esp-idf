# SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
from typing import Tuple

import pytest
from pytest_embedded_idf import IdfDut


@pytest.mark.esp32
@pytest.mark.sdio_master_slave
@pytest.mark.parametrize(
    'count, app_path',
    [
        (2, f'{os.path.join(os.path.dirname(__file__), "host")}|{os.path.join(os.path.dirname(__file__), "slave")}'),
    ],
    indirect=True,
)
def test_example_sdio_communication(dut: Tuple[IdfDut, IdfDut]) -> None:
    """
    Configurations
    host = host -> slave = slave
    should be in the same group of devices, otherwise may meet download issue
    group1: (Wroom-32 Series or PICO-D4 modules: PICO-Kit, DevKitC, WroverKit v2 or earlier)
    group2: (Wrover module: WroverKit v3)

    GPIO14->GPIO14
    GPIO15->GPIO15
    GPIO2->GPIO2
    GPIO4->GPIO4
    GND->GND

    VDD3.3 -> GPIO13 if slave uses WroverKit v3

    or use sdio test board, which has two wrover modules connect to a same FT3232
    Assume that first dut is host and second is slave
    """
    host = dut[0]
    slave = dut[1]

    host.pexpect_proc.timeout = 5
    slave.pexpect_proc.timeout = 5

    host.expect_exact('host ready, start initializing slave...')
    host.expect_exact('0a 0d 10 13 16 19 1c 1f  22 25 28 2b 2e 31 34 37')
    host.expect_exact('3a 3d 40 43 46 49 4c 4f  52 55 58 5b 5e 61 64 67')
    host.expect_exact('6a 6d 70 73 76 79 7c 7f  82 85 88 8b 8e 91 94 97')
    host.expect_exact('9a 9d a0 a3 a6 a9 ac af  b2 b5 b8 bb')

    slave.expect_exact('================ JOB_WRITE_REG ================')
    slave.expect_exact('0a 0d 10 13 16 19 1c 1f  22 25 28 2b 2e 31 34 37')
    slave.expect_exact('3a 3d 40 43 46 49 4c 4f  52 55 58 5b 5e 61 64 67')
    slave.expect_exact('6a 6d 70 73 76 79 7c 7f  82 85 88 8b 8e 91 94 97')
    slave.expect_exact('9a 9d a0 a3 a6 a9 ac af  b2 b5 b8 bb')

    host.expect_exact('host int: 0')
    host.expect_exact('host int: 1')
    host.expect_exact('host int: 2')
    host.expect_exact('host int: 3')
    host.expect_exact('host int: 4')
    host.expect_exact('host int: 5')
    host.expect_exact('host int: 6')
    host.expect_exact('host int: 7')
    host.expect_exact('host int: 0')
    host.expect_exact('host int: 1')
    host.expect_exact('host int: 2')
    host.expect_exact('host int: 3')
    host.expect_exact('host int: 4')
    host.expect_exact('host int: 5')
    host.expect_exact('host int: 6')
    host.expect_exact('host int: 7')

    slave.expect_exact('================ JOB_SEND_INT ================')
    slave.expect_exact('================ JOB_SEND_INT ================')

    host.expect_exact('send packet length: 6')
    host.expect_exact('send packet length: 12')
    host.expect_exact('send packet length: 1024')
    host.expect_exact('send packet length: 512')
    host.expect_exact('send packet length: 3')

    slave.expect_exact('Packet received, len: 6')
    slave.expect_exact('Buffer 0, len: 6')
    slave.expect_exact('Packet received, len: 12')
    slave.expect_exact('Buffer 0, len: 12')
    # 1024
    slave.expect_exact('Packet received, len: 1024')
    for i in range(8):
        slave.expect_exact(f'Buffer {i}, len: 128')
    # 512
    slave.expect_exact('Packet received, len: 512')
    for i in range(4):
        slave.expect_exact(f'Buffer {i}, len: 128')
    # 3
    slave.expect_exact('Packet received, len: 3')
    slave.expect_exact('Buffer 0, len: 3')

    # same as slave received
    host.expect_exact('receive data, size: 6')
    host.expect_exact('receive data, size: 12')
    for _ in range(8):
        host.expect_exact('receive data, size: 128')
    for _ in range(4):
        host.expect_exact('receive data, size: 128')
    host.expect_exact('receive data, size: 3')

    # the last valid line of one round
    host.expect_exact('aa af b4')

    # the first 2 lines of the second round
    host.expect_exact('46 4b 50 55 5a 5f')
    host.expect_exact('6e 73 78 7d 82 87 8c 91  96 9b a0 a5')
