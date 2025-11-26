# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded_idf.utils import idf_parametrize

# If `test_env` is define, should not run on generic runner


@pytest.mark.generic
@pytest.mark.parametrize('config', ['release'], indirect=True)
@pytest.mark.temp_skip_ci(targets=['esp32p4', 'esp32c61'], reason='p4 rev3 migration # TODO: IDF-14399')
@idf_parametrize(
    'target',
    ['esp32s2', 'esp32s3', 'esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4'],
    indirect=['target'],
)
def test_slave_hd_single_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_normal_cases(reset=True, timeout=180)


# if `test_env` not defined, will run on `generic_multi_device` by default
@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count, config', [(2, 'release')], indirect=True)
@pytest.mark.temp_skip_ci(targets=['esp32p4', 'esp32c61'], reason='p4 rev3 migration # TODO: IDF-14399')
@idf_parametrize(
    'target',
    ['esp32s2', 'esp32s3', 'esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4'],
    indirect=['target'],
)
def test_slave_hd_multi_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True)
