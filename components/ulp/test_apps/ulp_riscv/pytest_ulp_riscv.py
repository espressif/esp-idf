# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_ulp_riscv(dut: Dut) -> None:  # type: ignore
    dut.run_all_single_board_cases()


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic_multi_device
@pytest.mark.parametrize(
    'count', [2], indirect=True
)
def test_ulp_riscv_multi_device(case_tester) -> None:        # type: ignore
    for case in case_tester.test_menu:
        if case.attributes.get('test_env', 'generic_multi_device') == 'generic_multi_device':
            case_tester.run_multi_dev_case(case=case, reset=True)
