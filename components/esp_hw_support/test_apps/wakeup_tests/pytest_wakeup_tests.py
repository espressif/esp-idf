# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

from time import sleep
from typing import Tuple

import pytest
from pytest_embedded_idf.dut import IdfDut

TEST_CONFIGS = [
    pytest.param('default'),
]

# TODO: PM-66
# ESP32: need to fix GPIO16 and GPIO17 bug
# ESP32S2: need to fix GPIO43 bug
# ESP32S3: need to fix GPIO33, GPIO34 and GPIO43 bug
available_gpio_nums = {
    'esp32': [2, 4, 5, 12, 13, 14, 15, 18, 19, 21, 22, 23, 27],
    'esp32s2': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 21, 33, 34, 35, 36, 37, 38, 39, 40, 42, 45],
    'esp32s3': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 21, 35, 36, 37, 39, 40, 42, 45, 46],
    'esp32c2': [0, 1, 2, 3, 4, 5, 6, 7, 10, 18],
    'esp32c3': [0, 1, 2, 3, 4, 5, 6, 7, 10, 18, 19],
    'esp32c6': [0, 1, 2, 3, 4, 5, 6, 7, 10, 11, 12, 13, 15, 18, 19, 20, 21, 22, 23],
    'esp32h2': [0, 1, 2, 3, 4, 5, 10, 11, 12, 22, 25, 26, 27],
}

available_rtcio_nums = {
    'esp32': [36, 37, 38, 39, 34, 35, 4, 0, 2, 15, 13, 12, 14, 27],
    'esp32s2': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21],
    'esp32s3': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21],
    'esp32c2': [0, 1, 2, 3, 4, 5],
    'esp32c3': [0, 1, 2, 3, 4, 5],
    'esp32c6': [0, 1, 2, 3, 4, 5, 6, 7],
    'esp32h2': [7, 8, 9, 10, 11, 12, 13, 14],
}


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('config', TEST_CONFIGS, indirect=True)
def test_ext1_deepsleep(dut: Tuple[IdfDut, IdfDut]) -> None:
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
            wakee.expect(f'io_wakeup_level = {wakeup_level}', timeout=10)

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
            wakee.expect('Wake up from EXT1', timeout=10)
            wakee.write(f'ext1 -p {gpio_num} -d')
            wakee.expect(f'io_wakeup_num = {gpio_num}', timeout=10)


@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('config', TEST_CONFIGS, indirect=True)
def test_rtcio_deepsleep(dut: Tuple[IdfDut, IdfDut]) -> None:
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
            wakee.write(f'rtcio -p {gpio_num} -l {wakeup_level}')
            wakee.expect(f'io_wakeup_num = {gpio_num}', timeout=10)
            wakee.expect(f'io_wakeup_level = {wakeup_level}', timeout=10)

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
            wakee.expect('Wake up from GPIO', timeout=10)
            wakee.write(f'rtcio -p {gpio_num} -d')
            wakee.expect(f'io_wakeup_num = {gpio_num}', timeout=10)


@pytest.mark.esp32
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('config', TEST_CONFIGS, indirect=True)
def test_gpio_wakeup_enable_lightsleep(dut: Tuple[IdfDut, IdfDut]) -> None:
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
            wakee.expect(f'io_wakeup_level = {wakeup_level}', timeout=10)

            waker.expect('io_wakeup_test>', timeout=10)
            sleep_level = 1 - wakeup_level
            waker.write(f'gpio_control -p {gpio_num} -l {sleep_level}')
            waker.expect(f'io_num = {gpio_num}', timeout=10)
            waker.expect(f'io_level = {sleep_level}', timeout=10)

            wakee.write('sleep -m 0')

            waker.write(f'gpio_control -p {gpio_num} -l {wakeup_level}')
            waker.expect(f'io_num = {gpio_num}', timeout=10)
            waker.expect(f'io_level = {wakeup_level}', timeout=10)

            wakee.expect('esp_light_sleep_start', timeout=10)

            wakee.write('cause')
            wakee.expect('Wake up from GPIO', timeout=10)

            wakee.write(f'gpio -p {gpio_num} -d')
            wakee.expect(f'io_wakeup_num = {gpio_num}', timeout=10)
