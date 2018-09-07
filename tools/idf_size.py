#!/usr/bin/env python
#
# esp-idf alternative to "size" to print ELF file sizes, also analyzes
# the linker map file to dump higher resolution details.
#
# Includes information which is not shown in "xtensa-esp32-elf-size",
# or easy to parse from "xtensa-esp32-elf-objdump" or raw map files.
#
# Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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
from builtins import dict
import argparse, sys, subprocess, re
import os.path
import pprint
import operator

DEFAULT_TOOLCHAIN_PREFIX = "xtensa-esp32-elf-"

CHIP_SIZES = {
    "esp32" : {
        "total_iram" : 0x20000,
        "total_irom" : 0x330000,
        "total_drom" : 0x800000,
        # total dram is determined from objdump output
    }
}

def scan_to_header(f, header_line):
    """ Scan forward in a file until you reach 'header_line', then return """
    for line in f:
        if line.strip() == header_line:
            return
    raise RuntimeError("Didn't find line '%s' in file" % header_line)

def load_map_data(map_file):
    memory_config = load_memory_config(map_file)
    sections  = load_sections(map_file)
    return memory_config, sections

def load_memory_config(map_file):
    """ Memory Configuration section is the total size of each output section """
    result = {}
    scan_to_header(map_file, "Memory Configuration")
    RE_MEMORY_SECTION = r"(?P<name>[^ ]+) +0x(?P<origin>[\da-f]+) +0x(?P<length>[\da-f]+)"
    for line in map_file:
        m = re.match(RE_MEMORY_SECTION, line)
        if m is None:
            if len(result) == 0:
                continue  # whitespace or a header, before the content we want
            else:
                return result  # we're at the end of the Memory Configuration
        section = {
            "name" : m.group("name"),
            "origin" : int(m.group("origin"), 16),
            "length" : int(m.group("length"), 16),
        }
        if section["name"] != "*default*":
            result[section["name"]] = section
    raise RuntimeError("End of file while scanning memory configuration?")

def load_sections(map_file):
    """ Load section size information from the MAP file.

    Returns a dict of 'sections', where each key is a section name and the value
    is a dict with details about this section, including a "sources" key which holds a list of source file line information for each symbol linked into the section.
    """
    scan_to_header(map_file, "Linker script and memory map")
    sections = {}
    section = None
    sym_backup = None
    for line in map_file:
        # output section header, ie '.iram0.text     0x0000000040080400    0x129a5'
        RE_SECTION_HEADER = r"(?P<name>[^ ]+) +0x(?P<address>[\da-f]+) +0x(?P<size>[\da-f]+)$"
        m = re.match(RE_SECTION_HEADER, line)
        if m is not None:  # start of a new section
            section = {
                "name" : m.group("name"),
                "address" : int(m.group("address"), 16),
                "size" : int(m.group("size"), 16),
                "sources" : [],
            }
            sections[section["name"]] = section
            continue

        # source file line, ie
        # 0x0000000040080400       0xa4 /home/gus/esp/32/idf/examples/get-started/hello_world/build/esp32/libesp32.a(cpu_start.o)
        RE_SOURCE_LINE = r"\s*(?P<sym_name>\S*).* +0x(?P<address>[\da-f]+) +0x(?P<size>[\da-f]+) (?P<archive>.+\.a)\((?P<object_file>.+\.ob?j?)\)"

        m = re.match(RE_SOURCE_LINE, line, re.M)
        if not m:
            # cmake build system links some object files directly, not part of any archive
            RE_SOURCE_LINE = r"\s*(?P<sym_name>\S*).* +0x(?P<address>[\da-f]+) +0x(?P<size>[\da-f]+) (?P<object_file>.+\.ob?j?)"
            m = re.match(RE_SOURCE_LINE, line)
        if section is not None and m is not None:  # input source file details=ma,e
            sym_name = m.group("sym_name") if len(m.group("sym_name")) > 0 else sym_backup
            try:
                archive = m.group("archive")
            except IndexError:
                archive = "(exe)"

            source = {
                "size" : int(m.group("size"), 16),
                "address" : int(m.group("address"), 16),
                "archive" : os.path.basename(archive),
                "object_file" : os.path.basename(m.group("object_file")),
                "sym_name" : sym_name,
            }
            source["file"] = "%s:%s" % (source["archive"], source["object_file"])
            section["sources"] += [ source ]

        # In some cases the section name appears on the previous line, back it up in here
        RE_SYMBOL_ONLY_LINE = r"^ (?P<sym_name>\S*)$"
        m = re.match(RE_SYMBOL_ONLY_LINE, line)
        if section is not None and m is not None:
            sym_backup = m.group("sym_name")

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
    parser = argparse.ArgumentParser("idf_size - a tool to print IDF elf file sizes")

    parser.add_argument(
        '--toolchain-prefix',
        help="Triplet prefix to add before objdump executable",
        default=DEFAULT_TOOLCHAIN_PREFIX)

    parser.add_argument(
        'map_file', help='MAP file produced by linker',
        type=argparse.FileType('r'))

    parser.add_argument(
        '--archives', help='Print per-archive sizes', action='store_true')

    parser.add_argument(
        '--archive_details', help='Print detailed symbols per archive')

    parser.add_argument(
        '--files', help='Print per-file sizes', action='store_true')

    args = parser.parse_args()

    memory_config, sections = load_map_data(args.map_file)
    print_summary(memory_config, sections)

    if args.archives:
        print("Per-archive contributions to ELF file:")
        print_detailed_sizes(sections, "archive", "Archive File")
    if args.files:
        print("Per-file contributions to ELF file:")
        print_detailed_sizes(sections, "file", "Object File")
    if args.archive_details:
        print("Symbols within the archive:", args.archive_details, "(Not all symbols may be reported)")
        print_archive_symbols(sections, args.archive_details)

def print_summary(memory_config, sections):
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
    used_iram = sum( get_size(s) for s in sections if s.startswith(".iram0") )
    flash_code = get_size(".flash.text")
    flash_rodata = get_size(".flash.rodata")
    total_size = used_data + used_iram + flash_code + flash_rodata

    print("Total sizes:")
    print(" DRAM .data size: %7d bytes" % used_data)
    print(" DRAM .bss  size: %7d bytes" % used_bss)
    print("Used static DRAM: %7d bytes (%7d available, %.1f%% used)" %
          (used_dram, total_dram - used_dram,
           100.0 * used_dram / total_dram))
    print("Used static IRAM: %7d bytes (%7d available, %.1f%% used)" %
          (used_iram, total_iram - used_iram,
           100.0 * used_iram / total_iram))
    print("      Flash code: %7d bytes" % flash_code)
    print("    Flash rodata: %7d bytes" % flash_rodata)
    print("Total image size:~%7d bytes (.bin may be padded larger)" % (total_size))

def print_detailed_sizes(sections, key, header):
    sizes = sizes_by_key(sections, key)

    sub_heading = None
    headings = (header,
                "DRAM .data",
                "& .bss",
                "IRAM",
                "Flash code",
                "& rodata",
                "Total")
    print("%24s %10s %6s %6s %10s %8s %7s" % headings)
    result = {}
    for k in sizes:
        v = sizes[k]
        result[k] = {}
        result[k]["data"] = v.get(".dram0.data", 0)
        result[k]["bss"] = v.get(".dram0.bss", 0)
        result[k]["iram"] = sum(t for (s,t) in v.items() if s.startswith(".iram0"))
        result[k]["flash_text"] = v.get(".flash.text", 0)
        result[k]["flash_rodata"] = v.get(".flash.rodata", 0)
        result[k]["total"] = sum(result[k].values())

    def return_total_size(elem):
        val = elem[1]
        return val["total"]
    def return_header(elem):
        return elem[0]
    s = sorted(list(result.items()), key=return_header)
    # do a secondary sort in order to have consistent order (for diff-ing the output)
    for k,v in sorted(s, key=return_total_size, reverse=True):
        if ":" in k:  # print subheadings for key of format archive:file
            sh,k = k.split(":")
        print("%24s %10d %6d %6d %10d %8d %7d" % (k[:24],
                                                  v["data"],
                                                  v["bss"],
                                                  v["iram"],
                                                  v["flash_text"],
                                                  v["flash_rodata"],
                                                  v["total"]))

def print_archive_symbols(sections, archive):
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
            s["sym_name"] = re.sub("(.text.|.literal.|.data.|.bss.|.rodata.)", "", s["sym_name"]);
            result[section_name][s["sym_name"]] = result[section_name].get(s["sym_name"], 0) + s["size"]
    for t in interested_sections:
        print("\nSymbols from section:", t)
        section_total = 0
        s = sorted(list(result[t].items()), key=lambda k_v: k_v[0])
        # do a secondary sort in order to have consistent order (for diff-ing the output)
        for key,val in sorted(s, key=lambda k_v: k_v[1], reverse=True):
            print(("%s(%d)"% (key.replace(t + ".", ""), val)), end=' ')
            section_total += val
        print("\nSection total:",section_total)

if __name__ == "__main__":
    main()

