import os
import re
import select
import socket
import struct
import subprocess
import time
from threading import Event, Thread

import dpkt
import dpkt.dns
import ttfw_idf
from tiny_test_fw import DUT
from tiny_test_fw.Utility import console_log

stop_mdns_server = Event()
esp_answered = Event()


def get_dns_query_for_esp(esp_host):
    dns = dpkt.dns.DNS(b'\x00\x00\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01')
    dns.qd[0].name = esp_host + u'.local'
    console_log('Created query for esp host: {} '.format(dns.__repr__()))
    return dns.pack()


def get_dns_answer_to_mdns(tester_host):
    dns = dpkt.dns.DNS(b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')
    dns.op = dpkt.dns.DNS_QR | dpkt.dns.DNS_AA
    dns.rcode = dpkt.dns.DNS_RCODE_NOERR
    arr = dpkt.dns.DNS.RR()
    arr.cls = dpkt.dns.DNS_IN
    arr.type = dpkt.dns.DNS_A
    arr.name = tester_host
    arr.ip = socket.inet_aton('127.0.0.1')
    dns. an.append(arr)
    console_log('Created answer to mdns query: {} '.format(dns.__repr__()))
    return dns.pack()


def get_dns_answer_to_mdns_lwip(tester_host, id):
    dns = dpkt.dns.DNS(b'\x5e\x39\x84\x00\x00\x01\x00\x01\x00\x00\x00\x00\x0a\x64\x61\x76\x69\x64'
                       b'\x2d\x63\x6f\x6d\x70\x05\x6c\x6f\x63\x61\x6c\x00\x00\x01\x00\x01\xc0\x0c'
                       b'\x00\x01\x00\x01\x00\x00\x00\x0a\x00\x04\xc0\xa8\x0a\x6c')
    dns.qd[0].name = tester_host
    dns.an[0].name = tester_host
    dns.an[0].ip = socket.inet_aton('127.0.0.1')
    dns.an[0].rdata = socket.inet_aton('127.0.0.1')
    dns.id = id
    print('Created answer to mdns (lwip) query: {} '.format(dns.__repr__()))
    return dns.pack()


def mdns_server(esp_host):
    global esp_answered
    UDP_IP = '0.0.0.0'
    UDP_PORT = 5353
    MCAST_GRP = '224.0.0.251'
    TESTER_NAME = u'tinytester.local'
    TESTER_NAME_LWIP = u'tinytester-lwip.local'
    QUERY_TIMEOUT = 0.2
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
    sock.setblocking(False)
    sock.bind((UDP_IP,UDP_PORT))
    mreq = struct.pack('4sl', socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
    last_query_timepoint = time.time()
    while not stop_mdns_server.is_set():
        try:
            current_time = time.time()
            if current_time - last_query_timepoint > QUERY_TIMEOUT:
                last_query_timepoint = current_time
                if not esp_answered.is_set():
                    sock.sendto(get_dns_query_for_esp(esp_host), (MCAST_GRP,UDP_PORT))
            timeout = max(0, QUERY_TIMEOUT - (current_time - last_query_timepoint))
            read_socks, _, _ = select.select([sock], [], [], timeout)
            if not read_socks:
                continue
            data, addr = sock.recvfrom(1024)
            dns = dpkt.dns.DNS(data)
            if len(dns.qd) > 0 and dns.qd[0].type == dpkt.dns.DNS_A:
                if dns.qd[0].name == TESTER_NAME:
                    console_log('Received query: {} '.format(dns.__repr__()))
                    sock.sendto(get_dns_answer_to_mdns(TESTER_NAME), (MCAST_GRP,UDP_PORT))
                elif dns.qd[0].name == TESTER_NAME_LWIP:
                    console_log('Received query: {} '.format(dns.__repr__()))
                    sock.sendto(get_dns_answer_to_mdns_lwip(TESTER_NAME_LWIP, dns.id), addr)
            if len(dns.an) > 0 and dns.an[0].type == dpkt.dns.DNS_A:
                if dns.an[0].name == esp_host + u'.local':
                    console_log('Received answer to esp32-mdns query: {}'.format(dns.__repr__()))
                    esp_answered.set()
        except socket.timeout:
            break
        except dpkt.UnpackError:
            continue


@ttfw_idf.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_mdns(env, extra_data):
    global stop_mdns_server
    """
    steps: |
      1. obtain IP address + init mdns example
      2. get the dut host name (and IP address)
      3. check the mdns name is accessible
      4. check DUT output if mdns advertized host is resolved
    """
    dut1 = env.get_dut("mdns-test", "examples/protocols/mdns", dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "mdns-test.bin")
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('mdns-test_bin_size', '{}KB'.format(bin_size // 1024))
    # 1. start mdns application
    dut1.start_app()
    # 2. get the dut host name (and IP address)
    specific_host = dut1.expect(re.compile(r'mdns hostname set to: \[([^\]]+)\]'), timeout=30)[0]
    mdns_responder = Thread(target=mdns_server, args=(str(specific_host),))
    try:
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)[0]
        console_log('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    try:
        # 3. check the mdns name is accessible
        mdns_responder.start()
        if not esp_answered.wait(timeout=30):
            raise ValueError('Test has failed: did not receive mdns answer within timeout')
        # 4. check DUT output if mdns advertized host is resolved
        dut1.expect(re.compile(r'mdns-test: Query A: tinytester.local resolved to: 127.0.0.1'), timeout=30)
        dut1.expect(re.compile(r'mdns-test: gethostbyname: tinytester-lwip.local resolved to: 127.0.0.1'), timeout=30)
        dut1.expect(re.compile(r'mdns-test: getaddrinfo: tinytester-lwip.local resolved to: 127.0.0.1'), timeout=30)
        # 5. check the DUT answers to `dig` command
        dig_output = subprocess.check_output(['dig', '+short', '-p', '5353', '@224.0.0.251',
                                              '{}.local'.format(specific_host)])
        console_log('Resolving {} using "dig" succeeded with:\n{}'.format(specific_host, dig_output))
        if not ip_address.encode('utf-8') in dig_output:
            raise ValueError('Test has failed: Incorrectly resolved DUT hostname using dig'
                             "Output should've contained DUT's IP address:{}".format(ip_address))
    finally:
        stop_mdns_server.set()
        mdns_responder.join()


if __name__ == '__main__':
    test_examples_protocol_mdns()
