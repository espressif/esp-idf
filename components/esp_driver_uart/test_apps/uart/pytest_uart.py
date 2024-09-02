# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest

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
}


@pytest.mark.supported_targets
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
def test_uart_single_dev(case_tester) -> None:                # type: ignore
    dut = case_tester.first_dut
    chip_type = dut.app.target

    uart_ports = input_argv.get(chip_type, [])
    assert uart_ports, f"Error: Chip type '{chip_type}' is not defined in input_argv. Aborting..."

    for case in case_tester.test_menu:
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


@pytest.mark.esp32s3
@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
def test_uart_single_dev_psram(case_tester) -> None:          # type: ignore
    dut = case_tester.first_dut
    for case in case_tester.test_menu:
        dut.serial.hard_reset()
        dut._get_ready()
        dut.confirm_write(case.index, expect_str=f'Running {case.name}...')

        dut.expect("select to test 'uart' or 'lp_uart' port", timeout=10)
        dut.write('uart')
        dut.expect_unity_test_output()
