# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import re
import struct
import time


def create_new_bt_snoop_file(filename: str) -> None:
    with open(filename, 'wb') as f:
        magic = b'btsnoop\x00'
        f.write(magic)
        version = 1
        datalink = 1002
        header = struct.pack('>II', version, datalink)
        f.write(header)


def append_hci_to_bt_snoop_file(filename: str, direction: int, data: str, timestamp_us: int) -> None:
    if os.path.exists(filename):
        print(f'Appending to existing file: {filename}')
    else:
        print(f'Creating new file: {filename}')
        create_new_bt_snoop_file(filename)
    data_bytes = bytearray.fromhex(data)
    with open(filename, 'ab') as f:
        orig_len = incl_len = len(data_bytes)
        packet_flags = direction + (data_bytes[0] != 1 and data_bytes[0] != 3) * 2
        cumulative_drops = 0
        f.write(struct.pack('>IIIIQ', orig_len, incl_len, packet_flags, cumulative_drops, timestamp_us))
        f.write(data_bytes)


def log_data_clean(data: str) -> str:
    cleaned = re.sub(r'.*?<pre>', '', data)
    return cleaned


def parse_log(input_path: str, output_tag: str, has_timestamp: bool = True) -> None:
    if not os.path.exists(input_path):
        print(f"Error: The file '{input_path}' does not exist.")
        return
    output_dir = './parsed_logs'
    os.makedirs(output_dir, exist_ok=True)
    output_file = os.path.join(output_dir, f'parsed_log_{output_tag}.btsnoop.log')
    parsed_num = 0
    all_line_num = 0
    with open(input_path, 'r', encoding='utf-8') as infile:
        for line in infile:
            try:
                all_line_num += 1
                line = log_data_clean(line).strip()
                if not line:
                    continue
                parts = line.split()
                if len(parts) < 2:
                    continue
                parts_wo_ln = parts[1:]
                literal = None
                if ':' in parts_wo_ln[0]:
                    literal_part, sep, ts_byte = parts_wo_ln[0].partition(':')
                    if sep == ':' and literal_part in ['C', 'D', 'E', 'H'] and len(ts_byte) == 2:
                        literal = literal_part + ':'
                        parts_wo_ln = [literal, ts_byte] + parts_wo_ln[1:]
                    else:
                        literal = None
                if literal:
                    if has_timestamp:
                        try:
                            timestamp_bytes = bytes(int(b, 16) for b in parts_wo_ln[1:9])
                            timestamp_us = int.from_bytes(timestamp_bytes, byteorder='little', signed=False)
                            hci_data = ' '.join(parts_wo_ln[9:])
                        except Exception:
                            continue
                    else:
                        timestamp_us = int(time.time() * 1e6)
                        hci_data = ' '.join(parts_wo_ln[1:])
                    if not hci_data:
                        continue
                    if literal == 'C:':
                        hci_data = '01 ' + hci_data
                        direction = 0
                    elif literal == 'E:':
                        hci_data = '04 ' + hci_data
                        direction = 1
                    elif literal == 'H:':
                        hci_data = '02 ' + hci_data
                        direction = 0
                    elif literal == 'D:':
                        hci_data = '02 ' + hci_data
                        direction = 1
                    else:
                        continue
                    append_hci_to_bt_snoop_file(output_file, direction, hci_data, timestamp_us)
                    parsed_num += 1
                else:
                    if has_timestamp:
                        try:
                            timestamp_bytes = bytes(int(b, 16) for b in parts[1:9])
                            timestamp_us = int.from_bytes(timestamp_bytes, byteorder='little', signed=False)
                            adv_data = ' '.join(parts[9:])
                        except Exception:
                            continue
                    else:
                        timestamp_us = int(time.time() * 1e6)
                        adv_data = ' '.join(parts[1:])
                    if not adv_data:
                        continue
                    hci_data = '04 3e ' + adv_data
                    direction = 1
                    append_hci_to_bt_snoop_file(output_file, direction, hci_data, timestamp_us)
                    parsed_num += 1
            except Exception as e:
                print(f'Exception: {e}')
    if parsed_num > 0:
        print(
            f'Parsing completed, parsed_num {parsed_num}, all_line_num {all_line_num}.\nOutput saved to: {output_file}'
        )
    else:
        print('No data could be parsed.')


def main() -> None:
    parser = argparse.ArgumentParser(description='Log Parsing Tool')
    parser.add_argument('-p', '--path', required=True, help='Path to the input log file')
    parser.add_argument('-o', '--output', required=True, help='Name tag for the output file')
    parser.add_argument(
        '--has-ts',
        action='store_true',
        default=False,
        help='Set this if the input file has timestamp bytes at the beginning (default: False)',
    )
    args = parser.parse_args()
    parse_log(args.path, args.output, has_timestamp=args.has_ts)


if __name__ == '__main__':
    main()
