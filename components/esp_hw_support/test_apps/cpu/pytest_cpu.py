# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


def _run_non_recoverable_case(dut: IdfDut, test_name: str, expected_reason: str) -> None:
    dut.skip_decode_panic = True
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write(f'"{test_name}"')
    # TEST_CASE_MULTIPLE_STAGES shows a stage sub-menu before running; select stage 1 to trigger the fault
    dut.expect_exact('(1)')
    dut.write('1')

    actual_reason = dut.expect(r"Core\s+\d+\s+panic'ed \(([^)]+)\)", timeout=10).group(1).decode()
    assert actual_reason == expected_reason, f"expected panic reason '{expected_reason}', got '{actual_reason}'"

    dut.expect_exact('Rebooting...')


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [('default', 'supported_targets')],
    indirect=['config', 'target'],
)
def test_cpu(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='!cpu_reset&!cpu_non_recoverable', timeout=120)


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [('default', 'supported_targets')],
    indirect=['config', 'target'],
)
def test_cpu_reset(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='cpu_reset', timeout=120)


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [('default', 'supported_targets')],
    indirect=['config', 'target'],
)
def test_cpu_non_recoverable(dut: IdfDut) -> None:
    expected_reason = 'Unhandled debug exception' if dut.target in {'esp32', 'esp32s2', 'esp32s3'} else 'Breakpoint'
    present_cases = {case.name for case in dut.test_menu}

    for case_name in (
        'CPU breakpoint: trigger causes panic',
        'CPU watchpoint: store trigger causes panic',
        'CPU watchpoint: load trigger causes panic',
    ):
        if case_name in present_cases:
            _run_non_recoverable_case(dut, case_name, expected_reason)
