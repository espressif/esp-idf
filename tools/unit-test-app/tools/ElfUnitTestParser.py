# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
import subprocess
import sys
from typing import Dict, List

import yaml

try:
    import CreateSectionTable
except ImportError:
    sys.path.append(os.path.expandvars(os.path.join('$IDF_PATH', 'tools', 'unit-test-app', 'tools')))
    import CreateSectionTable


def get_target_objdump(idf_target: str) -> str:
    toolchain_for_target = {
        'esp32': 'xtensa-esp32-elf-',
        'esp32s2': 'xtensa-esp32s2-elf-',
        'esp32s3': 'xtensa-esp32s3-elf-',
        'esp32c2': 'riscv32-esp-elf-',
        'esp32c3': 'riscv32-esp-elf-',
    }
    return toolchain_for_target.get(idf_target, '') + 'objdump'


def parse_elf_test_cases(elf_file: str, idf_target: str) -> List[Dict]:
    objdump = get_target_objdump(idf_target)

    try:
        subprocess.check_output('{} -s {} > section_table.tmp'.format(objdump, elf_file), shell=True)
        table = CreateSectionTable.SectionTable('section_table.tmp')
    except subprocess.CalledProcessError:
        raise Exception('Can\'t resolve elf file. File not found.')
    finally:
        os.remove('section_table.tmp')

    bin_test_cases = []
    try:
        subprocess.check_output('{} -t {} | grep test_desc > case_address.tmp'.format(objdump, elf_file),
                                shell=True)

        with open('case_address.tmp', 'rb') as input_f:
            for line in input_f:
                # process symbol table like: "3ffb4310 l     O .dram0.data	00000018 test_desc_33$5010"
                sections = line.split()
                test_addr = int(sections[0], 16)
                section = sections[3]

                name_addr = table.get_unsigned_int(section, test_addr, 4)
                desc_addr = table.get_unsigned_int(section, test_addr + 4, 4)
                tc = {
                    'name': table.get_string('any', name_addr),
                    'desc': table.get_string('any', desc_addr),
                    'function_count': table.get_unsigned_int(section, test_addr + 20, 4),
                }
                bin_test_cases.append(tc)
    except subprocess.CalledProcessError:
        raise Exception('Test cases not found')
    finally:
        os.remove('case_address.tmp')

    return bin_test_cases


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('elf_file', help='Elf file to parse')
    parser.add_argument('-t', '--idf_target',
                        type=str, default=os.environ.get('IDF_TARGET', ''),
                        help='Target of the elf, e.g. esp32s2')
    parser.add_argument('-o', '--output_file',
                        type=str, default='elf_test_cases.yml',
                        help='Target of the elf, e.g. esp32s2')
    args = parser.parse_args()

    assert args.idf_target

    test_cases = parse_elf_test_cases(args.elf_file, args.idf_target)
    with open(args.output_file, 'w') as out_file:
        yaml.dump(test_cases, out_file, default_flow_style=False)
