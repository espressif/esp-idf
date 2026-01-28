# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging
import subprocess
import time

import pytest
from can import Bus
from can import Message
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

# ---------------------------------------------------------------------------
# Loop Back Tests
# ---------------------------------------------------------------------------


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target', ['esp32', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32s2', 'esp32s3', 'esp32p4'], indirect=['target']
)
def test_legacy_twai_self(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='twai-loop-back')


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


def esp_reset_and_wait_ready(dut: Dut) -> None:
    dut.serial.hard_reset()
    time.sleep(0.5)
    dut.expect_exact('Press ENTER to see the list of tests')


@pytest.fixture(name='socket_can')
def fixture_create_socket_can() -> Bus:
    # Set up the socket CAN with the bitrate
    start_command = 'sudo -n ip link set can0 up type can bitrate 250000 restart-ms 100'
    stop_command = 'sudo -n ip link set can0 down'
    status_command = 'sudo -n ip -details link show can0'

    try:
        result = subprocess.run(status_command, shell=True, capture_output=True, text=True)
        if result.returncode != 0:
            raise Exception('CAN interface "can0" not found')

        if 'UP' in result.stdout:  # Close the bus anyway if it is already up
            subprocess.run(stop_command, shell=True, capture_output=True, text=True)
        subprocess.run(start_command, shell=True, capture_output=True, text=True)

        time.sleep(0.5)
        bus = Bus(interface='socketcan', channel='can0', bitrate=250000)
        yield bus  # test invoked here

        bus.shutdown()
    except Exception as e:
        pytest.skip(f'Open usb-can bus Error: {str(e)}')
    finally:
        subprocess.run(stop_command, shell=True, capture_output=True, text=True)


# ---------------------------------------------------------------------------
# Interactive Tests
# ---------------------------------------------------------------------------


@pytest.mark.twai_std
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
    ],
    indirect=True,
)
@idf_parametrize(
    'target', ['esp32', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32s2', 'esp32s3', 'esp32p4'], indirect=['target']
)
def test_legacy_twai_listen_only(dut: Dut, socket_can: Bus) -> None:
    esp_reset_and_wait_ready(dut)

    # TEST_CASE("twai_listen_only", "[twai]")
    dut.write('"twai_listen_only"')

    # wait the DUT to start listening
    time.sleep(0.1)

    message = Message(
        arbitration_id=0x123,
        is_extended_id=False,
        data=[0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88],
    )
    socket_can.send(message, timeout=0.2)
    dut.expect_unity_test_output()
    esp_enter_flash_mode(dut)


@pytest.mark.twai_std
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target', ['esp32', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32s2', 'esp32s3', 'esp32p4'], indirect=['target']
)
def test_legacy_twai_remote_request(dut: Dut, socket_can: Bus) -> None:
    esp_reset_and_wait_ready(dut)

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
    esp_enter_flash_mode(dut)
