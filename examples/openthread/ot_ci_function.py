# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
# !/usr/bin/env python3
# this file defines some functions for testing cli and br under pytest framework
import logging
import os
import re
import socket
import struct
import subprocess
import time
from collections.abc import Callable
from functools import wraps

import netifaces
import pexpect
import yaml
from pytest_embedded_idf.dut import IdfDut


def extract_address(
    command: str,
    pattern: str,
    default_return: str = '',
    retries: int = 3,
    delay: int = 2,
) -> Callable[[Callable[[str], str]], Callable[[IdfDut], str]]:
    def decorator(func: Callable[[str], str]) -> Callable[[IdfDut], str]:
        @wraps(func)
        def wrapper(dut: IdfDut) -> str:
            last_exception: Exception | None = None
            for attempt in range(1, retries + 1):
                try:
                    clean_buffer(dut)
                    execute_command(dut, command)
                    result = dut.expect(pattern, timeout=5)[1].decode()
                    return func(result)
                except Exception as e:
                    logging.exception(f'[{command}] Attempt {attempt}/{retries} failed: {e}')
                    last_exception = e
                    if attempt < retries:
                        time.sleep(delay)

            if last_exception:
                logging.exception(f'[{command}] Giving up after {retries} retries.')
            return default_return

        return wrapper

    return decorator


class thread_parameter:
    def __init__(self, deviceRole: str = '', dataset: str = '', channel: str = '', exaddr: str = '', bbr: bool = False):
        self.deviceRole = deviceRole
        self.dataset = dataset
        self.channel = channel
        self.exaddr = exaddr
        self.bbr = bbr
        self.networkname = ''
        self.panid = ''
        self.extpanid = ''
        self.networkkey = ''
        self.pskc = ''

    def setnetworkname(self, networkname: str) -> None:
        self.networkname = networkname

    def setpanid(self, panid: str) -> None:
        self.panid = panid

    def setextpanid(self, extpanid: str) -> None:
        self.extpanid = extpanid

    def setnetworkkey(self, networkkey: str) -> None:
        self.networkkey = networkkey

    def setpskc(self, pskc: str) -> None:
        self.pskc = pskc


class wifi_parameter:
    def __init__(self, ssid: str = '', psk: str = '', retry_times: int = 10):
        self.ssid = ssid
        self.psk = psk
        self.retry_times = retry_times


def joinThreadNetwork(dut: IdfDut, thread: thread_parameter) -> None:
    if thread.dataset:
        command = 'dataset set active ' + thread.dataset
        execute_command(dut, command)
        dut.expect('Done', timeout=5)
    else:
        execute_command(dut, 'dataset init new')
        dut.expect('Done', timeout=5)
    if thread.channel:
        command = 'dataset channel ' + thread.channel
        execute_command(dut, command)
        dut.expect('Done', timeout=5)
    if thread.exaddr:
        command = 'extaddr ' + thread.exaddr
        execute_command(dut, command)
        dut.expect('Done', timeout=5)
    if thread.networkname:
        command = 'dataset networkname ' + thread.networkname
        execute_command(dut, command)
        dut.expect('Done', timeout=5)
    if thread.panid:
        command = 'dataset panid ' + thread.panid
        execute_command(dut, command)
        dut.expect('Done', timeout=5)
    if thread.extpanid:
        command = 'dataset extpanid ' + thread.extpanid
        execute_command(dut, command)
        dut.expect('Done', timeout=5)
    if thread.networkkey:
        command = 'dataset networkkey ' + thread.networkkey
        execute_command(dut, command)
        dut.expect('Done', timeout=5)
    if thread.pskc:
        command = 'dataset pskc ' + thread.pskc
        execute_command(dut, command)
        dut.expect('Done', timeout=5)
    execute_command(dut, 'dataset commit active')
    dut.expect('Done', timeout=5)
    if thread.bbr:
        execute_command(dut, 'bbr enable')
        dut.expect('Done', timeout=5)
    if thread.deviceRole == 'router':
        execute_command(dut, 'routerselectionjitter 1')
        dut.expect('Done', timeout=5)
    execute_command(dut, 'ifconfig up')
    dut.expect('Done', timeout=5)
    execute_command(dut, 'thread start')
    assert wait_for_join(dut, thread.deviceRole)


def wait_for_join(dut: IdfDut, role: str) -> bool:
    clean_buffer(dut)
    for _ in range(1, 30):
        time.sleep(1)
        execute_command(dut, 'state')
        try:
            dut.expect(re.compile(role), timeout=5)
            return True
        except Exception:
            continue
    return False


def joinWiFiNetwork(dut: IdfDut, wifi: wifi_parameter) -> tuple[str, int]:
    clean_buffer(dut)
    ip_address = ''
    for order in range(1, wifi.retry_times):
        command = 'wifi connect -s ' + str(wifi.ssid) + ' -p ' + str(wifi.psk)
        tmp = get_ouput_string(dut, command, 10)
        if 'sta ip' in str(tmp):
            ip_address = re.findall(r'sta ip: (\w+.\w+.\w+.\w+),', str(tmp))[0]
        wait(dut, 2)
        execute_command(dut, 'wifi state')
        if dut.expect('connected', timeout=5):
            return ip_address, order
    raise Exception(f'{dut} connect wifi {str(wifi.ssid)} with password {str(wifi.psk)} fail')


def getDeviceRole(dut: IdfDut) -> str:
    wait(dut, 1)
    execute_command(dut, 'state')
    role = dut.expect(r'\W+(\w+)\W+Done', timeout=5)[1].decode()
    logging.info(role)
    return str(role)


def changeDeviceRole(dut: IdfDut, role: str) -> None:
    command = 'state ' + role
    execute_command(dut, command)


def getDataset(dut: IdfDut) -> str:
    execute_command(dut, 'dataset active -x')
    dut_data = dut.expect(r'\n(\w+)\r', timeout=5)[1].decode()
    return str(dut_data)


def init_thread(dut: IdfDut) -> None:
    dut.expect('OpenThread attached to netif', timeout=10)
    wait(dut, 3)
    reset_thread(dut)


def stop_thread(dut: IdfDut) -> None:
    execute_command(dut, 'thread stop')
    dut.expect('disabled', timeout=20)
    reset_thread(dut)


def reset_thread(dut: IdfDut) -> None:
    execute_command(dut, 'factoryreset')
    dut.expect('OpenThread attached to netif', timeout=20)
    wait(dut, 3)
    clean_buffer(dut)


def hardreset_dut(dut: IdfDut) -> None:
    dut.serial.hard_reset()
    dut.expect('OpenThread attached to netif', timeout=20)
    execute_command(dut, 'factoryreset')
    dut.expect('OpenThread attached to netif', timeout=20)


# get the mleid address of the thread
@extract_address('ipaddr mleid', r'\n((?:\w+:){7}\w+)\r')
def get_mleid_addr(addr: str) -> str:
    return addr


# get the rloc address of the thread
@extract_address('ipaddr rloc', r'\n((?:\w+:){7}\w+)\r')
def get_rloc_addr(addr: str) -> str:
    return addr


# get the linklocal address of the thread
@extract_address('ipaddr linklocal', r'\n((?:\w+:){7}\w+)\r')
def get_linklocal_addr(addr: str) -> str:
    return addr


# get the global unicast address of the thread:
def get_global_unicast_addr(dut: IdfDut, br: IdfDut) -> str:
    dut_adress = ''
    clean_buffer(br)
    omrprefix = get_omrprefix(br)
    execute_command(dut, 'ipaddr')
    dut_adress = dut.expect(rf'({omrprefix}(?:\w+:){{3}}\w+)\r', timeout=5)[1].decode()
    return str(dut_adress)


@extract_address('rloc16', r'(\w{4})')
def get_rloc16_addr(rloc16: str) -> str:
    return rloc16


# ping of thread
def ot_ping(
    dut: IdfDut, target: str, timeout: int = 5, count: int = 1, size: int = 56, interval: int = 1, hoplimit: int = 64
) -> tuple[int, int]:
    command = f'ping {str(target)} {size} {count} {interval} {hoplimit} {str(timeout)}'
    execute_command(dut, command)
    transmitted = dut.expect(r'(\d+) packets transmitted', timeout=60)[1].decode()
    tx_count = int(transmitted)
    received = dut.expect(r'(\d+) packets received', timeout=60)[1].decode()
    rx_count = int(received)
    return tx_count, rx_count


def ping_and_check(dut: IdfDut, target: str, tx_total: int = 10, timeout: int = 6, pass_rate: float = 0.8) -> None:
    tx_count = 0
    rx_count = 0
    for _ in range(tx_total):
        tx, rx = ot_ping(dut, target, timeout=timeout, count=1, size=10, interval=6)
        tx_count += tx
        rx_count += rx

    assert tx_count == tx_total
    assert rx_count > tx_total * pass_rate


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
        command = (
            'ip -6 route | grep ' + interface_name + " | grep ra | awk {'print $1'} | xargs -I {} ip -6 route del {}"
        )
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(0.5)
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        command = (
            'ip -6 address show dev '
            + interface_name
            + " scope global | grep 'inet6' | awk {'print $2'} | xargs -I {} ip -6 addr del {} dev "
            + interface_name
        )
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        flag = True
    finally:
        time.sleep(1)
        assert flag


def get_host_interface_name() -> str:
    home_dir = os.path.expanduser('~')
    config_path = os.path.join(home_dir, 'config', 'env_config.yml')
    try:
        if os.path.exists(config_path):
            with open(config_path) as file:
                config = yaml.safe_load(file)
            interface_name = config.get('interface_name')
            if interface_name:
                if interface_name == 'eth0':
                    logging.warning(
                        f"Warning: 'eth0' is not recommended as a valid network interface. "
                        f"Please check and update the 'interface_name' in the configuration file: "
                        f'{config_path}'
                    )
                else:
                    return str(interface_name)
            else:
                logging.warning("Warning: Configuration file found but 'interface_name' is not defined.")
    except Exception as e:
        logging.error(f'Error: Failed to read or parse {config_path}. Details: {e}')
    if 'eth1' in netifaces.interfaces():
        return 'eth1'

    raise Exception('Warning: No valid network interface detected. Please check your configuration.')


def clean_buffer(dut: IdfDut) -> None:
    str_length = str(len(dut.expect(pexpect.TIMEOUT, timeout=0.1)))
    dut.expect(rf'[\s\S]{{{str_length}}}', timeout=10)


def check_if_host_receive_ra(br: IdfDut) -> bool:
    interface_name = get_host_interface_name()
    clean_buffer(br)
    omrprefix = get_omrprefix(br)
    command = 'ip -6 route | grep ' + str(interface_name)
    out_str = subprocess.getoutput(command)
    logging.info(f'br omrprefix: {omrprefix}')
    logging.info(f'host route table:\n {out_str}')
    return str(omrprefix) in str(out_str)


def host_connect_wifi() -> None:
    command = '. /home/test/wlan_connection_OTTE.sh'
    subprocess.call(command, shell=True, timeout=30)
    time.sleep(5)


def is_joined_wifi_network(br: IdfDut) -> bool:
    return check_if_host_receive_ra(br)


thread_ipv6_group = 'ff04:0:0:0:0:0:0:125'


def check_ipmaddr(dut: IdfDut) -> bool:
    info = get_ouput_string(dut, 'ipmaddr', 2)
    if thread_ipv6_group in str(info):
        return True
    return False


def thread_is_joined_group(dut: IdfDut) -> bool:
    command = 'mcast join ' + thread_ipv6_group
    execute_command(dut, command)
    dut.expect('Done', timeout=5)
    order = 0
    while order < 3:
        if check_ipmaddr(dut):
            return True
        execute_command(dut, command)
        wait(dut, 2)
        order = order + 1
    return False


class udp_parameter:
    def __init__(
        self,
        udp_type: str = '',
        addr: str = '::',
        port: int = 5090,
        group: str = '',
        init_flag: bool = False,
        timeout: float = 15.0,
        udp_bytes: bytes = b'',
    ):
        self.udp_type = udp_type
        self.addr = addr
        self.port = port
        self.group = group
        self.init_flag = init_flag
        self.timeout = timeout
        self.udp_bytes = udp_bytes


def create_host_udp_server(myudp: udp_parameter) -> None:
    interface_name = get_host_interface_name()
    try:
        if myudp.udp_type == 'INET6':
            AF_INET = socket.AF_INET6
        else:
            AF_INET = socket.AF_INET
        logging.info('The host start to create udp server!')
        if_index = socket.if_nametoindex(interface_name)
        sock = socket.socket(AF_INET, socket.SOCK_DGRAM)
        sock.bind((myudp.addr, myudp.port))

        if myudp.udp_type == 'INET6' and myudp.group != '':
            sock.setsockopt(
                socket.IPPROTO_IPV6,
                socket.IPV6_JOIN_GROUP,
                struct.pack('16si', socket.inet_pton(socket.AF_INET6, myudp.group), if_index),
            )
        sock.settimeout(myudp.timeout)
        myudp.init_flag = True
        logging.info('The host start to receive message!')
        myudp.udp_bytes = (sock.recvfrom(1024))[0]
        udp_str = str(myudp.udp_bytes)
        logging.info(f'The host has received message: {udp_str}')
    except OSError:
        logging.error('The host did not receive message!')
    finally:
        logging.info('Close the socket.')
        sock.close()


def host_udp_send_message(udp_target: udp_parameter) -> None:
    interface_name = get_host_interface_name()
    try:
        if udp_target.udp_type == 'INET6':
            AF_INET = socket.AF_INET6
        else:
            AF_INET = socket.AF_INET
        sock = socket.socket(AF_INET, socket.SOCK_DGRAM)
        sock.bind(('::', 12350))
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BINDTODEVICE, interface_name.encode())
        sock.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_MULTICAST_HOPS, 32)
        logging.info('Host is sending message')
        sock.sendto(udp_target.udp_bytes, (udp_target.addr, udp_target.port))
    except OSError:
        logging.error('Host cannot send message')
    finally:
        sock.close()


def wait(dut: IdfDut, wait_time: float) -> None:
    dut.expect(pexpect.TIMEOUT, timeout=wait_time)


def get_host_ipv4_address() -> str:
    interface_name = get_host_interface_name()
    command = 'ifconfig ' + interface_name + " | grep -w 'inet' | awk '{print $2}'"
    out_bytes = subprocess.check_output(command, shell=True, timeout=5)
    out_str = out_bytes.decode('utf-8')
    host_ipv4_address = ''
    host_ipv4_address = re.findall(r'((?:\d+.){3}\d+)', str(out_str))[0]
    return str(host_ipv4_address)


def restart_avahi() -> None:
    command = 'avahi-daemon -k'
    subprocess.Popen(command, shell=True)
    time.sleep(2)
    command = 'avahi-daemon'
    subprocess.Popen(command, shell=True)
    time.sleep(5)


def start_avahi() -> None:
    time.sleep(1)
    command = '/etc/init.d/dbus start'
    subprocess.Popen(command, shell=True)
    time.sleep(5)
    restart_avahi()


def host_close_service() -> None:
    command = 'ps auxww | grep avahi-publish-s'
    out_bytes = subprocess.check_output(command, shell=True, timeout=5)
    out_str = out_bytes.decode('utf-8')
    logging.info(f'host close service avahi status:\n {out_str}')
    service_info = [line for line in out_str.splitlines() if 'testxxx _testxxx._udp' in line]
    for line in service_info:
        logging.info(f'Process:{line}')
        pid = line.split()[1]
        command = 'kill -9 ' + pid
        logging.info(f'kill {pid}')
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
    logging.info(f'hostname is: {hostname}')
    command = 'ps -auxww | grep avahi-daemon | grep running'
    out_str = subprocess.getoutput(command)
    logging.info(f'avahi status:\n {out_str}')
    role = re.findall(r'\[([\w\W]+)\.local\]', str(out_str))[0]
    logging.info(f'active host is: {role}')
    return str(role)


def flush_ipv6_addr_by_interface() -> None:
    interface_name = get_host_interface_name()
    logging.info(f'flush ipv6 addr : {interface_name}')
    command_show_addr = f'ip -6 addr show dev {interface_name}'
    command_show_route = f'ip -6 route show dev {interface_name}'
    addr_before = subprocess.getoutput(command_show_addr)
    route_before = subprocess.getoutput(command_show_route)
    logging.info(f'Before flush, IPv6 addresses: \n{addr_before}')
    logging.info(f'Before flush, IPv6 routes: \n{route_before}')
    subprocess.run(['ip', 'link', 'set', interface_name, 'down'])
    subprocess.run(['ip', '-6', 'addr', 'flush', 'dev', interface_name])
    subprocess.run(['ip', '-6', 'route', 'flush', 'dev', interface_name])
    subprocess.run(['ip', 'link', 'set', interface_name, 'up'])
    time.sleep(5)
    addr_after = subprocess.getoutput(command_show_addr)
    route_after = subprocess.getoutput(command_show_route)
    logging.info(f'After flush, IPv6 addresses: \n{addr_after}')
    logging.info(f'After flush, IPv6 routes: \n{route_after}')


class tcp_parameter:
    def __init__(
        self,
        tcp_type: str = '',
        addr: str = '::',
        port: int = 12345,
        listen_flag: bool = False,
        recv_flag: bool = False,
        timeout: float = 15.0,
        tcp_bytes: bytes = b'',
    ):
        self.tcp_type = tcp_type
        self.addr = addr
        self.port = port
        self.listen_flag = listen_flag
        self.recv_flag = recv_flag
        self.timeout = timeout
        self.tcp_bytes = tcp_bytes


def create_host_tcp_server(mytcp: tcp_parameter) -> None:
    try:
        if mytcp.tcp_type == 'INET6':
            AF_INET = socket.AF_INET6
        else:
            AF_INET = socket.AF_INET
        logging.info('The host start to create a tcp server!')
        sock = socket.socket(AF_INET, socket.SOCK_STREAM)
        sock.bind((mytcp.addr, mytcp.port))
        sock.listen(5)
        mytcp.listen_flag = True

        logging.info('The tcp server is waiting for connection!')
        sock.settimeout(mytcp.timeout)
        connfd, addr = sock.accept()
        logging.info(f'The tcp server connected with {addr}')
        mytcp.recv_flag = True

        mytcp.tcp_bytes = connfd.recv(1024)
        tcp_str = str(mytcp.tcp_bytes)
        logging.info(f'The tcp server has received message: {tcp_str}')

    except OSError:
        if mytcp.recv_flag:
            logging.error('The tcp server did not receive message!')
        else:
            logging.error('The tcp server fail to connect!')
    finally:
        logging.info('Close the socket.')
        sock.close()


def get_ipv6_from_ipv4(ipv4_address: str, br: IdfDut) -> str:
    clean_buffer(br)
    nat64prefix = get_nat64prefix(br)
    ipv4_find = re.findall(r'\d+', ipv4_address)
    ipv6_16_1 = decimal_to_hex(ipv4_find[0]) + decimal_to_hex(ipv4_find[1])
    ipv6_16_2 = decimal_to_hex(ipv4_find[2]) + decimal_to_hex(ipv4_find[3])
    ipv6_get_from_ipv4 = nat64prefix + ':' + ipv6_16_1 + ':' + ipv6_16_2
    return str(ipv6_get_from_ipv4)


def decimal_to_hex(decimal_str: str) -> str:
    decimal_int = int(decimal_str)
    hex_str = hex(decimal_int)[2:]
    return hex_str


@extract_address('br omrprefix', r'Local: ((?:\w+:){4}):/\d+\r')
def get_omrprefix(addr: str) -> str:
    return addr


@extract_address('br onlinkprefix', r'Local: ((?:\w+:){4}):/\d+\r')
def get_onlinkprefix(addr: str) -> str:
    return addr


@extract_address('br nat64prefix', r'Local: ((?:\w+:){6}):/\d+')
def get_nat64prefix(addr: str) -> str:
    return addr


def execute_command(dut: IdfDut, command: str, prefix: str = 'ot ') -> None:
    clean_buffer(dut)
    dut.write(prefix + command)


def get_ouput_string(dut: IdfDut, command: str, wait_time: int) -> str:
    execute_command(dut, command)
    tmp = dut.expect(pexpect.TIMEOUT, timeout=wait_time)
    clean_buffer(dut)
    return str(tmp)


def wait_for_host_network(host: str = '8.8.8.8', retries: int = 6, interval: int = 10) -> None:
    for attempt in range(1, retries + 1):
        try:
            subprocess.run(['ping', '-c', '1', '-W', '2', host], check=True)
            logging.info(f'Host network reachable on attempt {attempt}')
            return
        except subprocess.CalledProcessError:
            logging.info(f'Ping attempt {attempt} failed, retrying in {interval} seconds...')
            if attempt < retries:
                time.sleep(interval)
            else:
                raise RuntimeError(f'Host network is not reachable after {retries} attempts.')
