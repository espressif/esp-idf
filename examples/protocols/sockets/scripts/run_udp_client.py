# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import argparse
import os
import socket

DEF_PORT = 3333
DEF_MESSAGE = 'Data to ESP'


def udp_client(address: str, port: int, payload: str) -> str:
    for res in socket.getaddrinfo(address, port, socket.AF_UNSPEC,
                                  socket.SOCK_DGRAM, 0, socket.AI_PASSIVE):
        family_addr, _, _, _, addr = res
    try:
        sock = socket.socket(family_addr, socket.SOCK_DGRAM)
        sock.settimeout(20.0)
    except socket.error as msg:
        print('Could not create socket')
        print(os.strerror(msg.errno))
        raise
    try:
        sock.sendto(payload.encode(), addr)
        reply, addr = sock.recvfrom(128)
        if not reply:
            return ''
        print('Reply[' + addr[0] + ':' + str(addr[1]) + '] - ' + str(reply))
    except socket.timeout:
        print('Socket operation timeout')
        return ''
    except socket.error as msg:
        print('Error while sending or receiving data from the socket')
        print(os.strerror(msg.errno))
        sock.close()
        raise
    return reply.decode()


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('remote_ip', help='UDP server ip address, eg: 192.168.1.1, fe80::2%%eth0')
    parser.add_argument('--port', default=DEF_PORT, type=int, help='UDP server port')
    parser.add_argument('--message', default=DEF_MESSAGE, help='Message to send to the server.')
    args = parser.parse_args()

    print(f'Send message to the server: {args.remote_ip}')
    data = udp_client(args.remote_ip, args.port, args.message)
    print(f'Received From server: {data}')


if __name__ == '__main__':
    main()
