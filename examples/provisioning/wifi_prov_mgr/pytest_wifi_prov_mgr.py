#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function

import logging
import os
import sys

try:
    import esp_prov
except ImportError:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', 'tools'))
    import esp_prov

import pytest
from pytest_embedded import Dut

logging.basicConfig(level=logging.INFO)


# Have esp_prov throw exception
esp_prov.config_throw_except = True


def test_wifi_prov_mgr(dut: Dut,  sec_ver: int) -> None:
    # Check if BT memory is released before provisioning starts
    dut.expect('wifi_prov_scheme_ble: BT memory released', timeout=60)

    # Parse BLE devname
    devname = dut.expect(b'Provisioning started with service name : PROV_(.{6})')[0].decode('utf-8').split(' : ')[1]
    logging.info('BLE Device Alias for DUT : {}'.format(devname))

    logging.info('Starting Provisioning')
    verbose = False
    protover = 'v1.1'
    provmode = 'ble'
    ap_ssid = 'myssid'
    ap_password = 'mypassword'

    logging.info('Getting security')
    if (sec_ver == 1):
        pop = 'abcd1234'
        sec2_username = None
        sec2_password = None
        security = esp_prov.get_security(sec_ver, sec2_username, sec2_password, pop, verbose)
    elif (sec_ver == 2):
        pop = None
        sec2_username = 'wifiprov'
        sec2_password = 'abcd1234'
        security = esp_prov.get_security(sec_ver, sec2_username, sec2_password, pop, verbose)

    if security is None:
        raise RuntimeError('Failed to get security')

    logging.info('Getting transport')
    transport = esp_prov.get_transport(provmode, devname)
    if transport is None:
        raise RuntimeError('Failed to get transport')

    logging.info('Verifying protocol version')
    if not esp_prov.version_match(transport, protover):
        raise RuntimeError('Mismatch in protocol version')

    logging.info('Verifying scan list capability')
    if not esp_prov.has_capability(transport, 'wifi_scan'):
        raise RuntimeError('Capability not present')

    logging.info('Starting Session')
    if not esp_prov.establish_session(transport, security):
        raise RuntimeError('Failed to start session')

    logging.info('Sending Custom Data')
    if not esp_prov.custom_data(transport, security, 'My Custom Data'):
        raise RuntimeError('Failed to send custom data')

    logging.info('Sending Wifi credential to DUT')
    if not esp_prov.send_wifi_config(transport, security, ap_ssid, ap_password):
        raise RuntimeError('Failed to send Wi-Fi config')

    logging.info('Applying config')
    if not esp_prov.apply_wifi_config(transport, security):
        raise RuntimeError('Failed to send apply config')

    if not esp_prov.wait_wifi_connected(transport, security):
        raise RuntimeError('Provisioning failed')

    # Check if BTDM memory is released after provisioning finishes
    dut.expect('wifi_prov_scheme_ble: BTDM memory released', timeout=30)


@pytest.mark.esp32
@pytest.mark.generic
@pytest.mark.parametrize('config', ['security1',], indirect=True)
@pytest.mark.xfail(reason='Runner unable to connect to target over Bluetooth', run=False)
def test_examples_wifi_prov_mgr_sec1(dut: Dut) -> None:

    test_wifi_prov_mgr(dut, 1)


@pytest.mark.esp32
@pytest.mark.generic
@pytest.mark.xfail(reason='Runner unable to connect to target over Bluetooth', run=False)
def test_examples_wifi_prov_mgr_sec2(dut: Dut) -> None:

    test_wifi_prov_mgr(dut, 2)
