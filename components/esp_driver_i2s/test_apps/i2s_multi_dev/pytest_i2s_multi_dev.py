# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_I2S_SUPPORTS_TDM == 1'), indirect=['target'])
def test_i2s_multi_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True, timeout=30)
