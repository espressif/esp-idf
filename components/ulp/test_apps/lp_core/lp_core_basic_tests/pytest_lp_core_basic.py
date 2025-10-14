# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf import CaseTester
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'defaults',
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_LP_CORE_SUPPORTED == 1'), indirect=['target'])
def test_lp_core(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'xtal',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_LP_CORE_SUPPORTED == 1 and SOC_CLK_LP_FAST_SUPPORT_XTAL == 1'),
    indirect=['target'],
)
def test_lp_core_xtal(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.lp_i2s
@pytest.mark.parametrize(
    'config',
    [
        'lp_vad',
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_LP_CORE_SUPPORTED == 1'), indirect=['target'])
def test_lp_vad(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='lp_vad')


# TODO: Support LP I2C test for esp32p4 (IDF-9581)
@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize(
    'config',
    [
        'defaults',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c6'], indirect=['target'])
def test_lp_core_multi_device(case_tester) -> None:  # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True)


@pytest.mark.generic_multi_device
@pytest.mark.parametrize(
    'target',
    soc_filtered_targets('SOC_LP_CORE_SUPPORTED == 1'),
    indirect=True,
)
@pytest.mark.parametrize(
    'config',
    [
        'defaults',
    ],
    indirect=True,
)
@pytest.mark.parametrize('count', [2], indirect=True)
def test_lp_uart_wakeup_modes(case_tester: CaseTester) -> None:
    relevant_cases = [case for case in case_tester.test_menu if {'wakeup', 'uart'}.issubset(case.groups)]
    assert len(relevant_cases) == 12, (
        f"Expected 12 test cases with groups 'wakeup' and 'uart', but found {len(relevant_cases)}."
    )

    for case in relevant_cases:
        case_tester.run_multi_dev_case(case=case, reset=True)
