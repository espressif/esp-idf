# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import argparse
import os
import socket

DEF_PORT = 3333
DEF_MESSAGE = 'Data to ESP'


def tcp_client(address: str, port: int, payload: str) -> str:
    for res in socket.getaddrinfo(address, port, socket.AF_UNSPEC,
                                  socket.SOCK_STREAM, 0, socket.AI_PASSIVE):
        family_addr, _, _, _, addr = res
    try:
        sock = socket.socket(family_addr, socket.SOCK_STREAM)
        sock.settimeout(60.0)
    except socket.error as msg:
        print('Could not create socket')
        print(os.strerror(msg.errno))
        raise
    try:
        sock.connect(addr)
    except socket.error as e:
        print('Could not open socket: ' + str(e))
        sock.close()
        raise
    sock.sendall(payload.encode())
    data = sock.recv(1024)
    if not data:
        return ''
    print('Reply : ' + data.decode())
    sock.close()
    return data.decode()


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('remote_ip', help='TCP server ip address, eg: 192.168.1.1, fe80::2%%eth0')
    parser.add_argument('--port', default=DEF_PORT, type=int, help='TCP server port')
    parser.add_argument('--message', default=DEF_MESSAGE, help='Message to send to the server.')
    args = parser.parse_args()

    print(f'Send message to the server: {args.remote_ip}')
    data = tcp_client(args.remote_ip, args.port, args.message)
    print(f'Received From server: {data}')


if __name__ == '__main__':
    main()
