#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function

import http.client
import logging
import os
import socket
import sys

import pexpect
import pytest
from pytest_embedded import Dut

try:
    import wifi_tools
except ImportError:
    wifi_tools_path = str(os.getenv('IDF_PATH')) + '/examples/provisioning/softap_prov/utils'
    if wifi_tools_path and wifi_tools_path not in sys.path:
        sys.path.insert(0, wifi_tools_path)
    import wifi_tools


def test_redirect(ip: str, port: str) -> str:
    # Establish HTTP connection
    sess = http.client.HTTPConnection(ip + ':' + port, timeout=15)

    uri = '/test'
    # GET response
    sess.request('GET', url=uri)
    resp = sess.getresponse()
    resp_hdrs = resp.getheaders()

    if resp.status != 302:
        raise RuntimeError('Redirect failed, response status: {}'.format(resp.status))

    for hdr in resp_hdrs:
        if hdr[0] == 'location':
            uri = hdr[1]

    print('Redirected to uri: {}'.format(uri))

    # Close HTTP connection
    sess.close()

    return uri


def test_captive_page(ip: str, port: str, uri: str) -> bool:
    # Establish HTTP connection
    sess = http.client.HTTPConnection(ip + ':' + port, timeout=15)

    # GET response
    sess.request('GET', url=uri)
    resp = sess.getresponse()

    resp_data = resp.read().decode()
    search_str = 'Redirect to the captive portal'

    if search_str not in resp_data:
        raise RuntimeError('Failed to match {} with data from captive portal: {}'.format(search_str, resp_data))

    # Close HTTP connection
    sess.close()
    return True


@pytest.mark.esp32
@pytest.mark.wifi_wlan
@pytest.mark.temp_skip_ci(targets=['esp32'], reason='unstable case')
@pytest.mark.xfail(reason='Runner unable to connect to target over WiFi', run=False)
def test_example_captive_portal(dut: Dut) -> None:

    # Get binary file
    binary_file = os.path.join(dut.app.binary_path, 'captive_portal.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('captive_portal_bin_size : {}KB'.format(bin_size // 1024))

    # Parse IP address of STA
    logging.info('Waiting to connect with softAP')
    ap_ip = dut.expect(r'Set up softAP with IP: (\d+.\d+.\d+.\d+)', timeout=60)[1]

    wifi_info = dut.expect(r"wifi_init_softap finished. SSID:'(\S+)' password:'(\S+)'", timeout=30)
    ssid = wifi_info[1].decode()
    password = wifi_info[2].decode()
    port = dut.expect(r"(?:[\s\S]*)Starting server on port: '(\d+)'", timeout=30)[1]

    iface = wifi_tools.get_wiface_name()
    if iface is None:
        raise RuntimeError('Failed to get Wi-Fi interface on host')
    print('Interface name  : ' + iface)
    print('SoftAP SSID     : ' + ssid)
    print('SoftAP Password : ' + password)

    try:
        ctrl = wifi_tools.wpa_cli(iface, reset_on_exit=True)
        print('Connecting to DUT SoftAP...')
        try:
            ip = ctrl.connect(ssid, password)
        except RuntimeError as err:
            logging.info('error: {}'.format(err))
        try:
            got_ip = dut.expect(r'DHCP server assigned IP to a station, IP is: (\d+.\d+.\d+.\d+)', timeout=60)[1].decode()
            logging.info('got_ip: {}'.format(got_ip))
            if ip != got_ip:
                raise RuntimeError('SoftAP connected to another host! {} != {}'.format(ip, got_ip))
        except pexpect.exceptions.TIMEOUT:
            # print what is happening on DUT side
            logging.info('in exception pexpect.exceptions.TIMEOUT')
            logging.info(dut.read())
            raise
        print('Connected to DUT SoftAP')

        host_name = 'www.google.com'
        host = socket.gethostbyname(host_name)
        print('hostname: {} resolved to: {}'.format(host_name, host))
        if host != ap_ip:
            raise RuntimeError("DNS server failed to redirect question to the softAP's IP")

        uri = test_redirect(ap_ip, port)
        test_captive_page(ap_ip, port, uri)
    finally:
        ctrl.reset()
