# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.wifi_ap
@pytest.mark.parametrize('config', [
    'mem',
], indirect=True)
def test_examples_simple_sniffer(dut: Dut) -> None:
    sniffer_num = 9
    dut.expect('sniffer>')
    dut.write('pcap --open -f simple-sniffer')
    dut.expect('cmd_pcap: open file successfully')
    dut.write(f'sniffer -i wlan -c 2 -n {sniffer_num}')
    dut.expect(f'cmd_sniffer: {sniffer_num} packages will be captured')
    dut.expect('cmd_sniffer: start WiFi promiscuous ok')
    dut.expect('cmd_sniffer: stop promiscuous ok')
    dut.write('pcap --summary -f simple-sniffer')
    dut.expect('cmd_pcap: Memory is to be parsed')
    dut.expect('Pcap packet Head:')
    dut.expect('Magic Number: a1b2c3d4')
    dut.expect(r'Major Version: [0-9]+')
    dut.expect(r'Minor Version: [0-9]+')
    dut.expect(r'SnapLen: [0-9]+')
    dut.expect(r'LinkType: [0-9]+')
    # Allow "save captured packet failed" twice
    for i in range(0, sniffer_num - 2):
        dut.expect(f'Packet {i}:')
        dut.expect(r'Timestamp \(Seconds\): [0-9]+')
        dut.expect(r'Timestamp \(Microseconds\): [0-9]+')
        dut.expect(r'Capture Length: [0-9]+')
        dut.expect(r'Packet Length: [0-9]+')
        dut.expect(r'Frame Type:\s+\w+')
        dut.expect(r'Frame Subtype:\s+\w+')
        dut.expect(r'Destination:\s+\w+')
        dut.expect(r'Source:\s+\w+')
    dut.expect(r'Pcap packet Number: [7-9]')
    dut.write('pcap --close -f simple-sniffer')
    dut.expect('cmd_pcap: free memory successfully')
    dut.expect('cmd_pcap: .pcap file close done')

    dut.write('')
    dut.expect('sniffer>')
