# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize('config', ['release', 'iram_safe'], indirect=True)
@idf_parametrize(
    'target', soc_filtered_targets('SOC_GPSPI_SUPPORTED == 1 and IDF_TARGET not in ["esp32c5"]'), indirect=['target']
)
def test_slave_single_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_normal_cases(reset=True)


@pytest.mark.generic
@pytest.mark.parametrize('config', ['iram_safe'], indirect=True)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_slave_single_dev_esp32c5(case_tester) -> None:  # type: ignore
    case_tester.run_all_normal_cases(reset=True)


@pytest.mark.generic
@pytest.mark.esp32c5_rev1
@pytest.mark.parametrize('config', ['release'], indirect=True)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_slave_single_dev_esp32c5_rev1(case_tester) -> None:  # type: ignore
    case_tester.run_all_normal_cases(reset=True)


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count, config', [(2, 'release'), (2, 'iram_safe')], indirect=True)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='s31 bringup on this module is not done')
def test_slave_multi_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True)
