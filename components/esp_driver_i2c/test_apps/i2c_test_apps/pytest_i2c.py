# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
        'iram_safe',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_i2c(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic_multi_device
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration # TODO: IDF-14395')
@pytest.mark.parametrize(
    'count, config',
    [
        (2, 'defaults'),
        (2, 'release'),
        (2, 'iram_safe'),
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_I2C_SUPPORTED == 1'), indirect=['target'])
def test_i2c_multi_device(case_tester) -> None:  # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True)


@pytest.mark.flash_suspend
@pytest.mark.parametrize(
    'config',
    [
        'i2c_isr_flash',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32c3'],
    indirect=['target'],
)
def test_flash_auto_suspend_for_i2c(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=30)
