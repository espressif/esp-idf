#!/usr/bin/env python
#
# esp-idf alternative to "size" to print ELF file sizes, also analyzes
# the linker map file to dump higher resolution details.
#
# Includes information which is not shown in "xtensa-esp32-elf-size",
# or easy to parse from "xtensa-esp32-elf-objdump" or raw map files.
#
# Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
from __future__ import print_function
from __future__ import unicode_literals
from __future__ import division
from future.utils import iteritems
import argparse
import collections
import json
import os.path
import re
import sys

GLOBAL_JSON_INDENT = 4
GLOBAL_JSON_SEPARATORS = (',', ': ')


class MemRegions(object):
    (DRAM_ID, IRAM_ID, DIRAM_ID) = range(3)

    @staticmethod
    def get_mem_regions(target):
        # The target specific memory structure is deduced from soc_memory_types defined in
        # $IDF_PATH/components/soc/**/soc_memory_layout.c files.

        # The order of variables in the tuple is the same as in the soc_memory_layout.c files
        MemRegDef = collections.namedtuple('MemRegDef', ['primary_addr', 'length', 'type', 'secondary_addr'])

        if target == 'esp32':
            return sorted([
                # Consecutive MemRegDefs of the same type are joined into one MemRegDef
                MemRegDef(0x3FFAE000, 17 * 0x2000 + 4 * 0x8000 + 4 * 0x4000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFAE000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFB0000, 0x8000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFB8000, 0x8000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFC0000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFC2000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFC4000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFC6000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFC8000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFCA000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFCC000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFCE000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFD0000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFD2000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFD4000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFD6000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFD8000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFDA000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFDC000, 0x2000, MemRegions.DRAM_ID, 0),
                # MemRegDef(0x3FFDE000, 0x2000, MemRegions.DRAM_ID, 0),
                #
                # The bootloader is there and it has to been counted as DRAM
                # MemRegDef(0x3FFE0000, 0x4000, MemRegions.DIRAM_ID, 0x400BC000),
                # MemRegDef(0x3FFE4000, 0x4000, MemRegions.DIRAM_ID, 0x400B8000),
                # MemRegDef(0x3FFE8000, 0x8000, MemRegions.DIRAM_ID, 0x400B0000),
                # MemRegDef(0x3FFF0000, 0x8000, MemRegions.DIRAM_ID, 0x400A8000),
                # MemRegDef(0x3FFF8000, 0x4000, MemRegions.DIRAM_ID, 0x400A4000),
                # MemRegDef(0x3FFFC000, 0x4000, MemRegions.DIRAM_ID, 0x400A0000),
                #
                MemRegDef(0x40070000, 2 * 0x8000 + 16 * 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x40070000, 0x8000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x40078000, 0x8000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x40080000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x40082000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x40084000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x40086000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x40088000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x4008A000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x4008C000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x4008E000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x40090000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x40092000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x40094000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x40096000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x40098000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x4009A000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x4009C000, 0x2000, MemRegions.IRAM_ID, 0),
                # MemRegDef(0x4009E000, 0x2000, MemRegions.IRAM_ID, 0),
            ])
        elif target == 'esp32s2':
            return sorted([
                MemRegDef(0x3FFB2000, 3 * 0x2000 + 18 * 0x4000, MemRegions.DIRAM_ID, 0x40022000),
                # MemRegDef(0x3FFB2000, 0x2000, MemRegions.DIRAM_ID, 0x40022000),
                # MemRegDef(0x3FFB4000, 0x2000, MemRegions.DIRAM_ID, 0x40024000),
                # MemRegDef(0x3FFB6000, 0x2000, MemRegions.DIRAM_ID, 0x40026000),
                # MemRegDef(0x3FFB8000, 0x4000, MemRegions.DIRAM_ID, 0x40028000),
                # MemRegDef(0x3FFBC000, 0x4000, MemRegions.DIRAM_ID, 0x4002C000),
                # MemRegDef(0x3FFC0000, 0x4000, MemRegions.DIRAM_ID, 0x40030000),
                # MemRegDef(0x3FFC4000, 0x4000, MemRegions.DIRAM_ID, 0x40034000),
                # MemRegDef(0x3FFC8000, 0x4000, MemRegions.DIRAM_ID, 0x40038000),
                # MemRegDef(0x3FFCC000, 0x4000, MemRegions.DIRAM_ID, 0x4003C000),
                # MemRegDef(0x3FFD0000, 0x4000, MemRegions.DIRAM_ID, 0x40040000),
                # MemRegDef(0x3FFD4000, 0x4000, MemRegions.DIRAM_ID, 0x40044000),
                # MemRegDef(0x3FFD8000, 0x4000, MemRegions.DIRAM_ID, 0x40048000),
                # MemRegDef(0x3FFDC000, 0x4000, MemRegions.DIRAM_ID, 0x4004C000),
                # MemRegDef(0x3FFE0000, 0x4000, MemRegions.DIRAM_ID, 0x40050000),
                #
                # MemRegDef(0x3FFE4000, 0x4000, MemRegions.DIRAM_ID, 0x40054000),
                # MemRegDef(0x3FFE8000, 0x4000, MemRegions.DIRAM_ID, 0x40058000),
                # MemRegDef(0x3FFEC000, 0x4000, MemRegions.DIRAM_ID, 0x4005C000),
                # MemRegDef(0x3FFF0000, 0x4000, MemRegions.DIRAM_ID, 0x40060000),
                # MemRegDef(0x3FFF4000, 0x4000, MemRegions.DIRAM_ID, 0x40064000),
                # MemRegDef(0x3FFF8000, 0x4000, MemRegions.DIRAM_ID, 0x40068000),
                # MemRegDef(0x3FFFC000, 0x4000, MemRegions.DIRAM_ID, 0x4006C000),
            ])
        else:
            return None

    def __init__(self, target):
        self.chip_mem_regions = self.get_mem_regions(target)
        if not self.chip_mem_regions:
            raise RuntimeError('Target {} is not implemented in idf_size'.format(target))

    def _address_in_range(self, address, length, reg_address, reg_length):
        return address >= reg_address and (address - reg_address) <= (reg_length - length)

    def get_names(self, dictionary, region_id):
        def get_address(d):
            try:
                return d['address']
            except KeyError:
                return d['origin']

        def get_size(d):
            try:
                return d['size']
            except KeyError:
                return d['length']

        result = set()  # using a set will remove possible duplicates and consequent operations with sets are more
        # efficient
        for m in self.chip_mem_regions:
            if m.type != region_id:
                continue
            # the following code is intentionally not a one-liner for better readability
            for (n, c) in iteritems(dictionary):
                if (self._address_in_range(get_address(c), get_size(c), m.primary_addr, m.length) or
                    (m.type == self.DIRAM_ID and
                     self._address_in_range(get_address(c), get_size(c), m.secondary_addr, m.length))):
                    result.add(n)
        return result


def scan_to_header(f, header_line):
    """ Scan forward in a file until you reach 'header_line', then return """
    for line in f:
        if line.strip() == header_line:
            return
    raise RuntimeError("Didn't find line '%s' in file" % header_line)


def format_json(json_object):
    return json.dumps(json_object,
                      allow_nan=False,
                      indent=GLOBAL_JSON_INDENT,
                      separators=GLOBAL_JSON_SEPARATORS) + os.linesep


def load_map_data(map_file):
    memory_config = load_memory_config(map_file)
    detected_chip = detect_target_chip(map_file)
    sections = load_sections(map_file)
    return detected_chip, memory_config, sections


def load_memory_config(map_file):
    """ Memory Configuration section is the total size of each output section """
    result = {}
    scan_to_header(map_file, "Memory Configuration")
    RE_MEMORY_SECTION = re.compile(r"(?P<name>[^ ]+) +0x(?P<origin>[\da-f]+) +0x(?P<length>[\da-f]+)")

    for line in map_file:
        m = RE_MEMORY_SECTION.match(line)
        if m is None:
            if len(result) == 0:
                continue  # whitespace or a header, before the content we want
            else:
                return result  # we're at the end of the Memory Configuration
        section = {
            "name": m.group("name"),
            "origin": int(m.group("origin"), 16),
            "length": int(m.group("length"), 16),
        }
        if section["name"] != "*default*":
            result[section["name"]] = section
    raise RuntimeError("End of file while scanning memory configuration?")


def detect_target_chip(map_file):
    ''' Detect target chip based on the xtensa toolchain name in in the linker script part of the MAP file '''
    scan_to_header(map_file, 'Linker script and memory map')

    RE_TARGET = re.compile(r'^LOAD .*?/xtensa-([^-]+)-elf/')

    for line in map_file:
        m = RE_TARGET.search(line)
        if m:
            return m.group(1)
        line = line.strip()
        # There could be empty line(s) between the "Linker script and memory map" header and "LOAD lines". Therefore,
        # line stripping and length is checked as well. The "LOAD lines" are between START GROUP and END GROUP for
        # older MAP files.
        if not line.startswith(('LOAD', 'START GROUP')) and len(line) > 0:
            # This break is a failsafe to not process anything load_sections() might want to analyze.
            break
    return None


def load_sections(map_file):
    """ Load section size information from the MAP file.

    Returns a dict of 'sections', where each key is a section name and the value
    is a dict with details about this section, including a "sources" key which holds a list of source file line
    information for each symbol linked into the section.
    """
    # output section header, ie '.iram0.text     0x0000000040080400    0x129a5'
    RE_SECTION_HEADER = re.compile(r"(?P<name>[^ ]+) +0x(?P<address>[\da-f]+) +0x(?P<size>[\da-f]+)$")

    # source file line, ie
    # 0x0000000040080400       0xa4 /home/gus/esp/32/idf/examples/get-started/hello_world/build/esp32/libesp32.a(cpu_start.o)
    # cmake build system links some object files directly, not part of any archive, so make that part optional
    #  .xtensa.info   0x0000000000000000       0x38 CMakeFiles/hello-world.elf.dir/project_elf_src.c.obj
    RE_SOURCE_LINE = re.compile(r"\s*(?P<sym_name>\S*) +0x(?P<address>[\da-f]+) +0x(?P<size>[\da-f]+) (?P<archive>.+\.a)?\(?(?P<object_file>.+\.(o|obj))\)?")

    # Fast check to see if line is a potential source line before running the slower full regex against it
    RE_PRE_FILTER = re.compile(r".*\.(o|obj)\)?")

    # Check for lines which only contain the sym name (and rest is on following lines)
    RE_SYMBOL_ONLY_LINE = re.compile(r"^ (?P<sym_name>\S*)$")

    sections = {}
    section = None
    sym_backup = None
    for line in map_file:

        if line.strip() == "Cross Reference Table":
            # stop processing lines because we are at the next section in the map file
            break

        m = RE_SECTION_HEADER.match(line)
        if m is not None:  # start of a new section
            section = {
                "name": m.group("name"),
                "address": int(m.group("address"), 16),
                "size": int(m.group("size"), 16),
                "sources": [],
            }
            sections[section["name"]] = section
            continue

        if section is not None:
            m = RE_SYMBOL_ONLY_LINE.match(line)
            if m is not None:
                # In some cases the section name appears on the previous line, back it up in here
                sym_backup = m.group("sym_name")
                continue

            if not RE_PRE_FILTER.match(line):
                # line does not match our quick check, so skip to next line
                continue

            m = RE_SOURCE_LINE.match(line)
            if m is not None:  # input source file details=ma,e
                sym_name = m.group("sym_name") if len(m.group("sym_name")) > 0 else sym_backup
                archive = m.group("archive")
                if archive is None:
                    # optional named group "archive" was not matched, so assign a value to it
                    archive = "(exe)"

                source = {
                    "size": int(m.group("size"), 16),
                    "address": int(m.group("address"), 16),
                    "archive": os.path.basename(archive),
                    "object_file": os.path.basename(m.group("object_file")),
                    "sym_name": sym_name,
                }
                source["file"] = "%s:%s" % (source["archive"], source["object_file"])
                section["sources"] += [source]

    return sections


class MemRegNames(object):

    @staticmethod
    def get(mem_regions, memory_config, sections):
        mreg = MemRegNames()
        mreg.iram_names = mem_regions.get_names(memory_config, MemRegions.IRAM_ID)
        mreg.dram_names = mem_regions.get_names(memory_config, MemRegions.DRAM_ID)
        mreg.diram_names = mem_regions.get_names(memory_config, MemRegions.DIRAM_ID)
        mreg.used_iram_names = mem_regions.get_names(sections, MemRegions.IRAM_ID)
        mreg.used_dram_names = mem_regions.get_names(sections, MemRegions.DRAM_ID)
        mreg.used_diram_names = mem_regions.get_names(sections, MemRegions.DIRAM_ID)
        return mreg


def main():
    parser = argparse.ArgumentParser(description="idf_size - a tool to print size information from an IDF MAP file")

    parser.add_argument(
        '--json',
        help="Output results as JSON",
        action="store_true")

    parser.add_argument(
        'map_file', help='MAP file produced by linker',
        type=argparse.FileType('r'))

    parser.add_argument(
        '--archives', help='Print per-archive sizes', action='store_true')

    parser.add_argument(
        '--archive_details', help='Print detailed symbols per archive')

    parser.add_argument(
        '--files', help='Print per-file sizes', action='store_true')

    parser.add_argument(
        '--target', help='Set target chip', default=None)

    parser.add_argument(
        '--diff', help='Show the differences in comparison with another MAP file',
        metavar='ANOTHER_MAP_FILE',
        default=None,
        dest='another_map_file')

    parser.add_argument(
        '-o',
        '--output-file',
        type=argparse.FileType('w'),
        default=sys.stdout,
        help="Print output to the specified file instead of stdout")

    args = parser.parse_args()

    detected_target, memory_config, sections = load_map_data(args.map_file)
    args.map_file.close()

    def check_target(target, map_file):
        if target is None:
            raise RuntimeError('The target chip cannot be detected for {}. '
                               'Please report the issue.'.format(map_file.name))

    check_target(detected_target, args.map_file)

    if args.target is not None:
        if args.target != detected_target:
            print('WARNING: The detected chip target is {} but command line argument overwrites it to '
                  '{}!'.format(detected_target, args.target))
        detected_target = args.target

    if args.another_map_file:
        with open(args.another_map_file, 'r') as f:
            detected_target_diff, memory_config_diff, sections_diff = load_map_data(f)
            check_target(detected_target_diff, f)
            if detected_target_diff != detected_target:
                print('WARNING: The target of the reference and other MAP files is {} and {}, respectively.'
                      ''.format(detected_target, detected_target_diff))
    else:
        memory_config_diff, sections_diff = None, None

    mem_regions = MemRegions(detected_target)
    mem_reg = MemRegNames.get(mem_regions, memory_config, sections)
    mem_reg_diff = MemRegNames.get(mem_regions, memory_config_diff, sections_diff) if args.another_map_file else None

    output = ''

    if not args.json or not (args.archives or args.files or args.archive_details):
        output += get_summary(args.map_file.name, mem_reg, memory_config, sections,
                              args.json,
                              args.another_map_file, mem_reg_diff, memory_config_diff, sections_diff)

    if args.archives:
        output += get_detailed_sizes(mem_reg, sections, "archive", "Archive File", args.json, sections_diff)
    if args.files:
        output += get_detailed_sizes(mem_reg, sections, "file", "Object File", args.json, sections_diff)

    if args.archive_details:
        output += get_archive_symbols(mem_reg, sections, args.archive_details, args.json, sections_diff)

    args.output_file.write(output)
    args.output_file.close()


class StructureForSummary(object):
    (dram_data_names, dram_bss_names, dram_other_names,
     diram_data_names, diram_bss_names) = (frozenset(), ) * 5

    (total_iram, total_dram, total_dram, total_diram,
     used_dram_data, used_dram_bss, used_dram_other,
     used_dram, used_dram_ratio,
     used_iram, used_iram_ratio,
     used_diram_data, used_diram_bss,
     used_diram, used_diram_ratio,
     flash_code, flash_rodata,
     total_size) = (0, ) * 18

    @staticmethod
    def get(reg, mem_conf, sects):

        def _get_size(sects, section):
            try:
                return sects[section]['size']
            except KeyError:
                return 0

        r = StructureForSummary()

        r.dram_data_names = frozenset([n for n in reg.used_dram_names if n.endswith('.data')])
        r.dram_bss_names = frozenset([n for n in reg.used_dram_names if n.endswith('.bss')])
        r.dram_other_names = reg.used_dram_names - r.dram_data_names - r.dram_bss_names

        r.diram_data_names = frozenset([n for n in reg.used_diram_names if n.endswith('.data')])
        r.diram_bss_names = frozenset([n for n in reg.used_diram_names if n.endswith('.bss')])

        r.total_iram = sum(mem_conf[n]['length'] for n in reg.iram_names)
        r.total_dram = sum(mem_conf[n]['length'] for n in reg.dram_names)
        r.total_diram = sum(mem_conf[n]['length'] for n in reg.diram_names)

        r.used_dram_data = sum(_get_size(sects, n) for n in r.dram_data_names)
        r.used_dram_bss = sum(_get_size(sects, n) for n in r.dram_bss_names)
        r.used_dram_other = sum(_get_size(sects, n) for n in r.dram_other_names)
        r.used_dram = r.used_dram_data + r.used_dram_bss + r.used_dram_other
        try:
            r.used_dram_ratio = r.used_dram / r.total_dram
        except ZeroDivisionError:
            r.used_dram_ratio = float('nan')

        r.used_iram = sum(_get_size(sects, s) for s in sects if s in reg.used_iram_names)
        try:
            r.used_iram_ratio = r.used_iram / r.total_iram
        except ZeroDivisionError:
            r.used_iram_ratio = float('nan')

        r.used_diram_data = sum(_get_size(sects, n) for n in r.diram_data_names)
        r.used_diram_bss = sum(_get_size(sects, n) for n in r.diram_bss_names)
        r.used_diram = sum(_get_size(sects, n) for n in reg.used_diram_names)
        try:
            r.used_diram_ratio = r.used_diram / r.total_diram
        except ZeroDivisionError:
            r.used_diram_ratio = float('nan')

        r.flash_code = _get_size(sects, '.flash.text')
        r.flash_rodata = _get_size(sects, '.flash.rodata')
        r.total_size = r.used_dram + r.used_iram + r.used_diram + r.flash_code + r.flash_rodata

        return r

    def get_json_dic(self):
        return collections.OrderedDict([
            ('dram_data', self.used_dram_data + self.used_diram_data),
            ('dram_bss', self.used_dram_bss + self.used_diram_bss),
            ('dram_other', self.used_dram_other),
            ('used_dram', self.used_dram),
            ('available_dram', self.total_dram - self.used_dram),
            ('used_dram_ratio', self.used_dram_ratio if self.total_dram != 0 else 0),
            ('used_iram', self.used_iram),
            ('available_iram', self.total_iram - self.used_iram),
            ('used_iram_ratio', self.used_iram_ratio if self.total_iram != 0 else 0),
            ('used_diram', self.used_diram),
            ('available_diram', self.total_diram - self.used_diram),
            ('used_diram_ratio', self.used_diram_ratio if self.total_diram != 0 else 0),
            ('flash_code', self.flash_code),
            ('flash_rodata', self.flash_rodata),
            ('total_size', self.total_size)
        ])


def get_summary(path, mem_reg, memory_config, sections,
                as_json=False,
                path_diff=None, mem_reg_diff=None, memory_config_diff=None, sections_diff=None):

    diff_en = mem_reg_diff and memory_config_diff and sections_diff

    current = StructureForSummary.get(mem_reg, memory_config, sections)
    reference = StructureForSummary.get(mem_reg_diff,
                                        memory_config_diff,
                                        sections_diff) if diff_en else StructureForSummary()

    if as_json:
        current_json_dic = current.get_json_dic()
        if diff_en:
            reference_json_dic = reference.get_json_dic()
            diff_json_dic = collections.OrderedDict([(k,
                                                      v - reference_json_dic[k]) for k, v in iteritems(current_json_dic)])
            output = format_json(collections.OrderedDict([('current', current_json_dic),
                                                          ('reference', reference_json_dic),
                                                          ('diff', diff_json_dic),
                                                          ]))
        else:
            output = format_json(current_json_dic)
    else:
        rows = []
        if diff_en:
            rows += [('<CURRENT> MAP file: {}'.format(path), '', '', '')]
            rows += [('<REFERENCE> MAP file: {}'.format(path_diff), '', '', '')]
            rows += [('Difference is counted as <CURRENT> - <REFERENCE>, '
                      'i.e. a positive number means that <CURRENT> is larger.',
                      '', '', '')]
        rows += [('Total sizes{}:'.format(' of <CURRENT>' if diff_en else ''), '<REFERENCE>', 'Difference', '')]
        rows += [(' DRAM .data size: {f_dram_data:>7} bytes', '{f_dram_data_2:>7}', '{f_dram_data_diff:+}', '')]
        rows += [(' DRAM .bss  size: {f_dram_bss:>7} bytes', '{f_dram_bss_2:>7}', '{f_dram_bss_diff:+}', '')]

        if current.used_dram_other > 0 or reference.used_dram_other > 0:
            diff_list = ['+{}'.format(x) for x in current.dram_other_names - reference.dram_other_names]
            diff_list += ['-{}'.format(x) for x in reference.dram_other_names - current.dram_other_names]
            other_diff_str = '' if len(diff_list) == 0 else '({})'.format(', '.join(sorted(diff_list)))
            rows += [(' DRAM other size: {f_dram_other:>7} bytes ' + '({})'.format(', '.join(current.dram_other_names)),
                      '{f_dram_other_2:>7}',
                      '{f_dram_other_diff:+}',
                      other_diff_str)]

        rows += [('Used static DRAM: {f_used_dram:>7} bytes ({f_dram_avail:>7} available, '
                  '{f_used_dram_ratio:.1%} used)',
                  '{f_used_dram_2:>7}',
                  '{f_used_dram_diff:+}',
                  '({f_dram_avail_diff:>+7} available, {f_dram_total_diff:>+7} total)')]
        rows += [('Used static IRAM: {f_used_iram:>7} bytes ({f_iram_avail:>7} available, '
                  '{f_used_iram_ratio:.1%} used)',
                  '{f_used_iram_2:>7}',
                  '{f_used_iram_diff:+}',
                  '({f_iram_avail_diff:>+7} available, {f_iram_total_diff:>+7} total)')]

        if current.total_diram > 0 or reference.total_diram > 0:
            rows += [('Used stat D/IRAM: {f_used_diram:>7} bytes ({f_diram_avail:>7} available, '
                      '{f_used_diram_ratio:.1%} used)',
                      '{f_used_diram_2:>7}',
                      '{f_used_diram_diff:+}',
                      '({f_diram_avail_diff:>+7} available, {f_diram_total_diff:>+7} total)')]

        rows += [('      Flash code: {f_flash_code:>7} bytes',
                  '{f_flash_code_2:>7}',
                  '{f_flash_code_diff:+}',
                  '')]
        rows += [('    Flash rodata: {f_flash_rodata:>7} bytes',
                  '{f_flash_rodata_2:>7}',
                  '{f_flash_rodata_diff:+}',
                  '')]
        rows += [('Total image size:~{f_total_size:>7} bytes (.bin may be padded larger)',
                  '{f_total_size_2:>7}',
                  '{f_total_size_diff:+}',
                  '')]

        f_dic = {'f_dram_data': current.used_dram_data + current.used_diram_data,
                 'f_dram_bss': current.used_dram_bss + current.used_diram_bss,
                 'f_dram_other': current.used_dram_other,
                 'f_used_dram': current.used_dram,
                 'f_dram_avail': current.total_dram - current.used_dram,
                 'f_used_dram_ratio': current.used_dram_ratio,
                 'f_used_iram': current.used_iram,
                 'f_iram_avail': current.total_iram - current.used_iram,
                 'f_used_iram_ratio': current.used_iram_ratio,
                 'f_used_diram': current.used_diram,
                 'f_diram_avail': current.total_diram - current.used_diram,
                 'f_used_diram_ratio': current.used_diram_ratio,
                 'f_flash_code': current.flash_code,
                 'f_flash_rodata': current.flash_rodata,
                 'f_total_size': current.total_size,

                 'f_dram_data_2': reference.used_dram_data + reference.used_diram_data,
                 'f_dram_bss_2': reference.used_dram_bss + reference.used_diram_bss,
                 'f_dram_other_2': reference.used_dram_other,
                 'f_used_dram_2': reference.used_dram,
                 'f_used_iram_2': reference.used_iram,
                 'f_used_diram_2': reference.used_diram,
                 'f_flash_code_2': reference.flash_code,
                 'f_flash_rodata_2': reference.flash_rodata,
                 'f_total_size_2': reference.total_size,

                 'f_dram_total_diff': current.total_dram - reference.total_dram,
                 'f_iram_total_diff': current.total_iram - reference.total_iram,
                 'f_diram_total_diff': current.total_diram - reference.total_diram,

                 'f_dram_data_diff': current.used_dram_data + current.used_diram_data - (reference.used_dram_data +
                                                                                         reference.used_diram_data),
                 'f_dram_bss_diff': current.used_dram_bss + current.used_diram_bss - (reference.used_dram_bss +
                                                                                      reference.used_diram_bss),
                 'f_dram_other_diff': current.used_dram_other - reference.used_dram_other,
                 'f_used_dram_diff': current.used_dram - reference.used_dram,
                 'f_dram_avail_diff': current.total_dram - current.used_dram - (reference.total_dram -
                                                                                reference.used_dram),
                 'f_used_iram_diff': current.used_iram - reference.used_iram,
                 'f_iram_avail_diff': current.total_iram - current.used_iram - (reference.total_iram -
                                                                                reference.used_iram),
                 'f_used_diram_diff': current.used_diram - reference.used_diram,
                 'f_diram_avail_diff': current.total_diram - current.used_diram - (reference.total_diram -
                                                                                   reference.used_diram),
                 'f_flash_code_diff': current.flash_code - reference.flash_code,
                 'f_flash_rodata_diff': current.flash_rodata - reference.flash_rodata,
                 'f_total_size_diff': current.total_size - reference.total_size,
                 }

        lf = '{:70}{:>15}{:>15} {}'
        output = os.linesep.join([lf.format(a.format(**f_dic),
                                            b.format(**f_dic) if diff_en else '',
                                            c.format(**f_dic) if (diff_en and
                                                                  not c.format(**f_dic).startswith('+0')) else '',
                                            d.format(**f_dic) if diff_en else ''
                                            ).rstrip() for a, b, c, d in rows])
        output += os.linesep  # last line need to be terminated because it won't be printed otherwise

    return output


class StructureForDetailedSizes(object):

    @staticmethod
    def sizes_by_key(sections, key):
        """ Takes a dict of sections (from load_sections) and returns
        a dict keyed by 'key' with aggregate output size information.

        Key can be either "archive" (for per-archive data) or "file" (for per-file data) in the result.
        """
        result = {}
        for _, section in iteritems(sections):
            for s in section["sources"]:
                if not s[key] in result:
                    result[s[key]] = {}
                archive = result[s[key]]
                if not section["name"] in archive:
                    archive[section["name"]] = 0
                archive[section["name"]] += s["size"]
        return result

    @staticmethod
    def get(mem_reg, sections, key):
        sizes = StructureForDetailedSizes.sizes_by_key(sections, key)

        # these sets are also computed in get_summary() but they are small ones so it should not matter
        dram_data_names = frozenset([n for n in mem_reg.used_dram_names if n.endswith('.data')])
        dram_bss_names = frozenset([n for n in mem_reg.used_dram_names if n.endswith('.bss')])
        dram_other_names = mem_reg.used_dram_names - dram_data_names - dram_bss_names

        diram_data_names = frozenset([n for n in mem_reg.used_diram_names if n.endswith('.data')])
        diram_bss_names = frozenset([n for n in mem_reg.used_diram_names if n.endswith('.bss')])

        s = []
        for k, v in iteritems(sizes):
            r = [('data', sum(v.get(n, 0) for n in dram_data_names | diram_data_names)),
                 ('bss', sum(v.get(n, 0) for n in dram_bss_names | diram_bss_names)),
                 ('other', sum(v.get(n, 0) for n in dram_other_names)),
                 ('iram', sum(t for (s,t) in iteritems(v) if s in mem_reg.used_iram_names)),
                 ('diram', sum(t for (s,t) in iteritems(v) if s in mem_reg.used_diram_names)),
                 ('flash_text', v.get('.flash.text', 0)),
                 ('flash_rodata', v.get('.flash.rodata', 0))]
            r.append(('total', sum([value for _, value in r])))
            s.append((k, collections.OrderedDict(r)))

        s = sorted(s, key=lambda elem: elem[0])
        # do a secondary sort in order to have consistent order (for diff-ing the output)
        s = sorted(s, key=lambda elem: elem[1]['total'], reverse=True)

        return collections.OrderedDict(s)


def get_detailed_sizes(mem_reg, sections, key, header, as_json=False, sections_diff=None):

    diff_en = sections_diff is not None
    current = StructureForDetailedSizes.get(mem_reg, sections, key)
    reference = StructureForDetailedSizes.get(mem_reg, sections_diff, key) if diff_en else {}

    if as_json:
        if diff_en:
            diff_json_dic = collections.OrderedDict()
            for name in sorted(list(frozenset(current.keys()) | frozenset(reference.keys()))):
                cur_name_dic = current.get(name, {})
                ref_name_dic = reference.get(name, {})
                all_keys = sorted(list(frozenset(cur_name_dic.keys()) | frozenset(ref_name_dic.keys())))
                diff_json_dic[name] = collections.OrderedDict([(k,
                                                                cur_name_dic.get(k, 0) -
                                                                ref_name_dic.get(k, 0)) for k in all_keys])
            output = format_json(collections.OrderedDict([('current', current),
                                                          ('reference', reference),
                                                          ('diff', diff_json_dic),
                                                          ]))
        else:
            output = format_json(current)
    else:
        def _get_output(data, selection):
            header_format = '{:>24} {:>10} {:>6} {:>7} {:>6} {:>8} {:>10} {:>8} {:>7}' + os.linesep
            output = header_format.format(header,
                                          'DRAM .data',
                                          '& .bss',
                                          '& other',
                                          'IRAM',
                                          'D/IRAM',
                                          'Flash code',
                                          '& rodata',
                                          'Total')

            for k, v in iteritems(data):
                if k not in selection:
                    continue

                try:
                    _, k = k.split(':', 1)
                    # print subheadings for key of format archive:file
                except ValueError:
                    # k remains the same
                    pass

                output += header_format.format(k[:24],
                                               v['data'],
                                               v['bss'],
                                               v['other'],
                                               v['iram'],
                                               v['diram'],
                                               v['flash_text'],
                                               v['flash_rodata'],
                                               v['total'],
                                               )
            return output

        def _get_output_diff(curr, ref):
            header_format = '{:>24}' + ' {:>23}' * 8
            output = header_format.format(header,
                                          'DRAM .data',
                                          'DRAM .bss',
                                          'DRAM other',
                                          'IRAM',
                                          'D/IRAM',
                                          'Flash code',
                                          'Flash rodata',
                                          'Total') + os.linesep
            f_print = ('-' * 23, '') * 4
            header_line = header_format.format('', *f_print).rstrip() + os.linesep

            header_format = '{:>24}' + '|{:>7}|{:>7}|{:>7}' * 8
            f_print = ('<C>', '<R>', '<C>-<R>') * 8
            output += header_format.format('', *f_print) + os.linesep
            output += header_line

            for k, v in iteritems(curr):
                try:
                    v2 = ref[k]
                except KeyError:
                    continue

                try:
                    _, k = k.split(':', 1)
                    # print subheadings for key of format archive:file
                except ValueError:
                    # k remains the same
                    pass

                def _get_items(name):
                    a = v[name]
                    b = v2[name]
                    diff = a - b
                    # the sign is added here and not in header_format in order to be able to print empty strings
                    return (a or '', b or '', '' if diff == 0 else '{:+}'.format(diff))

                v_data, v2_data, diff_data = _get_items('data')
                v_bss, v2_bss, diff_bss = _get_items('bss')
                v_other, v2_other, diff_other = _get_items('other')
                v_iram, v2_iram, diff_iram = _get_items('iram')
                v_diram, v2_diram, diff_diram = _get_items('diram')
                v_flash_text, v2_flash_text, diff_flash_text = _get_items('flash_text')
                v_flash_rodata, v2_flash_rodata, diff_flash_rodata = _get_items('flash_rodata')
                v_total, v2_total, diff_total = _get_items('total')

                output += header_format.format(k[:24],
                                               v_data, v2_data, diff_data,
                                               v_bss, v2_bss, diff_bss,
                                               v_other, v2_other, diff_other,
                                               v_iram, v2_iram, diff_iram,
                                               v_diram, v2_diram, diff_diram,
                                               v_flash_text, v2_flash_text, diff_flash_text,
                                               v_flash_rodata, v2_flash_rodata, diff_flash_rodata,
                                               v_total, v2_total, diff_total,
                                               ).rstrip() + os.linesep
            return output

        output = 'Per-{} contributions to ELF file:{}'.format(key, os.linesep)

        if diff_en:
            output += _get_output_diff(current, reference)

            in_current = frozenset(current.keys())
            in_reference = frozenset(reference.keys())
            only_in_current = in_current - in_reference
            only_in_reference = in_reference - in_current

            if len(only_in_current) > 0:
                output += 'The following entries are present in <CURRENT> only:{}'.format(os.linesep)
                output += _get_output(current, only_in_current)

            if len(only_in_reference) > 0:
                output += 'The following entries are present in <REFERENCE> only:{}'.format(os.linesep)
                output += _get_output(reference, only_in_reference)
        else:
            output += _get_output(current, current)

    return output


class StructureForArchiveSymbols(object):
    @staticmethod
    def get(mem_reg, archive, sections):
        interested_sections = mem_reg.used_dram_names | mem_reg.used_iram_names | mem_reg.used_diram_names
        interested_sections |= frozenset(['.flash.text', '.flash.rodata'])
        result = dict([(t, {}) for t in interested_sections])
        for _, section in iteritems(sections):
            section_name = section['name']
            if section_name not in interested_sections:
                continue
            for s in section['sources']:
                if archive != s['archive']:
                    continue
                s['sym_name'] = re.sub('(.text.|.literal.|.data.|.bss.|.rodata.)', '', s['sym_name'])
                result[section_name][s['sym_name']] = result[section_name].get(s['sym_name'], 0) + s['size']

        # build a new ordered dict of each section, where each entry is an ordereddict of symbols to sizes
        section_symbols = collections.OrderedDict()
        for t in sorted(list(interested_sections)):
            s = sorted(list(result[t].items()), key=lambda k_v: k_v[0])
            # do a secondary sort in order to have consistent order (for diff-ing the output)
            s = sorted(s, key=lambda k_v: k_v[1], reverse=True)
            section_symbols[t] = collections.OrderedDict(s)

        return section_symbols


def get_archive_symbols(mem_reg, sections, archive, as_json=False, sections_diff=None):
    diff_en = sections_diff is not None
    current = StructureForArchiveSymbols.get(mem_reg, archive, sections)
    reference = StructureForArchiveSymbols.get(mem_reg, archive, sections_diff) if diff_en else {}

    if as_json:
        if diff_en:
            diff_json_dic = collections.OrderedDict()
            for name in sorted(list(frozenset(current.keys()) | frozenset(reference.keys()))):
                cur_name_dic = current.get(name, {})
                ref_name_dic = reference.get(name, {})
                all_keys = sorted(list(frozenset(cur_name_dic.keys()) | frozenset(ref_name_dic.keys())))
                diff_json_dic[name] = collections.OrderedDict([(key,
                                                                cur_name_dic.get(key, 0) -
                                                                ref_name_dic.get(key, 0)) for key in all_keys])
            output = format_json(collections.OrderedDict([('current', current),
                                                          ('reference', reference),
                                                          ('diff', diff_json_dic),
                                                          ]))
        else:
            output = format_json(current)
    else:
        def _get_item_pairs(name, section):
            return collections.OrderedDict([(key.replace(name + '.', ''), val) for key, val in iteritems(section)])

        def _get_output(section_symbols):
            output = ''
            for t, s in iteritems(section_symbols):
                output += '{}Symbols from section: {}{}'.format(os.linesep, t, os.linesep)
                item_pairs = _get_item_pairs(t, s)
                output += ' '.join(['{}({})'.format(key, val) for key, val in iteritems(item_pairs)])
                section_total = sum([val for _, val in iteritems(item_pairs)])
                output += '{}Section total: {}{}'.format(os.linesep if section_total > 0 else '',
                                                         section_total,
                                                         os.linesep)
            return output

        output = 'Symbols within the archive: {} (Not all symbols may be reported){}'.format(archive, os.linesep)
        if diff_en:

            def _generate_line_tuple(curr, ref, name):
                cur_val = curr.get(name, 0)
                ref_val = ref.get(name, 0)
                diff_val = cur_val - ref_val
                # string slicing is used just to make sure it will fit into the first column of line_format
                return ((' ' * 4 + name)[:40], cur_val, ref_val, '' if diff_val == 0 else '{:+}'.format(diff_val))

            line_format = '{:40} {:>12} {:>12} {:>25}'
            all_section_names = sorted(list(frozenset(current.keys()) | frozenset(reference.keys())))
            for section_name in all_section_names:
                current_item_pairs = _get_item_pairs(section_name, current.get(section_name, {}))
                reference_item_pairs = _get_item_pairs(section_name, reference.get(section_name, {}))
                output += os.linesep + line_format.format(section_name[:40],
                                                          '<CURRENT>',
                                                          '<REFERENCE>',
                                                          '<CURRENT> - <REFERENCE>') + os.linesep
                current_section_total = sum([val for _, val in iteritems(current_item_pairs)])
                reference_section_total = sum([val for _, val in iteritems(reference_item_pairs)])
                diff_section_total = current_section_total - reference_section_total
                all_item_names = sorted(list(frozenset(current_item_pairs.keys()) |
                                             frozenset(reference_item_pairs.keys())))
                output += os.linesep.join([line_format.format(*_generate_line_tuple(current_item_pairs,
                                                                                    reference_item_pairs,
                                                                                    n)
                                                              ).rstrip() for n in all_item_names])
                output += os.linesep if current_section_total > 0 or reference_section_total > 0 else ''
                output += line_format.format('Section total:',
                                             current_section_total,
                                             reference_section_total,
                                             '' if diff_section_total == 0 else '{:+}'.format(diff_section_total)
                                             ).rstrip() + os.linesep
        else:
            output += _get_output(current)
    return output


if __name__ == '__main__':
    main()
