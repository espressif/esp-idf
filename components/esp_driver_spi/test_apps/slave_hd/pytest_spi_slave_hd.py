# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize('config', ['release'], indirect=True)
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_SPI_SUPPORT_SLAVE_HD_VER2 == 1 and IDF_TARGET not in ["esp32c5"]'),
    indirect=['target'],
)
def test_slave_hd_single_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_normal_cases(reset=True, timeout=180)


@pytest.mark.generic
@pytest.mark.esp32c5_rev1
@pytest.mark.parametrize('config', ['release'], indirect=True)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_slave_hd_single_dev_esp32c5_rev1(case_tester) -> None:  # type: ignore
    case_tester.run_all_normal_cases(reset=True, timeout=180)


def get_runner_psram_marks(target: str) -> tuple[pytest.MarkDecorator, ...]:
    if target == 'esp32s3':
        return (pytest.mark.flash_encryption_f8r8,)
    return (pytest.mark.flash_encryption,)


@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('flash_enc', target, marks=get_runner_psram_marks(target))
        for target in soc_filtered_targets('SOC_PSRAM_DMA_CAPABLE == 1 and SOC_SPI_SUPPORT_SLAVE_HD_VER2 == 1')
    ],
    indirect=True,
)
def test_spi_slave_hd_flash_encryption(case_tester) -> None:  # type: ignore
    case_tester.run_all_normal_cases(reset=True, timeout=180)


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count, config', [(2, 'release')], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_SPI_SUPPORT_SLAVE_HD_VER2 == 1'), indirect=['target'])
def test_slave_hd_multi_dev(case_tester) -> None:  # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True, timeout=30)
