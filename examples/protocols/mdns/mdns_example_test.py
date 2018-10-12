import re
import os
import sys
import socket
import time
import imp
import struct
import dpkt, dpkt.dns
from threading import Thread



# this is a test case write with tiny-test-fw.
# to run test cases outside tiny-test-fw,
# we need to set environment variable `TEST_FW_PATH`,
# then get and insert `TEST_FW_PATH` to sys path before import FW module

test_fw_path = os.getenv("TEST_FW_PATH")
if test_fw_path and test_fw_path not in sys.path:
    sys.path.insert(0, test_fw_path)

import TinyFW
import IDF

g_run_server = True
g_done = False

def mdns_server(esp_host):
    global g_run_server
    global g_done
    UDP_IP="0.0.0.0"
    UDP_PORT=5353
    MCAST_GRP = '224.0.0.251'
    sock = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
    sock.bind( (UDP_IP,UDP_PORT) )
    mreq = struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
    dns = dpkt.dns.DNS(b'\x00\x00\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01')
    # sock.sendto(dns.pack(),(MCAST_GRP,UDP_PORT))
    sock.settimeout(30)
    resp_dns = dpkt.dns.DNS(b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
    resp_dns.op = dpkt.dns.DNS_QR | dpkt.dns.DNS_AA
    resp_dns.rcode = dpkt.dns.DNS_RCODE_NOERR
    arr = dpkt.dns.DNS.RR()
    arr.cls = dpkt.dns.DNS_IN
    arr.type = dpkt.dns.DNS_A
    arr.name = u'tinytester.local'
    arr.ip =socket.inet_aton('127.0.0.1')
    resp_dns. an.append(arr)
    sock.sendto(resp_dns.pack(),(MCAST_GRP,UDP_PORT))
    while g_run_server:
        try:
            m=sock.recvfrom( 1024 );
            dns = dpkt.dns.DNS(m[0])
            if len(dns.qd)>0 and dns.qd[0].type == dpkt.dns.DNS_A:
                if dns.qd[0].name == u'tinytester.local':
                    print (dns.__repr__(),dns.qd[0].name)
                    sock.sendto(resp_dns.pack(),(MCAST_GRP,UDP_PORT))
            if len(dns.an)>0 and dns.an[0].type == dpkt.dns.DNS_A:
                if dns.an[0].name == esp_host +  u'.local':
                    print("Received answer esp32-mdns query")
                    g_done = True
                print (dns.an[0].name)
            dns = dpkt.dns.DNS(b'\x00\x00\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01')
            dns.qd[0].name= esp_host + u'.local'
            sock.sendto(dns.pack(),(MCAST_GRP,UDP_PORT))
            print("Sending esp32-mdns query")
            time.sleep(0.5)
        except socket.timeout:
            break

@IDF.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_mdns(env, extra_data):
    global g_run_server
    global g_done
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
    IDF.log_performance("mdns-test_bin_size", "{}KB".format(bin_size//1024))
    IDF.check_performance("mdns-test_bin_size", bin_size//1024)
    # 1. start mdns application
    dut1.start_app()
    # 2. get the dut host name (and IP address)
    specific_host = dut1.expect(re.compile(r"mdns hostname set to: \[([^\]]+)\]"), timeout=30)
    specific_host = str(specific_host[0])
    dut_ip = ""
    try:
        dut_ip = dut1.expect(re.compile(r" sta ip: ([^,]+),"), timeout=30)
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    # 3. check the mdns name is accessible
    thread1 = Thread(target = mdns_server, args = (specific_host,))
    thread1.start()
    start = time.time()
    while (time.time() - start) <= 60:
        if g_done:
            print("Test passed")
            break
    g_run_server = False
    thread1.join()
    if g_done == False:
        raise ValueError('Test has failed: did not receive mdns answer within timeout')
    # 4. check DUT output if mdns advertized host is resolved
    dut1.expect(re.compile(r"mdns-test: Query A: tinytester.local resolved to: 127.0.0.1"), timeout=30)

if __name__ == '__main__':
    test_examples_protocol_mdns()
