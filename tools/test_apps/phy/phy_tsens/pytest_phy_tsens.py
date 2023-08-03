# SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import random
import string
from typing import Tuple

import pexpect
import pytest
from pytest_embedded import Dut


def run_phy_tsens_test(dut: Tuple[Dut, Dut]) -> None:
    ap_dut = dut[0]
    sta_dut = dut[1]

    ap_dut.write('tsens_init')
    sta_dut.write('tsens_init')
    ap_dut.expect('Enable temperature sensor')
    sta_dut.expect('Enable temperature sensor')

    ap_dut.write('wifi_init')
    sta_dut.write('wifi_init')
    ap_dut.expect('initialize_wifi DONE')
    sta_dut.expect('initialize_wifi DONE')

    # check tsens read after wifi_init
    for _dut in [ap_dut, sta_dut]:
        _dut.write('tsens_read')
        temp_val = _dut.expect(r'Temperature value ([\d.]+)[^\d.]')[1].decode()
        assert 0 < float(temp_val) < 70
        _dut.write('tsens_read')
        temp_val2 = _dut.expect(r'Temperature value ([\d.]+)[^\d.]')[1].decode()
        assert -5 < float(temp_val) - float(temp_val2) < 5

    # start tsens read background task
    for _dut in [ap_dut, sta_dut]:
        _dut.write('tsens_read -c 100 -i 1000')  # 1000ms * 100
        temp_val = _dut.expect(r'Temperature value ([\d.]+)[^\d.]')[1].decode()
        assert 0 < float(temp_val) < 70

    # check wifi connect
    ssid = ''.join([random.choice(string.ascii_lowercase) for i in range(10)])
    password = ''.join([random.choice(string.ascii_lowercase) for i in range(10)])
    ap_dut.write(f'ap_set {ssid} {password}')
    ap_dut.expect('set ap config OK')
    sta_dut.write(f'sta_connect {ssid} {password}')
    sta_dut.expect('STA_GOT_IP')

    data = sta_dut.expect(pexpect.TIMEOUT, timeout=10).decode()
    assert 'DISCONNECT' not in data
    ap_dut.expect(pexpect.TIMEOUT, timeout=0.1)  # clear data

    # check tsens read again after wifi connected
    for _dut in [ap_dut, sta_dut]:
        temp_val = _dut.expect(r'Temperature value ([\d.]+)[^\d.]')[1].decode()
        assert 0 < float(temp_val) < 70


def run_phy_tsens_test_init_wifi_first(dut: Tuple[Dut, Dut]) -> None:
    ap_dut = dut[0]
    sta_dut = dut[1]

    ap_dut.write('tsens_init')
    sta_dut.write('tsens_init')
    ap_dut.expect('Enable temperature sensor')
    sta_dut.expect('Enable temperature sensor')

    ap_dut.write('wifi_init')
    sta_dut.write('wifi_init')
    ap_dut.expect('initialize_wifi DONE')
    sta_dut.expect('initialize_wifi DONE')

    # check tsens read after wifi_init
    for _dut in [ap_dut, sta_dut]:
        _dut.write('tsens_read')
        temp_val = _dut.expect(r'Temperature value ([\d.]+)[^\d.]')[1].decode()
        assert 0 < float(temp_val) < 70
        _dut.write('tsens_read')
        temp_val2 = _dut.expect(r'Temperature value ([\d.]+)[^\d.]')[1].decode()
        assert -5 < float(temp_val) - float(temp_val2) < 5

    # start tsens read background task
    for _dut in [ap_dut, sta_dut]:
        _dut.write('tsens_read -c 100 -i 1000')  # 1000ms * 100
        temp_val = _dut.expect(r'Temperature value ([\d.]+)[^\d.]')[1].decode()
        assert 0 < float(temp_val) < 70

    # check wifi connect
    ssid = ''.join([random.choice(string.ascii_lowercase) for i in range(10)])
    password = ''.join([random.choice(string.ascii_lowercase) for i in range(10)])
    ap_dut.write(f'ap_set {ssid} {password}')
    ap_dut.expect('set ap config OK')
    sta_dut.write(f'sta_connect {ssid} {password}')
    sta_dut.expect('STA_GOT_IP')

    data = sta_dut.expect(pexpect.TIMEOUT, timeout=10).decode()
    assert 'DISCONNECT' not in data
    ap_dut.expect(pexpect.TIMEOUT, timeout=0.1)  # clear data

    # check tsens read again after wifi connected
    for _dut in [ap_dut, sta_dut]:
        temp_val = _dut.expect(r'Temperature value ([\d.]+)[^\d.]')[1].decode()
        assert 0 < float(temp_val) < 70


def run_phy_tsens_test_with_light_sleep(dut: Tuple[Dut, Dut]) -> None:
    ap_dut = dut[0]
    sta_dut = dut[1]

    ap_dut.write('wifi_init')
    sta_dut.write('wifi_init')
    ap_dut.expect('initialize_wifi DONE')
    sta_dut.expect('initialize_wifi DONE')

    ap_dut.write('tsens_init')
    sta_dut.write('tsens_init')
    ap_dut.expect('Enable temperature sensor')
    sta_dut.expect('Enable temperature sensor')

    # start tsens read background task
    for _dut in [ap_dut, sta_dut]:
        _dut.write('tsens_read -c 100 -i 1000')  # 1000ms * 100
        temp_val = _dut.expect(r'Temperature value ([\d.]+)[^\d.]')[1].decode()
        assert 0 < float(temp_val) < 70

    # check wifi connect
    ssid = ''.join([random.choice(string.ascii_lowercase) for i in range(10)])
    password = ''.join([random.choice(string.ascii_lowercase) for i in range(10)])
    ap_dut.write(f'ap_set {ssid} {password}')
    ap_dut.expect('set ap config OK')
    sta_dut.write(f'sta_connect {ssid} {password}')
    sta_dut.expect('STA_GOT_IP')

    sta_dut.write('light_sleep enable')
    data = sta_dut.expect(pexpect.TIMEOUT, timeout=30).decode(errors='ignore')
    assert 'DISCONNECT' not in data
    assert 'LIGHT_SLEEP_ENABLED,OK' in data
    ap_dut.expect(pexpect.TIMEOUT, timeout=0.1)  # clear data

    # check tsens read again after wifi connected
    for _dut in [ap_dut, sta_dut]:
        temp_val = _dut.expect(r'Temperature value ([\d.]+)[^\d.]')[1].decode()
        assert 0 < float(temp_val) < 70


@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize('count', [2], indirect=True)
def test_phy_tsens_coexist(dut: Tuple[Dut, Dut]) -> None:
    for _dut in dut:
        _dut.expect('esp>')
    run_phy_tsens_test(dut)
    for _dut in dut:
        _dut.write('restart')
        _dut.expect('boot:')
        _dut.expect('esp>')
    run_phy_tsens_test_init_wifi_first(dut)
    for _dut in dut:
        _dut.write('restart')
        _dut.expect('boot:')
        _dut.expect('esp>')
    run_phy_tsens_test_with_light_sleep(dut)


@pytest.mark.esp32c2
@pytest.mark.wifi_two_dut
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, config, baud',
    [
        (2, 'c2_xtal26m', '74880'),
    ],
    indirect=True,
)
def test_phy_tsens_coexist_c2_xtal26m(dut: Tuple[Dut, Dut]) -> None:
    for _dut in dut:
        _dut.expect('esp>')
    run_phy_tsens_test(dut)
    for _dut in dut:
        _dut.write('restart')
        _dut.expect('boot:')
        _dut.expect('esp>')
    run_phy_tsens_test_init_wifi_first(dut)
    for _dut in dut:
        _dut.write('restart')
        _dut.expect('boot:')
        _dut.expect('esp>')
    run_phy_tsens_test_with_light_sleep(dut)
