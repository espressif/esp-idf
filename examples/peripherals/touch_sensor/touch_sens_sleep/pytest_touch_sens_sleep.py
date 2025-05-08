# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_TOUCH_SENSOR_SUPPORTED == 1 and SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP == 1'),
    indirect=['target'],
)
def test_touch_sens_sleep(dut: Dut) -> None:
    dut.expect(r'Touch \[CH [0-9]+\] enabled on GPIO[0-9]+')
    dut.expect(r'Initial benchmark and new threshold are:')
    dut.expect(r'Touch \[CH [0-9]+\] 0: [0-9]+, [0-9]+')
    dut.expect(r'touch_wakeup: touch wakeup source is ready')
    if dut.app.sdkconfig.get('EXAMPLE_TOUCH_LIGHT_SLEEP_WAKEUP'):
        sleep_mode_str = 'light'
    else:
        sleep_mode_str = 'deep'
    dut.expect(f'Entering {sleep_mode_str} sleep in:')
    dut.expect(r'5 sec...')
    dut.expect(r'4 sec...')
    dut.expect(r'3 sec...')
    dut.expect(r'2 sec...')
    dut.expect(r'1 sec...')
