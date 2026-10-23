# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import socket
import sys
from pathlib import Path
from typing import Any
from typing import TextIO

from vcd_dumper import VCDDumper


def _get_local_host_ip() -> Any:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(('8.8.8.8', 80))
    return s.getsockname()[0]


def _tcp_server(port: int, chan_num: int, vcd_out_io: TextIO | None) -> None:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    host = _get_local_host_ip()
    s.bind((host, port))
    s.listen(5)
    print(f'TCP listening at {host}:{port}')

    c, addr = s.accept()
    print(f'Client {addr[0]}:{addr[1]} joined')
    c.settimeout(3)
    vcd = VCDDumper()
    vcd.open_new_vcd_file(chan_num, vcd_out_io)
    recv_data = b''
    try:
        while True:
            temp = c.recv(10240)
            recv_data += temp
            if temp == b'':
                break
            print(f'Data received {len(temp)} bytes')
    except TimeoutError:
        print('Done!')
    finally:
        print(f'Received {len(recv_data)} bytes in total')
        vcd.dump_samples(recv_data)

    print(f'Client {addr[0]}:{addr[1]} left')
    vcd.close_vcd_file()
    c.close()
    s.close()
    print('TCP server closed')


def tcp_server_main() -> None:
    # Args parser
    parser = argparse.ArgumentParser(description='Dump raw data to VCD format')
    parser.add_argument('-n', '--chan-num', type=int, help='The channel number that probed')
    parser.add_argument('-p', '--port', type=int, help='The TCP port', default=8888)
    parser.add_argument('-o', '--output-file', type=Path, help='The output vcd file name (optional)', default=None)

    # Parse args
    args = parser.parse_args()
    if (
        args.output_file is not None
        and str(args.output_file) != '-'
        and args.output_file.exists()
        and not args.output_file.is_file()
    ):
        parser.error(f'argument -o/--output-file: {args.output_file} is not a file')
    chan_num = args.chan_num
    port = args.port
    # the output file is closed by VCDDumper.close_vcd_file
    # '-' means standard output
    vcd_out_io = None
    if args.output_file is not None:
        vcd_out_io = sys.stdout if str(args.output_file) == '-' else open(args.output_file, 'w', encoding='utf-8')
    if chan_num <= 0:
        raise ValueError('Invalid channel number')

    # Get the actual probe channel number (round up to the nearest number that is power of 2)
    p = 0
    while (1 << p) < chan_num:
        p = p + 1
    chan_num = 1 << p

    _tcp_server(port, chan_num, vcd_out_io)
    exit(0)


if __name__ == '__main__':
    tcp_server_main()
