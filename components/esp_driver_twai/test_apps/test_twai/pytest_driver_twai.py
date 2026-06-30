# SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
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
    stop_command = f'sudo -n ip link set {can_env} down'
    status_command = f'sudo -n ip -details link show {can_env}'

    try:
        result = subprocess.run(status_command, shell=True, capture_output=True, text=True)
        if result.returncode != 0:
            raise Exception(f'CAN interface "{can_env}" not found')

        if 'UP' in result.stdout:  # Close the bus anyway if it is already up
            subprocess.run(stop_command, shell=True, capture_output=True, text=True)
        subprocess.run(start_command, shell=True, capture_output=True, text=True)

        time.sleep(0.5)
        bus = Bus(interface='socketcan', channel=f'{can_env}', bitrate=250000)
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
