#!/usr/bin/env python
# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import os

import elftools.common.utils as ecu
import jsonschema
from elftools.elf.elffile import ELFFile
from idf_build_apps.constants import SUPPORTED_TARGETS

IDF_PATH = os.getenv('IDF_PATH', '')
ROMS_JSON = os.path.join(IDF_PATH, 'tools', 'idf_py_actions', 'roms.json')  # type: ignore


def test_roms_validate_json() -> None:
    with open(ROMS_JSON, 'r') as f:
        roms_json = json.load(f)

    json_schema_path = os.path.join(os.path.dirname(ROMS_JSON), 'roms_schema.json')
    with open(json_schema_path, 'r') as f:
        schema_json = json.load(f)
    jsonschema.validate(roms_json, schema_json)


def test_roms_check_supported_chips() -> None:
    with open(ROMS_JSON, 'r') as f:
        roms_json = json.load(f)
    for chip in SUPPORTED_TARGETS:
        assert chip in roms_json, f'Have no ROM data for chip {chip}'


def test_roms_validate_build_date() -> None:
    def get_string_from_elf_by_addr(filename: str, address: int) -> str:
        result = ''
        with open(filename, 'rb') as stream:
            elf_file = ELFFile(stream)
            ro = elf_file.get_section_by_name('.rodata')
            ro_addr_delta = ro['sh_addr'] - ro['sh_offset']
            cstring = ecu.parse_cstring_from_stream(ro.stream, address - ro_addr_delta)
            if cstring:
                result = str(cstring.decode('utf-8'))
        return result

    rom_elfs_dir = os.getenv('ESP_ROM_ELF_DIR', '')
    with open(ROMS_JSON, 'r') as f:
        roms_json = json.load(f)

    for chip in roms_json:
        for k in roms_json[chip]:
            rom_file = os.path.join(rom_elfs_dir, f'{chip}_rev{k["rev"]}_rom.elf')
            build_date_str = get_string_from_elf_by_addr(rom_file, int(k['build_date_str_addr'], base=16))
            assert len(build_date_str) == 11
            assert build_date_str == k['build_date_str']
