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
IPV4 = '192.168.0.42'
IPV6 = 'fd00:0000:0000:0000:260a:c4ff:fe09:885c'
# -------------------------------

if IP_VERSION == 'IPv4':
    family_addr = socket.AF_INET
    addr = (IPV4, PORT)
elif IP_VERSION == 'IPv6':
    family_addr = socket.AF_INET6
    for res in socket.getaddrinfo(IPV6, PORT, socket.AF_INET6,
                                  socket.SOCK_STREAM, socket.SOL_TCP):
        af, socktype, proto, canonname, addr = res
else:
    print('IP_VERSION must be IPv4 or IPv6')
    sys.exit(1)

try:
    sock = socket.socket(family_addr, socket.SOCK_STREAM)
except socket.error as msg:
        print('Could not create socket: ' + str(msg[0]) + ': ' + msg[1])
        sys.exit(1)

try:
    sock.connect((IPV6, PORT))
except socket.error as msg:
        print('Could not open socket: ', msg)
        sock.close()
        sys.exit(1)

while True:
    msg = input('Enter message to send: ')
    assert isinstance(msg, str)
    msg = msg.encode()
    sock.sendall(msg)
    data = sock.recv(1024)
    if not data:
        break
    print('Reply: ' + data.decode())
sock.close()
