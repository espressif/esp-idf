# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import subprocess
from time import sleep

import pytest
from can import Bus
from can import Message
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize('config', ['release', 'cache_safe'], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_driver_twai_loopbk(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='twai', reset=True)


# -------------------------------- test twai interactive ------------------------------
@pytest.fixture(name='socket_can')
def fixture_create_socket_can() -> Bus:
    # Set up the socket CAN with the bitrate
    start_command = 'sudo ip link set can0 up type can bitrate 250000'
    stop_command = 'sudo ip link set can0 down'
    try:
        subprocess.run(start_command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    except Exception as e:
        print(f'Open bus Error: {e}')
    bus = Bus(interface='socketcan', channel='can0', bitrate=250000)
    yield bus  # test invoked here
    bus.shutdown()
    subprocess.run(stop_command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)


@pytest.mark.twai_std
@pytest.mark.temp_skip_ci(targets=['esp32c5'], reason='no runner')
@pytest.mark.parametrize('config', ['release'], indirect=True)
@pytest.mark.timeout(10)  # Whole test timeout
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_driver_twai_listen_only(dut: Dut, socket_can: Bus) -> None:
    dut.serial.hard_reset()
    dut.expect_exact('Press ENTER to see the list of tests')

    dut.write('"twai_listen_only"')

    # wait the DUT to finish initialize
    sleep(0.1)

    message = Message(
        arbitration_id=0x6688,
        is_extended_id=True,
        data=[0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88],
    )
    print('USB Socket CAN Send:', message)
    socket_can.send(message, timeout=0.2)
    dut.expect_unity_test_output(timeout=10)


@pytest.mark.twai_std
@pytest.mark.temp_skip_ci(targets=['esp32c5'], reason='no runner')
@pytest.mark.parametrize('config', ['release'], indirect=True)
@pytest.mark.timeout(10)  # Whole test timeout
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_driver_twai_remote_request(dut: Dut, socket_can: Bus) -> None:
    dut.serial.hard_reset()
    dut.expect_exact('Press ENTER to see the list of tests')

    dut.write('"twai_remote_request"')

    print('Waiting remote frame ...')
    while True:
        req = socket_can.recv(timeout=0.2)
        if req is not None and req.is_remote_frame:
            break
    print(f'USB Socket CAN Received: {req}')

    reply = Message(
        arbitration_id=req.arbitration_id,
        is_extended_id=req.is_extended_id,
        data=[0x80, 0x70, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10],
    )
    socket_can.send(reply, timeout=0.2)
    print('USB Socket CAN Replied:', reply)
    dut.expect_unity_test_output(timeout=10)
