# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_I2S_SUPPORTED == 1 and IDF_TARGET not in ["esp32c5", "esp32s3"]'),
    indirect=['target'],
)
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='cannot pass')  # TODO: IDF-15609
def test_i2s(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32c5'],
    indirect=['target'],
)
def test_i2s_esp32c5(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.esp32c5_rev1
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32c5'],
    indirect=['target'],
)
def test_i2s_esp32c5_rev1(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32s3'],
    indirect=['target'],
)
def test_i2s_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases()
