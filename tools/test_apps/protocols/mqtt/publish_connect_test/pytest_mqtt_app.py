# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
from __future__ import print_function, unicode_literals

import logging
import os
import random
import re
import select
import socket
import ssl
import string
import subprocess
import sys
import time
import typing
from itertools import count
from threading import Event, Lock, Thread
from typing import Any

import paho.mqtt.client as mqtt
import pytest
from common_test_methods import get_host_ip4_by_dest_ip
from pytest_embedded import Dut
from pytest_embedded_qemu.dut import QemuDut

DEFAULT_MSG_SIZE = 16


def _path(f):  # type: (str) -> str
    return os.path.join(os.path.dirname(os.path.realpath(__file__)),f)


def set_server_cert_cn(ip):  # type: (str) -> None
    arg_list = [
        ['openssl', 'req', '-out', _path('srv.csr'), '-key', _path('server.key'),'-subj', '/CN={}'.format(ip), '-new'],
        ['openssl', 'x509', '-req', '-in', _path('srv.csr'), '-CA', _path('ca.crt'),
         '-CAkey', _path('ca.key'), '-CAcreateserial', '-out', _path('srv.crt'), '-days', '360']]
    for args in arg_list:
        if subprocess.check_call(args) != 0:
            raise RuntimeError('openssl command {} failed'.format(args))


# Publisher class creating a python client to send/receive published data from esp-mqtt client
class MqttPublisher:
    event_client_connected = Event()
    event_client_got_all = Event()
    expected_data = ''
    published = 0

    def __init__(self, dut, transport,
                 qos, repeat, published, queue, publish_cfg, log_details=False):  # type: (MqttPublisher, Dut, str, int, int, int, int, dict, bool) -> None
        # instance variables used as parameters of the publish test
        self.event_stop_client = Event()
        self.sample_string = ''.join(random.choice(string.ascii_uppercase + string.ascii_lowercase + string.digits) for _ in range(DEFAULT_MSG_SIZE))
        self.client = None
        self.dut = dut
        self.log_details = log_details
        self.repeat = repeat
        self.publish_cfg = publish_cfg
        self.publish_cfg['qos'] = qos
        self.publish_cfg['queue'] = queue
        self.publish_cfg['transport'] = transport
        self.lock = Lock()
        # static variables used to pass options to and from static callbacks of paho-mqtt client
        MqttPublisher.event_client_connected = Event()
        MqttPublisher.event_client_got_all = Event()
        MqttPublisher.published = published
        MqttPublisher.event_client_connected.clear()
        MqttPublisher.event_client_got_all.clear()
        MqttPublisher.expected_data = self.sample_string * self.repeat

    def print_details(self, text):  # type: (str) -> None
        if self.log_details:
            print(text)

    def mqtt_client_task(self, client, lock):  # type: (MqttPublisher, mqtt.Client, Lock) -> None
        while not self.event_stop_client.is_set():
            with lock:
                client.loop()
            time.sleep(0.001)   # yield to other threads

    # The callback for when the client receives a CONNACK response from the server (needs to be static)
    @staticmethod
    def on_connect(_client, _userdata, _flags, _rc):  # type: (mqtt.Client, tuple, bool, str) -> None
        MqttPublisher.event_client_connected.set()

    # The callback for when a PUBLISH message is received from the server (needs to be static)
    @staticmethod
    def on_message(client, userdata, msg):  # type: (mqtt.Client, int, mqtt.client.MQTTMessage) -> None
        payload = msg.payload.decode()
        if payload == MqttPublisher.expected_data:
            userdata += 1
            client.user_data_set(userdata)
            if userdata == MqttPublisher.published:
                MqttPublisher.event_client_got_all.set()

    def __enter__(self):  # type: (MqttPublisher) -> None

        qos = self.publish_cfg['qos']
        queue = self.publish_cfg['queue']
        transport = self.publish_cfg['transport']
        broker_host = self.publish_cfg['broker_host_' + transport]
        broker_port = self.publish_cfg['broker_port_' + transport]

        # Start the test
        self.print_details("PUBLISH TEST: transport:{}, qos:{}, sequence:{}, enqueue:{}, sample msg:'{}'"
                           .format(transport, qos, MqttPublisher.published, queue, MqttPublisher.expected_data))

        try:
            if transport in ['ws', 'wss']:
                self.client = mqtt.Client(transport='websockets')
            else:
                self.client = mqtt.Client()
            assert self.client is not None
            self.client.on_connect = MqttPublisher.on_connect
            self.client.on_message = MqttPublisher.on_message
            self.client.user_data_set(0)

            if transport in ['ssl', 'wss']:
                self.client.tls_set(None, None, None, cert_reqs=ssl.CERT_NONE, tls_version=ssl.PROTOCOL_TLSv1_2, ciphers=None)
                self.client.tls_insecure_set(True)
            self.print_details('Connecting...')
            self.client.connect(broker_host, broker_port, 60)
        except Exception:
            self.print_details('ENV_TEST_FAILURE: Unexpected error while connecting to broker {}'.format(broker_host))
            raise
        # Starting a py-client in a separate thread
        thread1 = Thread(target=self.mqtt_client_task, args=(self.client, self.lock))
        thread1.start()
        self.print_details('Connecting py-client to broker {}:{}...'.format(broker_host, broker_port))
        if not MqttPublisher.event_client_connected.wait(timeout=30):
            raise ValueError('ENV_TEST_FAILURE: Test script cannot connect to broker: {}'.format(broker_host))
        with self.lock:
            self.client.subscribe(self.publish_cfg['subscribe_topic'], qos)
        self.dut.write(' '.join(str(x) for x in (transport, self.sample_string, self.repeat, MqttPublisher.published, qos, queue)), eol='\n')
        try:
            # waiting till subscribed to defined topic
            self.dut.expect(re.compile(r'MQTT_EVENT_SUBSCRIBED'), timeout=30)
            for _ in range(MqttPublisher.published):
                with self.lock:
                    self.client.publish(self.publish_cfg['publish_topic'], self.sample_string * self.repeat, qos)
                self.print_details('Publishing...')
            self.print_details('Checking esp-client received msg published from py-client...')
            self.dut.expect(re.compile(r'Correct pattern received exactly x times'), timeout=60)
            if not MqttPublisher.event_client_got_all.wait(timeout=60):
                raise ValueError('Not all data received from ESP32')
            print(' - all data received from ESP32')
        finally:
            self.event_stop_client.set()
            thread1.join()

    def __exit__(self, exc_type, exc_value, traceback):  # type: (MqttPublisher, str, str, dict) -> None
        assert self.client is not None
        self.client.disconnect()
        self.event_stop_client.clear()


# Simple server for mqtt over TLS connection
class TlsServer:

    def __init__(self, port, client_cert=False, refuse_connection=False, use_alpn=False):  # type: (TlsServer, int, bool, bool, bool) -> None
        self.port = port
        self.socket = socket.socket()
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.settimeout(10.0)
        self.shutdown = Event()
        self.client_cert = client_cert
        self.refuse_connection = refuse_connection
        self.use_alpn = use_alpn

    def __enter__(self):  # type: (TlsServer) -> TlsServer
        try:
            self.socket.bind(('', self.port))
        except socket.error as e:
            print('Bind failed:{}'.format(e))
            raise

        self.socket.listen(1)
        self.server_thread = Thread(target=self.run_server)
        self.server_thread.start()

        return self

    def __exit__(self, exc_type, exc_value, traceback):  # type: (TlsServer, str, str, str) -> None
        self.shutdown.set()
        self.server_thread.join()
        self.socket.close()
        if (self.conn is not None):
            self.conn.close()

    def get_last_ssl_error(self):  # type: (TlsServer) -> str
        return self.ssl_error

    @typing.no_type_check
    def get_negotiated_protocol(self):
        return self.negotiated_protocol

    def run_server(self) -> None:
        context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
        if self.client_cert:
            context.verify_mode = ssl.CERT_REQUIRED
            context.load_verify_locations(cafile=_path('ca.crt'))
        context.load_cert_chain(certfile=_path('srv.crt'), keyfile=_path('server.key'))
        if self.use_alpn:
            context.set_alpn_protocols(['mymqtt', 'http/1.1'])
        self.socket = context.wrap_socket(self.socket, server_side=True)
        try:
            self.conn, address = self.socket.accept()  # accept new connection
            self.socket.settimeout(10.0)
            print(' - connection from: {}'.format(address))
            if self.use_alpn:
                self.negotiated_protocol = self.conn.selected_alpn_protocol()
                print(' - negotiated_protocol: {}'.format(self.negotiated_protocol))
            self.handle_conn()
        except ssl.SSLError as e:
            self.ssl_error = str(e)
            print(' - SSLError: {}'.format(str(e)))

    def handle_conn(self) -> None:
        while not self.shutdown.is_set():
            r,w,e = select.select([self.conn], [], [], 1)
            try:
                if self.conn in r:
                    self.process_mqtt_connect()

            except socket.error as err:
                print(' - error: {}'.format(err))
                raise

    def process_mqtt_connect(self) -> None:
        try:
            data = bytearray(self.conn.recv(1024))
            message = ''.join(format(x, '02x') for x in data)
            if message[0:16] == '101800044d515454':
                if self.refuse_connection is False:
                    print(' - received mqtt connect, sending ACK')
                    self.conn.send(bytearray.fromhex('20020000'))
                else:
                    # injecting connection not authorized error
                    print(' - received mqtt connect, sending NAK')
                    self.conn.send(bytearray.fromhex('20020005'))
            else:
                raise Exception(' - error process_mqtt_connect unexpected connect received: {}'.format(message))
        finally:
            # stop the server after the connect message in happy flow, or if any exception occur
            self.shutdown.set()


def connection_tests(dut, cases, dut_ip):  # type: (Dut, dict, str) -> None
    ip = get_host_ip4_by_dest_ip(dut_ip)
    set_server_cert_cn(ip)
    server_port = 2222

    def teardown_connection_suite() -> None:
        dut.write('conn teardown 0 0')

    def start_connection_case(case, desc):  # type: (str, str) -> Any
        print('Starting {}: {}'.format(case, desc))
        case_id = cases[case]
        dut.write('conn {} {} {}'.format(ip, server_port, case_id))
        dut.expect('Test case:{} started'.format(case_id))
        return case_id

    for case in ['EXAMPLE_CONNECT_CASE_NO_CERT', 'EXAMPLE_CONNECT_CASE_SERVER_CERT', 'EXAMPLE_CONNECT_CASE_SERVER_DER_CERT']:
        # All these cases connect to the server with no server verification or with server only verification
        with TlsServer(server_port):
            test_nr = start_connection_case(case, 'default server - expect to connect normally')
            dut.expect('MQTT_EVENT_CONNECTED: Test={}'.format(test_nr), timeout=30)
        with TlsServer(server_port, refuse_connection=True):
            test_nr = start_connection_case(case, 'ssl shall connect, but mqtt sends connect refusal')
            dut.expect('MQTT_EVENT_ERROR: Test={}'.format(test_nr), timeout=30)
            dut.expect('MQTT ERROR: 0x5')  # expecting 0x5 ... connection not authorized error
        with TlsServer(server_port, client_cert=True) as s:
            test_nr = start_connection_case(case, 'server with client verification - handshake error since client presents no client certificate')
            dut.expect('MQTT_EVENT_ERROR: Test={}'.format(test_nr), timeout=30)
            dut.expect('ESP-TLS ERROR: ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED')  # expect ... handshake error (PEER_DID_NOT_RETURN_A_CERTIFICATE)
            if 'PEER_DID_NOT_RETURN_A_CERTIFICATE' not in s.get_last_ssl_error():
                raise RuntimeError('Unexpected ssl error from the server {}'.format(s.get_last_ssl_error()))

    for case in ['EXAMPLE_CONNECT_CASE_MUTUAL_AUTH', 'EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_KEY_PWD']:
        # These cases connect to server with both server and client verification (client key might be password protected)
        with TlsServer(server_port, client_cert=True):
            test_nr = start_connection_case(case, 'server with client verification - expect to connect normally')
            dut.expect('MQTT_EVENT_CONNECTED: Test={}'.format(test_nr), timeout=30)

    case = 'EXAMPLE_CONNECT_CASE_INVALID_SERVER_CERT'
    with TlsServer(server_port) as s:
        test_nr = start_connection_case(case, 'invalid server certificate on default server - expect ssl handshake error')
        dut.expect('MQTT_EVENT_ERROR: Test={}'.format(test_nr), timeout=30)
        dut.expect('ESP-TLS ERROR: ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED')  # expect ... handshake error (TLSV1_ALERT_UNKNOWN_CA)
        if 'alert unknown ca' not in s.get_last_ssl_error():
            raise Exception('Unexpected ssl error from the server {}'.format(s.get_last_ssl_error()))

    case = 'EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_BAD_CRT'
    with TlsServer(server_port, client_cert=True) as s:
        test_nr = start_connection_case(case, 'Invalid client certificate on server with client verification - expect ssl handshake error')
        dut.expect('MQTT_EVENT_ERROR: Test={}'.format(test_nr), timeout=30)
        dut.expect('ESP-TLS ERROR: ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED')  # expect ... handshake error (CERTIFICATE_VERIFY_FAILED)
        if 'CERTIFICATE_VERIFY_FAILED' not in s.get_last_ssl_error():
            raise Exception('Unexpected ssl error from the server {}'.format(s.get_last_ssl_error()))

    for case in ['EXAMPLE_CONNECT_CASE_NO_CERT', 'EXAMPLE_CONNECT_CASE_NO_CERT_ALPN']:
        with TlsServer(server_port, use_alpn=True) as s:
            test_nr = start_connection_case(case, 'server with alpn - expect connect, check resolved protocol')
            dut.expect('MQTT_EVENT_CONNECTED: Test={}'.format(test_nr), timeout=30)
            if case == 'EXAMPLE_CONNECT_CASE_NO_CERT' and s.get_negotiated_protocol() is None:
                print(' - client with alpn off, no negotiated protocol: OK')
            elif case == 'EXAMPLE_CONNECT_CASE_NO_CERT_ALPN' and s.get_negotiated_protocol() == 'mymqtt':
                print(' - client with alpn on, negotiated protocol resolved: OK')
            else:
                raise Exception('Unexpected negotiated protocol {}'.format(s.get_negotiated_protocol()))

    teardown_connection_suite()


@pytest.mark.esp32
@pytest.mark.ethernet
def test_app_protocol_mqtt_publish_connect(dut: Dut) -> None:
    """
    steps:
      1. join AP
      2. connect to uri specified in the config
      3. send and receive data
    """
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'mqtt_publish_connect_test.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('[Performance][mqtt_publish_connect_test_bin_size]: %s KB', bin_size // 1024)

    # Look for test case symbolic names and publish configs
    cases = {}
    publish_cfg = {}
    try:

        # Get connection test cases configuration: symbolic names for test cases
        for case in ['EXAMPLE_CONNECT_CASE_NO_CERT',
                     'EXAMPLE_CONNECT_CASE_SERVER_CERT',
                     'EXAMPLE_CONNECT_CASE_MUTUAL_AUTH',
                     'EXAMPLE_CONNECT_CASE_INVALID_SERVER_CERT',
                     'EXAMPLE_CONNECT_CASE_SERVER_DER_CERT',
                     'EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_KEY_PWD',
                     'EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_BAD_CRT',
                     'EXAMPLE_CONNECT_CASE_NO_CERT_ALPN']:
            cases[case] = dut.app.sdkconfig.get(case)
    except Exception:
        print('ENV_TEST_FAILURE: Some mandatory CONNECTION test case not found in sdkconfig')
        raise

    esp_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30).group(1).decode()
    print('Got IP={}'.format(esp_ip))

    if not os.getenv('MQTT_SKIP_CONNECT_TEST'):
        connection_tests(dut,cases,esp_ip)

    #
    # start publish tests only if enabled in the environment (for weekend tests only)
    if not os.getenv('MQTT_PUBLISH_TEST'):
        return

    # Get publish test configuration
    try:
        @typing.no_type_check
        def get_host_port_from_dut(dut, config_option):
            value = re.search(r'\:\/\/([^:]+)\:([0-9]+)', dut.app.sdkconfig.get(config_option))
            if value is None:
                return None, None
            return value.group(1), int(value.group(2))

        publish_cfg['publish_topic'] = dut.app.sdkconfig.get('EXAMPLE_SUBSCRIBE_TOPIC').replace('"','')
        publish_cfg['subscribe_topic'] = dut.app.sdkconfig.get('EXAMPLE_PUBLISH_TOPIC').replace('"','')
        publish_cfg['broker_host_ssl'], publish_cfg['broker_port_ssl'] = get_host_port_from_dut(dut, 'EXAMPLE_BROKER_SSL_URI')
        publish_cfg['broker_host_tcp'], publish_cfg['broker_port_tcp'] = get_host_port_from_dut(dut, 'EXAMPLE_BROKER_TCP_URI')
        publish_cfg['broker_host_ws'], publish_cfg['broker_port_ws'] = get_host_port_from_dut(dut, 'EXAMPLE_BROKER_WS_URI')
        publish_cfg['broker_host_wss'], publish_cfg['broker_port_wss'] = get_host_port_from_dut(dut, 'EXAMPLE_BROKER_WSS_URI')

    except Exception:
        print('ENV_TEST_FAILURE: Some mandatory PUBLISH test case not found in sdkconfig')
        raise

    def start_publish_case(transport, qos, repeat, published, queue):  # type: (str, int, int, int, int) -> None
        print('Starting Publish test: transport:{}, qos:{}, nr_of_msgs:{}, msg_size:{}, enqueue:{}'
              .format(transport, qos, published, repeat * DEFAULT_MSG_SIZE, queue))
        with MqttPublisher(dut, transport, qos, repeat, published, queue, publish_cfg):
            pass

    # Initialize message sizes and repeat counts (if defined in the environment)
    messages = []
    for i in count(0):
        # Check env variable: MQTT_PUBLISH_MSG_{len|repeat}_{x}
        env_dict = {var:'MQTT_PUBLISH_MSG_' + var + '_' + str(i) for var in ['len', 'repeat']}
        if os.getenv(env_dict['len']) and os.getenv(env_dict['repeat']):
            messages.append({var: int(os.getenv(env_dict[var])) for var in ['len', 'repeat']})  # type: ignore
            continue
        break
    if not messages:    # No message sizes present in the env - set defaults
        messages = [{'len':0,   'repeat':5},    # zero-sized messages
                    {'len':2,   'repeat':10},   # short messages
                    {'len':200, 'repeat':3},    # long messages
                    {'len':20,  'repeat':50}    # many medium sized
                    ]

    # Iterate over all publish message properties
    for qos in [0, 1, 2]:
        for transport in ['tcp', 'ssl', 'ws', 'wss']:
            for q in [0, 1]:
                if publish_cfg['broker_host_' + transport] is None:
                    print('Skipping transport: {}...'.format(transport))
                    continue
                for msg in messages:
                    start_publish_case(transport, qos, msg['len'], msg['repeat'], q)


if __name__ == '__main__':
    test_app_protocol_mqtt_publish_connect(dut=QemuDut if sys.argv[1:] == ['qemu'] else Dut)
