# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest


@pytest.mark.esp32
@pytest.mark.multi_dut_modbus_rs485
@pytest.mark.parametrize('count, config', [(2, 'release',)], indirect=True)
def test_rs485_multi_dev(case_tester) -> None:                # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True)
