# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import argparse
import binascii
import hashlib
import hmac
import os
import subprocess


def generate_token_data(hmac_key_file: str) -> None:
    with open(hmac_key_file, 'rb') as file:
        key_data = file.read()
        data = bytes([0] * 32)
        token_data = hmac.HMAC(key_data, data, hashlib.sha256).digest()
        token_hex = binascii.hexlify(token_data).decode('utf-8')
        print(token_hex)


def generate_hmac_key(hmac_key_file: str) -> None:
    hmac_key = os.urandom(32)
    with open(hmac_key_file, 'wb') as file:
        file.write(hmac_key)


def check_jtag_status() -> None:
    esp_port = os.getenv('ESPPORT')
    if not esp_port:
        raise RuntimeError('ESPPORT not specified')

    output = subprocess.check_output(['espefuse.py', 'summary']).decode('utf-8')

    # check if JTAG is permenently/hard disabled
    if ('DIS_PAD_JTAG' in output and 'JTAG = True' in output) or ('HARD_DIS_JTAG' in output and 'JTAG = True' in output):
        print('JTAG functionality is permanently disabled')
    else:
        print('JTAG functionality is not permanently disabled')

    # check if JTAG is software disabled
    soft_dis_value = None
    lines = output.split('\n')
    for line in lines:
        if 'SOFT_DIS_JTAG' in line:
            hex_value = line.split('=')[-1].split(' ')[1]  # Extract the hexadecimal part
            soft_dis_value = int(hex_value, 16)
            break

    if soft_dis_value is not None:
        # Count the number of 1's in the binary representation of soft_dis_value
        ones_count = bin(soft_dis_value).count('1')
        if ones_count % 2 != 0:
            print('JTAG is software disabled')
        else:
            print('JTAG is not software disabled')
    else:
        print('SOFT_DIS_JTAG value not found in the output')

    print('If JTAG is permenently disabled, it cannot be re-enabled.\nThis example re-enables only software disabled JTAG access')


def main() -> None:
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest='command')

    subparsers.add_parser('check_jtag_status', help='Check JTAG current status')

    hmac_generator_parser = subparsers.add_parser('generate_hmac_key')
    hmac_generator_parser.add_argument('hmac_key_file', help='File to store generated HMAC key')

    token_generator_parser = subparsers.add_parser('generate_token')
    token_generator_parser.add_argument('hmac_key_file', help='File containing the HMAC key')

    args = parser.parse_args()

    if args.command == 'check_jtag_status':
        check_jtag_status()
    elif args.command == 'generate_hmac_key':
        generate_hmac_key(args.hmac_key_file)
    elif args.command == 'generate_token':
        generate_token_data(args.hmac_key_file)
    else:
        parser.print_help()


if __name__ == '__main__':
    main()
