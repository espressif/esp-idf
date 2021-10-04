#!/usr/bin/env python
#
# Copyright 2021 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import print_function

import http.client
import os
import re
import socket
import sys

import tiny_test_fw
import ttfw_idf
from tiny_test_fw import Utility

try:
    import wifi_tools
except ImportError:
    wifi_tools_path = str(os.getenv('IDF_PATH')) + '/examples/provisioning/softap_prov/utils'
    if wifi_tools_path and wifi_tools_path not in sys.path:
        sys.path.insert(0, wifi_tools_path)
    import wifi_tools


def test_redirect(ip, port):  # type: (str, str) -> str # pylint: disable=unused-argument
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


def test_captive_page(ip, port, uri):  # type: (str, str, str) -> bool # pylint: disable=unused-argument
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


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_BT', ignore=True)
def test_example_captive_portal(env, extra_data):  # type: (tiny_test_fw.Env.Env, None) -> None # pylint: disable=unused-argument
    # Acquire DUT
    dut1 = env.get_dut('captive_portal', 'examples/protocols/http_server/captive_portal', dut_class=ttfw_idf.ESP32DUT)

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, 'captive_portal.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('captive_portal_bin_size', '{}KB'.format(bin_size // 1024))

    # Upload binary and start testing
    dut1.start_app()

    # Parse IP address of STA
    Utility.console_log('Waiting to connect with softAP')
    ap_ip = dut1.expect(re.compile(r'Set up softAP with IP: (\d+.\d+.\d+.\d+)'), timeout=60)[0]

    [ssid, password] = dut1.expect(re.compile(r"wifi_init_softap finished. SSID:'(\S+)' password:'(\S+)'"), timeout=30)
    port = dut1.expect(re.compile(r"(?:[\s\S]*)Starting server on port: '(\d+)'"), timeout=30)[0]

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
            Utility.console_log('error: {}'.format(err))
        try:
            got_ip = dut1.expect(re.compile(r'DHCP server assigned IP to a station, IP is: (\d+.\d+.\d+.\d+)'), timeout=60)
            Utility.console_log('got_ip: {}'.format(got_ip))
            got_ip = got_ip[0]
            if ip != got_ip:
                raise RuntimeError('SoftAP connected to another host! {} != {}'.format(ip, got_ip))
        except tiny_test_fw.DUT.ExpectTimeout:
            # print what is happening on DUT side
            Utility.console_log('in exception tiny_test_fw.DUT.ExpectTimeout')
            Utility.console_log(dut1.read())
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


if __name__ == '__main__':
    test_example_captive_portal()  # pylint: disable=no-value-for-parameter
