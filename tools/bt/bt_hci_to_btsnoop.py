# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import re
import struct
import time

parsed_num = 0
all_line_num = 0


def create_new_bt_snoop_file(filename: str) -> None:
    with open(filename, 'wb') as f:
        magic = b'btsnoop\x00'
        f.write(magic)
        version = 1
        datalink = 1002
        header = struct.pack('>II', version, datalink)
        f.write(header)


def append_hci_to_bt_snoop_file(filename: str, direction: int, data: str) -> None:
    if os.path.exists(filename):
        print(f'Appending to existing file: {filename}')
    else:
        print(f'Creating new file: {filename}')
        create_new_bt_snoop_file(filename)

    # Ensure data is converted to bytearray from hex string
    data_bytes = bytearray.fromhex(data)  # Convert hex string to bytearray

    with open(filename, 'ab') as f:  # 'ab' mode for appending binary data
        global parsed_num
        parsed_num += 1
        data_len = len(data_bytes)
        orig_len = data_len
        incl_len = data_len
        packet_flags = direction + (data_bytes[0] != 1 or data_bytes[0] != 3) * 2
        cumulative_drops = 0
        # Calculate the timestamp with an offset from a predefined reference time(0x00DCDDB30F2F8000).
        timestamp_us = int(round(time.time() * 1000000)) + 0x00DCDDB30F2F8000

        # Writing structured data followed by the byte array data
        f.write(struct.pack('>IIIIQ', orig_len, incl_len, packet_flags, cumulative_drops, timestamp_us))
        f.write(data_bytes)  # Write bytearray (binary data) to file


def log_data_clean(data: str) -> str:
    cleaned = re.sub(r'.*?<pre>', '', data)
    return cleaned


def is_adv_report(data: str) -> bool:
    is_binary = all(re.fullmatch(r'[0-9a-fA-F]{2}', part) for part in data.split())
    return is_binary


def parse_log(input_path: str, output_tag: str) -> None:
    """
    Parses the specified log file and saves the results to an output file.

    Args:
        input_path (str): Path to the input log file.
        output_tag (str): Name tag for the output file.

    Returns:
        None
    """
    global parsed_num
    global all_line_num
    if not os.path.exists(input_path):
        print(f"Error: The file '{input_path}' does not exist.")
        return

    # Define the output directory and create it if it doesn't exist
    output_dir = './parsed_logs'
    os.makedirs(output_dir, exist_ok=True)

    # Define the output file name based on the tag
    output_file = os.path.join(output_dir, f'parsed_log_{output_tag}.btsnoop.log')

    with open(input_path, 'r', encoding='utf-8') as infile:
        # Example: Parse each line and save processed results
        for line in infile:
            try:
                all_line_num += 1
                line = log_data_clean(line)
                line = line.replace('C:', '01 ')
                line = line.replace('E:', '04 ')
                line = line[3:]
                if line[0] == 'H':
                    line = line.replace('H:', '02 ')
                    append_hci_to_bt_snoop_file(output_file, 0, line)
                    continue
                if line[0] == 'D':
                    line = line.replace('D:', '02 ')
                    append_hci_to_bt_snoop_file(output_file, 1, line)
                    continue
                if line.startswith('01'):
                    append_hci_to_bt_snoop_file(output_file, 0, line)
                    continue
                if line.startswith('04'):
                    append_hci_to_bt_snoop_file(output_file, 1, line)
                    continue
                if is_adv_report(line):
                    line = '04 3e ' + line
                    append_hci_to_bt_snoop_file(output_file, 1, line)
            except Exception as e:
                print(f'Exception: {e}')

    if parsed_num > 0:
        print(f'Log parsing completed, parsed_num {parsed_num}, all_line_num {all_line_num}.\nOutput saved to: {output_file}')
    else:
        print('No data could be parsed.')


def main() -> None:
    # Define command-line arguments
    parser = argparse.ArgumentParser(description='Log Parsing Tool')
    parser.add_argument('-p', '--path', required=True, help='Path to the input log file')
    parser.add_argument('-o', '--output', required=True, help='Name tag for the output file')

    # Parse arguments
    args = parser.parse_args()
    input_path = args.path
    output_tag = args.output

    # Call the log parsing function
    parse_log(input_path, output_tag)


if __name__ == '__main__':
    main()
