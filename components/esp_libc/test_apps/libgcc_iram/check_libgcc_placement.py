#!/usr/bin/env python
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Check that libgcc / compiler-rt helpers landed in the expected ELF sections."""

import argparse
import subprocess
import sys


def symbol_sections(objdump: str, elf: str) -> dict[str, str]:
    output = subprocess.check_output([objdump, '-t', elf], text=True, errors='replace')
    sections: dict[str, str] = {}
    for line in output.splitlines():
        parts = line.split()
        if len(parts) < 4:
            continue
        name = parts[-1]
        section = next((part for part in parts if part.startswith('.')), None)
        if section is None:
            continue
        sections[name] = section
    return sections


def expect(sections: dict[str, str], symbol: str, section: str, errors: list[str]) -> None:
    actual = sections.get(symbol)
    if actual is None:
        errors.append(f'{symbol} was not linked')
    elif actual != section:
        errors.append(f'{symbol} is in {actual}, expected {section}')


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument('--objdump', required=True)
    parser.add_argument('--elf-file', required=True)
    parser.add_argument('--double-in', choices=('iram', 'flash'), required=True)
    args = parser.parse_args()

    sections = symbol_sections(args.objdump, args.elf_file)
    errors: list[str] = []
    expect(sections, '__udivdi3', '.iram0.text', errors)
    # GCC libgcc defines this table. compiler-rt does not.
    if '__clz_tab' in sections:
        expect(sections, '__clz_tab', '.dram0.data', errors)
    double_section = '.iram0.text' if args.double_in == 'iram' else '.flash.text'
    expect(sections, '__adddf3', double_section, errors)

    if errors:
        print('libgcc placement check failed:', file=sys.stderr)
        for error in errors:
            print(f'  {error}', file=sys.stderr)
        return 1

    print(f'libgcc placement ok (__adddf3 in {double_section})')
    return 0


if __name__ == '__main__':
    sys.exit(main())
