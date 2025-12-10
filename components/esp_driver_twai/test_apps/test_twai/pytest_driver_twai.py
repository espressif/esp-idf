# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import subprocess
import time

import pytest
from can import Bus
from can import Message
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


# ---------------------------------------------------------------------------
# Loop Back Tests
# ---------------------------------------------------------------------------
@pytest.mark.generic
@pytest.mark.parametrize('config', ['release', 'cache_safe'], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_driver_twai_loopbk(dut: Dut) -> None:
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
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='no runner')
@pytest.mark.parametrize('config', ['release'], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_driver_twai_listen_only(dut: Dut, socket_can: Bus) -> None:
    esp_reset_and_wait_ready(dut)

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
    esp_enter_flash_mode(dut)


@pytest.mark.twai_std
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='no runner')
@pytest.mark.parametrize('config', ['release'], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_driver_twai_remote_request(dut: Dut, socket_can: Bus) -> None:
    esp_reset_and_wait_ready(dut)

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
    esp_enter_flash_mode(dut)
