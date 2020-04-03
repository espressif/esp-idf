from __future__ import print_function
from __future__ import unicode_literals
import re
import os
import socket
import select
import subprocess
from threading import Thread, Event
import ttfw_idf
import ssl


def _path(f):
    return os.path.join(os.path.dirname(os.path.realpath(__file__)),f)


def set_server_cert_cn(ip):
    arg_list = [
        ['openssl', 'req', '-out', _path('srv.csr'), '-key', _path('server.key'),'-subj', "/CN={}".format(ip), '-new'],
        ['openssl', 'x509', '-req', '-in', _path('srv.csr'), '-CA', _path('ca.crt'),
         '-CAkey', _path('ca.key'), '-CAcreateserial', '-out', _path('srv.crt'), '-days', '360']]
    for args in arg_list:
        if subprocess.check_call(args) != 0:
            raise("openssl command {} failed".format(args))


def get_my_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # doesn't even have to be reachable
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except Exception:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP


# Simple server for mqtt over TLS connection
class TlsServer:

    def __init__(self, port, client_cert=False, refuse_connection=False, use_alpn=False):
        self.port = port
        self.socket = socket.socket()
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.settimeout(10.0)
        self.shutdown = Event()
        self.client_cert = client_cert
        self.refuse_connection = refuse_connection
        self.ssl_error = None
        self.use_alpn = use_alpn
        self.negotiated_protocol = None

    def __enter__(self):
        try:
            self.socket.bind(('', self.port))
        except socket.error as e:
            print("Bind failed:{}".format(e))
            raise

        self.socket.listen(1)
        self.server_thread = Thread(target=self.run_server)
        self.server_thread.start()

        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.shutdown.set()
        self.server_thread.join()
        self.socket.close()
        if (self.conn is not None):
            self.conn.close()

    def get_last_ssl_error(self):
        return self.ssl_error

    def get_negotiated_protocol(self):
        return self.negotiated_protocol

    def run_server(self):
        context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
        if self.client_cert:
            context.verify_mode = ssl.CERT_REQUIRED
            context.load_verify_locations(cafile=_path("ca.crt"))
        context.load_cert_chain(certfile=_path("srv.crt"), keyfile=_path("server.key"))
        if self.use_alpn:
            context.set_alpn_protocols(["mymqtt", "http/1.1"])
        self.socket = context.wrap_socket(self.socket, server_side=True)
        try:
            self.conn, address = self.socket.accept()  # accept new connection
            self.socket.settimeout(10.0)
            print(" - connection from: {}".format(address))
            if self.use_alpn:
                self.negotiated_protocol = self.conn.selected_alpn_protocol()
                print(" - negotiated_protocol: {}".format(self.negotiated_protocol))
            self.handle_conn()
        except ssl.SSLError as e:
            self.conn = None
            self.ssl_error = str(e)
            print(" - SSLError: {}".format(str(e)))

    def handle_conn(self):
        while not self.shutdown.is_set():
            r,w,e = select.select([self.conn], [], [], 1)
            try:
                if self.conn in r:
                    self.process_mqtt_connect()

            except socket.error as err:
                print(" - error: {}".format(err))
                raise

    def process_mqtt_connect(self):
        try:
            data = bytearray(self.conn.recv(1024))
            message = ''.join(format(x, '02x') for x in data)
            if message[0:16] == '101800044d515454':
                if self.refuse_connection is False:
                    print(" - received mqtt connect, sending ACK")
                    self.conn.send(bytearray.fromhex("20020000"))
                else:
                    # injecting connection not authorized error
                    print(" - received mqtt connect, sending NAK")
                    self.conn.send(bytearray.fromhex("20020005"))
            else:
                raise Exception(" - error process_mqtt_connect unexpected connect received: {}".format(message))
        finally:
            # stop the server after the connect message in happy flow, or if any exception occur
            self.shutdown.set()


@ttfw_idf.idf_custom_test(env_tag="Example_WIFI", group="test-apps")
def test_app_protocol_mqtt_publish_connect(env, extra_data):
    """
    steps:
      1. join AP
      2. connect to uri specified in the config
      3. send and receive data
    """
    dut1 = env.get_dut("mqtt_publish_connect_test", "tools/test_apps/protocols/mqtt/publish_connect_test", dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "mqtt_publish_connect_test.bin")
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("mqtt_publish_connect_test_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("mqtt_publish_connect_test_bin_size_vin_size", bin_size // 1024, dut1.TARGET)
    # Look for test case symbolic names
    cases = {}
    try:
        for i in ["CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT",
                  "CONFIG_EXAMPLE_CONNECT_CASE_SERVER_CERT",
                  "CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH",
                  "CONFIG_EXAMPLE_CONNECT_CASE_INVALID_SERVER_CERT",
                  "CONFIG_EXAMPLE_CONNECT_CASE_SERVER_DER_CERT",
                  "CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_KEY_PWD",
                  "CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_BAD_CRT",
                  "CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT_ALPN"]:
            cases[i] = dut1.app.get_sdkconfig()[i]
    except Exception:
        print('ENV_TEST_FAILURE: Some mandatory test case not found in sdkconfig')
        raise

    dut1.start_app()
    esp_ip = dut1.expect(re.compile(r" IPv4 address: ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)"), timeout=30)
    print("Got IP={}".format(esp_ip[0]))
    #
    # start connection test
    ip = get_my_ip()
    set_server_cert_cn(ip)
    server_port = 2222

    def start_case(case, desc):
        print("Starting {}: {}".format(case, desc))
        case_id = cases[case]
        dut1.write("conn {} {} {}".format(ip, server_port, case_id))
        dut1.expect("Test case:{} started".format(case_id))
        return case_id

    for case in ["CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT", "CONFIG_EXAMPLE_CONNECT_CASE_SERVER_CERT", "CONFIG_EXAMPLE_CONNECT_CASE_SERVER_DER_CERT"]:
        # All these cases connect to the server with no server verification or with server only verification
        with TlsServer(server_port):
            test_nr = start_case(case, "default server - expect to connect normally")
            dut1.expect("MQTT_EVENT_CONNECTED: Test={}".format(test_nr), timeout=30)
        with TlsServer(server_port, refuse_connection=True):
            test_nr = start_case(case, "ssl shall connect, but mqtt sends connect refusal")
            dut1.expect("MQTT_EVENT_ERROR: Test={}".format(test_nr), timeout=30)
            dut1.expect("MQTT ERROR: 0x5")  # expecting 0x5 ... connection not authorized error
        with TlsServer(server_port, client_cert=True) as s:
            test_nr = start_case(case, "server with client verification - handshake error since client presents no client certificate")
            dut1.expect("MQTT_EVENT_ERROR: Test={}".format(test_nr), timeout=30)
            dut1.expect("ESP-TLS ERROR: 0x8010")  # expect ... handshake error (PEER_DID_NOT_RETURN_A_CERTIFICATE)
            if "PEER_DID_NOT_RETURN_A_CERTIFICATE" not in s.get_last_ssl_error():
                raise("Unexpected ssl error from the server {}".format(s.get_last_ssl_error()))

    for case in ["CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH", "CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_KEY_PWD"]:
        # These cases connect to server with both server and client verification (client key might be password protected)
        with TlsServer(server_port, client_cert=True):
            test_nr = start_case(case, "server with client verification - expect to connect normally")
            dut1.expect("MQTT_EVENT_CONNECTED: Test={}".format(test_nr), timeout=30)

    case = "CONFIG_EXAMPLE_CONNECT_CASE_INVALID_SERVER_CERT"
    with TlsServer(server_port) as s:
        test_nr = start_case(case, "invalid server certificate on default server - expect ssl handshake error")
        dut1.expect("MQTT_EVENT_ERROR: Test={}".format(test_nr), timeout=30)
        dut1.expect("ESP-TLS ERROR: 0x8010")  # expect ... handshake error (TLSV1_ALERT_UNKNOWN_CA)
        if "alert unknown ca" not in s.get_last_ssl_error():
            raise Exception("Unexpected ssl error from the server {}".format(s.get_last_ssl_error()))

    case = "CONFIG_EXAMPLE_CONNECT_CASE_MUTUAL_AUTH_BAD_CRT"
    with TlsServer(server_port, client_cert=True) as s:
        test_nr = start_case(case, "Invalid client certificate on server with client verification - expect ssl handshake error")
        dut1.expect("MQTT_EVENT_ERROR: Test={}".format(test_nr), timeout=30)
        dut1.expect("ESP-TLS ERROR: 0x8010")  # expect ... handshake error (CERTIFICATE_VERIFY_FAILED)
        if "CERTIFICATE_VERIFY_FAILED" not in s.get_last_ssl_error():
            raise Exception("Unexpected ssl error from the server {}".format(s.get_last_ssl_error()))

    for case in ["CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT", "CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT_ALPN"]:
        with TlsServer(server_port, use_alpn=True) as s:
            test_nr = start_case(case, "server with alpn - expect connect, check resolved protocol")
            dut1.expect("MQTT_EVENT_CONNECTED: Test={}".format(test_nr), timeout=30)
            if case == "CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT" and s.get_negotiated_protocol() is None:
                print(" - client with alpn off, no negotiated protocol: OK")
            elif case == "CONFIG_EXAMPLE_CONNECT_CASE_NO_CERT_ALPN" and s.get_negotiated_protocol() == "mymqtt":
                print(" - client with alpn on, negotiated protocol resolved: OK")
            else:
                raise Exception("Unexpected negotiated protocol {}".format(s.get_negotiated_protocol()))


if __name__ == '__main__':
    test_app_protocol_mqtt_publish_connect()
