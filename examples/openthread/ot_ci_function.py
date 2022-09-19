# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
# !/usr/bin/env python3
# this file defines some functions for testing cli and br under pytest framework

import re
import subprocess
import time
from typing import Tuple, Union

import netifaces
import pexpect
from pytest_embedded_idf.dut import IdfDut


def reset_thread(dut:IdfDut) -> None:
    time.sleep(1)
    dut.write('factoryreset')
    time.sleep(3)
    dut.expect('OpenThread attached to netif', timeout=10)
    dut.write(' ')
    dut.write('state')


# config thread
def config_thread(dut:IdfDut, model:str, dataset:str='0') -> Union[str, None]:
    if model == 'random':
        dut.write('dataset init new')
        dut.expect('Done', timeout=2)
        dut.write('dataset commit active')
        dut.expect('Done', timeout=2)
        dut.write('ifconfig up')
        dut.expect('Done', timeout=2)
        dut.write('dataset active -x')          # get dataset
        dut_data = dut.expect(r'\n(\w{212})\r', timeout=5)[1].decode()
        return str(dut_data)
    if model == 'appointed':
        tmp = 'dataset set active ' + str(dataset)
        dut.write(tmp)
        dut.expect('Done', timeout=2)
        dut.write('ifconfig up')
        dut.expect('Done', timeout=2)
        return None
    return None


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


# start thread
def start_thread(dut:IdfDut) -> str:
    role = ''
    dut.write('thread start')
    tmp = dut.expect(r'Role detached -> (\w+)\W', timeout=20)[0]
    role = re.findall(r'Role detached -> (\w+)\W', str(tmp))[0]
    return role


# config br and cli manually
def form_network_using_manual_configuration(leader:IdfDut, child:IdfDut, leader_name:str, thread_dataset_model:str,
                                            thread_dataset:str, wifi:IdfDut, wifi_ssid:str, wifi_psk:str) -> str:
    time.sleep(3)
    leader.expect('OpenThread attached to netif', timeout=10)
    leader.write(' ')
    leader.write('state')
    child.expect('OpenThread attached to netif', timeout=10)
    child.write(' ')
    child.write('state')
    reset_thread(leader)
    reset_thread(child)
    leader.write('channel 12')
    leader.expect('Done', timeout=2)
    child.write('channel 12')
    child.expect('Done', timeout=2)
    res = '0000'
    if wifi_psk != '0000':
        res = connect_wifi(wifi, wifi_ssid, wifi_psk, 10)[0]
    leader_data = ''
    if thread_dataset_model == 'random':
        leader_data = str(config_thread(leader, 'random'))
    else:
        config_thread(leader, 'appointed', thread_dataset)
    if leader_name == 'br':
        leader.write('bbr enable')
        leader.expect('Done', timeout=2)
    role = start_thread(leader)
    assert role == 'leader'
    if thread_dataset_model == 'random':
        config_thread(child, 'appointed', leader_data)
    else:
        config_thread(child, 'appointed', thread_dataset)
    if leader_name != 'br':
        child.write('bbr enable')
        child.expect('Done', timeout=2)
    role = start_thread(child)
    assert role == 'child'
    return res


# ping of thread
def ot_ping(dut:IdfDut, target:str, times:int) -> Tuple[int, int]:
    command = 'ping ' + str(target) + ' 0 ' + str(times)
    dut.write(command)
    transmitted = dut.expect(r'(\d+) packets transmitted', timeout=30)[1].decode()
    tx_count = int(transmitted)
    received = dut.expect(r'(\d+) packets received', timeout=30)[1].decode()
    rx_count = int(received)
    return tx_count, rx_count


# connect Wi-Fi
def connect_wifi(dut:IdfDut, ssid:str, psk:str, nums:int) -> Tuple[str, int]:
    clean_buffer(dut)
    ip_address = ''
    information = ''
    for order in range(1, nums):
        dut.write('wifi connect -s ' + str(ssid) + ' -p ' + str(psk))
        tmp = dut.expect(pexpect.TIMEOUT, timeout=5)
        ip_address = re.findall(r'sta ip: (\w+.\w+.\w+.\w+),', str(tmp))[0]
        information = dut.expect(r'wifi sta (\w+ \w+ \w+)\W', timeout=5)[1].decode()
        if information == 'is connected successfully':
            break
    assert information == 'is connected successfully'
    return ip_address, order


def reset_host_interface() -> None:
    interface_name = get_host_interface_name()
    flag = False
    try:
        command = 'ifconfig ' + interface_name + ' down'
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(10)
        command = 'ifconfig ' + interface_name + ' up'
        subprocess.call(command, shell=True, timeout=10)
        time.sleep(20)
        flag = True
    finally:
        time.sleep(10)
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
        time.sleep(5)
        flag = True
    finally:
        time.sleep(5)
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
        time.sleep(5)
        assert flag


def get_host_interface_name() -> str:
    interfaces = netifaces.interfaces()
    interface_name = [s for s in interfaces if 'wl' in s][0]
    return str(interface_name)


def clean_buffer(dut:IdfDut) -> None:
    str_length = str(len(dut.expect(pexpect.TIMEOUT, timeout=0.1)))
    dut.expect(r'[\s\S]{%s}' % str(str_length), timeout=10)
