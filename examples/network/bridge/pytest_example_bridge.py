# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import base64
import io
import ipaddress
import logging
import os
import re
import socket
import subprocess
import time
from collections.abc import Generator
from concurrent.futures import Future
from concurrent.futures import ThreadPoolExecutor
from re import Match

import netifaces
import paramiko  # type: ignore
import pytest
from common_test_methods import get_host_ip_by_interface
from netmiko import ConnectHandler
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

# Testbed configuration

ETHVM_ENDNODE_USER = 'ci.ethvm'

BR_PORTS_NUM = 2
IPERF_BW_LIM = 6
MIN_UDP_THROUGHPUT = 5
MIN_TCP_THROUGHPUT = 4


class EndnodeSsh:
    def __init__(self, host_ip: str, usr: str, passwd: str | None = None):
        key_string = os.getenv('CI_ETHVM_KEY')
        key = None
        if key_string:
            decoded_key_string = base64.b64decode(key_string)
            key = paramiko.Ed25519Key.from_private_key(io.StringIO(decoded_key_string.decode('utf-8')))

        self.host_ip = host_ip
        self.ssh_client = paramiko.SSHClient()
        self.ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.ssh_client.connect(hostname=self.host_ip, username=usr, pkey=key, password=passwd)
        self.executor: ThreadPoolExecutor
        self.async_result: Future

    def exec_cmd(self, cmd: str) -> str:
        _, stdout, stderr = self.ssh_client.exec_command(cmd)

        out = stdout.read().decode().strip()
        error = stderr.read().decode().strip()
        if error:
            out = ''
            logging.error(f'ssh_endnode_exec error: {error}')

        return out  # type: ignore

    def exec_cmd_async(self, cmd: str) -> None:
        self.executor = ThreadPoolExecutor(max_workers=1)
        self.async_result = self.executor.submit(self.exec_cmd, cmd)

    def get_async_res(self) -> str:
        return self.async_result.result(10)  # type: ignore

    def close(self) -> None:
        self.ssh_client.close()


class SwitchSsh:
    EDGE_SWITCH_5XP = 0
    EDGE_SWITCH_10XP = 1

    def __init__(self, host_ip: str, usr: str, passwd: str, device_type: int):
        self.host_ip = host_ip
        self.type = device_type

        if self.type == self.EDGE_SWITCH_5XP:
            self.ssh_client = paramiko.SSHClient()
            self.ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            self.ssh_client.connect(hostname=self.host_ip, username=usr, password=passwd)
        else:
            edgeSwitch = {
                'device_type': 'ubiquiti_edgeswitch',
                'host': self.host_ip,
                'username': usr,
                'password': passwd,
            }
            self.ssh_client = ConnectHandler(**edgeSwitch)

    def exec_cmd(self, cmd: str | list[str]) -> str:
        if self.type == self.EDGE_SWITCH_5XP:
            _, stdout, stderr = self.ssh_client.exec_command(cmd)

            out = stdout.read().decode().strip()
            error = stderr.read().decode().strip()

            if error != 'TSW Init OK!':
                raise Exception(f'switch_5xp exec_cmd error: {error}')
        else:
            out = self.ssh_client.send_config_set(cmd, cmd_verify=False, exit_config_mode=False)
        return out  # type: ignore

    def switch_port_down(self, port: int) -> None:
        if self.type == self.EDGE_SWITCH_5XP:
            command = '/usr/bin/tswconf debug phy set ' + str(port - 1) + ' 0 0x800'
            self.exec_cmd(command)
        else:
            commands = ['interface GigabitEthernet ' + str(port), 'shutdown']
            self.exec_cmd(commands)

    def switch_port_up(self, port: int) -> None:
        if self.type == self.EDGE_SWITCH_5XP:
            command = '/usr/bin/tswconf debug phy set ' + str(port - 1) + ' 0 0x1000'
            self.exec_cmd(command)
        else:
            commands = ['interface GigabitEthernet' + str(port), 'no shutdown']
            self.exec_cmd(commands)

    def close(self) -> None:
        if self.type == self.EDGE_SWITCH_5XP:
            self.ssh_client.close()
        else:
            self.ssh_client.disconnect()


def get_endnode_mac_by_interface(endnode: EndnodeSsh, if_name: str) -> str:
    ip_info = endnode.exec_cmd(f'ip addr show {if_name}')
    regex = (
        if_name
        + r':.*?link/ether ([0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2})'
    )
    mac_addr = re.search(regex, ip_info, re.DOTALL)
    if mac_addr is None:
        return ''
    return mac_addr.group(1)


def get_endnode_ip_by_interface(endnode: EndnodeSsh, if_name: str) -> str:
    ip_info = endnode.exec_cmd(f'ip addr show {if_name}')
    regex = if_name + r':.*?inet (\d+[.]\d+[.]\d+[.]\d+)\/'
    ip_addr = re.search(regex, ip_info, re.DOTALL)
    if ip_addr is None:
        return ''
    return ip_addr.group(1)


def get_host_interface_name_in_same_net(ip_addr: str) -> str:
    ip_net = ipaddress.IPv4Network(f'{ip_addr}/24', strict=False)
    for interface in netifaces.interfaces():
        addr = get_host_ip_by_interface(interface)
        if ipaddress.IPv4Address(addr) in ip_net:
            return str(interface)
    return ''


def get_host_mac_by_interface(interface_name: str, addr_type: int = netifaces.AF_LINK) -> str:
    for _addr in netifaces.ifaddresses(interface_name)[addr_type]:
        host_mac = _addr['addr'].replace(f'%{interface_name}', '')
        assert isinstance(host_mac, str)
        return host_mac
    return ''


def get_host_brcast_ip_by_interface(interface_name: str, ip_type: int = netifaces.AF_INET) -> str:
    for _addr in netifaces.ifaddresses(interface_name)[ip_type]:
        host_ip = _addr['broadcast'].replace(f'%{interface_name}', '')
        assert isinstance(host_ip, str)
        return host_ip
    return ''


def run_iperf(
    proto: str,
    endnode: EndnodeSsh,
    server_ip: str,
    bandwidth_lim: int = 10,
    interval: int = 5,
    server_if: str = '',
    client_if: str = '',
) -> float:
    if proto == 'tcp':
        proto = ''
    else:
        proto = '-u'

    if ipaddress.ip_address(server_ip).is_multicast:
        # Configure Multicast Server
        server_proc = subprocess.Popen(
            ['iperf', '-u', '-s', '-i', '1', '-t', str(interval), '-B', f'{server_ip}%{server_if}'],
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        # Configure Multicast Client
        endnode_ip = get_endnode_ip_by_interface(endnode, client_if)
        if endnode_ip == '':
            raise RuntimeError('End node IP address not found')
        client_res = endnode.exec_cmd(
            f'iperf -u -c {server_ip} -t {interval} -i 1 -b {bandwidth_lim}M --ttl 5 -B {endnode_ip}'
        )
        if server_proc.wait(10) is None:  # Process did not finish.
            server_proc.terminate()
    else:
        # Configure Server
        server_proc = subprocess.Popen(
            ['iperf', proto, '-s', '-i', '1', '-t', str(interval)],
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        # Configure Client
        client_res = endnode.exec_cmd(f'iperf {proto} -c {server_ip} -t {interval} -i 1 -b {bandwidth_lim}M')
        if server_proc.wait(10) is None:  # Process did not finish.
            server_proc.terminate()

    try:
        server_res = server_proc.communicate(timeout=15)[0]
    except subprocess.TimeoutExpired:
        server_proc.kill()
        server_res = server_proc.communicate()[0]

    print('\n')
    print(client_res)
    print('\n')
    print(server_res)

    SERVER_BANDWIDTH_LOG_PATTERN = r'(\d+\.\d+)\s*-\s*(\d+.\d+)\s+sec\s+[\d.]+\s+MBytes\s+([\d.]+)\s+Mbits\/sec'
    performance = re.search(SERVER_BANDWIDTH_LOG_PATTERN, server_res, re.DOTALL)
    if performance is None:
        return -1.0
    return float(performance.group(3))


def send_brcast_msg_host_to_endnode(endnode: EndnodeSsh, host_brcast_ip: str, port: int, test_msg: str) -> str:
    endnode.exec_cmd_async(f'timeout 4s nc -u -w 0 -l -p {port}')
    time.sleep(1)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        sock.sendto(test_msg.encode('utf-8'), (host_brcast_ip, port))
    except OSError as e:
        logging.error(f'Host brcast send failed {e}')
        sock.close()
        return ''

    nc_endnode_out = endnode.get_async_res()
    sock.close()
    return nc_endnode_out


def send_brcast_msg_endnode_to_host(endnode: EndnodeSsh, host_brcast_ip: str, port: int, test_msg: str) -> str:
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # Allow binding if port still in TIME_WAIT
    sock.settimeout(5)
    try:
        sock.bind(('', port))
    except OSError as e:
        logging.error(f'Host bind failed {e}')
        sock.close()
        return ''

    # Give socket time to be fully ready to receive before we tell endnode to send.
    # Even with SSH latency, there's a small window where a fast-received packet could be dropped.
    time.sleep(1)

    endnode.exec_cmd(f'echo -n "{test_msg}" | nc -b -w0 -u {host_brcast_ip} {port}')

    nc_host_out = ''
    try:
        nc_host_out = sock.recv(1500).decode('utf-8')
    except TimeoutError:
        logging.error('Host recv timed out after 5 seconds')
    except OSError as e:
        logging.error(f'Host recv failed {e}')

    sock.close()
    return nc_host_out


def get_legacy_host_name_match() -> Match[str] | None:
    host_name = socket.gethostname()
    regex = r'ethVM-(\d+)-(\d+)'
    host_name_match = re.search(regex, host_name, re.DOTALL)
    return host_name_match


def get_host_info() -> tuple[int, int]:
    # Get switch configuration info from the hostname (legacy runners)
    sw_info = get_legacy_host_name_match()
    if sw_info is not None:
        sw_num = int(sw_info.group(1))
        port_num = int(sw_info.group(2))
        return sw_num, port_num
    else:
        # Get switch configuration info from the IP address of the `switch` interface (new runners)
        switch_if_ip = get_host_ip_by_interface('switch', netifaces.AF_INET)
        # Parse IP address: x.y.<sw_num>.<port_num>
        ip_parts = switch_if_ip.split('.')
        if len(ip_parts) == 4:
            sw_num = int(ip_parts[2])
            port_num = int(ip_parts[3])
            return sw_num, port_num
        else:
            raise RuntimeError('Unexpected switch IP address')


def eth_bridge_test(dut: Dut, dev_user: str, dev_password: str) -> None:
    # ------------------------------ #
    # Pre-test testbed configuration #
    # ------------------------------ #
    sw_num, port_num = get_host_info()
    port_num_endnode = int(port_num) + 1  # endnode address is always + 1 to the host

    endnode = EndnodeSsh(f'10.10.{sw_num}.{port_num_endnode}', ETHVM_ENDNODE_USER)
    switch1 = SwitchSsh(f'10.10.{sw_num}.100', dev_user, dev_password, SwitchSsh.EDGE_SWITCH_10XP)

    # Collect all addresses in our network
    # ------------------------------------
    # Bridge (DUT) MAC
    br_mac = dut.expect(
        r'esp_netif_br_glue: '
        r'([0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:'
        r'[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2})'
    )
    br_mac = br_mac.group(1).decode('utf-8')
    logging.info('ESP Bridge MAC %s', br_mac)
    # Get unique identification of each Ethernet port
    p1_id = dut.expect(r'Ethernet \((0x[0-9A-Fa-f]{8})\) Started')
    p1_id = p1_id.group(1).decode('utf-8')
    p2_id = dut.expect(r'Ethernet \((0x[0-9A-Fa-f]{8})\) Started')
    p2_id = p2_id.group(1).decode('utf-8')
    # Bridge (DUT) IP
    dut.expect_exact('Ethernet Got IP Address')
    br_ip = dut.expect(r'ETHIP:(\d+[.]\d+[.]\d+[.]\d+)\r')
    br_ip = br_ip.group(1).decode('utf-8')
    logging.info('ESP Bridge IP %s', br_ip)

    # Host interface is in the same network as DUT
    host_if = get_host_interface_name_in_same_net(br_ip)
    # Test Host MAC
    host_mac = get_host_mac_by_interface(host_if)
    logging.info('Host MAC %s', host_mac)
    # Test Host IP
    host_ip = get_host_ip_by_interface(host_if, netifaces.AF_INET)
    logging.info('Host IP %s', host_ip)

    if get_legacy_host_name_match() is not None:
        endnode_if = host_if  # endnode is a clone of the host (legacy runners)
    else:
        endnode_if = 'dut_p2'  # interface name connected to the second port of the DUT (new runners)
    # Endnode MAC
    endnode_mac = get_endnode_mac_by_interface(endnode, endnode_if)
    logging.info('Endnode MAC %s', endnode_mac)
    # Toggle link status at the End Node to initiate DHCP request
    endnode.exec_cmd(f'sudo ip link set down dev {endnode_if}')
    endnode.exec_cmd(f'sudo ip link set up dev {endnode_if}')
    # Endnode IP
    for i in range(15):
        endnode_ip = get_endnode_ip_by_interface(endnode, endnode_if)
        if endnode_ip != '':
            break
        time.sleep(1)
        logging.info('End node waiting for DHCP IP addr, %isec...', i)
    else:
        raise RuntimeError('End node IP address not found')
    logging.info('Endnode IP %s', endnode_ip)

    # --------------------------------------------------
    # TEST Objective 1: Ping the devices on the network
    # --------------------------------------------------
    # ping bridge
    ping_test = subprocess.call(['ping', br_ip, '-c', '2'])
    if ping_test != 0:
        raise RuntimeError('ESP bridge is not reachable')

    # ping the end nodes of the network
    ping_test = subprocess.call(['ping', endnode_ip, '-c', '2'])
    if ping_test != 0:
        raise RuntimeError('End node is not reachable')

    # -------------------------------------------------
    # TEST Objective 2: Ports Link Up/Down combinations
    # -------------------------------------------------
    logging.info('link down the port #1')
    switch1.switch_port_down(port_num)
    dut.expect_exact(f'Ethernet ({p1_id}) Link Down')

    logging.info('link down both ports')
    switch1.switch_port_down(port_num_endnode)
    dut.expect_exact(f'Ethernet ({p2_id}) Link Down')

    logging.info('link up the port #1')
    switch1.switch_port_up(port_num)
    dut.expect_exact(f'Ethernet ({p1_id}) Link Up')
    dut.expect_exact('Ethernet Got IP Address')  # DHCP Server is connected to port #1

    logging.info('link down both ports')
    switch1.switch_port_down(port_num)
    dut.expect_exact(f'Ethernet ({p1_id}) Link Down')

    logging.info('link up the port #2')
    switch1.switch_port_up(port_num_endnode)
    dut.expect_exact(
        f'Ethernet ({p2_id}) Link Up'
    )  # Note: No "Ethernet Got IP Address" since DHCP Server is connected to port #1

    logging.info('link down both ports')
    switch1.switch_port_down(port_num_endnode)
    dut.expect_exact(f'Ethernet ({p2_id}) Link Down')

    logging.info('link up both ports')
    switch1.switch_port_up(port_num_endnode)
    dut.expect_exact(f'Ethernet ({p2_id}) Link Up')
    switch1.switch_port_up(port_num)  # link up port #1 as last to ensure we Got IP address after link port #2 is up
    dut.expect_exact(f'Ethernet ({p1_id}) Link Up')
    dut.expect_exact('Ethernet Got IP Address')

    # --------------------------------------------------------------------------
    # TEST Objective 3: IP traffic forwarding (iPerf between network end nodes)
    # --------------------------------------------------------------------------
    # unicast UDP
    bandwidth_udp = run_iperf('udp', endnode, host_ip, IPERF_BW_LIM, 5)
    if bandwidth_udp < MIN_UDP_THROUGHPUT:
        logging.warning(
            'Unicast UDP bandwidth was less than expected. '
            'Trying again over longer period to compensate transient drops.'
        )
        bandwidth_udp = run_iperf('udp', endnode, host_ip, IPERF_BW_LIM, 60)
    logging.info('Unicast UDP average bandwidth: %s Mbits/s', bandwidth_udp)

    # unicast TCP
    bandwidth_tcp = run_iperf('tcp', endnode, host_ip, IPERF_BW_LIM, 5)
    if bandwidth_tcp < MIN_TCP_THROUGHPUT:
        logging.warning(
            'Unicast TCP bandwidth was less than expected. '
            'Trying again over longer period to compensate transient drops.'
        )
        bandwidth_tcp = run_iperf('tcp', endnode, host_ip, IPERF_BW_LIM, 60)
    logging.info('Unicast TCP average bandwidth: %s Mbits/s', bandwidth_tcp)

    # multicast UDP
    bandwidth_mcast_udp = run_iperf('udp', endnode, '224.0.1.4', IPERF_BW_LIM, 5, host_if, endnode_if)
    if bandwidth_mcast_udp < MIN_UDP_THROUGHPUT:
        logging.warning(
            'Multicast UDP bandwidth was less than expected. '
            'Trying again over longer period to compensate transient drops.'
        )
        bandwidth_mcast_udp = run_iperf('udp', endnode, '224.0.1.4', IPERF_BW_LIM, 60, host_if, endnode_if)
    logging.info('Multicast UDP average bandwidth: %s Mbits/s', bandwidth_mcast_udp)

    if bandwidth_udp < MIN_UDP_THROUGHPUT:
        raise RuntimeError(
            f'Unicast UDP throughput expected {MIN_UDP_THROUGHPUT:.2f}, actual {bandwidth_udp:.2f} Mbits/s'
        )
    if bandwidth_tcp < MIN_TCP_THROUGHPUT:
        raise RuntimeError(
            f'Unicast TCP throughput expected {MIN_TCP_THROUGHPUT:.2f}, actual {bandwidth_tcp:.2f} Mbits/s'
        )
    if bandwidth_mcast_udp < MIN_UDP_THROUGHPUT:
        raise RuntimeError(
            f'Multicast UDP throughput expected {MIN_UDP_THROUGHPUT:.2f}, actual {bandwidth_mcast_udp:.2f} Mbits/s'
        )

    # ------------------------------------------------
    # TEST Objective 4: adding/deleting entries in FDB
    # ------------------------------------------------
    # At first test the Bridge Example Command Interface
    MAC_ADDR = '01:02:03:04:05:06'
    dut.write('\n')
    dut.expect_exact('bridge>')
    # invalid MAC format
    dut.write('add --addr=01:125:02:00:00:0A -d')
    dut.expect_exact('Ivalid MAC address format')
    dut.expect_exact('Command returned non-zero error code: 0x1')
    dut.write('add --addr=01:QA:02:00:00:0A -d')
    dut.expect_exact('Ivalid MAC address format')
    dut.expect_exact('Command returned non-zero error code: 0x1')
    dut.write('add --addr=01:00:02:00:0A -d')
    dut.expect_exact('Ivalid MAC address format')
    dut.expect_exact('Command returned non-zero error code: 0x1')
    # invalid number of config parameters
    dut.write('add --addr=' + MAC_ADDR + ' -d -c -p 1')
    dut.expect_exact('Invalid number or combination of arguments')
    dut.expect_exact('Command returned non-zero error code: 0x1')
    dut.write('add --addr=' + MAC_ADDR + ' -d -c')
    dut.expect_exact('Invalid number or combination of arguments')
    dut.expect_exact('Command returned non-zero error code: 0x1')
    dut.write('add --addr=' + MAC_ADDR + ' -f -c')
    dut.expect_exact('Invalid number or combination of arguments')
    dut.expect_exact('Command returned non-zero error code: 0x1')
    dut.write('add --addr=' + MAC_ADDR + ' -d -p 1')
    dut.expect_exact('Invalid number or combination of arguments')
    dut.expect_exact('Command returned non-zero error code: 0x1')
    dut.write('add --addr=' + MAC_ADDR + ' -f -p 1 -p 2')
    dut.expect_exact('Invalid number or combination of arguments')
    dut.expect_exact('Command returned non-zero error code: 0x1')
    dut.write('add -p 1')
    dut.expect_exact('Command returned non-zero error code: 0x1')
    dut.write('add --addr=' + MAC_ADDR + ' -p')
    dut.expect_exact('Command returned non-zero error code: 0x1')
    dut.write('remove')
    dut.expect_exact('Command returned non-zero error code: 0x1')
    dut.write('remove --addr=' + MAC_ADDR + ' -d')
    dut.expect_exact('Command returned non-zero error code: 0x1')
    # Invalid port interval number
    dut.write('add --addr=' + MAC_ADDR + ' -p 0')
    dut.expect_exact('Invalid port number')
    dut.expect_exact('Command returned non-zero error code: 0x1')
    dut.write('add --addr=' + MAC_ADDR + ' -p ' + str(BR_PORTS_NUM + 1))
    dut.expect_exact('Invalid port number')
    dut.expect_exact('Command returned non-zero error code: 0x1')

    # try to add more FDB entries than configured max number
    for i in range(BR_PORTS_NUM + 1):
        dut.write(f'add --addr=01:02:03:00:00:{i:02x} -d')
        if i < BR_PORTS_NUM:
            dut.expect_exact('Bridge Config OK!')
        else:
            dut.expect_exact('Adding FDB entry failed')
            dut.expect_exact('Command returned non-zero error code: 0x1')

    # try to remove non-existent FDB entry
    dut.write('remove --addr=' + MAC_ADDR)
    dut.expect_exact('Removing FDB entry failed')
    dut.expect_exact('Command returned non-zero error code: 0x1')

    # remove dummy entries
    for i in range(BR_PORTS_NUM):
        dut.write(f'remove --addr=01:02:03:00:00:{i:02x}')
        dut.expect_exact('Bridge Config OK!')

    # valid multiple ports at once
    dut.write('add --addr=' + MAC_ADDR + ' -c -p 1 -p 2')
    dut.expect_exact('Bridge Config OK!')
    dut.write('remove --addr=' + MAC_ADDR)
    dut.expect_exact('Bridge Config OK!')
    dut.write('add --addr=' + MAC_ADDR + ' -p 1 -p 2')
    dut.expect_exact('Bridge Config OK!')
    dut.write('remove --addr=' + MAC_ADDR)
    dut.expect_exact('Bridge Config OK!')

    # drop `Endnode` MAC and try to ping it from `Test Host`
    logging.info('Drop `Endnode` MAC')
    dut.write('add --addr=' + endnode_mac + ' -d')
    dut.expect_exact('Bridge Config OK!')
    ping_test = subprocess.call(['ping', endnode_ip, '-c', '2'])
    if ping_test == 0:
        raise RuntimeError('End node should not be reachable')
    logging.info('Remove Drop `Endnode` MAC entry')
    dut.write('remove --addr=' + endnode_mac)
    dut.expect_exact('Bridge Config OK!')
    ping_test = subprocess.call(['ping', endnode_ip, '-c', '2'])
    if ping_test != 0:
        raise RuntimeError('End node is not reachable')

    # Since we have only two ports on DUT, it is kind of tricky to verify the forwarding directly with devices'
    # specific MAC addresses. However, we can verify it using broadcast address and to observe the system
    # behavior in all directions.

    # At first, check normal condition
    TEST_MSG = 'ESP32 bridge test message'
    host_brcast_ip = get_host_brcast_ip_by_interface(host_if, netifaces.AF_INET)
    endnode_recv = send_brcast_msg_host_to_endnode(endnode, host_brcast_ip, 5100, TEST_MSG)
    if endnode_recv != TEST_MSG:
        raise RuntimeError('Broadcast message was not received by endnode')
    host_recv = send_brcast_msg_endnode_to_host(endnode, host_brcast_ip, 5200, TEST_MSG)
    if host_recv != TEST_MSG:
        raise RuntimeError('Broadcast message was not received by host (normal condition)')

    # now, configure forward the broadcast only to port #1
    dut.write('add --addr=ff:ff:ff:ff:ff:ff -p 1')
    dut.expect_exact('Bridge Config OK!')
    # we should not be able to receive a message at endnode (no forward to port #2)...
    endnode_recv = send_brcast_msg_host_to_endnode(endnode, host_brcast_ip, 5101, TEST_MSG)
    if endnode_recv != '':
        raise RuntimeError('Broadcast message should not be received by endnode')

    # ... but we should be able to do the same in opposite direction (forward to port #1)
    host_recv = send_brcast_msg_endnode_to_host(endnode, host_brcast_ip, 5201, TEST_MSG)
    if host_recv != TEST_MSG:
        raise RuntimeError('Broadcast message was not received by host (forward only to port #1)')

    # Remove ARP record from Test host computer. ARP is broadcasted, hence Bridge port does not reply to a request since
    # it does not receive it (no forward to Bridge port). As a result, Bridge is not pingable.
    subprocess.call(['sudo', 'arp', '-d', br_ip])
    subprocess.call(['arp', '-a'])
    ping_test = subprocess.call(['ping', br_ip, '-c', '2'])
    if ping_test == 0:
        raise RuntimeError('Bridge should not be reachable')

    # Remove current broadcast entry and replace it with extended one which includes Bridge port
    # Now, we should be able to ping the Bridge...
    dut.write('remove --addr=ff:ff:ff:ff:ff:ff')
    dut.expect_exact('Bridge Config OK!')
    dut.write('add --addr=ff:ff:ff:ff:ff:ff -p 1 -c')
    dut.expect_exact('Bridge Config OK!')
    ping_test = subprocess.call(['ping', br_ip, '-c', '2'])
    if ping_test != 0:
        raise RuntimeError('Bridge is not reachable')

    # ...but we should still not be able to receive a message at endnode (no forward to port #2)
    endnode_recv = send_brcast_msg_host_to_endnode(endnode, host_brcast_ip, 5102, TEST_MSG)
    if endnode_recv != '':
        raise RuntimeError('Broadcast message should not be received by endnode')

    endnode.close()
    switch1.close()


@pytest.fixture(scope='session', autouse=True)
def setup_test_environment() -> Generator[None, None, None]:
    # Fixture code to run before any tests in the session
    # make sure dut_p2 is down (only for new runners)
    if get_legacy_host_name_match() is None:
        subprocess.call(['sudo', 'ip', 'link', 'set', 'down', 'dev', 'dut_p2'])

    yield  # Tests run here

    # Optional teardown after all tests...


@pytest.mark.eth_w5500
@pytest.mark.parametrize(
    'config',
    [
        'w5500',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_eth_bridge(dut: Dut, dev_user: str, dev_password: str) -> None:
    eth_bridge_test(dut, dev_user, dev_password)
