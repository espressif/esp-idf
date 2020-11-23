from __future__ import print_function
from __future__ import unicode_literals
import re
import socket
import subprocess
import ttfw_idf
import time
import netifaces
from threading import Thread, Event


def run_server(server_stop, port, server_ip, client_ip):
    print("Starting PPP server on port: {}".format(port))
    try:
        arg_list = ['pppd', port, '115200', '{}:{}'.format(server_ip, client_ip), 'modem', 'local', 'noauth', 'debug', 'nocrtscts', 'nodetach', '+ipv6']
        p = subprocess.Popen(arg_list, stdout=subprocess.PIPE, bufsize=1)
        while not server_stop.is_set():
            if p.poll() is not None:
                raise ValueError('ENV_TEST_FAILURE: PPP terminated unexpectedly with {}'.format(p.poll()))
            line = p.stdout.readline()
            if line:
                print("[PPPD:]{}".format(line.rstrip()))
            time.sleep(0.1)
    except Exception as e:
        print(e)
        raise ValueError('ENV_TEST_FAILURE: Error running PPP server')
    finally:
        p.terminate()
    print("PPP server stopped")


@ttfw_idf.idf_custom_test(env_tag="Example_PPP", group="test-apps")
def test_examples_protocol_pppos_connect(env, extra_data):
    """
    steps:
      1. starts PPP server
      2. get DUT as PPP client to connect to the server
      3. check TCP client-server connection between client-server
    """

    dut1 = env.get_dut("pppos_connect_test", "tools/test_apps/protocols/pppos", dut_class=ttfw_idf.ESP32DUT)
    # Look for test case symbolic names
    try:
        server_ip = dut1.app.get_sdkconfig()["CONFIG_TEST_APP_PPP_SERVER_IP"].replace('"','')
        client_ip = dut1.app.get_sdkconfig()["CONFIG_TEST_APP_PPP_CLIENT_IP"].replace('"','')
        port_nr = dut1.app.get_sdkconfig()["CONFIG_TEST_APP_TCP_PORT"]
    except Exception:
        print('ENV_TEST_FAILURE: Some mandatory configuration not found in sdkconfig')
        raise

    print("Starting the test on {}".format(dut1))
    dut1.start_app()

    # the PPP test env uses two ttyUSB's: one for ESP32 board, another one for ppp server
    # use the other port for PPP server than the DUT/ESP
    port = '/dev/ttyUSB0' if dut1.port == '/dev/ttyUSB1' else '/dev/ttyUSB1'
    # Start the PPP server
    server_stop = Event()
    t = Thread(target=run_server, args=(server_stop, port, server_ip, client_ip))
    t.start()
    try:
        ppp_server_timeout = time.time() + 30
        while "ppp0" not in netifaces.interfaces():
            print("PPP server haven't yet setup its netif, list of active netifs:{}".format(netifaces.interfaces()))
            time.sleep(0.5)
            if time.time() > ppp_server_timeout:
                raise ValueError("ENV_TEST_FAILURE: PPP server failed to setup ppp0 interface within timeout")
        ip6_addr = dut1.expect(re.compile(r"Got IPv6 address (\w{4}\:\w{4}\:\w{4}\:\w{4}\:\w{4}\:\w{4}\:\w{4}\:\w{4})"), timeout=30)[0]
        print("IPv6 address of ESP: {}".format(ip6_addr))

        dut1.expect(re.compile(r"Socket listening"))
        print("Starting the IPv6 test...")
        # Connect to TCP server on ESP using IPv6 address
        for res in socket.getaddrinfo(ip6_addr + "%ppp0", int(port_nr), socket.AF_INET6,
                                      socket.SOCK_STREAM, socket.SOL_TCP):
            af, socktype, proto, canonname, addr = res
        sock = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
        sock.connect(addr)
        sock.sendall(b"Espressif")
        sock.close()

        dut1.expect(re.compile(r"IPv6 test passed"))
        print("IPv6 test passed!")

        print("Starting the IPv4 test...")
        # Start the TCP server and wait for the ESP to connect with IPv4 address
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.bind(('', int(port_nr)))
            sock.listen(1)
            conn, addr = sock.accept()
        except socket.error as msg:
            print('Socket error: ' + str(msg[0]) + ': ' + msg[1])
            raise
        timeout = time.time() + 60
        while time.time() < timeout:
            data = conn.recv(128)
            if not data:
                break
            data = data.decode()
            print('Received data: ' + data)
            if data.startswith('Espressif'):
                conn.send(data.encode())
                break
        conn.close()
        dut1.expect(re.compile(r"IPv4 test passed"))
        print("IPv4 test passed!")
    finally:
        server_stop.set()
        t.join()


if __name__ == '__main__':
    test_examples_protocol_pppos_connect()
