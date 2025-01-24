# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import re
import sys
from pathlib import Path
from typing import Iterator
from typing import List

idf_path = os.environ.get('IDF_PATH', None)


def find_files(base_path: str, patterns: List[str]) -> Iterator[Path]:
    for pattern in patterns:
        yield from Path(base_path).rglob(f'**/{pattern}')


def read_and_capture_suspicious_fields(file_path: Path) -> List[str]:
    with open(file_path, 'r') as file:
        content = file.read()
    captures = re.findall(r'uint32_t\s+(\w+)\s*:\s*(16|8)\s*;', content)
    return [capture[0] for capture in captures if not capture[0].startswith('reserve')]


def main(
    target_chip: str, soc_module_patterns: List[str], hal_module_patterns: List[str]
) -> None:
    soc_path = f'{idf_path}/components/soc/{target_chip}/'
    hal_path = f'{idf_path}/components/hal/{target_chip}/'
    struct_file_patterns = [f'{pattern}_struct.h' for pattern in soc_module_patterns]
    ll_file_patterns = [f'{pattern}_ll.h' for pattern in hal_module_patterns]

    # Find `xxx_struct.h` files and `hal/yyy_ll.h` files
    struct_files = find_files(soc_path, struct_file_patterns)

    no_violation_found = True
    for struct_file in struct_files:
        # Capture suspicious fields from `xxx_struct.h`
        captures = read_and_capture_suspicious_fields(struct_file)
        # if no suspicious fields found, continue to next struct file
        if not captures:
            continue

        # check if any of the captured fields are being read/written as half-words
        ll_files = find_files(hal_path, ll_file_patterns)
        for ll_file in ll_files:
            with open(ll_file, 'r') as f:
                content = f.read()
                for capture in captures:
                    error_usages = [f'.{capture}', f'->{capture}']
                    for error_usage in error_usages:
                        word_bound_pattern = rf'{re.escape(error_usage)}\s*[;|=](.*)\b'
                        # print the line number where the error_usage is found
                        for match in re.finditer(word_bound_pattern, content, re.MULTILINE):
                            no_violation_found = False
                            start_line = content.count('\n', 0, match.start()) + 1
                            print(
                                f'Found pattern {error_usage} at line {start_line} in file {ll_file}'
                            )

    if no_violation_found:
        print('No violations found')
    else:
        sys.exit(1)


if __name__ == '__main__':
    if idf_path is None:
        print('IDF_PATH must be set before running this script', file=sys.stderr)
        sys.exit(1)

    parser = argparse.ArgumentParser(
        description='Check if any register fields are being read/written as half-words.'
    )
    parser.add_argument(
        '--target-chip',
        type=str,
        required=True,
        help='The name of the chip. e.g. esp32s3',
    )
    parser.add_argument(
        '--soc-modules',
        required=True,
        type=str,
        nargs='+',  # allow multiple soc modules
        help='Name pattern for the SOC module. e.g. spi means spi_struct.h',
    )
    parser.add_argument(
        '--hal-modules',
        required=True,
        type=str,
        nargs='+',  # allow multiple hal modules
        help='Name pattern for the HAL module. e.g. spi means spi_ll.h',
    )

    args = parser.parse_args()

    main(args.target_chip, args.soc_modules, args.hal_modules)
