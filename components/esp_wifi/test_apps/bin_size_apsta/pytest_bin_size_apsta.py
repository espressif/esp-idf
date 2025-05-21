# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os
from typing import Callable
from typing import Tuple

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

# The standard value has 30~100 bytes fault tolerance
SAVE_BIN_SIZE_TH = {
    'disable_sae_h2e': {
        'esp32': 16600,
        'esp32c2': 19700,
        'esp32c3': 19600,
        'esp32c5': 19650,
        'esp32c6': 19650,
        'esp32c61': 19700,
        'esp32s2': 16600,
        'esp32s3': 16550,
        'default': 16000,
    },
    'disable_nan': {
        'esp32': 29600,
        'esp32c5': 32000,
        'esp32c61': 32000,
        'esp32s2': 28000,
        # other chips does not support nan
        'default': 0,
    },
}


def _get_diff_th(
    config_name: str,
    target: str,
) -> int:
    assert config_name in SAVE_BIN_SIZE_TH
    diff_threshold = SAVE_BIN_SIZE_TH[config_name]
    return diff_threshold.get(target) or diff_threshold['default']


@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, config, skip_autoflash',
    [
        (2, 'default|disable_sae_h2e', 'y'),
        (2, 'default|enable_nan', 'y'),
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32', 'esp32c2', 'esp32c3', 'esp32s2', 'esp32s3', 'esp32c5', 'esp32c6', 'esp32c61'],
    indirect=['target'],
)
def test_wifi_bin_size_apsta(
    dut: Tuple[Dut, Dut],
    config: Tuple[str, str],
    log_performance: Callable[[str, object], None],
) -> None:
    # dut logs are not needed
    dut[0].serial.close()
    dut[1].serial.close()
    target = dut[0].target
    config_name = config[1]

    app_default = dut[0].app
    app_config = dut[1].app

    diff = os.path.getsize(app_default.bin_file) - os.path.getsize(app_config.bin_file)
    if 'enable' in config_name:
        # always log disable-xxx performance
        config_name = config_name.replace('enable', 'disable')
        diff = -diff
    log_performance(f'wifi_{config_name}_save_bin_size', f'{diff} bytes')

    diff_threshold = _get_diff_th(config_name, target)
    assert diff >= diff_threshold
