# This example code is in the Public Domain (or CC0 licensed, at your option.)

# Unless required by applicable law or agreed to in writing, this
# software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied.

# -*- coding: utf-8 -*-

import socket
import sys

# -----------  Config  ---------- 
IP_VERSION = 'IPv4'
PORT = 3333;
# -------------------------------

if IP_VERSION == 'IPv4':
    family_addr = socket.AF_INET
elif IP_VERSION == 'IPv6':
    family_addr = socket.AF_INET6
else:
    print('IP_VERSION must be IPv4 or IPv6')
    sys.exit(1)


try:
    sock = socket.socket(family_addr, socket.SOCK_STREAM)
except socket.error as msg:
    print('Error: ' + str(msg[0]) + ': ' + msg[1])
    sys.exit(1)

print('Socket created')
    
try:
    sock.bind(('', PORT))
    print('Socket binded')
    sock.listen(1)
    print('Socket listening')
    conn, addr = sock.accept()
    print('Connected by', addr)
except socket.error as msg:
    print('Error: ' + str(msg[0]) + ': ' + msg[1])
    sock.close()
    sys.exit(1)

while True:
    data = conn.recv(128)
    if not data: break
    data = data.decode()
    print('Received data: ' + data)
    reply = 'OK: ' + data
    conn.send(reply.encode())
conn.close()
