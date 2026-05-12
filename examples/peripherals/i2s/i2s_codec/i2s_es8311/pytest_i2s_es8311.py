# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_I2S_SUPPORTED == 1'),
    indirect=['target'],
)
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='cannot pass')  # TODO: IDF-15609
def test_i2s_es8311_example_generic(dut: Dut) -> None:
    dut.expect('i2s es8311 codec example start')
    dut.expect('-----------------------------')
    dut.expect('i2s_es8311: i2s driver init success')


@pytest.mark.es8311
@pytest.mark.parametrize(
    'config',
    [
        's3_korvo2_v3',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_i2s_es8311_example_korvo2_v3(dut: Dut) -> None:
    dut.expect(r'i2s es8311 codec example start')
    dut.expect(r'-----------------------------')
    dut.expect(r'i2s_es8311: i2s driver init success')
    dut.expect(r'i2s_es8311: es8311 codec init success')
    dut.expect(r'Returned from app_main\(\)')
