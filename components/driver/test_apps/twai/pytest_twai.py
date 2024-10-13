# SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging
import subprocess
from time import sleep

import pytest
from can import Bus
from can import Message
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
def test_twai_self(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='twai-loop-back')


@pytest.fixture(name='socket_can', scope='module')
def fixture_create_socket_can() -> Bus:
    # Set up the socket CAN with the bitrate
    start_command = 'sudo ip link set can0 up type can bitrate 250000 restart-ms 100'
    stop_command = 'sudo ip link set can0 down'
    subprocess.run(start_command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    bus = Bus(interface='socketcan', channel='can0', bitrate=250000)
    yield bus   # test invoked here
    bus.shutdown()
    subprocess.run(stop_command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.twai_std
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
    ],
    indirect=True,
)
def test_twai_listen_only(dut: Dut, socket_can: Bus) -> None:
    dut.serial.hard_reset()
    dut.expect_exact('Press ENTER to see the list of tests')

    # TEST_CASE("twai_listen_only", "[twai]")
    dut.write('"twai_listen_only"')

    # wait the DUT to block at the receive API
    sleep(0.03)

    message = Message(
        arbitration_id=0x123,
        is_extended_id=False,
        data=[0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88],
    )
    socket_can.send(message, timeout=0.2)
    dut.expect_unity_test_output()


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.twai_std
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
def test_twai_remote_request(dut: Dut, socket_can: Bus) -> None:
    dut.serial.hard_reset()
    dut.expect_exact('Press ENTER to see the list of tests')

    # TEST_CASE("twai_remote_request", "[twai]")
    dut.write('"twai_remote_request"')

    while True:
        req = socket_can.recv(timeout=0.2)
        # wait for the remote request frame
        if req is not None and req.is_remote_frame:
            break

    logging.info(f'Received message: {req}')

    reply = Message(
        arbitration_id=req.arbitration_id,
        is_extended_id=req.is_extended_id,
        data=[0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80],
    )
    socket_can.send(reply, timeout=0.2)
    print('send', reply)

    dut.expect_unity_test_output()
