# This example code is in the Public Domain (or CC0 licensed, at your option.)

# Unless required by applicable law or agreed to in writing, this
# software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied.

# -*- coding: utf-8 -*-

from builtins import input
import socket
import sys

# -----------  Config  ----------
PORT = 3333
IP_VERSION = 'IPv6'
IPV4 = '192.168.0.167'
IPV6 = 'fe80:0000:0000:0000:260a:c4ff:fe11:a1e0%wlp1s0'
# -------------------------------

if IP_VERSION == 'IPv4':
    addr = (IPV4, PORT)
    family_addr = socket.AF_INET
elif IP_VERSION == 'IPv6':
    family_addr = socket.AF_INET6
    for res in socket.getaddrinfo(IPV6, PORT, socket.AF_INET6,
                                  socket.SOCK_DGRAM, socket.SOL_UDP):
        af, socktype, proto, canonname, addr = res
else:
    print('IP_VERSION must be IPv4 or IPv6')
    sys.exit(1)


try:
    sock = socket.socket(family_addr, socket.SOCK_DGRAM)
except socket.error:
    print('Failed to create socket')
    sys.exit()

while True:
    msg = input('Enter message to send : ')
    try:
        sock.sendto(msg.encode(), addr)
        reply, addr = sock.recvfrom(128)
        if not reply:
            break
        print('Reply[' + addr[0] + ':' + str(addr[1]) + '] - ' + str(reply))
    except socket.error as msg:
        print('Error Code : ' + str(msg[0]) + ' Message: ' + msg[1])
        sys.exit()
