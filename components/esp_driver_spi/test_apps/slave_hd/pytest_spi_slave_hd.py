# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize('config', ['release'], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_SPI_SUPPORT_SLAVE_HD_VER2 == 1'), indirect=['target'])
def test_slave_hd_single_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_normal_cases(reset=True, timeout=180)


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count, config', [(2, 'release')], indirect=True)
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration # TODO: IDF-14399')
@idf_parametrize('target', soc_filtered_targets('SOC_SPI_SUPPORT_SLAVE_HD_VER2 == 1'), indirect=['target'])
def test_slave_hd_multi_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True)
