# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import re
import select
import socket
import struct
import time
from threading import Event, Thread

import dpkt
import dpkt.dns
import ttfw_idf
from tiny_test_fw.Utility import console_log

UDP_PORT = 5353
MCAST_GRP = '224.0.0.251'

# This service is created from esp board startup
SERVICE_NAME = u'ESP32-WebServer._http._tcp.local'
SUB_SERVICE_NAME = u'_server._sub._http._tcp.local'

# This host name answer sent by host, when there is query from board
HOST_NAME = u'tinytester.local'

# This servce answer sent by host, when there is query from board
MDNS_HOST_SERVICE = u'ESP32._http._tcp.local'

stop_mdns_listener = Event()
start_mdns_listener = Event()
esp_service_answered = Event()
esp_sub_service_answered = Event()
esp_host_answered = Event()
esp_delegated_host_answered = Event()


# Get query of ESP32-WebServer._http._tcp.local service
def get_mdns_service_query(service):  # type:(str) -> dpkt.dns.Msg
    dns = dpkt.dns.DNS()
    dns.op = dpkt.dns.DNS_QR | dpkt.dns.DNS_AA
    dns.rcode = dpkt.dns.DNS_RCODE_NOERR
    arr = dpkt.dns.DNS.RR()
    arr.cls = dpkt.dns.DNS_IN
    arr.type = dpkt.dns.DNS_SRV
    arr.name = service
    arr.target = socket.inet_aton('127.0.0.1')
    arr.srvname = service
    dns.qd.append(arr)
    console_log('Created mdns service query: {} '.format(dns.__repr__()))
    return dns.pack()


# Get query of _server_.sub._http._tcp.local sub service
def get_mdns_sub_service_query(sub_service):  # type:(str) -> dpkt.dns.Msg
    dns = dpkt.dns.DNS()
    dns.op = dpkt.dns.DNS_QR | dpkt.dns.DNS_AA
    dns.rcode = dpkt.dns.DNS_RCODE_NOERR
    arr = dpkt.dns.DNS.RR()
    arr.cls = dpkt.dns.DNS_IN
    arr.type = dpkt.dns.DNS_PTR
    arr.name = sub_service
    arr.target = socket.inet_aton('127.0.0.1')
    arr.ptrname = sub_service
    dns.qd.append(arr)
    console_log('Created mdns subtype service query: {} '.format(dns.__repr__()))
    return dns.pack()


# Get query for host resolution
def get_dns_query_for_esp(esp_host):  # type:(str) -> dpkt.dns.Msg
    dns = dpkt.dns.DNS()
    arr = dpkt.dns.DNS.RR()
    arr.cls = dpkt.dns.DNS_IN
    arr.name = esp_host + u'.local'
    dns.qd.append(arr)
    console_log('Created query for esp host: {} '.format(dns.__repr__()))
    return dns.pack()


# Get mdns answer for host resoloution
def get_dns_answer_to_mdns(tester_host):  # type:(str) -> dpkt.dns.Msg
    dns = dpkt.dns.DNS()
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


# Get mdns answer for service query
def get_dns_answer_to_service_query(mdns_service):  # type:(str) -> dpkt.dns.Msg
    dns = dpkt.dns.DNS()
    dns.op = dpkt.dns.DNS_QR | dpkt.dns.DNS_AA
    dns.rcode = dpkt.dns.DNS_RCODE_NOERR
    arr = dpkt.dns.DNS.RR()
    arr.name = mdns_service
    arr.cls = dpkt.dns.DNS_IN
    arr.type = dpkt.dns.DNS_SRV
    arr.priority = 0
    arr.weight = 0
    arr.port = 100
    arr.srvname = mdns_service
    arr.ip = socket.inet_aton('127.0.0.1')
    dns. an.append(arr)
    console_log('Created answer to mdns query: {} '.format(dns.__repr__()))
    return dns.pack()


def mdns_listener(esp_host):  # type:(str) -> None
    print('mdns_listener thread started')

    UDP_IP = '0.0.0.0'
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
    sock.setblocking(False)
    sock.bind((UDP_IP,UDP_PORT))
    mreq = struct.pack('4sl', socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
    last_query_timepoint = time.time()
    QUERY_TIMEOUT = 0.2
    while not stop_mdns_listener.is_set():
        try:
            start_mdns_listener.set()
            current_time = time.time()
            if current_time - last_query_timepoint > QUERY_TIMEOUT:
                last_query_timepoint = current_time
            timeout = max(0, QUERY_TIMEOUT - (current_time - last_query_timepoint))
            read_socks, _, _ = select.select([sock], [], [], timeout)
            if not read_socks:
                continue
            data, _ = sock.recvfrom(1024)
            dns = dpkt.dns.DNS(data)
            if len(dns.qd) > 0:
                if dns.qd[0].name == HOST_NAME:
                    console_log('Received query: {} '.format(dns.__repr__()))
                    sock.sendto(get_dns_answer_to_mdns(HOST_NAME), (MCAST_GRP,UDP_PORT))
                if dns.qd[0].name == HOST_NAME:
                    console_log('Received query: {} '.format(dns.__repr__()))
                    sock.sendto(get_dns_answer_to_mdns(HOST_NAME), (MCAST_GRP,UDP_PORT))
                if dns.qd[0].name == MDNS_HOST_SERVICE:
                    print(dns.qd[0].name)
                    console_log('Received query: {} '.format(dns.__repr__()))
                    sock.sendto(get_dns_answer_to_service_query(MDNS_HOST_SERVICE), (MCAST_GRP,UDP_PORT))
            if len(dns.an) == 1:
                if dns.an[0].name == SERVICE_NAME:
                    console_log('Received answer to service query: {}'.format(dns.__repr__()))
                    esp_service_answered.set()
            if len(dns.an) > 1:
                if dns.an[1].name == SUB_SERVICE_NAME:
                    console_log('Received answer for sub service query: {}'.format(dns.__repr__()))
                    esp_sub_service_answered.set()
            if len(dns.an) > 0 and dns.an[0].type == dpkt.dns.DNS_A:
                if dns.an[0].name == esp_host + u'.local':
                    console_log('Received answer to esp32-mdns query: {}'.format(dns.__repr__()))
                    esp_host_answered.set()
                if dns.an[0].name == esp_host + u'-delegated.local':
                    console_log('Received answer to esp32-mdns-delegate query: {}'.format(dns.__repr__()))
                    esp_delegated_host_answered.set()
        except socket.timeout:
            break
        except dpkt.UnpackError:
            continue


def create_socket():  # type:() -> socket.socket
    UDP_IP = '0.0.0.0'
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
    sock.setblocking(False)
    sock.bind((UDP_IP,UDP_PORT))
    mreq = struct.pack('4sl', socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
    return sock


def test_query_dns_http_service(service):  # type: (str) -> None
    print('SRV: Query {}'.format(service))
    sock = create_socket()
    sock.sendto(get_mdns_service_query(service), (MCAST_GRP,UDP_PORT))
    if not esp_service_answered.wait(timeout=25):
        raise ValueError('Test has failed: did not receive mdns answer within timeout')


def test_query_dns_sub_service(sub_service):  # type: (str) -> None
    print('PTR: Query {}'.format(sub_service))
    sock = create_socket()
    sock.sendto(get_mdns_sub_service_query(sub_service), (MCAST_GRP,UDP_PORT))
    if not esp_sub_service_answered.wait(timeout=25):
        raise ValueError('Test has failed: did not receive mdns answer within timeout')


def test_query_dns_host(esp_host):  # type: (str) -> None
    print('A: {}'.format(esp_host))
    sock = create_socket()
    sock.sendto(get_dns_query_for_esp(esp_host), (MCAST_GRP,UDP_PORT))
    if not esp_host_answered.wait(timeout=25):
        raise ValueError('Test has failed: did not receive mdns answer within timeout')


def test_query_dns_host_delegated(esp_host):  # type: (str) -> None
    print('A: {}'.format(esp_host))
    sock = create_socket()
    sock.sendto(get_dns_query_for_esp(esp_host + '-delegated'), (MCAST_GRP,UDP_PORT))
    if not esp_delegated_host_answered.wait(timeout=25):
        raise ValueError('Test has failed: did not receive mdns answer within timeout')


@ttfw_idf.idf_custom_test(env_tag='Example_WIFI', group='test-apps')
def test_app_esp_mdns(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    dut1 = env.get_dut('mdns', 'tools/test_apps/protocols/mdns', dut_class=ttfw_idf.ESP32DUT)

    # 1. start mdns application
    dut1.start_app()
    # 2. get the dut host name (and IP address)
    specific_host = dut1.expect(re.compile(r'mdns hostname set to: \[([^\]]+)\]'), timeout=30)[0]

    esp_ip = dut1.expect(re.compile(r' IPv4 address: ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)'), timeout=30)
    print('Got IP={}'.format(esp_ip[0]))

    mdns_responder = Thread(target=mdns_listener, args=(str(specific_host),))

    def start_case(case, desc, result):  # type: (str, str, str) -> None
        print('Starting {}: {}'.format(case, desc))
        dut1.write(case)
        dut1.expect(re.compile(result), timeout=10)

    try:
        # start dns listener thread
        mdns_responder.start()

        # wait untill mdns listener thred started
        if not start_mdns_listener.wait(timeout=5):
            raise ValueError('Test has failed: mdns listener thread did not start')

        # query dns service from host, answer should be received from esp board
        test_query_dns_http_service(SERVICE_NAME)

        # query dns sub-service from host, answer should be received from esp board
        test_query_dns_sub_service(SUB_SERVICE_NAME)

        # query dns host name, answer should be received from esp board
        test_query_dns_host(specific_host)

        # query dns host name delegated, answer should be received from esp board
        test_query_dns_host_delegated(specific_host)

        # query dns-host from esp board, answer should be received from host
        start_case('CONFIG_TEST_QUERY_HOST', 'Query tinytester.local', 'tinytester.local resolved to: 127.0.0.1')

        # query dns-host aynchrounusely from esp board, answer should be received from host
        start_case('CONFIG_TEST_QUERY_HOST_ASYNC', 'Query tinytester.local async', 'Async query resolved to A:127.0.0.1')

        # query service from esp board, answer should be received from host
        start_case('CONFIG_TEST_QUERY_SERVICE', 'Query SRV ESP32._http._tcp.local', 'SRV:ESP32')
    finally:
        stop_mdns_listener.set()
        mdns_responder.join()


if __name__ == '__main__':
    test_app_esp_mdns()
