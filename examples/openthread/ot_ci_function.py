# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
# !/usr/bin/env python3
# this file defines some functions for testing cli and br under pytest framework

import re
import socket
import struct
import subprocess
import time
from typing import Tuple

import netifaces
import pexpect
from pytest_embedded_idf.dut import IdfDut


class thread_parameter:

    def __init__(self, deviceRole:str='', dataset:str='', channel:str='', exaddr:str='', bbr:bool=False):
        self.deviceRole = deviceRole
        self.dataset = dataset
        self.channel = channel
        self.exaddr = exaddr
        self.bbr = bbr


class wifi_parameter:

    def __init__(self, ssid:str='', psk:str='', retry_times:int=10):
        self.ssid = ssid
        self.psk = psk
        self.retry_times = retry_times


def joinThreadNetwork(dut:IdfDut, thread:thread_parameter) -> None:
    if thread.dataset != '':
        command = 'dataset set active ' + thread.dataset
        dut.write(command)
        dut.expect('Done', timeout=5)
    else:
        dut.write('dataset init new')
        dut.expect('Done', timeout=5)
        dut.write('dataset commit active')
        dut.expect('Done', timeout=5)
    if thread.channel != '':
        command = 'channel ' + thread.channel
        dut.write(command)
        dut.expect('Done', timeout=5)
    if thread.exaddr != '':
        command = 'extaddr ' + thread.exaddr
        dut.write(command)
        dut.expect('Done', timeout=5)
    if thread.bbr:
        dut.write('bbr enable')
        dut.expect('Done', timeout=5)
    dut.write('ifconfig up')
    dut.expect('Done', timeout=5)
    dut.write('thread start')
    dut.expect('Role detached ->', timeout=20)
    if thread.deviceRole == 'leader':
        assert getDeviceRole(dut) == 'leader'
    elif thread.deviceRole == 'router':
        if getDeviceRole(dut) != 'router':
            changeDeviceRole(dut, 'router')
            wait(dut, 10)
            assert getDeviceRole(dut) == 'router'
    else:
        assert False


def joinWiFiNetwork(dut:IdfDut, wifi:wifi_parameter) -> Tuple[str, int]:
    clean_buffer(dut)
    ip_address = ''
    information = ''
    for order in range(1, wifi.retry_times):
        dut.write('wifi connect -s ' + str(wifi.ssid) + ' -p ' + str(wifi.psk))
        tmp = dut.expect(pexpect.TIMEOUT, timeout=10)
        if 'sta ip' in str(tmp):
            ip_address = re.findall(r'sta ip: (\w+.\w+.\w+.\w+),', str(tmp))[0]
        information = dut.expect(r'wifi sta (\w+ \w+ \w+)\W', timeout=20)[1].decode()
        if information == 'is connected successfully':
            break
    assert information == 'is connected successfully'
    return ip_address, order


def getDeviceRole(dut:IdfDut) -> str:
    clean_buffer(dut)
    dut.write('state')
    role = dut.expect(r'state\W+(\w+)\W+Done', timeout=5)[1].decode()
    print(role)
    return str(role)


def changeDeviceRole(dut:IdfDut, role:str) -> None:
    command = 'state ' + role
    dut.write(command)


def getDataset(dut:IdfDut) -> str:
    clean_buffer(dut)
    dut.write('dataset active -x')
    dut_data = dut.expect(r'\n(\w{212})\r', timeout=5)[1].decode()
    return str(dut_data)


def reset_thread(dut:IdfDut) -> None:
    dut.write(' ')
    dut.write('state')
    clean_buffer(dut)
    wait(dut, 1)
    dut.write('factoryreset')
    dut.expect('OpenThread attached to netif', timeout=20)
    dut.write(' ')
    dut.write('state')


# get the mleid address of the thread
def get_mleid_addr(dut:IdfDut) -> str:
    dut_adress = ''
    clean_buffer(dut)
    dut.write('ipaddr mleid')
    dut_adress = dut.expect(r'\n((?:\w+:){7}\w+)\r', timeout=5)[1].decode()
    return dut_adress


# get the rloc address of the thread
def get_rloc_addr(dut:IdfDut) -> str:
    dut_adress = ''
    clean_buffer(dut)
    dut.write('ipaddr rloc')
    dut_adress = dut.expect(r'\n((?:\w+:){7}\w+)\r', timeout=5)[1].decode()
    return dut_adress


# get the linklocal address of the thread
def get_linklocal_addr(dut:IdfDut) -> str:
    dut_adress = ''
    clean_buffer(dut)
    dut.write('ipaddr linklocal')
    dut_adress = dut.expect(r'\n((?:\w+:){7}\w+)\r', timeout=5)[1].decode()
    return dut_adress


# get the global unicast address of the thread:
def get_global_unicast_addr(dut:IdfDut, br:IdfDut) -> str:
    dut_adress = ''
    clean_buffer(br)
    br.write('br omrprefix')
    omrprefix = br.expect(r'\n((?:\w+:){4}):/\d+\r', timeout=5)[1].decode()
    clean_buffer(dut)
    dut.write('ipaddr')
    dut_adress = dut.expect(r'(%s(?:\w+:){3}\w+)\r' % str(omrprefix), timeout=5)[1].decode()
    return dut_adress


# ping of thread
def ot_ping(dut:IdfDut, target:str, times:int) -> Tuple[int, int]:
    command = 'ping ' + str(target) + ' 0 ' + str(times)
    dut.write(command)
    transmitted = dut.expect(r'(\d+) packets transmitted', timeout=30)[1].decode()
    tx_count = int(transmitted)
    received = dut.expect(r'(\d+) packets received', timeout=30)[1].decode()
    rx_count = int(received)
    return tx_count, rx_count


def reset_host_interface() -> None:
    interface_name = get_host_interface_name()
    flag = False
    try:
        command = 'ifconfig ' + interface_name + ' down'
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        command = 'ifconfig ' + interface_name + ' up'
        subprocess.call(command, shell=True, timeout=10)
        time.sleep(1)
        flag = True
    finally:
        time.sleep(1)
        assert flag


def set_interface_sysctl_options() -> None:
    interface_name = get_host_interface_name()
    flag = False
    try:
        command = 'sysctl -w net/ipv6/conf/' + interface_name + '/accept_ra=2'
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        command = 'sysctl -w net/ipv6/conf/' + interface_name + '/accept_ra_rt_info_max_plen=128'
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        flag = True
    finally:
        time.sleep(2)
        assert flag


def init_interface_ipv6_address() -> None:
    interface_name = get_host_interface_name()
    flag = False
    try:
        command = 'ip -6 route | grep ' + interface_name + " | grep ra | awk {'print $1'} | xargs -I {} ip -6 route del {}"
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(0.5)
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        command = 'ip -6 address show dev ' + interface_name + \
            " scope global | grep 'inet6' | awk {'print $2'} | xargs -I {} ip -6 addr del {} dev " + interface_name
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        flag = True
    finally:
        time.sleep(1)
        assert flag


def get_host_interface_name() -> str:
    interfaces = netifaces.interfaces()
    interface_name = [s for s in interfaces if 'wl' in s][0]
    return str(interface_name)


def clean_buffer(dut:IdfDut) -> None:
    str_length = str(len(dut.expect(pexpect.TIMEOUT, timeout=0.1)))
    dut.expect(r'[\s\S]{%s}' % str(str_length), timeout=10)


def check_if_host_receive_ra(br:IdfDut) -> bool:
    interface_name = get_host_interface_name()
    clean_buffer(br)
    br.write('br omrprefix')
    omrprefix = br.expect(r'\n((?:\w+:){4}):/\d+\r', timeout=5)[1].decode()
    command = 'ip -6 route | grep ' + str(interface_name)
    out_str = subprocess.getoutput(command)
    print('br omrprefix: ', str(omrprefix))
    print('host route table:\n', str(out_str))
    return str(omrprefix) in str(out_str)


def host_connect_wifi() -> None:
    command = '. /home/test/wlan_connection_OTTE.sh'
    subprocess.call(command, shell=True, timeout=30)
    time.sleep(5)


def is_joined_wifi_network(br:IdfDut) -> bool:
    return check_if_host_receive_ra(br)


thread_ipv6_group = 'ff04:0:0:0:0:0:0:125'


def check_ipmaddr(dut:IdfDut) -> bool:
    clean_buffer(dut)
    dut.write('ipmaddr')
    info = dut.expect(pexpect.TIMEOUT, timeout=2)
    if thread_ipv6_group in str(info):
        return True
    return False


def thread_is_joined_group(dut:IdfDut) -> bool:
    command = 'mcast join ' + thread_ipv6_group
    dut.write(command)
    dut.expect('Done', timeout=5)
    order = 0
    while order < 3:
        if check_ipmaddr(dut):
            return True
        dut.write(command)
        wait(dut, 2)
        order = order + 1
    return False


def host_joined_group(group:str='') -> bool:
    interface_name = get_host_interface_name()
    command = 'netstat -g | grep ' + str(interface_name)
    out_str = subprocess.getoutput(command)
    print('groups:\n', str(out_str))
    return group in str(out_str)


class udp_parameter:

    def __init__(self, udp_type:str='', addr:str='::', port:int=5090, group:str='', init_flag:bool=False, timeout:float=15.0, udp_bytes:bytes=b''):
        self.udp_type = udp_type
        self.addr = addr
        self.port = port
        self.group = group
        self.init_flag = init_flag
        self.timeout = timeout
        self.udp_bytes = udp_bytes


def create_host_udp_server(myudp:udp_parameter) -> None:
    interface_name = get_host_interface_name()
    try:
        if myudp.udp_type == 'INET6':
            AF_INET = socket.AF_INET6
        else:
            AF_INET = socket.AF_INET
        print('The host start to create udp server!')
        if_index = socket.if_nametoindex(interface_name)
        sock = socket.socket(AF_INET, socket.SOCK_DGRAM)
        sock.bind((myudp.addr, myudp.port))

        if myudp.udp_type == 'INET6' and myudp.group != '':
            sock.setsockopt(
                socket.IPPROTO_IPV6, socket.IPV6_JOIN_GROUP,
                struct.pack('16si', socket.inet_pton(socket.AF_INET6, myudp.group),
                            if_index))
        sock.settimeout(myudp.timeout)
        myudp.init_flag = True
        print('The host start to receive message!')
        myudp.udp_bytes = (sock.recvfrom(1024))[0]
        print('The host has received message: ', myudp.udp_bytes)
    except socket.error:
        print('The host did not receive message!')
    finally:
        print('Close the socket.')
        sock.close()


def wait(dut:IdfDut, wait_time:float) -> None:
    dut.expect(pexpect.TIMEOUT, timeout=wait_time)


def get_host_ipv4_address() -> str:
    interface_name = get_host_interface_name()
    command = 'ifconfig ' + interface_name + " | grep -w 'inet' | awk '{print $2}'"
    out_bytes = subprocess.check_output(command, shell=True, timeout=5)
    out_str = out_bytes.decode('utf-8')
    host_ipv4_address = ''
    host_ipv4_address = re.findall(r'((?:\d+.){3}\d+)', str(out_str))[0]
    return host_ipv4_address


def start_avahi() -> None:
    time.sleep(1)
    command = '/etc/init.d/dbus start'
    subprocess.Popen(command, shell=True)
    time.sleep(5)
    command = 'avahi-daemon'
    subprocess.Popen(command, shell=True)
    time.sleep(5)


def host_publish_service() -> None:
    command = 'avahi-publish-service testxxx _testxxx._udp 12347 test=1235 dn="for_ci_br_test"'
    subprocess.Popen(command, shell=True)
    time.sleep(2)


def host_close_service() -> None:
    command = "ps | grep avahi-publish-s | awk '{print $1}'"
    out_bytes = subprocess.check_output(command, shell=True, timeout=5)
    out_str = out_bytes.decode('utf-8')
    the_pid = re.findall(r'(\d+)\n', str(out_str))
    for pid in the_pid:
        command = 'kill -9 ' + pid
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)


def close_host_interface() -> None:
    interface_name = get_host_interface_name()
    flag = False
    try:
        command = 'ifconfig ' + interface_name + ' down'
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        flag = True
    finally:
        time.sleep(1)
        assert flag


def open_host_interface() -> None:
    interface_name = get_host_interface_name()
    flag = False
    try:
        command = 'ifconfig ' + interface_name + ' up'
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        flag = True
    finally:
        time.sleep(1)
        assert flag


def get_domain() -> str:
    hostname = socket.gethostname()
    print('hostname is: ', hostname)
    command = 'ps -aux | grep avahi-daemon | grep running'
    out_str = subprocess.getoutput(command)
    print('avahi status:\n', out_str)
    role = re.findall(r'\[([\w\W]+)\.local\]', str(out_str))[0]
    print('active host is: ', role)
    return str(role)


class tcp_parameter:

    def __init__(self, tcp_type:str='', addr:str='::', port:int=12345, listen_flag:bool=False, recv_flag:bool=False, timeout:float=15.0, tcp_bytes:bytes=b''):
        self.tcp_type = tcp_type
        self.addr = addr
        self.port = port
        self.listen_flag = listen_flag
        self.recv_flag = recv_flag
        self.timeout = timeout
        self.tcp_bytes = tcp_bytes


def create_host_tcp_server(mytcp:tcp_parameter) -> None:
    try:
        if mytcp.tcp_type == 'INET6':
            AF_INET = socket.AF_INET6
        else:
            AF_INET = socket.AF_INET
        print('The host start to create a tcp server!')
        sock = socket.socket(AF_INET, socket.SOCK_STREAM)
        sock.bind((mytcp.addr, mytcp.port))
        sock.listen(5)
        mytcp.listen_flag = True

        print('The tcp server is waiting for connection!')
        sock.settimeout(mytcp.timeout)
        connfd,addr = sock.accept()
        print('The tcp server connected with ',addr)
        mytcp.recv_flag = True

        mytcp.tcp_bytes = connfd.recv(1024)
        print('The tcp server has received message: ', mytcp.tcp_bytes)

    except socket.error:
        if mytcp.recv_flag:
            print('The tcp server did not receive message!')
        else:
            print('The tcp server fail to connect!')
    finally:
        print('Close the socket.')
        sock.close()


def get_ipv6_from_ipv4(ipv4_address:str, br:IdfDut) -> str:
    clean_buffer(br)
    br.write('br nat64prefix')
    omrprefix = br.expect(r'\n((?:\w+:){6}):/\d+', timeout=5)[1].decode()
    ipv4_find = re.findall(r'\d+', ipv4_address)
    ipv6_16_1 = decimal_to_hex(ipv4_find[0]) + decimal_to_hex(ipv4_find[1])
    ipv6_16_2 = decimal_to_hex(ipv4_find[2]) + decimal_to_hex(ipv4_find[3])
    ipv6_get_from_ipv4 = omrprefix + ':' + ipv6_16_1 + ':' + ipv6_16_2
    return str(ipv6_get_from_ipv4)


def decimal_to_hex(decimal_str:str) -> str:
    decimal_int = int(decimal_str)
    hex_str = hex(decimal_int)[2:]
    return hex_str
