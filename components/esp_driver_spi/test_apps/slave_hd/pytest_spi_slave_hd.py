# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize('config', ['release'], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_SPI_SUPPORT_SLAVE_HD_VER2 == 1'), indirect=['target'])
def test_slave_hd_single_dev(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if 'test_env' in case.attributes:
            continue  # If `test_env` is define, should not run on generic runner
        case_tester.run_normal_case(case=case, reset=True, timeout=180)


@pytest.mark.generic_multi_device
# TODO: [ESP32C61] IDF-10949
@pytest.mark.temp_skip_ci(targets=['esp32c61'], reason='no multi-dev runner')
@pytest.mark.parametrize('count, config', [(2, 'release')], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_SPI_SUPPORT_SLAVE_HD_VER2 == 1'), indirect=['target'])
def test_slave_hd_multi_dev(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        # if `test_env` not defined, will run on `generic_multi_device` by default
        if case.attributes.get('test_env', 'generic_multi_device') == 'generic_multi_device':
            case_tester.run_multi_dev_case(case=case, reset=True)
