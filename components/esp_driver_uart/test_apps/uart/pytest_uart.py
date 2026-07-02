# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import CaseTester
from pytest_embedded_idf.utils import idf_parametrize

input_argv = {
    'esp32': ['uart'],
    'esp32s2': ['uart'],
    'esp32s3': ['uart'],
    'esp32c3': ['uart'],
    'esp32c2': ['uart'],
    'esp32c6': ['uart', 'lp_uart'],
    'esp32h2': ['uart'],
    'esp32p4': ['uart', 'lp_uart'],
    'esp32c5': ['uart', 'lp_uart'],
    'esp32c61': ['uart'],
    'esp32s31': ['uart', 'lp_uart'],
}


def _run_uart_flow_ctrl_case(dut, case) -> None:  # type: ignore
    # Only the HP UART port is exercised in CI: its RX/TX borrow the console UART RX/TX pads, so the
    # host can inject and observe the XON (0x11) / XOFF (0x13) characters over the console connection
    # without extra wiring. The LP UART port cannot borrow the console pads (LP-only GPIOs) and would
    # need UART0 to be physically wired to the LP UART pins, so it is left for manual testing only.
    dut.serial.hard_reset()
    dut._get_ready()
    dut.confirm_write(case.index, expect_str=f'Running {case.name}...')

    dut.expect("select to test 'uart' or 'lp_uart' port", timeout=10)
    dut.write('uart')

    # Drive the HP UART software-flow-control case (both directions) after the 'uart' port has been selected.
    # Part 1: the DUT reacts to XON/XOFF that we inject over the console connection.
    dut.expect_exact('Send 0x13 (XOFF - Ctrl+S) to stop UART transmission', timeout=10)
    dut.write(b'\x13')
    dut.expect_exact('UART transmission stopped', timeout=25)

    dut.expect_exact('Send 0x11 (XON - Ctrl+Q) to start UART transmission', timeout=10)
    dut.write(b'\x11')
    dut.expect_exact('UART transmission resumed', timeout=25)

    # Part 2: the DUT auto-sends XOFF/XON when its RX FIFO fills/drains. It temporarily borrows the console TX
    # pad, so we read the raw XOFF (0x13) then XON (0x11) bytes back over the console connection.
    dut.expect_exact('Send 64 bytes to fill RX FIFO', timeout=10)
    dut.write(b'A' * 64)
    dut.expect(b'\x13')  # XOFF
    dut.expect(b'\x11')  # XON

    dut.expect_unity_test_output()


@pytest.mark.temp_skip_ci(targets=['esp32s3'], reason='skip due to duplication with test_uart_single_dev_psram')
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='cannot pass')  # TODO: IDF-15619
def test_uart_single_dev(case_tester) -> None:  # type: ignore
    dut = case_tester.first_dut
    chip_type = dut.app.target

    uart_ports = input_argv.get(chip_type, [])
    assert uart_ports, f"Error: Chip type '{chip_type}' is not defined in input_argv. Aborting..."

    for case in case_tester.test_menu:
        if 'wakeup' in case.groups:
            # multi-dev cases, skip on generic runner
            continue

        if 'uart_flow_ctrl' in case.groups:
            _run_uart_flow_ctrl_case(dut, case)
            continue

        if 'hp-uart-only' not in case.groups:
            for uart_port in uart_ports:
                dut.serial.hard_reset()
                dut._get_ready()
                dut.confirm_write(case.index, expect_str=f'Running {case.name}...')

                dut.expect("select to test 'uart' or 'lp_uart' port", timeout=10)
                dut.write(f'{uart_port}')
                dut.expect_unity_test_output()
        else:
            dut._run_normal_case(case, reset=True)


@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='cannot pass')  # TODO: IDF-15619
def test_uart_single_dev_psram(case_tester) -> None:  # type: ignore
    dut = case_tester.first_dut
    for case in case_tester.test_menu:
        if 'wakeup' in case.groups:
            # multi-dev cases, skip on generic runner
            continue

        if 'uart_flow_ctrl' in case.groups:
            _run_uart_flow_ctrl_case(dut, case)
            continue

        dut.serial.hard_reset()
        dut._get_ready()
        dut.confirm_write(case.index, expect_str=f'Running {case.name}...')

        dut.expect("select to test 'uart' or 'lp_uart' port", timeout=10)
        dut.write('uart')
        dut.expect_unity_test_output()


# ESP32 only supports uart wakeup if signal routes through IOMUX
# ESP32S3 multi device runner has no psram IDF-12837,
@pytest.mark.temp_skip_ci(targets=['esp32', 'esp32s3'], reason='no multi-dev runner')
@pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='cannot pass')  # TODO: IDF-15619
@pytest.mark.generic_multi_device
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        # Note: Software clock ICG (Idle Clock Gating) feature in ESP32-P4 won't work in release test.
        'release',
    ],
    indirect=True,
)
@pytest.mark.parametrize('count', [2], indirect=True)
def test_hp_uart_wakeup_modes(case_tester: CaseTester) -> None:
    relevant_cases = [case for case in case_tester.test_menu if {'wakeup', 'uart'}.issubset(case.groups)]
    for case in relevant_cases:
        case_tester.run_multi_dev_case(case=case, reset=True)
