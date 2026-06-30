# SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import random
import subprocess
import time

import pytest
from can import Bus
from can import Message
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

can_env = os.getenv('CAN_PORT', 'can0')
print(f'CAN_PORT={can_env}')


# ---------------------------------------------------------------------------
# Loop Back Tests
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.parametrize('config', ['release', 'cache_safe'], indirect=True)
@idf_parametrize(
    'target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1 and IDF_TARGET not in ["esp32c5"]'), indirect=['target']
)
def test_driver_twai_loopbk(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='twai', reset=True)


@pytest.mark.generic
@pytest.mark.esp32c5_rev1
@pytest.mark.parametrize('config', ['release', 'cache_safe'], indirect=True)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_driver_twai_loopbk_c5rev1(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='twai', reset=True)


# ---------------------------------------------------------------------------
# Helper Functions
# ---------------------------------------------------------------------------


def esp_enter_flash_mode(dut: Dut) -> None:
    ser = dut.serial.proc
    ser.setRTS(True)  # EN Low
    time.sleep(0.5)
    ser.setDTR(True)  # GPIO0 Low
    ser.setRTS(False)  # EN High
    dut.expect('waiting for download', timeout=2)
    ser.setDTR(False)  # Back RTS/DTR to 1/1 to avoid affect to esptool


@pytest.fixture(name='socket_can')
def fixture_create_socket_can() -> Bus:
    # Set up the socket CAN with the bitrate
    start_command = f'sudo -n ip link set {can_env} up type can bitrate 250000'
    fdstart_command = f'sudo -n ip link set {can_env} up type can bitrate 250000 dbitrate 4000000 fd on'
    stop_command = f'sudo -n ip link set {can_env} down'
    status_command = f'sudo -n ip -details link show {can_env}'

    try:
        result = subprocess.run(status_command, shell=True, capture_output=True, text=True)
        if result.returncode != 0:
            raise Exception(f'CAN interface "{can_env}" not found')

        if 'UP' in result.stdout:  # Close the bus anyway if it is already up
            subprocess.run(stop_command, shell=True, capture_output=True, text=True)
        result = subprocess.run(fdstart_command, shell=True, capture_output=True, text=True)
        fd_enabled = result.returncode == 0
        if not fd_enabled:
            subprocess.run(start_command, shell=True, capture_output=True, text=True)
        print(f'FD mode {"Enabled" if fd_enabled else "NOT supported"} on USB CAN')

        time.sleep(0.5)
        bus = Bus(interface='socketcan', channel=f'{can_env}', bitrate=250000, fd=fd_enabled)
        bus.fd_enabled = fd_enabled
        yield bus  # test invoked here

        bus.shutdown()
    finally:
        subprocess.run(stop_command, shell=True, capture_output=True, text=True)


# ---------------------------------------------------------------------------
# Interactive Tests
# ---------------------------------------------------------------------------
@pytest.mark.twai_adapter
@pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='no runner')
@pytest.mark.parametrize('config', ['release'], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='no runner')  # TODO: IDFCI-11110
def test_driver_twai_listen_only(dut: Dut, socket_can: Bus) -> None:
    try:
        dut.expect_exact('Press ENTER to see the list of tests')
        dut.write('"twai_listen_only"')

        # wait the DUT to finish initialize
        time.sleep(0.1)

        message = Message(
            arbitration_id=0x6688,
            is_extended_id=True,
            data=[0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88],
        )
        print('USB Socket CAN Send:', message, 'Return:', socket_can.send(message))
        dut.expect_unity_test_output(timeout=10)
    finally:
        esp_enter_flash_mode(dut)


@pytest.mark.twai_adapter
@pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='no runner')
@pytest.mark.parametrize('config', ['release'], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='no runner')  # TODO: IDFCI-11110
def test_driver_twai_remote_request(dut: Dut, socket_can: Bus) -> None:
    try:
        dut.expect_exact('Press ENTER to see the list of tests')
        dut.write('"twai_remote_request"')

        print('Waiting remote frame ...')
        deadline = time.time() + 5.0
        req = None
        while time.time() < deadline:
            req = socket_can.recv(timeout=0.2)
            if req is not None and req.is_remote_frame:
                break

        if req is None:
            raise Exception('Remote frame not received')
        print(f'USB Socket CAN Received: {req}')

        reply = Message(
            arbitration_id=req.arbitration_id,
            is_extended_id=req.is_extended_id,
            data=[0x80, 0x70, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10],
        )
        socket_can.send(reply, timeout=0.2)
        print('USB Socket CAN Replied:', reply)

        dut.expect_unity_test_output(timeout=10)
    finally:
        esp_enter_flash_mode(dut)


# Test send 100 random frames and ESP echo them back with data + 2
# Verify the frame is received correctly
@pytest.mark.twai_adapter
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_FD_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32s31', 'esp32h4'], reason='no runner')
@pytest.mark.parametrize('config', ['release'], indirect=True)
def test_driver_twai_fd_trans(dut: Dut, socket_can: Bus) -> None:
    if not socket_can.fd_enabled:
        pytest.fail(f'CAN interface "{can_env}" does not support CAN FD')
    try:
        dut.expect_exact('Press ENTER to see the list of tests')
        dut.write('"twai_fd_echo_random_trans"')

        # wait the DUT to finish initialize
        time.sleep(0.5)

        rng = random.Random(0x20260702)
        canfd_lengths = [0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64]

        for index in range(100):
            is_fd = rng.choice([False, True])
            is_extended_id = rng.choice([False, True])
            arbitration_id = rng.randrange(0x20000000 if is_extended_id else 0x800)
            data_len = rng.choice(canfd_lengths if is_fd else list(range(9)))
            message = Message(
                arbitration_id=arbitration_id,
                is_extended_id=is_extended_id,
                is_fd=is_fd,
                bitrate_switch=is_fd and rng.choice([False, True]),
                data=bytes(rng.randrange(256) for _ in range(data_len)),
            )
            esp_to_usb_data = bytes((byte + 2) & 0xFF for byte in message.data)
            print(f'USB TX {index}:', message, 'Return:', socket_can.send(message, timeout=0.2))

            deadline = time.time() + 5.0
            reply = None
            while time.time() < deadline:
                candidate = socket_can.recv(timeout=0.2)
                if (
                    candidate is not None
                    and candidate.arbitration_id == message.arbitration_id
                    and candidate.is_extended_id == message.is_extended_id
                ):
                    reply = candidate
                    break
            if reply is None:
                raise Exception('CAN frame not received')
            print(f'USB RX {index}: {reply}')

            assert reply.arbitration_id == message.arbitration_id
            assert reply.is_extended_id == message.is_extended_id
            assert reply.is_fd == message.is_fd
            assert reply.bitrate_switch == message.bitrate_switch
            assert bytes(reply.data) == esp_to_usb_data

        dut.expect_unity_test_output(timeout=10)
    finally:
        esp_enter_flash_mode(dut)
