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

DEFAULT_TOOLCHAIN_PREFIX = "xtensa-esp32-elf-"
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
                # TODO comment this
                MemRegDef(0x3FFAE000, 17 * 0x2000 + 2 * 0x8000, MemRegions.DRAM_ID, 0),
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
                MemRegDef(0x3FFE0000, 4 * 0x4000 + 2 * 0x8000, MemRegions.DIRAM_ID, 0x400BC000),
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
                # The following one memory region is defined instead of defining all 3 + 11 individually because their
                # type (MemRegions.DIRAM_ID) is the same
                MemRegDef(0x3FFB2000, 3 * 0x2000 + 11 * 0x4000, MemRegions.DIRAM_ID, 0x40022000),
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
                # 2nd stage bootloader iram_loader_seg starts at block 15. Therefore, the following blocks are not
                # defined here and will not be counted in the total amount of available space
                # MemRegDef(0x3FFE4000, 0x4000, MemRegions.DIRAM_ID, 0x40054000),
                # MemRegDef(0x3FFE8000, 0x4000, MemRegions.DIRAM_ID, 0x40058000),
                # MemRegDef(0x3FFEC000, 0x4000, MemRegions.DIRAM_ID, 0x4005C000),
                # MemRegDef(0x3FFF0000, 0x4000, MemRegions.DIRAM_ID, 0x40060000),
                # MemRegDef(0x3FFF4000, 0x4000, MemRegions.DIRAM_ID, 0x40064000),
                # MemRegDef(0x3FFF8000, 0x4000, MemRegions.DIRAM_ID, 0x40068000),
                # MemRegDef(0x3FFFC000, 0x4000, MemRegions.DIRAM_ID, 0x4006C000),
                # Note the type of the last block which is in contrast with soc_memory_layout.c. It is used for
                # startup stack therefore the type is D/IRAM (from the perspective of idf_size) and not DRAM.
            ])
        else:
            return None

    def __init__(self, target):
        self.chip_mem_regions = self.get_mem_regions(target)
        if not self.chip_mem_regions:
            raise RuntimeError('Target {} is not implemented in idf_size'.format(target))

    def _address_in_range(address, length, reg_address, reg_length):
        return address >= reg_address and (address - reg_address) <= (reg_length - length)

    def get_names(self, dictionary, region_id):
        result = []
        # TODO alebo origin a length
        for m in self.chip_mem_regions:
            result.append([n for (n, c) in iteritems(dictionary) if (self._address_in_range(c.address, c.size,
                                                                     m.primary_addr, m.length) or
                                                                     (m.type == self.DIRAM_ID and
                                                                      self._address_in_range(c.address,
                                                                                             c.size,
                                                                                             m.secondary_addr,
                                                                                             m.length)))])
        return result


def scan_to_header(f, header_line):
    """ Scan forward in a file until you reach 'header_line', then return """
    for line in f:
        if line.strip() == header_line:
            return
    raise RuntimeError("Didn't find line '%s' in file" % header_line)


def format_json(json_object):
    return json.dumps(json_object, indent=GLOBAL_JSON_INDENT, separators=GLOBAL_JSON_SEPARATORS) + "\n"


def load_map_data(map_file):
    memory_config = load_memory_config(map_file)
    sections  = load_sections(map_file)
    return memory_config, sections


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


def load_sections(map_file):
    """ Load section size information from the MAP file.

    Returns a dict of 'sections', where each key is a section name and the value
    is a dict with details about this section, including a "sources" key which holds a list of source file line
    information for each symbol linked into the section.
    """
    scan_to_header(map_file, "Linker script and memory map")

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


def sizes_by_key(sections, key):
    """ Takes a dict of sections (from load_sections) and returns
    a dict keyed by 'key' with aggregate output size information.

    Key can be either "archive" (for per-archive data) or "file" (for per-file data) in the result.
    """
    result = {}
    for section in sections.values():
        for s in section["sources"]:
            if not s[key] in result:
                result[s[key]] = {}
            archive = result[s[key]]
            if not section["name"] in archive:
                archive[section["name"]] = 0
            archive[section["name"]] += s["size"]
    return result


def main():
    parser = argparse.ArgumentParser(description="idf_size - a tool to print size information from an IDF MAP file")

    parser.add_argument(
        # FIXME: toolchain is not used
        '--toolchain-prefix',
        help="Triplet prefix to add before objdump executable",
        default=DEFAULT_TOOLCHAIN_PREFIX)

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
        '--target', help='Set target chip', default='esp32')

    parser.add_argument(
        '-o',
        '--output-file',
        type=argparse.FileType('w'),
        default=sys.stdout,
        help="Print output to the specified file instead of stdout")

    args = parser.parse_args()

    mem_regions = MemRegions(args.target)

    output = ""

    memory_config, sections = load_map_data(args.map_file)
    if not args.json or not (args.archives or args.files or args.archive_details):
        output += get_summary(mem_regions, memory_config, sections, args.json)

    if args.archives:
        output += get_detailed_sizes(mem_regions, sections, "archive", "Archive File", args.json)
    if args.files:
        output += get_detailed_sizes(mem_regions, sections, "file", "Object File", args.json)

    if args.archive_details:
        output += get_archive_symbols(mem_regions, sections, args.archive_details, args.json)

    args.output_file.write(output)


def get_summary(mem_regions, memory_config, sections, as_json=False):
    def get_size(section):
        try:
            return sections[section]["size"]
        except KeyError:
            return 0

    # if linker script changes, these need to change
    total_iram = memory_config["iram0_0_seg"]["length"]
    total_dram = memory_config["dram0_0_seg"]["length"]
    used_data = get_size(".dram0.data")
    used_bss = get_size(".dram0.bss")
    used_dram = used_data + used_bss
    try:
        used_dram_ratio = used_dram / total_dram
    except ZeroDivisionError:
        used_dram_ratio = float('nan')
    used_iram = sum(get_size(s) for s in sections if s.startswith(".iram0"))
    try:
        used_iram_ratio = used_iram / total_iram
    except ZeroDivisionError:
        used_iram_ratio = float('nan')
    flash_code = get_size(".flash.text")
    flash_rodata = get_size(".flash.rodata")
    total_size = used_data + used_iram + flash_code + flash_rodata

    output = ""
    if as_json:
        output = format_json(collections.OrderedDict([
            ("dram_data", used_data),
            ("dram_bss", used_bss),
            ("used_dram", used_dram),
            ("available_dram", total_dram - used_dram),
            ("used_dram_ratio", used_dram_ratio),
            ("used_iram", used_iram),
            ("available_iram", total_iram - used_iram),
            ("used_iram_ratio", used_iram_ratio),
            ("flash_code", flash_code),
            ("flash_rodata", flash_rodata),
            ("total_size", total_size)
        ]))
    else:
        output += "Total sizes:\n"
        output += " DRAM .data size: {:>7} bytes\n".format(used_data)
        output += " DRAM .bss  size: {:>7} bytes\n".format(used_bss)
        output += "Used static DRAM: {:>7} bytes ({:>7} available, {:.1%} used)\n".format(
            used_dram, total_dram - used_dram, used_dram_ratio)
        output += "Used static IRAM: {:>7} bytes ({:>7} available, {:.1%} used)\n".format(
            used_iram, total_iram - used_iram, used_iram_ratio)
        output += "      Flash code: {:>7} bytes\n".format(flash_code)
        output += "    Flash rodata: {:>7} bytes\n".format(flash_rodata)
        output += "Total image size:~{:>7} bytes (.bin may be padded larger)\n".format(total_size)

    return output


def get_detailed_sizes(mem_regions, sections, key, header, as_json=False):
    sizes = sizes_by_key(sections, key)

    result = {}
    for k in sizes:
        v = sizes[k]
        r = collections.OrderedDict()
        r["data"] = v.get(".dram0.data", 0)
        r["bss"] = v.get(".dram0.bss", 0)
        r["iram"] = sum(t for (s,t) in v.items() if s.startswith(".iram0"))
        r["flash_text"] = v.get(".flash.text", 0)
        r["flash_rodata"] = v.get(".flash.rodata", 0)
        r["total"] = sum(r.values())
        result[k] = r

    def return_total_size(elem):
        val = elem[1]
        return val["total"]

    def return_header(elem):
        return elem[0]
    s = sorted(list(result.items()), key=return_header)

    # do a secondary sort in order to have consistent order (for diff-ing the output)
    s = sorted(s, key=return_total_size, reverse=True)

    output = ""

    if as_json:
        output = format_json(collections.OrderedDict(s))
    else:
        header_format = "{:>24} {:>10} {:>6} {:>6} {:>10} {:>8} {:>7}\n"

        output += "Per-{} contributions to ELF file:\n".format(key)
        output += header_format.format(header,
                                       "DRAM .data",
                                       "& .bss",
                                       "IRAM",
                                       "Flash code",
                                       "& rodata",
                                       "Total")

        for k,v in s:
            if ":" in k:  # print subheadings for key of format archive:file
                sh,k = k.split(":")
            output += header_format.format(k[:24],
                                           v["data"],
                                           v["bss"],
                                           v["iram"],
                                           v["flash_text"],
                                           v["flash_rodata"],
                                           v["total"])

    return output


def get_archive_symbols(mem_regions, sections, archive, as_json=False):
    interested_sections = [".dram0.data", ".dram0.bss", ".iram0.text", ".iram0.vectors", ".flash.text", ".flash.rodata"]
    result = {}
    for t in interested_sections:
        result[t] = {}
    for section in sections.values():
        section_name = section["name"]
        if section_name not in interested_sections:
            continue
        for s in section["sources"]:
            if archive != s["archive"]:
                continue
            s["sym_name"] = re.sub("(.text.|.literal.|.data.|.bss.|.rodata.)", "", s["sym_name"])
            result[section_name][s["sym_name"]] = result[section_name].get(s["sym_name"], 0) + s["size"]

    # build a new ordered dict of each section, where each entry is an ordereddict of symbols to sizes
    section_symbols = collections.OrderedDict()
    for t in interested_sections:
        s = sorted(list(result[t].items()), key=lambda k_v: k_v[0])
        # do a secondary sort in order to have consistent order (for diff-ing the output)
        s = sorted(s, key=lambda k_v: k_v[1], reverse=True)
        section_symbols[t] = collections.OrderedDict(s)

    output = ""
    if as_json:
        output = format_json(section_symbols)
    else:
        output += "Symbols within the archive: {} (Not all symbols may be reported)\n".format(archive)
        for t,s in section_symbols.items():
            section_total = 0
            output += "\nSymbols from section: {}\n".format(t)
            for key, val in s.items():
                output += "{}({}) ".format(key.replace(t + ".", ""), val)
                section_total += val
            output += "\nSection total: {}\n".format(section_total)

    return output


if __name__ == "__main__":
    main()
