#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import logging
import os
import re
import sys
from threading import Event, Thread

import paho.mqtt.client as mqtt
import pytest
from pytest_embedded import Dut

event_client_connected = Event()
event_stop_client = Event()
event_client_received_correct = Event()
message_log = ''


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):  # type: (mqtt.Client, tuple, bool, str) -> None
    _ = (userdata, flags)
    print('Connected with result code ' + str(rc))
    event_client_connected.set()
    client.subscribe('/topic/qos0')


def mqtt_client_task(client):  # type: (mqtt.Client) -> None
    while not event_stop_client.is_set():
        client.loop()


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):  # type: (mqtt.Client, tuple, mqtt.client.MQTTMessage) -> None
    _ = userdata
    global message_log
    payload = msg.payload.decode()
    if not event_client_received_correct.is_set() and payload == 'data':
        client.publish('/topic/qos0', 'data_to_esp32')
        if msg.topic == '/topic/qos0' and payload == 'data':
            event_client_received_correct.set()
    message_log += 'Received data:' + msg.topic + ' ' + payload + '\n'


@pytest.mark.esp32
@pytest.mark.ethernet
def test_examples_protocol_mqtt_ws(dut):  # type: (Dut) -> None
    broker_url = ''
    broker_port = 0
    """
    steps: |
      1. join AP and connects to ws broker
      2. Test connects a client to the same broker
      3. Test evaluates it received correct qos0 message
      4. Test ESP32 client received correct qos0 message
    """
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'mqtt_websocket.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('[Performance][mqtt_websocket_bin_size]: %s KB', bin_size // 1024)
    # Look for host:port in sdkconfig
    try:
        value = re.search(r'\:\/\/([^:]+)\:([0-9]+)', dut.app.sdkconfig.get('BROKER_URI'))
        assert value is not None
        broker_url = value.group(1)
        broker_port = int(value.group(2))
    except Exception:
        print('ENV_TEST_FAILURE: Cannot find broker url in sdkconfig')
        raise
    client = None
    # 1. Test connects to a broker
    try:
        client = mqtt.Client(transport='websockets')
        client.on_connect = on_connect
        client.on_message = on_message
        print('Connecting...')
        client.connect(broker_url, broker_port, 60)
    except Exception:
        print('ENV_TEST_FAILURE: Unexpected error while connecting to broker {}: {}:'.format(broker_url, sys.exc_info()[0]))
        raise
    # Starting a py-client in a separate thread
    thread1 = Thread(target=mqtt_client_task, args=(client,))
    thread1.start()
    try:
        print('Connecting py-client to broker {}:{}...'.format(broker_url, broker_port))
        if not event_client_connected.wait(timeout=30):
            raise ValueError('ENV_TEST_FAILURE: Test script cannot connect to broker: {}'.format(broker_url))
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[0]
            print('Connected to AP with IP: {}'.format(ip_address))
        except Dut.ExpectTimeout:
            print('ENV_TEST_FAILURE: Cannot connect to AP')
            raise
        print('Checking py-client received msg published from esp...')
        if not event_client_received_correct.wait(timeout=30):
            raise ValueError('Wrong data received, msg log: {}'.format(message_log))
        print('Checking esp-client received msg published from py-client...')
        dut.expect(r'DATA=data_to_esp32', timeout=30)
    finally:
        event_stop_client.set()
        thread1.join()
