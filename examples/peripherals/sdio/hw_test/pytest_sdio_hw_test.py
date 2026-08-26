# SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os

import pytest
from pytest_embedded_idf import IdfDut

HOST_PATH = os.path.join(os.path.dirname(__file__), 'host')
SLAVE_PATH = os.path.join(os.path.dirname(__file__), 'slave')
TRANSFER_REPEAT_COUNT = 5
HOST_FAILURE_PATTERNS = [
    'TX transfer error:',
    'RX transfer error:',
    'RX pattern validation: failure',
]
SLAVE_FAILURE_PATTERNS = [
    'RX pattern validation: failure',
]

_WIDTH_SPEED_PARAMS = [
    pytest.param('1', 'd', id='1line_ds'),
    pytest.param('1', 'h', id='1line_hs'),
    pytest.param('4', 'd', id='4line_ds'),
    pytest.param('4', 'h', id='4line_hs'),
]


def parameter_expand(existing_parameters: list[list[str]], value_list: list[str]) -> list[list[str]]:
    ret = []
    for param in existing_parameters:
        ret.extend([param + [value] for value in value_list])
    return ret


esp32_32_param = [[f'{HOST_PATH}|{SLAVE_PATH}', 'esp32|esp32']]
esp32_c6_param = [[f'{HOST_PATH}|{SLAVE_PATH}', 'esp32|esp32c6']]
esp32p4_c5_param = [[f'{HOST_PATH}|{SLAVE_PATH}', 'esp32p4|esp32c5']]
esp32_c61_param = [[f'{HOST_PATH}|{SLAVE_PATH}', 'esp32|esp32c61']]

esp32_param_default = [pytest.param(*param) for param in parameter_expand(esp32_32_param, ['default|default'])]
c6_param_default = [pytest.param(*param) for param in parameter_expand(esp32_c6_param, ['default|default'])]
c5_param_default = [pytest.param(*param) for param in parameter_expand(esp32p4_c5_param, ['default|default'])]
c61_param_default = [pytest.param(*param) for param in parameter_expand(esp32_c61_param, ['default|default'])]


def _send_choice(dut: IdfDut, choice: str) -> None:
    dut.write(f'\n{choice}\n')


def _expect_success_or_fail(dut: IdfDut, success_pattern: str, failure_patterns: list[str], timeout: int = 10) -> None:
    matched = dut.expect_exact([success_pattern, *failure_patterns], timeout=timeout)
    matched_text = matched.decode() if isinstance(matched, bytes) else str(matched)
    assert matched_text == success_pattern, f'Expected "{success_pattern}", got "{matched_text}"'


def _expect_setup_menu(host: IdfDut) -> None:
    host.expect_exact('=== SDIO Link Setup ===', timeout=10)
    host.expect_exact('Select bus width:', timeout=5)
    host.expect_exact('Select clock mode:', timeout=5)
    host.expect_exact('Press s to start link initialization', timeout=5)


def _expect_transfer_menu(host: IdfDut) -> None:
    host.expect_exact('=== SDIO Traffic Menu ===', timeout=10)
    host.expect_exact('Press t for transmit mode', timeout=5)
    host.expect_exact('Press r for receive mode', timeout=5)
    host.expect_exact('Press x to close the link', timeout=5)


def _select_link_mode(host: IdfDut, width: str, speed: str) -> None:
    _expect_setup_menu(host)
    _send_choice(host, width)
    _send_choice(host, speed)
    _send_choice(host, 's')
    host.expect_exact('SDIO link initialized successfully', timeout=15)
    _expect_transfer_menu(host)


def _run_transmit_check(host: IdfDut, slave: IdfDut) -> None:
    _send_choice(host, 't')
    host.expect_exact('Entering transmit mode', timeout=5)
    for _ in range(TRANSFER_REPEAT_COUNT):
        _expect_success_or_fail(host, 'TX transfer success', HOST_FAILURE_PATTERNS)
        _expect_success_or_fail(slave, 'RX pattern validation: success', SLAVE_FAILURE_PATTERNS)
    _send_choice(host, 'z')
    _expect_transfer_menu(host)


def _run_receive_check(host: IdfDut, slave: IdfDut) -> None:
    _send_choice(host, 'r')
    host.expect_exact('Entering receive mode', timeout=5)
    for _ in range(TRANSFER_REPEAT_COUNT):
        _expect_success_or_fail(host, 'RX pattern validation: success', HOST_FAILURE_PATTERNS)
        _expect_success_or_fail(slave, 'TX buffer recycled', SLAVE_FAILURE_PATTERNS)
    _send_choice(host, 'z')
    _expect_transfer_menu(host)


def _run_channel_quality_test(
    dut: tuple[IdfDut, IdfDut],
    width_choice: str,
    speed_choice: str,
) -> None:
    host = dut[0]
    slave = dut[1]

    host.pexpect_proc.timeout = 10
    slave.pexpect_proc.timeout = 10

    slave.expect_exact('SDIO slave ready for channel test', timeout=15)

    _select_link_mode(host, width_choice, speed_choice)
    _run_transmit_check(host, slave)
    _run_receive_check(host, slave)


@pytest.mark.sdio_multidev_32_c6
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('app_path, target, config', c6_param_default, indirect=True)
@pytest.mark.parametrize('width_choice, speed_choice', _WIDTH_SPEED_PARAMS)
def test_sdio_hw_esp32_esp32c6(
    dut: tuple[IdfDut, IdfDut],
    width_choice: str,
    speed_choice: str,
) -> None:
    _run_channel_quality_test(dut, width_choice, speed_choice)


@pytest.mark.sdio_master_slave
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('app_path, target, config', esp32_param_default, indirect=True)
@pytest.mark.parametrize('width_choice, speed_choice', _WIDTH_SPEED_PARAMS)
def test_sdio_hw_esp32_esp32(
    dut: tuple[IdfDut, IdfDut],
    width_choice: str,
    speed_choice: str,
) -> None:
    _run_channel_quality_test(dut, width_choice, speed_choice)


@pytest.mark.sdio_multidev_p4_c5
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('app_path, target, config', c5_param_default, indirect=True)
@pytest.mark.parametrize('width_choice, speed_choice', _WIDTH_SPEED_PARAMS)
def test_sdio_hw_esp32p4_esp32c5(
    dut: tuple[IdfDut, IdfDut],
    width_choice: str,
    speed_choice: str,
) -> None:
    _run_channel_quality_test(dut, width_choice, speed_choice)


@pytest.mark.sdio_multidev_32_c61
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('app_path, target, config', c61_param_default, indirect=True)
@pytest.mark.parametrize('width_choice, speed_choice', _WIDTH_SPEED_PARAMS)
def test_sdio_hw_esp32_esp32c61(
    dut: tuple[IdfDut, IdfDut],
    width_choice: str,
    speed_choice: str,
) -> None:
    _run_channel_quality_test(dut, width_choice, speed_choice)
