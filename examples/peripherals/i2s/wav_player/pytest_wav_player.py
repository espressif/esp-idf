# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

I2S_TARGETS = soc_filtered_targets('SOC_I2S_SUPPORTED == 1')


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'es8311',
        'es8389',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    I2S_TARGETS,
    indirect=['target'],
)
def test_wav_player_example(dut: Dut, config: str) -> None:
    dut.expect_exact('wav player example start')
    dut.expect_exact('wav_player: i2s driver init success')

    # Generic CI runners have no codec; only check the rest when running locally.
    if os.getenv('CI_JOB_ID'):
        return

    dut.expect_exact(f'wav_player: {config.upper()} codec init success', timeout=10)
