# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32c5'], reason='c5 eco2 does not support top pd')
@pytest.mark.parametrize('config', ['release', 'iram_safe'], indirect=True)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_slave_single_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_normal_cases(reset=True)


@pytest.mark.generic
@pytest.mark.parametrize('config', ['iram_safe'], indirect=True)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_slave_single_dev_esp32c5(case_tester) -> None:  # type: ignore
    case_tester.run_all_normal_cases(reset=True)


@pytest.mark.generic
@pytest.mark.esp32c5_eco3
@pytest.mark.parametrize('config', ['release'], indirect=True)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_slave_single_dev_esp32c5_eco3(case_tester) -> None:  # type: ignore
    case_tester.run_all_normal_cases(reset=True)


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count, config', [(2, 'release'), (2, 'iram_safe')], indirect=True)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_slave_multi_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True)
