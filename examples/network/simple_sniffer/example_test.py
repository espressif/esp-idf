# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from __future__ import unicode_literals

import re
from typing import Any

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_Protocols')
def test_examples_simple_sniffer(env, _):  # type: (Any, Any) -> None

    dut = env.get_dut('simple_sniffer', 'examples/network/simple_sniffer', app_config_name='mem')
    dut.start_app()

    dut.expect('sniffer>')
    dut.write('pcap --open -f simple-sniffer')
    dut.expect('cmd_pcap: open file successfully')
    dut.write('sniffer -i wlan -c 2 -n 10')
    dut.expect('cmd_sniffer: 10 packages will be captured')
    dut.expect('cmd_sniffer: start WiFi promiscuous ok')
    dut.expect('cmd_sniffer: stop promiscuous ok')
    dut.write('pcap --summary -f simple-sniffer')
    dut.expect('cmd_pcap: Memory is to be parsed')
    dut.expect('Pcap packet Head:')
    dut.expect('Magic Number: a1b2c3d4')
    dut.expect(re.compile(r'Major Version: [0-9]*'))
    dut.expect(re.compile(r'Minor Version: [0-9]*'))
    dut.expect(re.compile(r'SnapLen: [0-9]*'))
    dut.expect(re.compile(r'LinkType: [0-9]*'))
    for i in range(0, 10):
        dut.expect('Packet ' + str(i) + ':')
        dut.expect(re.compile(r'Timestamp \(Seconds\): [0-9]*'))
        dut.expect(re.compile(r'Timestamp \(Microseconds\): [0-9]*'))
        dut.expect(re.compile(r'Capture Length: [0-9]*'))
        dut.expect(re.compile(r'Packet Length: [0-9]*'))
        dut.expect(re.compile(r'Frame Type: .*'))
        dut.expect(re.compile(r'Frame Subtype: .*'))
        dut.expect(re.compile(r'Destination: .*'))
        dut.expect(re.compile(r'Source: .*'))
    dut.expect('Pcap packet Number: 10')
    dut.write('pcap --close -f simple-sniffer')
    dut.expect('cmd_pcap: free memory successfully')
    dut.expect('cmd_pcap: .pcap file close done')

    dut.write('')
    dut.expect('sniffer>')


if __name__ == '__main__':
    test_examples_simple_sniffer()
