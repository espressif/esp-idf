# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32c6', 'esp32p4'], indirect=['target'])
def test_lp_core(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# TODO: Enable LP I2C test for esp32p4 (IDF-9407)
@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@idf_parametrize('target', ['esp32c6'], indirect=['target'])
def test_lp_core_multi_device(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if case.attributes.get('test_env', 'generic_multi_device') == 'generic_multi_device':
            case_tester.run_multi_dev_case(case=case, reset=True)
