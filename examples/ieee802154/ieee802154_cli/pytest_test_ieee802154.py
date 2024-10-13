# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import random
import re
from time import sleep
from typing import List, Tuple

import pexpect
import pytest
from pytest_embedded_idf.dut import IdfDut


def generate_shortaddr() -> str:
    shortaddr = ''
    cnt = 0
    while cnt < 2:
        num = random.randint(0, 255)
        shortaddr = shortaddr + '0x%02X ' % (num)
        cnt = cnt + 1
    shortaddr = shortaddr.rstrip()
    return shortaddr


def generate_extaddr() -> str:
    extaddr = ''
    cnt = 0
    while cnt < 8:
        num = random.randint(0, 255)
        extaddr = extaddr + '0x%02X ' % (num)
        cnt = cnt + 1
    extaddr = extaddr.rstrip()
    return extaddr


def dlt_pendingtable(ser:IdfDut, addr:str) -> None:
    if addr != '':
        addr_list = re.findall(r'0x([A-Za-z0-9]{2})', addr)
        addr_field = '0x' + ''.join(reversed(addr_list))
        addr_field = addr_field.lower()
        addr_list = addr.split(' ')
        if len(addr_list) == 2 or len(addr_list) == 8:
            cmd = 'pending'
            for item in addr_list:
                cmd = cmd + ' -d {}'.format(item)
            cmd = cmd + '\n'
            ser.write(cmd)
            if len(addr_list) == 2:
                ser.expect('delete pending shortaddr: {}'.format(addr_field), timeout=10)
            else:
                ser.expect('delete pending extaddr: {}'.format(addr_field), timeout=10)
            sleep(0.1)


def add_pendingtable(ser:IdfDut, addr:str='') -> None:
    if len(addr.split(' ')) == 2 or len(addr.split(' ')) == 8:
        cmd = 'pending %s\n' % (addr)
        addr_list = re.findall(r'0x([A-Za-z0-9]{2})', addr)
        addr_field = '0x' + ''.join(reversed(addr_list))
        addr_field = addr_field.lower()
        ser.write(cmd)
        if len(addr.split(' ')) == 2:
            ser.expect('add pending shortaddr: {}'.format(addr_field), timeout=10)
        else:
            ser.expect('add pending extaddr: {}'.format(addr_field), timeout=10)
        sleep(0.1)


def generate_wrong_PANID_addr(right:str) -> str:
    wrong = ''
    for i in right.split(' '):
        num = (int(i, 16) + random.randint(1, 255)) % 256
        wrong = wrong + '0x%02X ' % (num)
    wrong = wrong.rstrip()
    return wrong


def set_mismatch_short_extern_addr_pendingtable(ser:IdfDut, addr:str, short:int=12, extern:int=12) -> List[List]:
    short_addr = []
    extern_addr = []
    cnt = 0
    while cnt < short:
        if len(addr.split(' ')) == 2:
            addr = generate_wrong_PANID_addr(addr)
            add_pendingtable(ser, addr)
        else:
            addr = generate_shortaddr()
            add_pendingtable(ser, addr)
        short_addr.append(addr)
        cnt = cnt + 1
    while cnt < extern:
        if len(addr.split(' ')) == 8:
            addr = generate_wrong_PANID_addr(addr)
            add_pendingtable(ser, addr)
        else:
            addr = generate_extaddr()
            add_pendingtable(ser, addr)
        extern_addr.append(addr)
        cnt = cnt + 1
    return [short_addr, extern_addr]


def dlt_short_extern_addr_in_pendingtable(ser:IdfDut, table:List, short:int=5, extern:int=5) -> None:
    cnt = 0
    while cnt < short:
        dlt_pendingtable(ser, table[0][cnt])
        cnt = cnt + 1
    while cnt < extern:
        dlt_pendingtable(ser, table[1][cnt])
        cnt = cnt + 1


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'count, config', [
        (2, 'release'),
    ], indirect=True
)
def test_based_txrx(dut: Tuple[IdfDut, IdfDut]) -> None:
    transmit = dut[0]
    receive = dut[1]
    receive.expect('ieee802154>', timeout=10)
    transmit.write('channel -s 23')
    transmit.expect('set channel: 23', timeout=10)
    receive.write('channel -s 23')
    receive.expect('set channel: 23', timeout=10)
    receive.write('rx -r 1')
    receive.expect('RX Start', timeout=10)
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('tx -l 10')
    transmit.expect('tx sfd done, Radio state: 4', timeout=10)
    transmit.expect('Tx Done 10 bytes', timeout=10)
    transmit.expect('00 01 02 03 04 05 06 07', timeout=10)
    transmit.expect('08 09 00 00 00 00 00 00', timeout=10)
    receive.expect('rx sfd done, Radio state: 3', timeout=10)
    receive.expect('Rx Done 10 bytes', timeout=10)
    receive.expect('00 01 02 03 04 05 06 07', timeout=10)
    receive.write('rx -r 0')
    receive.expect('radio exit receive mode', timeout=10)
    transmit.write('tx -l 10')
    transmit.expect('tx sfd done, Radio state: 4', timeout=10)
    transmit.expect('Tx Done 10 bytes', timeout=10)
    transmit.expect('00 01 02 03 04 05 06 07', timeout=10)
    transmit.expect('08 09 00 00 00 00 00 00', timeout=10)
    tmp = receive.expect(pexpect.TIMEOUT, timeout=10)
    assert 'Rx Done' not in str(tmp)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'config', ['release',],
    indirect=True
)
def test_based_energy(dut: IdfDut) -> None:
    transmit = dut
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('ed -d 8')
    transmit.expect('ed start', timeout=10)
    transmit.expect('ed_scan_rss_value:', timeout=10)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'config', ['release',],
    indirect=True
)
def test_based_channel(dut: IdfDut) -> None:
    transmit = dut
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('channel -s 23')
    transmit.expect('set channel: 23', timeout=10)
    transmit.write('channel -g')
    transmit.expect('current channel: 23', timeout=10)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'config', ['release',],
    indirect=True
)
def test_based_txpower(dut: IdfDut) -> None:
    transmit = dut
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('txpower -s 13')
    transmit.expect('set txpower: 13', timeout=10)
    transmit.write('txpower -g')
    transmit.expect('current txpower: 13', timeout=10)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'config', ['release',],
    indirect=True
)
def test_based_promiscuous(dut: IdfDut) -> None:
    transmit = dut
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('promisc -e')
    transmit.expect('promiscuous mode is enabled', timeout=10)
    transmit.write('promisc -g')
    transmit.expect('hardware promiscuous mode was enabled', timeout=10)
    transmit.write('promisc -d')
    transmit.expect('promiscuous mode is disabled', timeout=10)
    transmit.write('promisc -g')
    transmit.expect('hardware promiscuous mode was disabled', timeout=10)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'config', ['release',],
    indirect=True
)
def test_based_panid(dut: IdfDut) -> None:
    transmit = dut
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('panid 0x60')
    transmit.expect('set panid: 0x60', timeout=10)
    transmit.write('panid -g')
    transmit.expect('current panid: 0x60', timeout=10)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'config', ['release',],
    indirect=True
)
def test_based_shortaddr(dut: IdfDut) -> None:
    transmit = dut
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('shortaddr 0x1234')
    transmit.expect('set shortaddr: 0x1234', timeout=10)
    transmit.write('shortaddr -g')
    transmit.expect('current shortaddr: 0x1234', timeout=10)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'config', ['release',],
    indirect=True
)
def test_based_extaddr(dut: IdfDut) -> None:
    transmit = dut
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('extaddr 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08')
    transmit.expect('set extaddr: 0807060504030201', timeout=10)
    transmit.write('extaddr -g')
    transmit.expect('get extaddr: 0807060504030201', timeout=10)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'config', ['release',],
    indirect=True
)
def test_based_coordinator(dut: IdfDut) -> None:
    transmit = dut
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('coordinator -e')
    transmit.expect('hardware coordinator is enabled', timeout=10)
    transmit.write('coordinator -g')
    transmit.expect('hardware coordinator was enabled', timeout=10)
    transmit.write('coordinator -d')
    transmit.expect('hardware coordinator is disabled', timeout=10)
    transmit.write('coordinator -g')
    transmit.expect('hardware coordinator was disabled', timeout=10)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'config', ['release',],
    indirect=True
)
def test_based_pending(dut: IdfDut) -> None:
    transmit = dut
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('pending -m 2')
    transmit.expect('set auto pending mode as: 2', timeout=10)
    transmit.write('pending -g')
    transmit.expect('get auto pending mode as: 2', timeout=10)
    add_pendingtable(transmit, addr='0x01 0x02')
    add_pendingtable(transmit, addr='0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08')
    dlt_pendingtable(transmit, addr='0x22 0x11')
    dlt_pendingtable(transmit, addr='0x77 0x66 0x55 0x44 0x33 0x22 0x11 0x00')
    transmit.write('pending -r')
    transmit.expect('clear the pending address table', timeout=10)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'config', ['release',],
    indirect=True
)
def test_based_cca(dut: IdfDut) -> None:
    transmit = dut
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('cca -v -60')
    transmit.expect('set cca threshold as: -60', timeout=10)
    transmit.write('cca -m 0')
    transmit.expect('set cca mode as: 0', timeout=10)
    transmit.write('cca -g')
    transmit.expect('threshold:-60 dB, mode: 0', timeout=10)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'count, config', [
        (2, 'release'),
    ], indirect=True
)
def test_based_autoack(dut: Tuple[IdfDut, IdfDut]) -> None:
    transmit = dut[0]
    receive = dut[1]
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('channel -s 23')
    transmit.expect('set channel: 23', timeout=10)
    receive.expect('ieee802154>', timeout=10)
    receive.write('channel -s 23')
    receive.expect('set channel: 23', timeout=10)
    transmit.write('promisc -d')
    transmit.expect('promiscuous mode is disabled', timeout=10)
    receive.write('promisc -d')
    receive.expect('promiscuous mode is disabled', timeout=10)
    transmit.write('panid 0x280A')
    transmit.expect('set panid: 0x280a', timeout=10)
    transmit.write('shortaddr 0x435A')
    transmit.expect('set shortaddr: 0x435a', timeout=10)
    receive.write('panid 0x94BC')
    receive.expect('set panid: 0x94bc', timeout=10)
    receive.write('shortaddr 0x6FDB')
    receive.expect('set shortaddr: 0x6fdb', timeout=10)
    receive.write('rx -r 1')
    receive.expect('RX Start', timeout=10)

    transmit.write('tx 0x20 0x88 0x00 0x0A 0x28 0xDB 0x6F 0xBC 0x94 0x5A 0x43 0x68 0x02 0xaa 0x15 0x30 0x01 0x02')
    transmit.expect('tx sfd done, Radio state: 4', timeout=10)
    transmit.expect('rx sfd done, Radio state: 4', timeout=10)
    transmit.expect('Tx Done 18 bytes', timeout=10)
    transmit.expect('20 88 00 0a 28 db 6f bc', timeout=10)
    transmit.expect('94 5a 43 68 02 aa 15 30', timeout=10)

    receive.expect('rx sfd done, Radio state: 3', timeout=10)
    receive.expect('Rx Done 18 bytes', timeout=10)
    receive.expect('20 88 00 0a 28 db 6f bc', timeout=10)
    receive.expect('94 5a 43 68 02 aa 15 30', timeout=10)

    transmit.expect('Rx ack 5 bytes', timeout=10)
    transmit.expect('02 00 00', timeout=10)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'count, config', [
        (2, 'release'),
    ], indirect=True
)
def test_based_autopending(dut: Tuple[IdfDut, IdfDut]) -> None:
    transmit = dut[0]
    receive = dut[1]

# mode 0: CMD ID = 0x04 --> FP = 1
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('rx -r 0')
    transmit.expect('radio exit receive mode')
    transmit.write('channel -s 23')
    transmit.expect('set channel: 23')
    transmit.write('promisc -d')
    transmit.expect('promiscuous mode is disabled')

    receive.expect('ieee802154>', timeout=10)
    receive.write('pending -r')
    receive.expect('clear the pending address table', timeout=10)
    receive.write('channel -s 23')
    receive.expect('set channel: 23', timeout=10)
    receive.write('promisc -d')
    receive.expect('promiscuous mode is disabled', timeout=10)
    receive.write('rx -r 0')
    receive.expect('radio exit receive mode', timeout=10)
    receive.write('pending -m 0')
    receive.expect('set auto pending mode as: 0', timeout=10)
    receive.write('panid 0x2E74')
    receive.expect('set panid: 0x2e74', timeout=10)
    receive.write('shortaddr 0x2649')
    receive.expect('set shortaddr: 0x2649', timeout=10)

    receive.write('rx -r 1')
    receive.expect('RX Start', timeout=10)
    transmit.write('tx 0x23 0x08 0x00 0x74 0x2E 0x49 0x26 0x04 0x53 0x04 0xe3 0x71 0xc2 0x36 0xf7 0xa5 0x2b 0x68 0x79 0x8c 0x72 0x50 0x8b 0x61 0x55 0x56')
    transmit.expect('Rx ack 5 bytes', timeout=10)
    transmit.expect('12 00 00', timeout=10)

# mode 0: CMD ID != 0x04 --> FP = 0
    receive.write('pending -r')
    receive.expect('clear the pending address table', timeout=10)
    receive.write('promisc -d')
    receive.expect('promiscuous mode is disabled', timeout=10)

    transmit.write('rx -r 0')
    transmit.expect('radio exit receive mode', timeout=10)
    transmit.write('promisc -d')
    transmit.expect('promiscuous mode is disabled', timeout=10)

    receive.write('rx -r 0')
    receive.expect('radio exit receive mode', timeout=10)
    receive.write('pending -m 0')
    receive.expect('set auto pending mode as: 0', timeout=10)
    receive.write('panid 0x3E66')
    receive.expect('set panid: 0x3e66', timeout=10)
    receive.write('shortaddr 0x3970')
    receive.expect('set shortaddr: 0x3970', timeout=10)
    receive.write('rx -r 1')
    receive.expect('RX Start', timeout=10)
    transmit.write('tx 0x23 0x08 0x0c 0x66 0x3E 0x70 0x39 0x01 0xef 0x2e 0x9f 0xf3 0xfe 0x75 0x99 0x35 0x66 0xd3')
    transmit.expect('Rx ack 5 bytes', timeout=10)
    transmit.expect('02 00 0c', timeout=10)

# mode 1: CMD ID = 0x04, src addr in pending table --> FP = 1
    receive.write('pending -r')
    receive.expect('clear the pending address table', timeout=10)
    receive.write('promisc -d')
    receive.expect('promiscuous mode is disabled', timeout=10)
    receive.write('rx -r 0')
    receive.expect('radio exit receive mode', timeout=10)
    receive.write('pending -m 1')
    receive.expect('set auto pending mode as: 1', timeout=10)
    receive.write('panid 0x2D9B')
    receive.expect('set panid: 0x2d9b', timeout=10)
    receive.write('shortaddr 0x3B21')
    receive.expect('set shortaddr: 0x3b21', timeout=10)

    transmit.write('rx -r 0')
    transmit.expect('radio exit receive mode', timeout=10)
    transmit.write('promisc -d')
    transmit.expect('promiscuous mode is disabled', timeout=10)
    transmit.write('panid 0xD9C3')
    transmit.expect('set panid: 0xd9c3', timeout=10)
    transmit.write('shortaddr 0x93EE')
    transmit.expect('set shortaddr: 0x93ee', timeout=10)

    wrong_table = set_mismatch_short_extern_addr_pendingtable(receive, '0xEE 0x93')
    dlt_short_extern_addr_in_pendingtable(receive, wrong_table)
    add_pendingtable(receive, '0xEE 0x93')

    receive.write('rx -r 1')
    receive.expect('RX Start', timeout=10)
    transmit.write('tx 0x23 0x88 0x00 0x9B 0x2D 0x21 0x3B 0xC3 0xD9 0xEE 0x93 0x04 0xc8 0x05 0xf8 0x1d 0x6e 0xe3')
    transmit.expect('Rx ack 5 bytes', timeout=10)
    transmit.expect('12 00 00', timeout=10)

# mode 1: CMD ID = 0x04, src addr not in pending table --> FP = 0
    receive.write('pending -r')
    receive.expect('clear the pending address table', timeout=10)
    receive.write('promisc -d')
    receive.expect('promiscuous mode is disabled', timeout=10)
    receive.write('rx -r 0')
    receive.expect('radio exit receive mode', timeout=10)
    receive.write('pending -m 1')
    receive.expect('set auto pending mode as: 1', timeout=10)
    receive.write('panid 0x9FE3')
    receive.expect('set panid: 0x9fe3', timeout=10)
    receive.write('shortaddr 0x9E20')
    receive.expect('set shortaddr: 0x9e20', timeout=10)

    transmit.write('rx -r 0')
    transmit.expect('radio exit receive mode', timeout=10)
    transmit.write('promisc -d')
    transmit.expect('promiscuous mode is disabled', timeout=10)
    transmit.write('panid 0xE718')
    transmit.expect('set panid: 0xe718', timeout=10)
    transmit.write('shortaddr 0xC466')
    transmit.expect('set shortaddr: 0xc466', timeout=10)

    wrong_table = set_mismatch_short_extern_addr_pendingtable(receive, '0x66 0xC4')
    dlt_short_extern_addr_in_pendingtable(receive, wrong_table)

    receive.write('rx -r 1')
    receive.expect('RX Start', timeout=10)
    transmit.write('tx 0x2b 0x88 0x08 0xE3 0x9F 0x20 0x9E 0x18 0xE7 0x66 0xC4 0x17 0x92 0x8a 0xcd 0x4c 0xd0 0x20 0x40 0x0d 0x46 0x04 0xa0 0xe3 0x9c 0x57')
    transmit.expect('Rx ack 5 bytes', timeout=10)
    transmit.expect('02 00 08', timeout=10)

# mode 2: frame type is data, src addr in pending table --> FP = 1
    receive.write('pending -r')
    receive.expect('clear the pending address table', timeout=10)
    receive.write('promisc -d')
    receive.expect('promiscuous mode is disabled', timeout=10)
    receive.write('rx -r 0')
    receive.expect('radio exit receive mode', timeout=10)
    receive.write('pending -m 2')
    receive.expect('set auto pending mode as: 2', timeout=10)
    receive.write('panid 0xA116')
    receive.expect('set panid: 0xa116', timeout=10)
    receive.write('shortaddr 0x939C')
    receive.expect('set shortaddr: 0x939c', timeout=10)

    transmit.write('rx -r 0')
    transmit.expect('radio exit receive mode', timeout=10)
    transmit.write('promisc -d')
    transmit.expect('promiscuous mode is disabled', timeout=10)
    transmit.write('panid 0x9B0B')
    transmit.expect('set panid: 0x9b0b', timeout=10)
    transmit.write('shortaddr 0x04E4')
    transmit.expect('set shortaddr: 0x4e4', timeout=10)

    wrong_table = set_mismatch_short_extern_addr_pendingtable(receive, '0xE4 0x04')
    dlt_short_extern_addr_in_pendingtable(receive, wrong_table)
    add_pendingtable(receive, '0xE4 0x04')

    receive.write('rx -r 1')
    receive.expect('RX Start', timeout=10)
    transmit.write('tx 0x21 0x88 0x10 0x16 0xA1 0x9C 0x93 0x0B 0x9B 0xE4 0x04 0xb8 0xc5 0x63 0x43 0xa7 0x81 0xaf')
    transmit.expect('Rx ack 5 bytes', timeout=10)
    transmit.expect('12 00 10', timeout=10)

# mode 2: frame type is data, src addr not in pending table --> FP = 0
    receive.write('pending -r')
    receive.expect('clear the pending address table', timeout=10)
    receive.write('promisc -d')
    receive.expect('promiscuous mode is disabled', timeout=10)
    receive.write('rx -r 0')
    receive.expect('radio exit receive mode', timeout=10)
    receive.write('pending -m 2')
    receive.expect('set auto pending mode as: 2', timeout=10)
    receive.write('panid 0xE0C9')
    receive.expect('set panid: 0xe0c9', timeout=10)
    receive.write('shortaddr 0x3325')
    receive.expect('set shortaddr: 0x3325', timeout=10)

    transmit.write('rx -r 0')
    transmit.expect('radio exit receive mode', timeout=10)
    transmit.write('promisc -d')
    transmit.expect('promiscuous mode is disabled', timeout=10)
    transmit.write('panid 0x844F')
    transmit.expect('set panid: 0x844f', timeout=10)
    transmit.write('shortaddr 0x7C4C')
    transmit.expect('set shortaddr: 0x7c4c', timeout=10)

    wrong_table = set_mismatch_short_extern_addr_pendingtable(receive, '0x4C 0x7C')
    dlt_short_extern_addr_in_pendingtable(receive, wrong_table)

    receive.write('rx -r 1')
    receive.expect('RX Start', timeout=10)
    transmit.write('tx 0x21 0x88 0x10 0xC9 0xE0 0x25 0x33 0x4F 0x84 0x4C 0x7C 0xa8 0xff 0x51 0x72 0xef 0xc1 0xbb')
    transmit.expect('Rx ack 5 bytes', timeout=10)
    transmit.expect('02 00 10', timeout=10)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'config', ['release',],
    indirect=True
)
def test_based_transmit_failed(dut: IdfDut) -> None:
    transmit = dut
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('channel -s 23')
    transmit.expect('set channel: 23', timeout=10)
    transmit.write('cca -v -120')
    transmit.expect('set cca threshold as: -120', timeout=10)
    transmit.write('tx -l 10 -C')
    transmit.expect('the Frame Transmission failed, Failure reason: 1', timeout=10)
    transmit.write('tx -l 10')
    transmit.expect('tx sfd done, Radio state: 4', timeout=10)
    transmit.expect('Tx Done 10 bytes', timeout=10)
    transmit.expect('00 01 02 03 04 05 06 07', timeout=10)
    transmit.expect('08 09 00 00 00 00 00 00', timeout=10)


@pytest.mark.esp32c6
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'config', ['release',],
    indirect=True
)
def test_based_initialize(dut: IdfDut) -> None:
    transmit = dut
    transmit.expect('ieee802154>', timeout=10)
    transmit.write('heap')
    fst_data = transmit.expect(r'min heap size: [0-9]+', timeout=10)[0].decode()
    fst_size = re.findall(r'min heap size: ([0-9]+)', fst_data)
    transmit.write('esp154 -e')
    transmit.expect('initialize the IEEE 802.15.4 subsystem', timeout=10)
    transmit.write('esp154 -d')
    transmit.expect('deinitialize the IEEE 802.15.4 subsystem', timeout=10)
    transmit.write('heap')
    scd_data = transmit.expect(r'min heap size: [0-9]+', timeout=10)[0].decode()
    scd_size = re.findall(r'min heap size: ([0-9]+)', scd_data)
    print('first heap size: ', int(fst_size[0]))
    print('second heap size: ', int(scd_size[0]))
