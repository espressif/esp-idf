# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from common_test_methods import get_env_config_variable
from pytest_embedded import Dut


def _sniffer_packets_check(dut: Dut, channel: int, packet_num: int) -> None:
    dut.write('pcap --open -f simple-sniffer')
    dut.expect('cmd_pcap: open file successfully')
    dut.write(f'sniffer -i wlan -c {channel} -n {packet_num}')
    dut.expect(f'cmd_sniffer: {packet_num} packages will be captured')
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
    for i in range(0, packet_num):
        dut.expect(f'Packet {i}:')
        dut.expect(r'Timestamp \(Seconds\): [0-9]+')
        dut.expect(r'Timestamp \(Microseconds\): [0-9]+')
        dut.expect(r'Capture Length: [0-9]+')
        dut.expect(r'Packet Length: [0-9]+')
        dut.expect(r'Frame Type:\s+\w+')
        dut.expect(r'Frame Subtype:\s+\w+')
        dut.expect(r'Destination:\s+\w+')
        dut.expect(r'Source:\s+\w+')
    dut.expect(f'Pcap packet Number: {packet_num}')
    dut.write('pcap --close -f simple-sniffer')
    dut.expect('cmd_pcap: free memory successfully')
    dut.expect('cmd_pcap: .pcap file close done')


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.wifi_ap
@pytest.mark.parametrize('config', [
    'mem',
], indirect=True)
def test_examples_simple_sniffer(dut: Dut) -> None:
    dut.expect('sniffer>')
    channel = get_env_config_variable('wifi_ap', 'sniffer_channel', default=1)
    # Sniffer multiple times with few packets
    for _ in range(3):
        _sniffer_packets_check(dut, channel, 5)
    dut.write('')
    dut.expect('sniffer>')
