#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import argparse
import json
import os
import re
from typing import Dict

IDF_PATH = os.environ['IDF_PATH']
MAX_SIZE_DIFF = 50


def mem_test(size_json, esptool_output):  # type: (dict, list) -> None
    seg_len = {}  # type: Dict[str, int]
    for i in esptool_output:
        tmp = i.split(' ')
        if tmp[0] == 'Segment':
            # tmp look like ['Segment', '2:', 'len', '0x02780', 'load', '0x3fc90610', 'file_offs', '0x00007ab0', '[BYTE_ACCESSIBLE,MEM_INTERNAL,DRAM]']
            # tmp[3] contains the size of the segment and tmp[8] contains the name of the memory segment
            esptool_mem = {'mem_type':tmp[8], 'size':tmp[3]}
            seg = re.sub(r'MEM_INTERNAL|,|BYTE_ACCESSIBLE|\n|\[|\]', '', esptool_mem['mem_type'])
            # If there are two IRAMs in esptool output it will compute these two IRAM lengths in a seg_len['IRAM']
            seg_len[seg] = int(esptool_mem['size'], 16) if seg not in seg_len else seg_len[seg] + int(esptool_mem['size'], 16)
    # including flash_other to DROM because flash_other contain .flash.appdesc that includes in DROM that produced by esptool
    size_from_map = [('IROM', size_json['flash_code']), ('IRAM', size_json['iram_text'] + size_json['iram_vectors'] + size_json['diram_text']
                     + size_json['diram_vectors']), ('DROM', size_json['flash_rodata'] + size_json['flash_other']), ('DRAM', size_json
                     ['dram_data'] + size_json['diram_data'])]
    for mem_type, size in size_from_map:
        if abs(size - seg_len[mem_type]) > MAX_SIZE_DIFF:
            raise RuntimeError(mem_type + " segment in idf_size isn't correct regarding esptool")
    print('Test complete without errors')


def main():  # type: () -> None
    parser = argparse.ArgumentParser(description='mem_test.py - a tool to test accuracy of the sizes of the memory segments regarding idf.py size by esptool')

    parser.add_argument(
        'size_json', help='JSON file with the output of the idf.py size',
        type=argparse.FileType('r'))
    parser.add_argument(
        'esptool_output', help='File with the output of the esptool',
        type=argparse.FileType('r'))

    args = parser.parse_args()
    mem_test(json.loads(args.size_json.read()), args.esptool_output.read().split('\n'))


if __name__ == '__main__':
    main()
