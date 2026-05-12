# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_I2S_SUPPORTS_TDM == 1'), indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners')
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='cannot pass')  # TODO: IDF-15609
def test_i2s_es7210_tdm_example(dut: Dut) -> None:
    dut.expect_exact('example: Create I2S receive channel')
    dut.expect_exact('example: Configure I2S receive channel to TDM mode')
    dut.expect_exact('example: Init I2C used to configure ES7210')
    dut.expect_exact('example: Configure ES7210 codec parameters')
