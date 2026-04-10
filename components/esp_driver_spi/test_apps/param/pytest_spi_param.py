# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='s31 bringup on this module is not done')
def test_param_single_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_normal_cases(reset=True)


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='s31 bringup on this module is not done')
def test_param_multi_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True)
