# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded_idf.utils import idf_parametrize

# If `test_env` is define, should not run on generic runner


@pytest.mark.generic
@pytest.mark.parametrize('config', ['release'], indirect=True)
@idf_parametrize(
    'target',
    ['esp32s2', 'esp32s3', 'esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4'],
    indirect=['target'],
)
def test_slave_hd_single_dev(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if 'test_env' in case.attributes:
            continue
        case_tester.run_normal_case(case=case, reset=True, timeout=180)


# if `test_env` not defined, will run on `generic_multi_device` by default
@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count, config', [(2, 'release')], indirect=True)
@idf_parametrize(
    'target',
    ['esp32s2', 'esp32s3', 'esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4'],
    indirect=['target'],
)
def test_slave_hd_multi_dev(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if case.attributes.get('test_env', 'generic_multi_device') == 'generic_multi_device':
            case_tester.run_multi_dev_case(case=case, reset=True)
