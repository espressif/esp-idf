# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
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
}


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
def test_uart_single_dev(case_tester) -> None:  # type: ignore
    dut = case_tester.first_dut
    chip_type = dut.app.target

    uart_ports = input_argv.get(chip_type, [])
    assert uart_ports, f"Error: Chip type '{chip_type}' is not defined in input_argv. Aborting..."

    for case in case_tester.test_menu:
        if 'wakeup' in case.groups:
            # multi-dev cases, skip on generic runner
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
def test_uart_single_dev_psram(case_tester) -> None:  # type: ignore
    dut = case_tester.first_dut
    for case in case_tester.test_menu:
        if 'wakeup' not in case.groups:
            dut.serial.hard_reset()
            dut._get_ready()
            dut.confirm_write(case.index, expect_str=f'Running {case.name}...')

            dut.expect("select to test 'uart' or 'lp_uart' port", timeout=10)
            dut.write('uart')
            dut.expect_unity_test_output()


# ESP32 only supports uart wakeup if signal routes through IOMUX, ESP32S3 multi device runner has no psram IDF-12837,
# ESP32C61 lack of runner IDF-10949, ESP32P4 not yet supported IDF-12839.
@pytest.mark.temp_skip_ci(targets=['esp32', 'esp32s3', 'esp32c61', 'esp32p4'], reason='no multi-dev runner')
@pytest.mark.generic_multi_device
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@pytest.mark.parametrize('count', [2], indirect=True)
def test_hp_uart_wakeup_modes(case_tester: CaseTester) -> None:
    relevant_cases = [case for case in case_tester.test_menu if {'wakeup', 'uart'}.issubset(case.groups)]
    for case in relevant_cases:
        case_tester.run_multi_dev_case(case=case, reset=True)
