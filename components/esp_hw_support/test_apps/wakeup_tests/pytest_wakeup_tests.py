# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from time import sleep

import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

TEST_CONFIGS = [
    pytest.param('default'),
]

available_gpio_nums = {
    'esp32': [2, 4, 5, 12, 13, 14, 15, 18, 19, 21, 22, 23, 27],
    'esp32s2': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 21, 33, 34, 35, 36, 37, 38, 39, 40, 42, 45],
    'esp32s3': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 21, 35, 36, 37, 39, 40, 42, 45, 46],
    'esp32c2': [0, 1, 2, 3, 4, 5, 6, 7, 10, 18],
    'esp32c3': [0, 1, 2, 3, 4, 5, 6, 7, 10, 18, 19],
    'esp32c6': [0, 1, 2, 3, 4, 5, 6, 7, 10, 11, 12, 13, 15, 18, 19, 20, 21, 22, 23],
    'esp32h2': [0, 1, 2, 3, 4, 5, 10, 11, 12, 22, 25, 26, 27],
    'esp32p4': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17]
    + [18, 19, 20, 21, 22, 23, 39, 40, 41, 42, 43, 45, 46, 47, 48]
    + [28, 29, 30, 31, 32, 33, 36, 49, 50, 51, 52, 53, 54],
    'esp32c5': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 13, 14, 23, 24, 25, 26],
    'esp32c61': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 13, 22, 23, 24, 25, 26, 27, 28, 29],
    'esp32h4': [0, 1, 2, 3, 4, 5, 15, 16, 18, 19, 20, 21, 22, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 38, 39],
    'esp32s31': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
    + [42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57],
}

# Matches SOC_RTC_GPIO_EDGE_WAKEUP_SUPPORTED in soc_caps.h (HP-periph PD GPIO wakeup edge)
RTC_GPIO_EDGE_HP_PD_TARGETS = frozenset({'esp32c5', 'esp32c6', 'esp32c61', 'esp32p4', 'esp32h4'})

available_rtcio_nums = {
    'esp32': [36, 37, 38, 39, 34, 35, 25, 26, 33, 32, 4, 0, 2, 15, 13, 12, 14, 27],
    'esp32s2': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21],
    'esp32s3': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21],
    'esp32c2': [0, 1, 2, 3, 4, 5],
    'esp32c3': [0, 1, 2, 3, 4, 5],
    'esp32c6': [0, 1, 2, 3, 4, 5, 6, 7],
    'esp32h2': [7, 8, 9, 10, 11, 12, 13, 14],
    'esp32p4': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15],
    'esp32c5': [0, 1, 2, 3, 4, 5, 6],
    'esp32c61': [0, 1, 2, 3, 4, 5, 6],
    'esp32h4': [0, 1, 2, 3, 4, 5],
    'esp32s31': [0, 1, 2, 3, 4, 5, 6, 7],
}


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('config', TEST_CONFIGS, indirect=True)
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_PM_SUPPORT_EXT1_WAKEUP == 1 and SOC_DEEP_SLEEP_SUPPORTED == 1'),
    indirect=['target'],
)
def test_ext1_deepsleep(dut: tuple[IdfDut, IdfDut]) -> None:
    wakee = dut[0]
    waker = dut[1]

    chip_type = wakee.app.target
    gpio_nums = available_gpio_nums.get(chip_type, [])
    rtcio_nums = available_rtcio_nums.get(chip_type, [])

    for wakeup_level in [0, 1]:
        for gpio_num in rtcio_nums:
            if gpio_num not in gpio_nums:
                continue

            wakee.write('\r\n')
            sleep(0.1)
            wakee.expect('io_wakeup_test>', timeout=10)
            wakee.write(f'ext1 -p {gpio_num} -m {wakeup_level}')
            wakee.expect(f'io_wakeup_num = {gpio_num}', timeout=10)
            wakee.expect(f'io_wakeup_type = {wakeup_level}', timeout=10)

            waker.expect('io_wakeup_test>', timeout=10)
            sleep_level = 1 - wakeup_level
            waker.write(f'gpio_control -p {gpio_num} -l {sleep_level}')
            waker.expect(f'io_num = {gpio_num}', timeout=10)
            waker.expect(f'io_level = {sleep_level}', timeout=10)

            wakee.write('sleep -m 1')

            waker.write(f'gpio_control -p {gpio_num} -l {wakeup_level}')
            waker.expect(f'io_num = {gpio_num}', timeout=10)
            waker.expect(f'io_level = {wakeup_level}', timeout=10)

            wakee.expect('io_wakeup_test>', timeout=10)

            sleep(2)

            wakee.write('cause')
            # esp32 ext1 all low wakeup mode can not detect wakeup pin.
            if (dut[0].target == 'esp32') and (wakeup_level == 0):
                wakee.expect('Wake up from EXT1', timeout=10)
            else:
                wakee.expect(f'Wake up from EXT1 at IO{gpio_num}', timeout=10)

            wakee.write(f'ext1 -p {gpio_num} -d')
            wakee.expect(f'io_wakeup_num = {gpio_num}', timeout=10)


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('config', TEST_CONFIGS, indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_GPIO_SUPPORT_HP_PERIPH_PD_SLEEP_WAKEUP == 1'), indirect=['target'])
def test_rtcio_deepsleep(dut: tuple[IdfDut, IdfDut]) -> None:
    wakee = dut[0]
    waker = dut[1]

    chip_type = wakee.app.target
    gpio_nums = available_gpio_nums.get(chip_type, [])
    rtcio_nums = available_rtcio_nums.get(chip_type, [])

    # Modes: 0=LOW, 1=HIGH, 2=POSEDGE, 3=NEGEDGE, 4=ANYEDGE
    # Tuple: (esp_sleep_gpio_wake_up_mode_t as int, waker_level_before_sleep, waker_level_to_wake)
    dslp_io_wakeup_cases = [
        (0, 1, 0),  # low level: hold high, then pull low
        (1, 0, 1),  # high level: hold low, then drive high
    ]
    if chip_type in RTC_GPIO_EDGE_HP_PD_TARGETS:
        dslp_io_wakeup_cases += [
            (2, 0, 1),  # posedge: low then rising edge
            (3, 1, 0),  # negedge: high then falling edge
            (4, 0, 1),  # anyedge: rising edge from low
            (4, 1, 0),  # anyedge: falling edge from high
        ]

    for wakeup_mode, prep_level, wake_level in dslp_io_wakeup_cases:
        for gpio_num in rtcio_nums:
            if gpio_num not in gpio_nums:
                continue

            wakee.write('\r\n')
            sleep(0.1)
            wakee.expect('io_wakeup_test>', timeout=10)
            wakee.write(f'rtcio -p {gpio_num} -l {wakeup_mode}')
            wakee.expect(f'io_wakeup_num = {gpio_num}', timeout=10)
            wakee.expect(f'io_wakeup_type = {wakeup_mode}', timeout=10)

            waker.expect('io_wakeup_test>', timeout=10)
            waker.write(f'gpio_control -p {gpio_num} -l {prep_level}')
            waker.expect(f'io_num = {gpio_num}', timeout=10)
            waker.expect(f'io_level = {prep_level}', timeout=10)

            wakee.write('sleep -m 1')
            wakee.expect('enter deep sleep', timeout=10)
            sleep(2)

            waker.write(f'gpio_control -p {gpio_num} -l {wake_level}')
            waker.expect(f'io_num = {gpio_num}', timeout=10)
            waker.expect(f'io_level = {wake_level}', timeout=10)
            wakee.expect('io_wakeup_test>', timeout=10)

            wakee.write('cause')
            wakee.expect(f'Wake up from GPIO at IO{gpio_num}', timeout=10)
            wakee.write(f'rtcio -p {gpio_num} -d')
            wakee.expect(f'io_wakeup_num = {gpio_num}', timeout=10)


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('config', TEST_CONFIGS, indirect=True)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_gpio_wakeup_enable_lightsleep(dut: tuple[IdfDut, IdfDut]) -> None:
    wakee = dut[0]
    waker = dut[1]

    chip_type = wakee.app.target
    gpio_nums = available_gpio_nums.get(chip_type, [])

    for wakeup_level in [1, 0]:
        for gpio_num in gpio_nums:
            wakee.write('\r\n')
            wakee.expect('io_wakeup_test>', timeout=10)
            wakee.write(f'gpio -p {gpio_num} -l {wakeup_level}')
            wakee.expect(f'io_wakeup_num = {gpio_num}', timeout=10)
            wakee.expect(f'io_wakeup_type = {wakeup_level}', timeout=10)

            waker.expect('io_wakeup_test>', timeout=10)
            sleep_level = 1 - wakeup_level
            waker.write(f'gpio_control -p {gpio_num} -l {sleep_level}')
            waker.expect(f'io_num = {gpio_num}', timeout=10)
            waker.expect(f'io_level = {sleep_level}', timeout=10)

            wakee.write('sleep -m 0')
            wakee.expect('enter light sleep', timeout=10)
            sleep(1)

            waker.write(f'gpio_control -p {gpio_num} -l {wakeup_level}')
            waker.expect(f'io_num = {gpio_num}', timeout=10)
            waker.expect(f'io_level = {wakeup_level}', timeout=10)
            wakee.expect('wakeup from lightsleep', timeout=10)

            wakee.write('cause')
            wakee.expect(f'Wake up from GPIO at IO{gpio_num}', timeout=10)

            wakee.write(f'gpio -p {gpio_num} -d')
            wakee.expect(f'io_wakeup_num = {gpio_num}', timeout=10)
