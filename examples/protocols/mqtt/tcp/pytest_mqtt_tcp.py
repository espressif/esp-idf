# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import logging
import os
import socket
import struct
import sys
import time
from threading import Thread

import pexpect
import pytest
from common_test_methods import get_host_ip4_by_dest_ip
from pytest_embedded import Dut

msgid = -1


def mqqt_server_sketch(my_ip, port):  # type: (str, str) -> None
    global msgid
    print('Starting the server on {}'.format(my_ip))
    s = None
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(60)
        s.bind((my_ip, port))
        s.listen(1)
        q,addr = s.accept()
        q.settimeout(30)
        print('connection accepted')
    except Exception:
        print('Local server on {}:{} listening/accepting failure: {}'
              'Possibly check permissions or firewall settings'
              'to accept connections on this address'.format(my_ip, port, sys.exc_info()[0]))
        raise
    data = q.recv(1024)
    # check if received initial empty message
    print('received from client {!r}'.format(data))
    data = bytearray([0x20, 0x02, 0x00, 0x00])
    q.send(data)
    # try to receive qos1
    data = q.recv(1024)
    msgid = struct.unpack('>H', data[15:17])[0]
    print('received from client {!r}, msgid: {}'.format(data, msgid))
    data = bytearray([0x40, 0x02, data[15], data[16]])
    q.send(data)
    time.sleep(5)
    s.close()
    print('server closed')


@pytest.mark.esp32
@pytest.mark.ethernet
def test_examples_protocol_mqtt_qos1(dut: Dut) -> None:
    global msgid
    """
    steps: (QoS1: Happy flow)
      1. start the broker broker (with correctly sending ACK)
      2. DUT client connects to a broker and publishes qos1 message
      3. Test evaluates that qos1 message is queued and removed from queued after ACK received
      4. Test the broker received the same message id evaluated in step 3
    """
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'mqtt_tcp.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('[Performance][mqtt_tcp_bin_size]: %s KB', bin_size // 1024)
    # waiting for getting the IP address
    try:
        ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)', timeout=30).group(1).decode()
        print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
    except pexpect.TIMEOUT:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')

    # 2. start mqtt broker sketch
    host_ip = get_host_ip4_by_dest_ip(ip_address)
    thread1 = Thread(target=mqqt_server_sketch, args=(host_ip,1883))
    thread1.start()

    data_write = 'mqtt://' + host_ip
    print('writing to device: {}'.format(data_write))
    dut.write(data_write)
    thread1.join()
    print('Message id received from server: {}'.format(msgid))
    # 3. check the message id was enqueued and then deleted
    msgid_enqueued = dut.expect(b'outbox: ENQUEUE msgid=([0-9]+)', timeout=30).group(1).decode()
    msgid_deleted = dut.expect(b'outbox: DELETED msgid=([0-9]+)', timeout=30).group(1).decode()
    # 4. check the msgid of received data are the same as that of enqueued and deleted from outbox
    if (msgid_enqueued == str(msgid) and msgid_deleted == str(msgid)):
        print('PASS: Received correct msg id')
    else:
        print('Failure!')
        raise ValueError('Mismatch of msgid: received: {}, enqueued {}, deleted {}'.format(msgid, msgid_enqueued, msgid_deleted))
