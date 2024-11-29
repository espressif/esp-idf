#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import os
import sys
from textwrap import indent

IDF_PATH = os.getenv('IDF_PATH', '')
ROMS_JSON = os.path.join(IDF_PATH, 'tools', 'idf_py_actions', 'roms.json')  # type: ignore


# Direct string extraction and comparison is not feasible due to:
# 1. ROM ELF binaries for esp32XX chips are little-endian. The byte order in
#    the binary may differ from the system's endianness. We must ensure the
#    bytes are read correctly for the system where the script runs.
# 2. GDB lacks built-in string comparison functionality. To work around this,
#    strings are converted to numeric values for comparison.
def get_rom_if_condition_str(date_addr: int, date_str: str) -> str:
    r = []
    for i in range(0, len(date_str), 4):
        value = hex(int.from_bytes(bytes(date_str[i:i + 4], 'utf-8'), 'little'))
        r.append(f'(*(int*) {hex(date_addr + i)}) == {value}')
    return 'if ' + ' && '.join(r)


def generate_gdbinit_rom_add_symbols(target: str) -> str:
    base_ident = '  '
    rom_elfs_dir = os.getenv('ESP_ROM_ELF_DIR')
    if not rom_elfs_dir:
        raise EnvironmentError(
            'ESP_ROM_ELF_DIR environment variable is not defined. Please try to run IDF "install" and "export" scripts.')
    if os.name == 'nt':
        # convert to posix-path for windows
        rom_elfs_dir = rom_elfs_dir.replace('\\', '/')
    with open(ROMS_JSON, 'r') as f:
        roms = json.load(f)
    if target not in roms:
        msg_body = f'Warning: ROM ELF is not supported yet for "{target}".'  # noqa: E713
        return f'echo {msg_body}\\n\n'
    r = ['', f'# Load {target} ROM ELF symbols']
    r.append('define target hookpost-remote')
    r.append('set confirm off')
    # Since GDB does not have 'else if' statement than we use nested 'if..else' instead.
    for i, k in enumerate(roms[target], 1):
        indent_str = base_ident * i
        rom_file = f'{target}_rev{k["rev"]}_rom.elf'
        build_date_addr = int(k['build_date_str_addr'], base=16)
        r.append(indent(f'# if $_streq((char *) {hex(build_date_addr)}, "{k["build_date_str"]}")', indent_str))
        r.append(indent(get_rom_if_condition_str(build_date_addr, k['build_date_str']), indent_str))
        r.append(indent(f'add-symbol-file {rom_elfs_dir}{rom_file}', indent_str + base_ident))
        r.append(indent('else', indent_str))
        if i == len(roms[target]):
            # In case no one known ROM ELF fits - print warning
            indent_str += base_ident
            msg_body = f'Warning: Unknown {target} ROM revision.'
            r.append(indent(f'echo {msg_body}\\n', indent_str))
    # Close 'else' operators
    for i in range(len(roms[target]), 0, -1):
        r.append(indent('end', base_ident * i))
    r.append('set confirm on')
    r.append('end')
    return '\n'.join(r)[1:]


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise ValueError('Please pass only one argument (target).')

    target = sys.argv[1]
    gdbinit_lines = generate_gdbinit_rom_add_symbols(target)
    print(gdbinit_lines)
