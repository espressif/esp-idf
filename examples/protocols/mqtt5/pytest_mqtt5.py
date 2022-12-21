#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.ethernet
def test_examples_protocol_mqtt5(dut: Dut) -> None:
    """
    steps: |
      1. join AP
      2. connect to mqtt://mqtt.eclipseprojects.io
      3. check conneciton success
    """
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'mqtt5.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('mqtt5_bin_size : {}KB'.format(bin_size // 1024))
    # check if connected or not
    dut.expect_exact('MQTT_EVENT_CONNECTED', timeout=30)
    # check log
    res = dut.expect(r'sent publish successful, msg_id=(\d+)[^\d]')
    msgid_pub1 = res.group(1).decode('utf8')
    res = dut.expect(r'sent subscribe successful, msg_id=(\d+)[^\d]')
    msgid_sub1 = res.group(1).decode('utf8')
    res = dut.expect(r'sent subscribe successful, msg_id=(\d+)[^\d]')
    msgid_sub2 = res.group(1).decode('utf8')
    res = dut.expect(r'sent unsubscribe successful, msg_id=(\d+)[^\d]')
    msgid_unsub = res.group(1).decode('utf8')
    res = dut.expect(r'MQTT_EVENT_PUBLISHED, msg_id=(\d+)[^\d]')
    msgid_pubd = res.group(1).decode('utf8')
    assert msgid_pubd == msgid_pub1

    res = dut.expect(r'MQTT_EVENT_SUBSCRIBED, msg_id=(\d+)[^\d]')
    msgid_subd = res.group(1).decode('utf8')
    assert msgid_subd == msgid_sub1

    dut.expect_exact('sent publish successful, msg_id=0')
    res = dut.expect(r'MQTT_EVENT_SUBSCRIBED, msg_id=(\d+)[^\d]')
    msgid_subd = res.group(1).decode('utf8')
    assert msgid_subd == msgid_sub2

    dut.expect_exact('sent publish successful, msg_id=0')
    dut.expect_exact('MQTT_EVENT_DATA')
    dut.expect_exact('key is board, value is esp32')
    dut.expect_exact('key is u, value is user')
    dut.expect_exact('key is p, value is password')
    dut.expect_exact('payload_format_indicator is 1')
    dut.expect_exact('response_topic is /topic/test/response')
    dut.expect_exact('correlation_data is 123456')
    dut.expect_exact('TOPIC=/topic/qos1')
    dut.expect_exact('DATA=data_3')
    res = dut.expect(r'MQTT_EVENT_UNSUBSCRIBED, msg_id=(\d+)[^\d]')
    msgid_unsubd = res.group(1).decode('utf8')
    assert msgid_unsubd == msgid_unsub

    dut.expect_exact('MQTT_EVENT_DISCONNECTED')
    logging.info('MQTT5 pytest pass')
