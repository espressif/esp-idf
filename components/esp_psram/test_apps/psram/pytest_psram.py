# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32_release',
        'esp32_2t',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_psram_esp32(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.psramv0
@pytest.mark.parametrize(
    'config',
    [
        'esp32_hspi',
        'esp32_vspi',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_psram_esp32_psramv0(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32s2_release',
        'esp32s2_advanced',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s2'], indirect=['target'])
def test_psram_esp32s2(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32s3_quad_release',
        'esp32s3_quad_advanced',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_psram_esp32s3(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'esp32s3_octal_release',
        'esp32s3_octal_advanced',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_psram_esp32s3_octal(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['esp32p4_200m_release', 'esp32p4_xip'],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_psram_esp32p4(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32c5_release',
        'esp32c5_advanced',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_psram_esp32c5(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32c61_release',
        'esp32c61_advanced',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c61'], indirect=['target'])
def test_psram_esp32c61(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'xip_psram_no_boot_init',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s2', 'esp32s3', 'esp32c5', 'esp32c61'], indirect=['target'])
def test_xip_psram_no_boot_init(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'psram_no_boot_init',
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_SPIRAM_SUPPORTED == 1'), indirect=['target'])
def test_psram_no_boot_init(dut: Dut) -> None:
    dut.run_all_single_board_cases()
