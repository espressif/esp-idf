# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os
from typing import Callable
from typing import Tuple

import pytest
from pytest_embedded import Dut

# diff of esp32s2/esp32s3 ~45K, others ~50K
DIFF_THRESHOLD = {
    'esp32s2': 40 * 1000,
    'esp32s3': 40 * 1000,
    'default': 45 * 1000,
}


@pytest.mark.esp32
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c5
@pytest.mark.esp32c6
@pytest.mark.esp32c61
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, config, skip_autoflash', [
        (2, 'default|enable_softap', 'y')
    ], indirect=True
)
def test_wifi_sdkconfig_disable_softap_save_binary_size(
    dut: Tuple[Dut, Dut],
    log_performance: Callable[[str, object], None],
) -> None:
    # dut logs are not needed
    dut[0].serial.close()
    dut[1].serial.close()

    app_without_softap = dut[0].app
    app_with_softap = dut[1].app
    assert app_without_softap.sdkconfig['ESP_WIFI_SOFTAP_SUPPORT'] is False
    assert app_with_softap.sdkconfig['ESP_WIFI_SOFTAP_SUPPORT'] is True

    diff = os.path.getsize(app_with_softap.bin_file) - os.path.getsize(app_without_softap.bin_file)
    log_performance('wifi_disable_softap_save_bin_size', f'{diff} bytes')

    diff_threshold = DIFF_THRESHOLD.get(dut[0].target) or DIFF_THRESHOLD['default']
    assert diff > diff_threshold
