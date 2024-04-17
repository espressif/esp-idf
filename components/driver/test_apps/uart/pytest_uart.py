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
}


@pytest.mark.supported_targets
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
    dut = case_tester.dut
    chip_type = dut.app.target
    for uart_port in input_argv.get(chip_type, []):
        for case in case_tester.test_menu:
            dut.serial.hard_reset()
            dut._get_ready()
            dut.confirm_write(case.index, expect_str=f'Running {case.name}...')

            dut.expect("select to test 'uart' or 'lp_uart' port", timeout=10)
            dut.write(f'{uart_port}')
            dut.expect_unity_test_output()
