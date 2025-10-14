# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import contextlib
import difflib
import logging
import os
import random
import re
import ssl
import string
import time
from itertools import count
from itertools import product
from threading import Event
from threading import Lock
from typing import Any
from typing import Dict
from typing import List
from typing import Tuple
from typing import no_type_check

import paho.mqtt.client as mqtt
import pexpect
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

DEFAULT_MSG_SIZE = 16


# Publisher class creating a python client to send/receive published data from esp-mqtt client
class MqttPublisher(mqtt.Client):
    def __init__(self, config, log_details=False):  # type: (MqttPublisher, dict, bool) -> None
        self.log_details = log_details
        self.config = config
        self.expected_data = f'{config["pattern"] * config["scenario"]["msg_len"]}'
        self.received = 0
        self.subscribe_mid = 0
        self.lock = Lock()
        self.event_client_connected = Event()
        self.event_client_subscribed = Event()
        self.event_client_got_all = Event()
        transport = 'websockets' if self.config['transport'] in ['ws', 'wss'] else 'tcp'
        client_id = 'MqttTestRunner' + ''.join(
            random.choice(string.ascii_uppercase + string.ascii_lowercase) for _ in range(5)
        )
        super().__init__(client_id, userdata=0, transport=transport)

    def print_details(self, text):  # type: (str) -> None
        if self.log_details:
            logging.info(text)

    def on_subscribe(self, client: Any, userdata: Any, mid: Any, granted_qos: Any) -> None:
        """Verify successful subscription."""
        if mid == self.subscribe_mid:
            logging.info(f'Subscribed to {self.config["subscribe_topic"]} successfully with QoS: {granted_qos}')
            self.event_client_subscribed.set()

    def on_connect(self, mqttc: Any, obj: Any, flags: Any, rc: int) -> None:
        self.event_client_connected.set()

    def on_connect_fail(self, mqttc: Any, obj: Any) -> None:
        logging.error('Connect failed')

    def on_message(self, mqttc: mqtt.Client, obj: Any, msg: mqtt.MQTTMessage) -> None:
        payload = msg.payload.decode('utf-8')
        if payload == self.expected_data:
            self.received += 1
            if self.received == self.config['scenario']['nr_of_msgs']:
                self.event_client_got_all.set()
        else:
            differences = len(list(filter(lambda data: data[0] != data[1], zip(payload, self.expected_data))))
            logging.error(
                f'Payload on topic "{msg.topic}" (QoS {msg.qos}) differs in {differences} positions '
                'from expected data. '
                f'Received size: {len(payload)}, expected size: {len(self.expected_data)}.'
            )
            logging.info(f'Repetitions: {payload.count(self.config["pattern"])}')
            logging.info(f'Pattern: {self.config["pattern"]}')
            logging.info(f'First: {payload[:DEFAULT_MSG_SIZE]}')
            logging.info(f'Last: {payload[-DEFAULT_MSG_SIZE:]}')
            matcher = difflib.SequenceMatcher(a=payload, b=self.expected_data)
            for match in matcher.get_matching_blocks():
                logging.info(f'Match: {match}')

    def __enter__(self) -> Any:
        qos = self.config['qos']
        broker_host = self.config['broker_host_' + self.config['transport']]
        broker_port = self.config['broker_port_' + self.config['transport']]
        connect_timeout_seconds = self.config.get('client_connect_timeout', 30)

        try:
            self.print_details('Connecting...')
            if self.config['transport'] in ['ssl', 'wss']:
                self.tls_set(None, None, None, cert_reqs=ssl.CERT_NONE, tls_version=ssl.PROTOCOL_TLSv1_2, ciphers=None)
                self.tls_insecure_set(True)
            self.event_client_connected.clear()
            self.loop_start()
            self.connect(broker_host, broker_port, 60)  # paho's keepalive
        except Exception:
            self.print_details(f'ENV_TEST_FAILURE: Unexpected error while connecting to broker {broker_host}')
            raise
        self.print_details(f'Connecting py-client to broker {broker_host}:{broker_port}...')

        if not self.event_client_connected.wait(timeout=connect_timeout_seconds):
            raise ValueError(
                f'ENV_TEST_FAILURE: Test script cannot connect to broker: {broker_host} '
                f'within {connect_timeout_seconds}s'
            )
        self.event_client_got_all.clear()
        result, self.subscribe_mid = self.subscribe(self.config['subscribe_topic'], qos)
        assert result == 0
        return self

    def __exit__(self, exc_type, exc_value, traceback):  # type: (MqttPublisher, str, str, dict) -> None
        self.disconnect()
        self.loop_stop()


def get_configurations(dut: Dut, test_case: Any) -> Dict[str, Any]:
    publish_cfg = {}
    try:

        @no_type_check
        def get_config_from_dut(dut, config_option):
            # logging.info('Option:', config_option, dut.app.sdkconfig.get(config_option))
            value = re.search(r'\:\/\/([^:]+)\:([0-9]+)', dut.app.sdkconfig.get(config_option))
            if value is None:
                return None, None
            return value.group(1), int(value.group(2))

        # Get publish test configuration
        publish_cfg['broker_host_ssl'], publish_cfg['broker_port_ssl'] = get_config_from_dut(
            dut, 'EXAMPLE_BROKER_SSL_URI'
        )
        publish_cfg['broker_host_tcp'], publish_cfg['broker_port_tcp'] = get_config_from_dut(
            dut, 'EXAMPLE_BROKER_TCP_URI'
        )
        publish_cfg['broker_host_ws'], publish_cfg['broker_port_ws'] = get_config_from_dut(dut, 'EXAMPLE_BROKER_WS_URI')
        publish_cfg['broker_host_wss'], publish_cfg['broker_port_wss'] = get_config_from_dut(
            dut, 'EXAMPLE_BROKER_WSS_URI'
        )

    except Exception:
        logging.info('ENV_TEST_FAILURE: Some mandatory PUBLISH test case not found in sdkconfig')
        raise
    transport, qos, enqueue, scenario = test_case
    if publish_cfg['broker_host_' + transport] is None:
        pytest.skip(f'Skipping transport: {transport}...')
    publish_cfg['scenario'] = scenario
    publish_cfg['qos'] = qos
    publish_cfg['enqueue'] = enqueue
    publish_cfg['transport'] = transport
    publish_cfg['pattern'] = ''.join(
        random.choice(string.ascii_uppercase + string.ascii_lowercase + string.digits) for _ in range(DEFAULT_MSG_SIZE)
    )
    publish_cfg['client_connect_timeout'] = 30
    publish_cfg['dut_subscribe_timeout'] = 60
    publish_cfg['publish_ack_timeout'] = 60
    publish_cfg['test_timeout'] = get_timeout(test_case)

    unique_topic = ''.join(
        random.choice(string.ascii_uppercase + string.ascii_lowercase) for _ in range(DEFAULT_MSG_SIZE)
    )
    publish_cfg['subscribe_topic'] = 'test/subscribe_to/' + unique_topic
    publish_cfg['publish_topic'] = 'test/subscribe_to/' + unique_topic
    logging.info(f'configuration: {publish_cfg}')
    return publish_cfg


@contextlib.contextmanager
def connected_and_subscribed(dut: Dut, config: Dict[str, Any]) -> Any:
    dut.write('start')
    dut_subscribe_timeout = config.get('dut_subscribe_timeout', 60)
    dut.expect(re.compile(rb'MQTT_EVENT_SUBSCRIBED'), timeout=dut_subscribe_timeout)
    yield
    dut.write('stop')


def get_scenarios() -> List[Dict[str, int]]:
    scenarios = []
    # Initialize message sizes and repeat counts (if defined in the environment)
    for i in count(0):
        # Check env variable: MQTT_PUBLISH_MSG_{len|repeat}_{x}
        env_dict = {var: 'MQTT_PUBLISH_MSG_' + var + '_' + str(i) for var in ['len', 'repeat']}
        if os.getenv(env_dict['len']) and os.getenv(env_dict['repeat']):
            scenarios.append({var: int(os.getenv(env_dict[var])) for var in ['len', 'repeat']})  # type: ignore
            continue
        break
    if not scenarios:  # No message sizes present in the env - set defaults
        logging.info('Using predefined cases')
        scenarios = [
            {'msg_len': 0, 'nr_of_msgs': 5},  # zero-sized messages
            {'msg_len': 2, 'nr_of_msgs': 5},  # short messages
            {'msg_len': 200, 'nr_of_msgs': 3},  # long messages
        ]
    return scenarios


def get_timeout(test_case: Any) -> int:
    transport, qos, enqueue, scenario = test_case
    timeout = int(scenario['nr_of_msgs'] * 20)
    if qos == 1:
        timeout += 30
    if qos == 2:
        timeout += 45
    if transport in ['ws', 'wss']:
        timeout += 30
    return timeout


def run_publish_test_case(dut: Dut, config: Any) -> None:
    logging.info(
        f'Starting Publish test: transport:{config["transport"]}, qos:{config["qos"]},'
        f'nr_of_msgs:{config["scenario"]["nr_of_msgs"]},'
        f' msg_size:{config["scenario"]["msg_len"]}, enqueue:{config["enqueue"]}'
    )
    dut.write(
        f'publish_setup {config["transport"]} {config["publish_topic"]}'
        f' {config["subscribe_topic"]} {config["pattern"]} {config["scenario"]["msg_len"]}'
    )
    with MqttPublisher(config) as publisher, connected_and_subscribed(dut, config):
        py_client_subscribe_timeout = config.get('py_client_subscribe_timeout', config['test_timeout'])
        assert publisher.event_client_subscribed.wait(timeout=py_client_subscribe_timeout), 'Runner failed to subscribe'
        msgs_published: List[mqtt.MQTTMessageInfo] = []
        dut.write(f'publish {config["scenario"]["nr_of_msgs"]} {config["qos"]} {config["enqueue"]}')
        assert publisher.event_client_got_all.wait(timeout=config['test_timeout']), (
            f'Not all data received from ESP32: {config["transport"]} '
            f'qos={config["qos"]} received: {publisher.received} '
            f'expected: {config["scenario"]["nr_of_msgs"]}'
        )
        logging.info(' - all data received from ESP32')
        payload = config['pattern'] * config['scenario']['msg_len']
        for _ in range(config['scenario']['nr_of_msgs']):
            with publisher.lock:
                msg = publisher.publish(topic=config['publish_topic'], payload=payload, qos=config['qos'])
                if config['qos'] > 0:
                    msgs_published.append(msg)
        logging.info(f'Published: {len(msgs_published)} messages from script with QoS > 0 needing ACK.')

        if msgs_published:
            publish_ack_timeout_seconds = config.get('publish_ack_timeout', 60)  # Default 60s, make configurable
            ack_wait_start_time = time.time()
            initial_unacked_count = len(msgs_published)
            logging.info(f'Waiting {initial_unacked_count} publish ack with timeout {publish_ack_timeout_seconds}s...')

            while msgs_published:
                if time.time() - ack_wait_start_time > publish_ack_timeout_seconds:
                    unacked_mids = [msg.mid for msg in msgs_published if msg.mid is not None and not msg.is_published()]
                    logging.error(
                        f'Timeout waiting for publish acknowledgements. '
                        f'{len(unacked_mids)} of {initial_unacked_count} messages remain unacknowledged. '
                        f'Unacked MIDs: {unacked_mids}'
                    )
                    # This will likely cause the test to fail at a later assertion,
                    # or you could raise an explicit error here.
                    # e.g. raise Exception('Timeout waiting for publish acknowledgements')
                    break
                msgs_published = [msg for msg in msgs_published if not msg.is_published()]
                if msgs_published:  # Avoid busy-looping if list is not empty
                    time.sleep(0.1)  # Brief pause
            if not msgs_published:
                logging.info('All script-published QoS > 0 messages acknowledged by broker.')

        logging.info('All messages from runner published (or timed out waiting for ACK).')

        try:
            dut.expect(re.compile(rb'Correct pattern received exactly x times'), timeout=config['test_timeout'])
        except pexpect.exceptions.ExceptionPexpect:
            dut.write('publish_report')
            dut.expect(re.compile(rb'Test Report'), timeout=30)
            raise
        logging.info('ESP32 received all data from runner')


stress_scenarios = [{'msg_len': 20, 'nr_of_msgs': 30}]  # many medium sized
transport_cases = ['tcp', 'ws', 'wss', 'ssl']
qos_cases = [0, 1, 2]
enqueue_cases = [0, 1]
local_broker_supported_transports = ['tcp']
local_broker_scenarios = [
    {'msg_len': 0, 'nr_of_msgs': 5},  # zero-sized messages
    {'msg_len': 5, 'nr_of_msgs': 20},  # short messages
    {'msg_len': 500, 'nr_of_msgs': 10},  # long messages
    {'msg_len': 20, 'nr_of_msgs': 20},
]  # many medium sized


def make_cases(transport: Any, scenarios: List[Dict[str, int]]) -> List[Tuple[str, int, int, Dict[str, int]]]:
    return [test_case for test_case in product(transport, qos_cases, enqueue_cases, scenarios)]


test_cases = make_cases(transport_cases, get_scenarios())
stress_test_cases = make_cases(transport_cases, stress_scenarios)


@pytest.mark.ethernet
@pytest.mark.parametrize('test_case', test_cases)
@pytest.mark.parametrize('config', ['default'], indirect=True)
@idf_parametrize('target', ['esp32'], indirect=['target'])
@pytest.mark.flaky(reruns=1, reruns_delay=1)
def test_mqtt_publish(dut: Dut, test_case: Any) -> None:
    publish_cfg = get_configurations(dut, test_case)
    dut.expect(re.compile(rb'mqtt>'), timeout=30)
    dut.confirm_write('init', expect_pattern='init', timeout=30)
    run_publish_test_case(dut, publish_cfg)


@pytest.mark.ethernet_stress
@pytest.mark.nightly_run
@pytest.mark.parametrize('test_case', stress_test_cases)
@pytest.mark.parametrize('config', ['default'], indirect=True)
@pytest.mark.flaky(reruns=1, reruns_delay=1)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_mqtt_publish_stress(dut: Dut, test_case: Any) -> None:
    publish_cfg = get_configurations(dut, test_case)
    dut.expect(re.compile(rb'mqtt>'), timeout=30)
    dut.write('init')
    run_publish_test_case(dut, publish_cfg)


@pytest.mark.ethernet
@pytest.mark.parametrize('test_case', make_cases(local_broker_supported_transports, local_broker_scenarios))
@pytest.mark.parametrize('config', ['local_broker'], indirect=True)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_mqtt_publish_local(dut: Dut, test_case: Any) -> None:
    if test_case[0] not in local_broker_supported_transports:
        pytest.skip(f'Skipping transport: {test_case[0]}...')
    dut_ip = dut.expect(r'esp_netif_handlers: .+ ip: (\d+\.\d+\.\d+\.\d+),').group(1)
    publish_cfg = get_configurations(dut, test_case)
    publish_cfg['broker_host_tcp'] = dut_ip
    publish_cfg['broker_port_tcp'] = 1234
    dut.expect(re.compile(rb'mqtt>'), timeout=30)
    dut.confirm_write('init', expect_pattern='init', timeout=30)
    run_publish_test_case(dut, publish_cfg)
