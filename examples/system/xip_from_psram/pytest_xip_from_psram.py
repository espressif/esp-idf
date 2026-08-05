# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded.dut import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


def _expect_erase_irq_stats(dut: Dut, where: str) -> None:
    res = dut.expect(
        rf'erase with callback in {where}: duration_ms=(\d+\.\d+), interval_ms=(\d+\.\d+), '
        rf'irq_count=(\d+), expected=(\d+\.\d+)'
    )
    irq_count = int(res.group(3).decode('utf8'))
    expected = float(res.group(4).decode('utf8'))
    assert irq_count >= 0.8 * expected


@pytest.mark.generic
# in order to build the default sdkconfig(the CI won't build the sdkconfig.defaults if there is a sdkconfig.ci.xx)
@pytest.mark.parametrize(
    'config',
    [
        'generic',
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_SPIRAM_XIP_SUPPORTED == 1'), indirect=['target'])
def test_xip_from_psram_example_generic(dut: Dut) -> None:
    dut.expect_exact('found partition')
    _expect_erase_irq_stats(dut, 'PSRAM')
    _expect_erase_irq_stats(dut, 'IRAM')


@pytest.mark.MSPI_F4R8
@pytest.mark.parametrize(
    'config',
    [
        'esp32s3_f4r8',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_xip_from_psram_example_f4r8(dut: Dut) -> None:
    dut.expect_exact('found partition')
    _expect_erase_irq_stats(dut, 'PSRAM')
    _expect_erase_irq_stats(dut, 'IRAM')


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32p4_200m',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_xip_from_psram_example_p4_200m(dut: Dut) -> None:
    dut.expect_exact('found partition')
    _expect_erase_irq_stats(dut, 'PSRAM')
    _expect_erase_irq_stats(dut, 'IRAM')


@pytest.mark.flash_120m
@pytest.mark.parametrize(
    'config',
    [
        'esp32p4_250m',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_xip_from_psram_example_p4_250m(dut: Dut) -> None:
    dut.expect_exact('found partition')
    _expect_erase_irq_stats(dut, 'PSRAM')
    _expect_erase_irq_stats(dut, 'IRAM')


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32s31_250m',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s31'], indirect=['target'])
def test_xip_from_psram_example_s31(dut: Dut) -> None:
    dut.expect_exact('found partition')
    _expect_erase_irq_stats(dut, 'PSRAM')
    _expect_erase_irq_stats(dut, 'IRAM')
