#!/usr/bin/env python
# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import importlib.util
import json
import os

import elftools.common.utils as ecu
import jsonschema
from elftools.elf.elffile import ELFFile
from idf_build_apps.constants import SUPPORTED_TARGETS

ROMS_JSON = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'roms.json')


def test_roms_validate_json() -> None:
    with open(ROMS_JSON) as f:
        roms_json = json.load(f)

    json_schema_path = os.path.join(os.path.dirname(ROMS_JSON), 'roms_schema.json')
    with open(json_schema_path) as f:
        schema_json = json.load(f)
    jsonschema.validate(roms_json, schema_json)


def test_roms_check_supported_chips() -> None:
    with open(ROMS_JSON) as f:
        roms_json = json.load(f)
    for chip in SUPPORTED_TARGETS:
        if chip in ['esp32c5', 'esp32c61']:
            # IDFCI-3109
            continue
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
    with open(ROMS_JSON) as f:
        roms_json = json.load(f)

    for chip in roms_json:
        for k in roms_json[chip]:
            rom_file = os.path.join(rom_elfs_dir, f'{chip}_rev{k["rev"]}_rom.elf')
            build_date_str = get_string_from_elf_by_addr(rom_file, int(k['build_date_str_addr'], base=16))
            assert len(build_date_str) == 11
            assert build_date_str == k['build_date_str']


def test_gen_gdbinit_joins_rom_elf_dir() -> None:
    module_path = os.path.join(os.path.dirname(ROMS_JSON), 'gen_gdbinit.py')
    spec = importlib.util.spec_from_file_location('gen_gdbinit', module_path)
    assert spec is not None and spec.loader is not None
    gen_gdbinit = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(gen_gdbinit)

    rom_dir = '/tmp/esp-rom-elfs/20241011'
    old = os.environ.get('ESP_ROM_ELF_DIR')
    os.environ['ESP_ROM_ELF_DIR'] = rom_dir
    try:
        out = gen_gdbinit.generate_gdbinit_rom_add_symbols('esp32c3')
    finally:
        if old is None:
            os.environ.pop('ESP_ROM_ELF_DIR', None)
        else:
            os.environ['ESP_ROM_ELF_DIR'] = old

    assert f'add-symbol-file {rom_dir}/esp32c3_rev' in out
    assert f'add-symbol-file {rom_dir}esp32c3_rev' not in out

    os.environ['ESP_ROM_ELF_DIR'] = rom_dir + '/'
    try:
        out_slash = gen_gdbinit.generate_gdbinit_rom_add_symbols('esp32c3')
    finally:
        if old is None:
            os.environ.pop('ESP_ROM_ELF_DIR', None)
        else:
            os.environ['ESP_ROM_ELF_DIR'] = old

    assert f'add-symbol-file {rom_dir}/esp32c3_rev' in out_slash
