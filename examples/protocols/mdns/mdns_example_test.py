import re
import os
import socket
import time
import struct
import dpkt
import dpkt.dns
from threading import Thread

from tiny_test_fw import DUT
import ttfw_idf

g_run_server = True
g_done = False


def mdns_server(esp_host):
    global g_done
    UDP_IP = "0.0.0.0"
    UDP_PORT = 5353
    MCAST_GRP = '224.0.0.251'
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
    sock.bind((UDP_IP,UDP_PORT))
    mreq = struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
    dns = dpkt.dns.DNS(b'\x00\x00\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01')
    sock.settimeout(30)
    resp_dns = dpkt.dns.DNS(b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
    resp_dns.op = dpkt.dns.DNS_QR | dpkt.dns.DNS_AA
    resp_dns.rcode = dpkt.dns.DNS_RCODE_NOERR
    arr = dpkt.dns.DNS.RR()
    arr.cls = dpkt.dns.DNS_IN
    arr.type = dpkt.dns.DNS_A
    arr.name = u'tinytester.local'
    arr.ip = socket.inet_aton('127.0.0.1')
    resp_dns. an.append(arr)
    sock.sendto(resp_dns.pack(),(MCAST_GRP,UDP_PORT))
    while g_run_server:
        try:
            m = sock.recvfrom(1024)
            dns = dpkt.dns.DNS(m[0])
            if len(dns.qd) > 0 and dns.qd[0].type == dpkt.dns.DNS_A:
                if dns.qd[0].name == u'tinytester.local':
                    print(dns.__repr__(),dns.qd[0].name)
                    sock.sendto(resp_dns.pack(),(MCAST_GRP,UDP_PORT))
            if len(dns.an) > 0 and dns.an[0].type == dpkt.dns.DNS_A:
                if dns.an[0].name == esp_host + u'.local':
                    print("Received answer esp32-mdns query")
                    g_done = True
                print(dns.an[0].name)
            dns = dpkt.dns.DNS(b'\x00\x00\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01')
            dns.qd[0].name = esp_host + u'.local'
            sock.sendto(dns.pack(),(MCAST_GRP,UDP_PORT))
            print("Sending esp32-mdns query")
            time.sleep(0.5)
            sock.sendto(resp_dns.pack(),(MCAST_GRP,UDP_PORT))
        except socket.timeout:
            break
        except dpkt.UnpackError:
            continue


@ttfw_idf.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_mdns(env, extra_data):
    global g_run_server
    """
    steps: |
      1. join AP + init mdns example
      2. get the dut host name (and IP address)
      3. check the mdns name is accessible
      4. check DUT output if mdns advertized host is resolved
    """
    dut1 = env.get_dut("mdns-test", "examples/protocols/mdns")
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "mdns-test.bin")
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("mdns-test_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("mdns-test_bin_size", bin_size // 1024)
    # 1. start mdns application
    dut1.start_app()
    # 2. get the dut host name (and IP address)
    specific_host = dut1.expect(re.compile(r"mdns hostname set to: \[([^\]]+)\]"), timeout=30)
    specific_host = str(specific_host[0])
    thread1 = Thread(target=mdns_server, args=(specific_host,))
    thread1.start()
    try:
        dut1.expect(re.compile(r" sta ip: ([^,]+),"), timeout=30)
    except DUT.ExpectTimeout:
        g_run_server = False
        thread1.join()
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    # 3. check the mdns name is accessible
    start = time.time()
    while (time.time() - start) <= 60:
        if g_done:
            break
        time.sleep(0.5)
    if g_done is False:
        raise ValueError('Test has failed: did not receive mdns answer within timeout')
    # 4. check DUT output if mdns advertized host is resolved
    try:
        dut1.expect(re.compile(r"mdns-test: Query A: tinytester.local resolved to: 127.0.0.1"), timeout=30)
    finally:
        g_run_server = False
        thread1.join()


if __name__ == '__main__':
    test_examples_protocol_mdns()
