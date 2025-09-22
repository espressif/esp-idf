# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os.path
import subprocess

import pytest
from can import Bus
from can import Message
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import soc_filtered_targets

TWAI_SUPPORTED_TARGETS = soc_filtered_targets('SOC_TWAI_SUPPORTED == 1')


# Socket CAN fixture
@pytest.fixture(name='socket_can')
def fixture_create_socket_can() -> Bus:
    start_command = 'sudo ip link set can0 up type can bitrate 1000000'
    stop_command = 'sudo ip link set can0 down'
    try:
        subprocess.run(start_command, shell=True, capture_output=True, text=True)
    except Exception as e:
        print(f'Open bus Error: {e}')
    bus = Bus(interface='socketcan', channel='can0', bitrate=1000000)
    yield bus  # test invoked here
    bus.shutdown()
    subprocess.run(stop_command, shell=True, capture_output=True, text=True)


# Generate minimal combinations that each target appears in each app
def generate_target_combinations(target_list: list, count: int = 2) -> list:
    combinations = []
    num_targets = len(target_list)
    for round_num in range(num_targets):
        selected_targets = [target_list[(round_num + i) % num_targets] for i in range(count)]
        combinations.append('|'.join(selected_targets))

    return combinations


@pytest.mark.twai_std
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.timeout(120)
@pytest.mark.parametrize(
    'app_path,target',
    [
        (
            f'{os.path.join(os.path.dirname(__file__), "twai_listen_only")}|'
            f'{os.path.join(os.path.dirname(__file__), "twai_sender")}',
            target_combo,
        )
        for target_combo in generate_target_combinations(TWAI_SUPPORTED_TARGETS)
    ],
    indirect=True,
)
def test_twai_network_multi(dut: tuple[IdfDut, IdfDut], socket_can: Bus) -> None:
    """
    Test TWAI network communication between two nodes:
    - dut[0]: listener (first chip)  - uses twai_listen_only
    - dut[1]: sender (second chip)    - uses twai_sender
    """

    # Print chip information for debugging
    print(f'===> Pytest testing with chips: {dut[0].app.target} (listener), {dut[1].app.target} (sender)')

    # Initialize listener node first
    dut[0].expect('===================TWAI Listen Only Example Starting...===================')
    dut[0].expect('TWAI start listening...')

    # Initialize sender node and start communication
    dut[1].expect('===================TWAI Sender Example Starting...===================')
    dut[1].expect('TWAI Sender started successfully')

    # Verify communication is working
    # Wait for sender to send messages
    dut[1].expect('Sending heartbeat message:', timeout=10)

    # Check that listener is receiving data
    dut[0].expect('RX:', timeout=15)  # Listener should see filtered messages

    # Check if socket receive any messages
    socket_rcv_cnt = 0
    for i in range(100):
        msg = socket_can.recv(timeout=1)
        if msg is not None:
            socket_rcv_cnt += 1
    print(f'Socket receive {socket_rcv_cnt} messages')
    assert socket_rcv_cnt > 50, 'Socket NO messages'

    # Wait a bit more to ensure stable communication
    dut[1].expect('Sending packet of', timeout=10)
    dut[0].expect('RX:', timeout=10)

    # Check if esp32 receive messages from usb can
    message = Message(
        arbitration_id=0x10A,
        is_extended_id=False,
        data=b'Hi ESP32',
    )
    print('USB CAN Send:', message)
    socket_can.send(message, timeout=0.2)
    dut[0].expect_exact('RX: 10a [8] 48 69 20 45 53 50 33 32', timeout=10)  # ASCII: Hi ESP32

    print('===> TWAI network communication test completed successfully')
