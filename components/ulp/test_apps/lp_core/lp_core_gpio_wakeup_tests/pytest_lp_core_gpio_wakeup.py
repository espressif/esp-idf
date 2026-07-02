# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from typing import TYPE_CHECKING

import pytest
from pexpect.exceptions import TIMEOUT
from pytest_embedded_idf import CaseTester
from pytest_embedded_idf import UnittestMenuCase
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

if TYPE_CHECKING:
    from pytest_embedded_idf.dut import IdfDut

WAKEE_DUT_INDEX = 0
WAKER_DUT_INDEX = 1
TEST_TIMEOUT = 120.0
NO_EARLY_WAKE_TIMEOUT = 2.0
LP_GPIO_WAKEUP_GROUP = 'lp_gpio_wakeup'
GPIO_IDLE_HIGH_SIGNAL = 'gpio idle high'


def _wait_signal_line(signal: str) -> str:
    return f'Waiting for signal: [{signal}]!'


def _start_unity_case(dut: 'IdfDut', case: UnittestMenuCase, timeout: float, *, wait_ready: bool = True) -> None:
    if wait_ready:
        dut._get_ready(timeout)

    dut.confirm_write(case.index, expect_str=f'Running {case.name}...')


def _run_unity_case(dut: 'IdfDut', case: UnittestMenuCase, timeout: float) -> None:
    _start_unity_case(dut, case, timeout)
    dut.expect_unity_test_output(timeout=timeout)


def _run_unity_stage(
    dut: 'IdfDut',
    case: UnittestMenuCase,
    stage_index: int,
    timeout: float,
    *,
    wait_ready: bool = True,
) -> None:
    _start_unity_case(dut, case, timeout, wait_ready=wait_ready)
    dut.write(str(stage_index))


def _find_single_test_case(
    case_tester: CaseTester,
    case_type: str,
    name_fragment: str | None = None,
) -> UnittestMenuCase:
    cases = [
        case
        for case in case_tester.test_menu
        if case.type == case_type
        and LP_GPIO_WAKEUP_GROUP in case.groups
        and (name_fragment is None or name_fragment in case.name)
    ]
    assert len(cases) == 1, f"Expected 1 {case_type} case with group '{LP_GPIO_WAKEUP_GROUP}', found {len(cases)}."
    return cases[0]


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize(
    'config',
    [
        'defaults',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    soc_filtered_targets(
        '(SOC_LP_CORE_SUPPORTED == 1) and '
        '(SOC_RTCIO_PIN_COUNT > 0) and '
        '(SOC_DEEP_SLEEP_SUPPORTED == 1) and '
        '(SOC_RTC_GPIO_EDGE_WAKEUP_SUPPORTED == 1)'
    ),
    indirect=['target'],
)
def test_lp_gpio_wakeup_from_deep_sleep(case_tester: CaseTester) -> None:
    wakee_case = _find_single_test_case(case_tester, 'multi_stage')
    waker_idle_case = _find_single_test_case(case_tester, 'normal', name_fragment='idle high')
    waker_wake_case = _find_single_test_case(case_tester, 'normal', name_fragment='wake low')
    assert len(wakee_case.subcases) == 2, f'Expected 2 wakee stages, found {len(wakee_case.subcases)}.'

    wakee = case_tester.dut[WAKEE_DUT_INDEX]
    waker = case_tester.dut[WAKER_DUT_INDEX]
    wakee_init_stage, wakee_after_wake_stage = wakee_case.subcases

    wakee.serial.hard_reset()
    waker.serial.hard_reset()

    _run_unity_case(waker, waker_idle_case, TEST_TIMEOUT)

    _run_unity_stage(wakee, wakee_case, int(wakee_init_stage['index']), TEST_TIMEOUT)
    wakee.expect_exact(_wait_signal_line(GPIO_IDLE_HIGH_SIGNAL), timeout=TEST_TIMEOUT)
    wakee.write('')

    try:
        wakee._get_ready(NO_EARLY_WAKE_TIMEOUT)
    except TIMEOUT:
        pass
    else:
        pytest.fail('Wakee rebooted before the waker drove the GPIO falling edge')

    _run_unity_case(waker, waker_wake_case, TEST_TIMEOUT)

    wakee._get_ready(TEST_TIMEOUT)
    _run_unity_stage(wakee, wakee_case, int(wakee_after_wake_stage['index']), TEST_TIMEOUT, wait_ready=False)
    wakee.expect_unity_test_output(timeout=TEST_TIMEOUT)
