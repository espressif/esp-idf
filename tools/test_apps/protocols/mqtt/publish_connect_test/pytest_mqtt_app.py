# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import contextlib
import logging
import os
import re
import socketserver
import ssl
import subprocess
from threading import Thread
from typing import Any, Callable, Dict, Optional

import pytest
from common_test_methods import get_host_ip4_by_dest_ip
from pytest_embedded import Dut

SERVER_PORT = 2222


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


class MQTTHandler(socketserver.StreamRequestHandler):

    def handle(self) -> None:
        logging.info(' - connection from: {}'.format(self.client_address))
        data = bytearray(self.request.recv(1024))
        message = ''.join(format(x, '02x') for x in data)
        if message[0:16] == '101800044d515454':
            if self.server.refuse_connection is False:  # type: ignore
                logging.info(' - received mqtt connect, sending ACK')
                self.request.send(bytearray.fromhex('20020000'))
            else:
                # injecting connection not authorized error
                logging.info(' - received mqtt connect, sending NAK')
                self.request.send(bytearray.fromhex('20020005'))
        else:
            raise Exception(' - error process_mqtt_connect unexpected connect received: {}'.format(message))


# Simple server for mqtt over TLS connection
class TlsServer(socketserver.TCPServer):
    timeout = 30.0
    allow_reuse_address = True
    allow_reuse_port = True

    def __init__(self,
                 port:int = SERVER_PORT,
                 ServerHandler: Callable[[Any, Any, Any], socketserver.BaseRequestHandler] = MQTTHandler,
                 client_cert:bool=False,
                 refuse_connection:bool=False,
                 use_alpn:bool=False):
        self.refuse_connection = refuse_connection
        self.context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
        self.ssl_error = ''
        self.alpn_protocol: Optional[str] = None
        if client_cert:
            self.context.verify_mode = ssl.CERT_REQUIRED
            self.context.load_verify_locations(cafile=_path('ca.crt'))
        self.context.load_cert_chain(certfile=_path('srv.crt'), keyfile=_path('server.key'))
        if use_alpn:
            self.context.set_alpn_protocols(['mymqtt', 'http/1.1'])
        self.server_thread = Thread(target=self.serve_forever)
        super().__init__(('',port), ServerHandler)

    def server_activate(self) -> None:
        self.socket = self.context.wrap_socket(self.socket, server_side=True)
        super().server_activate()

    def __enter__(self):  # type: ignore
        self.server_thread.start()
        return self

    def server_close(self) -> None:
        try:
            self.shutdown()
            self.server_thread.join()
            super().server_close()
        except RuntimeError as e:
            logging.exception(e)

    # We need to override it here to capture ssl.SSLError
    # The implementation is a slightly modified version from cpython original code.
    def _handle_request_noblock(self) -> None:
        try:
            request, client_address = self.get_request()
            self.alpn_protocol = request.selected_alpn_protocol()  # type: ignore
        except ssl.SSLError as e:
            self.ssl_error = e.reason
            return
        except OSError:
            return
        if self.verify_request(request, client_address):
            try:
                self.process_request(request, client_address)
            except Exception:
                self.handle_error(request, client_address)
                self.shutdown_request(request)
            except:  # noqa: E722
                self.shutdown_request(request)
                raise
        else:
            self.shutdown_request(request)

    def last_ssl_error(self):  # type: (TlsServer) -> str
        return self.ssl_error

    def get_negotiated_protocol(self) -> Optional[str]:
        return self.alpn_protocol


def get_test_cases(dut: Dut) -> Any:
    cases = {}
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
        logging.error('ENV_TEST_FAILURE: Some mandatory CONNECTION test case not found in sdkconfig')
        raise
    return cases


def get_dut_ip(dut: Dut) -> Any:
    dut_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30).group(1).decode()
    logging.info('Got IP={}'.format(dut_ip))
    return get_host_ip4_by_dest_ip(dut_ip)


@contextlib.contextmanager
def connect_dut(dut: Dut, uri:str, case_id:int) -> Any:
    dut.write('connection_setup')
    dut.write(f'connect {uri} {case_id}')
    dut.expect(f'Test case:{case_id} started')
    dut.write('reconnect')
    yield
    dut.write('connection_teardown')
    dut.write('disconnect')


def run_cases(dut:Dut, uri:str, cases:Dict[str, int]) -> None:
    try:
        dut.write('init')
        dut.write(f'start')
        dut.write(f'disconnect')
        for case in ['EXAMPLE_CONNECT_CASE_NO_CERT', 'EXAMPLE_CONNECT_CASE_SERVER_CERT', 'EXAMPLE_CONNECT_CASE_SERVER_DER_CERT']:
            # All these cases connect to the server with no server verification or with server only verification
            with TlsServer(), connect_dut(dut, uri, cases[case]):
                logging.info(f'Running {case}: default server - expect to connect normally')
                dut.expect(f'MQTT_EVENT_CONNECTED: Test={cases[case]}', timeout=30)
            with TlsServer(refuse_connection=True), connect_dut(dut, uri, cases[case]):
                logging.info(f'Running {case}: ssl shall connect, but mqtt sends connect refusal')
                dut.expect(f'MQTT_EVENT_ERROR: Test={cases[case]}', timeout=30)
                dut.expect('MQTT ERROR: 0x5')  # expecting 0x5 ... connection not authorized error
            with TlsServer(client_cert=True) as server, connect_dut(dut, uri, cases[case]):
                logging.info(f'Running {case}: server with client verification - handshake error since client presents no client certificate')
                dut.expect(f'MQTT_EVENT_ERROR: Test={cases[case]}', timeout=30)
                dut.expect('ESP-TLS ERROR: ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED')  # expect ... handshake error (PEER_DID_NOT_RETURN_A_CERTIFICATE)
                assert 'PEER_DID_NOT_RETURN_A_CERTIFICATE' in server.last_ssl_error()

        for case in ['EXAMPLE_CONNECT_CASE_MUTUAL_AUTH', 'EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_KEY_PWD']:
            # These cases connect to server with both server and client verification (client key might be password protected)
            with TlsServer(client_cert=True), connect_dut(dut, uri, cases[case]):
                logging.info(f'Running {case}: server with client verification - expect to connect normally')
                dut.expect(f'MQTT_EVENT_CONNECTED: Test={cases[case]}', timeout=30)

        case = 'EXAMPLE_CONNECT_CASE_INVALID_SERVER_CERT'
        with TlsServer() as s, connect_dut(dut, uri, cases[case]):
            logging.info(f'Running {case}: invalid server certificate on default server - expect ssl handshake error')
            dut.expect(f'MQTT_EVENT_ERROR: Test={cases[case]}', timeout=30)
            dut.expect('ESP-TLS ERROR: ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED')  # expect ... handshake error (TLSV1_ALERT_UNKNOWN_CA)
            if re.match('.*alert.*unknown.*ca',s.last_ssl_error(), flags=re.I) is None:
                raise Exception(f'Unexpected ssl error from the server: {s.last_ssl_error()}')

        case = 'EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_BAD_CRT'
        with TlsServer(client_cert=True) as s, connect_dut(dut, uri, cases[case]):
            logging.info(f'Running {case}: Invalid client certificate on server with client verification - expect ssl handshake error')
            dut.expect(f'MQTT_EVENT_ERROR: Test={cases[case]}', timeout=30)
            dut.expect('ESP-TLS ERROR: ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED')  # expect ... handshake error (CERTIFICATE_VERIFY_FAILED)
            if 'CERTIFICATE_VERIFY_FAILED' not in s.last_ssl_error():
                raise Exception('Unexpected ssl error from the server {}'.format(s.last_ssl_error()))

        for case in ['EXAMPLE_CONNECT_CASE_NO_CERT', 'EXAMPLE_CONNECT_CASE_NO_CERT_ALPN']:
            with TlsServer(use_alpn=True) as s, connect_dut(dut, uri, cases[case]):
                logging.info(f'Running {case}: server with alpn - expect connect, check resolved protocol')
                dut.expect(f'MQTT_EVENT_CONNECTED: Test={cases[case]}', timeout=30)
                if case == 'EXAMPLE_CONNECT_CASE_NO_CERT':
                    assert s.get_negotiated_protocol() is None
                elif case == 'EXAMPLE_CONNECT_CASE_NO_CERT_ALPN':
                    assert s.get_negotiated_protocol() == 'mymqtt'
                else:
                    assert False, f'Unexpected negotiated protocol {s.get_negotiated_protocol()}'
    finally:
        dut.write('stop')
        dut.write('destroy')


@pytest.mark.esp32
@pytest.mark.ethernet
def test_mqtt_connect(
    dut: Dut,
    log_performance: Callable[[str, object], None],
) -> None:
    """
    steps:
      1. join AP
      2. connect to uri specified in the config
      3. send and receive data
    """
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'mqtt_publish_connect_test.bin')
    bin_size = os.path.getsize(binary_file)
    log_performance('mqtt_publish_connect_test_bin_size', f'{bin_size // 1024} KB')

    ip = get_dut_ip(dut)
    set_server_cert_cn(ip)
    uri = f'mqtts://{ip}:{SERVER_PORT}'

    # Look for test case symbolic names and publish configs
    cases = get_test_cases(dut)
    dut.expect_exact('mqtt>', timeout=30)
    run_cases(dut, uri, cases)
