# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import re
import sys
from collections.abc import Iterator
from pathlib import Path

idf_path = os.environ.get('IDF_PATH')


def find_files(base_path: str, patterns: list[str]) -> Iterator[Path]:
    for pattern in patterns:
        yield from Path(base_path).rglob(f'**/{pattern}')


def read_and_capture_suspicious_fields(file_path: Path) -> list[str]:
    with open(file_path) as file:
        content = file.read()
    captures = re.findall(r'uint32_t\s+(\w+)\s*:\s*(16|8)\s*;', content)
    return [capture[0] for capture in captures if not capture[0].startswith('reserve')]


def get_hal_paths(idf_root: str, target_chip: str) -> list[Path]:
    """
    Return all HAL roots for the target chip.

    Includes:
    - monolithic HAL: components/hal/<target_chip>
    - standalone HAL: components/esp_hal_*/<target_chip>
    """
    components_path = Path(idf_root) / 'components'
    hal_paths: list[Path] = []

    monolithic_hal = components_path / 'hal' / target_chip
    if monolithic_hal.exists():
        hal_paths.append(monolithic_hal)

    for standalone_hal in components_path.glob('esp_hal_*'):
        chip_hal_path = standalone_hal / target_chip
        if chip_hal_path.exists():
            hal_paths.append(chip_hal_path)

    return hal_paths


def main(idf_root: str, target_chip: str, soc_module_patterns: list[str], hal_module_patterns: list[str]) -> None:
    soc_path = f'{idf_root}/components/soc/{target_chip}/'
    struct_file_patterns = [f'{pattern}_struct.h' for pattern in soc_module_patterns]
    ll_file_patterns = [f'{pattern}_ll.h' for pattern in hal_module_patterns]
    hal_paths = get_hal_paths(idf_root, target_chip)

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
        for hal_path in hal_paths:
            ll_files = find_files(str(hal_path), ll_file_patterns)
            for ll_file in ll_files:
                with open(ll_file) as f:
                    content = f.read()
                    for capture in captures:
                        error_usages = [f'.{capture}', f'->{capture}']
                        for error_usage in error_usages:
                            word_bound_pattern = rf'{re.escape(error_usage)}\s*[;|=](.*)\b'
                            # print the line number where the error_usage is found
                            for match in re.finditer(word_bound_pattern, content, re.MULTILINE):
                                no_violation_found = False
                                start_line = content.count('\n', 0, match.start()) + 1
                                print(f'Found pattern {error_usage} at line {start_line} in file {ll_file}')

    if no_violation_found:
        print('No violations found')
    else:
        sys.exit(1)


if __name__ == '__main__':
    if idf_path is None:
        print('IDF_PATH must be set before running this script', file=sys.stderr)
        sys.exit(1)

    parser = argparse.ArgumentParser(description='Check if any register fields are being read/written as half-words.')
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

    main(idf_path, args.target_chip, args.soc_modules, args.hal_modules)
