# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import contextlib
import difflib
import logging
import os
import random
import re
import ssl
import string
from itertools import count, product
from threading import Event, Lock
from typing import Any, Dict, List, Tuple, no_type_check

import paho.mqtt.client as mqtt
import pexpect
import pytest
from pytest_embedded import Dut

DEFAULT_MSG_SIZE = 16


# Publisher class creating a python client to send/receive published data from esp-mqtt client
class MqttPublisher(mqtt.Client):

    def __init__(self, repeat, published, publish_cfg, log_details=False):  # type: (MqttPublisher, int, int, dict, bool) -> None
        self.sample_string = ''.join(random.choice(string.ascii_uppercase + string.ascii_lowercase + string.digits) for _ in range(DEFAULT_MSG_SIZE))
        self.log_details = log_details
        self.repeat = repeat
        self.publish_cfg = publish_cfg
        self.expected_data = f'{self.sample_string * self.repeat}'
        self.published = published
        self.received = 0
        self.lock = Lock()
        self.event_client_connected = Event()
        self.event_client_got_all = Event()
        transport = 'websockets' if self.publish_cfg['transport'] in ['ws', 'wss'] else 'tcp'
        super().__init__('MqttTestRunner', userdata=0, transport=transport)

    def print_details(self, text):  # type: (str) -> None
        if self.log_details:
            logging.info(text)

    def on_connect(self, mqttc: Any, obj: Any, flags: Any, rc:int) -> None:
        self.event_client_connected.set()

    def on_connect_fail(self, mqttc: Any, obj: Any) -> None:
        logging.error('Connect failed')

    def on_message(self, mqttc: Any, userdata: Any, msg: mqtt.MQTTMessage) -> None:
        payload = msg.payload.decode('utf-8')
        if payload == self.expected_data:
            userdata += 1
            self.user_data_set(userdata)
            self.received = userdata
            if userdata == self.published:
                self.event_client_got_all.set()
        else:
            differences = len(list(filter(lambda data: data[0] != data[1], zip(payload, self.expected_data))))
            logging.error(f'Payload differ in {differences} positions from expected data. received size: {len(payload)} expected size:'
                          f'{len(self.expected_data)}')
            logging.info(f'Repetitions: {payload.count(self.sample_string)}')
            logging.info(f'Pattern: {self.sample_string}')
            logging.info(f'First  : {payload[:DEFAULT_MSG_SIZE]}')
            logging.info(f'Last   : {payload[-DEFAULT_MSG_SIZE:]}')
            matcher = difflib.SequenceMatcher(a=payload, b=self.expected_data)
            for match in matcher.get_matching_blocks():
                logging.info(f'Match: {match}')

    def __enter__(self) -> Any:
        qos = self.publish_cfg['qos']
        broker_host = self.publish_cfg['broker_host_' + self.publish_cfg['transport']]
        broker_port = self.publish_cfg['broker_port_' + self.publish_cfg['transport']]

        try:
            self.print_details('Connecting...')
            if self.publish_cfg['transport'] in ['ssl', 'wss']:
                self.tls_set(None, None, None, cert_reqs=ssl.CERT_NONE, tls_version=ssl.PROTOCOL_TLSv1_2, ciphers=None)
                self.tls_insecure_set(True)
            self.event_client_connected.clear()
            self.loop_start()
            self.connect(broker_host, broker_port, 60)
        except Exception:
            self.print_details(f'ENV_TEST_FAILURE: Unexpected error while connecting to broker {broker_host}')
            raise
        self.print_details(f'Connecting py-client to broker {broker_host}:{broker_port}...')

        if not self.event_client_connected.wait(timeout=30):
            raise ValueError(f'ENV_TEST_FAILURE: Test script cannot connect to broker: {broker_host}')
        self.event_client_got_all.clear()
        self.subscribe(self.publish_cfg['subscribe_topic'], qos)
        return self

    def __exit__(self, exc_type, exc_value, traceback):  # type: (MqttPublisher, str, str, dict) -> None
        self.disconnect()
        self.loop_stop()


def get_configurations(dut: Dut) -> Dict[str,Any]:
    publish_cfg = {}
    try:
        @no_type_check
        def get_broker_from_dut(dut, config_option):
            # logging.info('Option:', config_option, dut.app.sdkconfig.get(config_option))
            value = re.search(r'\:\/\/([^:]+)\:([0-9]+)', dut.app.sdkconfig.get(config_option))
            if value is None:
                return None, None
            return value.group(1), int(value.group(2))
        # Get publish test configuration
        publish_cfg['publish_topic'] = dut.app.sdkconfig.get('EXAMPLE_SUBSCRIBE_TOPIC').replace('"','')
        publish_cfg['subscribe_topic'] = dut.app.sdkconfig.get('EXAMPLE_PUBLISH_TOPIC').replace('"','')
        publish_cfg['broker_host_ssl'], publish_cfg['broker_port_ssl'] = get_broker_from_dut(dut, 'EXAMPLE_BROKER_SSL_URI')
        publish_cfg['broker_host_tcp'], publish_cfg['broker_port_tcp'] = get_broker_from_dut(dut, 'EXAMPLE_BROKER_TCP_URI')
        publish_cfg['broker_host_ws'], publish_cfg['broker_port_ws'] = get_broker_from_dut(dut, 'EXAMPLE_BROKER_WS_URI')
        publish_cfg['broker_host_wss'], publish_cfg['broker_port_wss'] = get_broker_from_dut(dut, 'EXAMPLE_BROKER_WSS_URI')

    except Exception:
        logging.info('ENV_TEST_FAILURE: Some mandatory PUBLISH test case not found in sdkconfig')
        raise
    logging.info(f'configuration: {publish_cfg}')
    return publish_cfg


@contextlib.contextmanager
def connected_and_subscribed(dut:Dut, transport:str, pattern:str, pattern_repetitions:int) -> Any:
    dut.write(f'publish_setup {transport} {pattern} {pattern_repetitions}')
    dut.write(f'start')
    dut.expect(re.compile(rb'MQTT_EVENT_SUBSCRIBED'), timeout=60)
    yield
    dut.write(f'stop')


def get_scenarios() -> List[Dict[str, int]]:
    scenarios = []
    # Initialize message sizes and repeat counts (if defined in the environment)
    for i in count(0):
        # Check env variable: MQTT_PUBLISH_MSG_{len|repeat}_{x}
        env_dict = {var:'MQTT_PUBLISH_MSG_' + var + '_' + str(i) for var in ['len', 'repeat']}
        if os.getenv(env_dict['len']) and os.getenv(env_dict['repeat']):
            scenarios.append({var: int(os.getenv(env_dict[var])) for var in ['len', 'repeat']})  # type: ignore
            continue
        break
    if not scenarios:    # No message sizes present in the env - set defaults
        scenarios = [{'len':0,   'repeat':5},    # zero-sized messages
                     {'len':2,   'repeat':5},   # short messages
                     {'len':200, 'repeat':3},    # long messages
                     ]
    return scenarios


def get_timeout(test_case: Any) -> int:
    transport, qos, enqueue, scenario = test_case
    if transport in ['ws', 'wss'] or qos == 2:
        return 90
    return 60


def run_publish_test_case(dut: Dut, test_case: Any, publish_cfg: Any) -> None:
    transport, qos, enqueue, scenario = test_case
    if publish_cfg['broker_host_' + transport] is None:
        pytest.skip(f'Skipping transport: {transport}...')
    repeat = scenario['len']
    published = scenario['repeat']
    publish_cfg['qos'] = qos
    publish_cfg['queue'] = enqueue
    publish_cfg['transport'] = transport
    test_timeout = get_timeout(test_case)
    logging.info(f'Starting Publish test: transport:{transport}, qos:{qos}, nr_of_msgs:{published},'
                 f' msg_size:{repeat*DEFAULT_MSG_SIZE}, enqueue:{enqueue}')
    with MqttPublisher(repeat, published, publish_cfg) as publisher, connected_and_subscribed(dut, transport, publisher.sample_string, scenario['len']):
        msgs_published: List[mqtt.MQTTMessageInfo] = []
        dut.write(f'publish {publisher.published} {qos} {enqueue}')
        assert publisher.event_client_got_all.wait(timeout=test_timeout), (f'Not all data received from ESP32: {transport} '
                                                                           f'qos={qos} received: {publisher.received} '
                                                                           f'expected: {publisher.published}')
        logging.info(' - all data received from ESP32')
        payload = publisher.sample_string * publisher.repeat
        for _ in range(publisher.published):
            with publisher.lock:
                msg = publisher.publish(topic=publisher.publish_cfg['publish_topic'], payload=payload, qos=qos)
                if qos > 0:
                    msgs_published.append(msg)
        logging.info(f'Published: {len(msgs_published)}')
        while msgs_published:
            msgs_published = [msg for msg in msgs_published if msg.is_published()]

        try:
            dut.expect(re.compile(rb'Correct pattern received exactly x times'), timeout=test_timeout)
        except pexpect.exceptions.ExceptionPexpect:
            dut.write(f'publish_report')
            dut.expect(re.compile(rb'Test Report'), timeout=30)
            raise
        logging.info('ESP32 received all data from runner')


stress_scenarios = [{'len':20,  'repeat':50}]    # many medium sized
transport_cases = ['tcp', 'ws', 'wss', 'ssl']
qos_cases = [0, 1, 2]
enqueue_cases = [0, 1]


def make_cases(scenarios: List[Dict[str, int]]) -> List[Tuple[str, int, int, Dict[str, int]]]:
    return [test_case for test_case in product(transport_cases, qos_cases, enqueue_cases, scenarios)]


test_cases = make_cases(get_scenarios())
stress_test_cases = make_cases(stress_scenarios)


@pytest.mark.esp32
@pytest.mark.ethernet
@pytest.mark.nightly_run
@pytest.mark.parametrize('test_case', test_cases)
def test_mqtt_publish(dut: Dut, test_case: Any) -> None:
    publish_cfg = get_configurations(dut)
    dut.expect(re.compile(rb'mqtt>'), timeout=30)
    dut.confirm_write('init', expect_pattern='init', timeout=30)
    run_publish_test_case(dut, test_case, publish_cfg)


@pytest.mark.esp32
@pytest.mark.ethernet
@pytest.mark.nightly_run
@pytest.mark.parametrize('test_case', stress_test_cases)
def test_mqtt_publish_stress(dut: Dut, test_case: Any) -> None:
    publish_cfg = get_configurations(dut)
    dut.expect(re.compile(rb'mqtt>'), timeout=30)
    dut.write('init')
    run_publish_test_case(dut, test_case, publish_cfg)
