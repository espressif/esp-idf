#!/usr/bin/env python
#
# esp-idf alternative to "size" to print ELF file sizes, also analyzes
# the linker map file to dump higher resolution details.
#
# Includes information which is not shown in "xtensa-esp32-elf-size",
# or easy to parse from "xtensa-esp32-elf-objdump" or raw map files.
#
# SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
from __future__ import division, print_function, unicode_literals

import argparse
import collections
import json
import os.path
import re
import sys
from typing import Any, Callable, Collection, Dict, Iterable, List, Optional, TextIO, Tuple, Union

import yaml
from future.utils import iteritems

Section = Dict[str, Union[str, int]]
SectionDict = Dict[str, Section]


try:
    basestring
except NameError:
    basestring = str


GLOBAL_JSON_INDENT = 4
GLOBAL_JSON_SEPARATORS = (',', ': ')


class MemRegions(object):
    """
    Regions determined by the chip target.
    """

    # DIRAM is not added here. The DIRAM is indicated by the `secondary_addr` of each MemRegDef
    (DRAM_ID, IRAM_ID, CACHE_D_ID, CACHE_I_ID, RTC_FAST_D_ID, RTC_FAST_I_ID, RTC_SLOW_D_ID) = range(7)

    # The order of variables in the tuple is the same as in the soc_memory_layout.c files
    MemRegDef = collections.namedtuple('MemRegDef', ['primary_addr', 'length', 'type', 'secondary_addr'])

    class Region(object):
        # Helper class to store region information
        def __init__(self, start: int, length: int, region: 'MemRegions.MemRegDef', section: Optional[str]=None) -> None:
            self.start = start
            self.len = length
            self.region = region
            self.section = section

    @staticmethod
    def get_mem_regions(target: str) -> List:
        """
        Get memory regions for specific target
        """
        # The target specific memory structure is deduced from soc_memory_types defined in
        # $IDF_PATH/components/soc/**/soc_memory_layout.c files.

        MemRegDef = MemRegions.MemRegDef

        def change_to_proper_format(length: Union[str, bytes]) -> Any:
            '''
            Change `length` if it is string like `'0x8000 + 6 * 0x10000'` to resolve of this math equation
            or if `length` is number function return it without changing.
            '''
            try:
                return eval(length)
            except TypeError:
                return length

        def get_mem_reg_def(chip_info: Dict, memory_reg: str) -> Tuple:
            chip_info[memory_reg]['secondary_address'] = chip_info[memory_reg].get('secondary_address') or 0
            return MemRegDef(chip_info[memory_reg]['primary_address'], change_to_proper_format(chip_info[memory_reg]['length']),
                             getattr(MemRegions, memory_reg.strip('_12') + '_ID'), chip_info[memory_reg]['secondary_address'])
        try:
            with open(os.path.join(os.path.dirname(__file__), 'idf_size_yaml', target + '_data_info.yaml'), 'r') as stream:
                chip_info = (yaml.safe_load(stream))
        except FileNotFoundError:
            raise RuntimeError('Target not detected.')
        return sorted([get_mem_reg_def(chip_info, item) for item in chip_info])

    def __init__(self, target: str) -> None:
        self.chip_mem_regions = self.get_mem_regions(target)
        if not self.chip_mem_regions:
            raise RuntimeError('Target {} is not implemented in idf_size'.format(target))

    def _get_first_region(self, start: int, length: int) -> Tuple[Union['MemRegions.MemRegDef', None], int]:
        for region in self.chip_mem_regions:  # type: ignore
            if region.primary_addr <= start < region.primary_addr + region.length:
                return (region, length)
            if region.secondary_addr and region.secondary_addr <= start < region.secondary_addr + region.length:
                return (region, length)
        print('WARNING: Given section not found in any memory region.')
        print('Check whether the LD file is compatible with the definitions in get_mem_regions in idf_size.py')
        return (None, length)

    def _get_regions(self, start: int, length: int, name: Optional[str]=None) -> List:
        ret = []
        while length > 0:
            (region, cur_len) = self._get_first_region(start, length)
            if region is None:
                # skip regions that not in given section
                length -= cur_len
                start += cur_len
                continue
            ret.append(MemRegions.Region(start, cur_len, region, name))
            length -= cur_len
            start += cur_len

        return ret

    def fit_segments_into_regions(self, segments: Dict) -> List:
        region_list = []

        for segment in segments.values():
            sorted_segments = self._get_regions(segment['origin'], segment['length'])
            region_list.extend(sorted_segments)

        return region_list

    def fit_sections_into_regions(self, sections: Dict) -> List:
        region_list = []

        for section in sections.values():
            sorted_sections = self._get_regions(section['address'], section['size'], section['name'])
            region_list.extend(sorted_sections)

        return region_list


class LinkingSections(object):

    _section_type_dict = {key: re.compile(value) for key, value in {
        'text': r'.*\.text',
        'data': r'.*\.data',
        'bss': r'.*\.bss',
        'rodata': r'.*\.rodata',
        'noinit': r'.*noinit',
        'vectors': r'.*\.vectors',
        'flash': r'.*flash.*',
    }.items()}

    @staticmethod
    def in_section(section: str, section_name_or_list: Union[str, Iterable]) -> bool:
        """
        Check if section in section_name_or_list
        """
        if isinstance(section_name_or_list, basestring):
            section_name_or_list = [section_name_or_list]

        for section_name in section_name_or_list:
            if LinkingSections._section_type_dict[section_name].match(section):
                return True
        return False

    @staticmethod
    def filter_sections(sections: Dict) -> Dict:
        return {key: v for key, v in sections.items()
                if LinkingSections.in_section(key, LinkingSections._section_type_dict.keys())}

    @staticmethod
    def get_display_name_order(section_name_list: List[str]) -> Tuple[List[str], List[str]]:
        '''
        Return two lists, in the suggested display order.
        First list is the reordered section_name_list, second list is the suggested display name, corresponding to the first list
        '''

        def get_memory_name(split_name: List) -> Tuple[str, str]:
            memory_name = '.{}'.format(split_name[1])
            display_name = section
            for seg_name in ['iram','dram','flash']:
                if seg_name in split_name[1]:
                    memory_name = '.{}'.format(seg_name)
                    seg_name = seg_name.upper() if seg_name != 'flash' else seg_name.capitalize()
                    display_name = ''.join([seg_name,
                                            split_name[1].replace('iram', '') if seg_name == 'IRAM' else '',
                                            ' .{}'.format(split_name[2]) if len(split_name) > 2 else ''])
            return memory_name, display_name

        ordered_name_list = sorted(section_name_list)
        display_name_list = ordered_name_list.copy()

        memory_name = ''
        ordered_name_list = sort_dict(ordered_name_list)
        for i, section in enumerate(ordered_name_list):
            if memory_name and section.startswith(memory_name):
                # If the section has same memory type with the previous one, use shorter name
                display_name_list[i] = section.replace(memory_name, '& ')
                continue

            memory_name = ''

            split_name = section.split('.')
            if len(split_name) > 1:
                # If the section has a memory type, update the type and try to display the type properly
                assert split_name[0] == '', 'Unexpected section name "{}"'.format(section)
                memory_name, display_name_list[i] = get_memory_name(split_name)
                continue

            # Otherwise use its original name
            display_name_list[i] = section

        return ordered_name_list, display_name_list


def scan_to_header(file: Iterable, header_line: str) -> None:
    """ Scan forward in a file until you reach 'header_line', then return """
    for line in file:
        if line.strip() == header_line:
            return
    raise RuntimeError("Didn't find line '%s' in file" % header_line)


def format_json(json_object: Dict) -> str:
    return json.dumps(json_object,
                      allow_nan=True,
                      indent=GLOBAL_JSON_INDENT,
                      separators=GLOBAL_JSON_SEPARATORS) + os.linesep


def load_map_data(map_file: TextIO) -> Tuple[str, Dict, Dict]:
    segments = load_segments(map_file)
    detected_chip = detect_target_chip(map_file)
    sections = load_sections(map_file)

    # Exclude the dummy and .text_end section, which usually means shared region among I/D buses
    for key in list(sections.keys()):
        if key.endswith(('dummy', '.text_end')):
            sections.pop(key)
    return detected_chip, segments, sections


def load_segments(map_file: TextIO) -> Dict:
    """ Memory Configuration section is the total size of each segment """
    result = {}  # type: Dict[Any, Dict]
    scan_to_header(map_file, 'Memory Configuration')
    RE_MEMORY_SECTION = re.compile(r'(?P<name>[^ ]+) +0x(?P<origin>[\da-f]+) +0x(?P<length>[\da-f]+)')

    for line in map_file:
        match_section = RE_MEMORY_SECTION.match(line)
        if match_section is None:
            if len(result) == 0:
                continue  # whitespace or a header, before the content we want
            else:
                return result  # we're at the end of the Memory Configuration
        segment = {
            'name': match_section.group('name'),
            'origin': int(match_section.group('origin'), 16),
            'length': int(match_section.group('length'), 16),
        }
        if segment['name'] != '*default*':
            result[segment['name']] = segment
    raise RuntimeError('End of file while scanning memory configuration?')


def detect_target_chip(map_file: Iterable) -> str:
    ''' Detect target chip based on the target archive name in the linker script part of the MAP file '''
    scan_to_header(map_file, 'Linker script and memory map')

    RE_TARGET = re.compile(r'project_elf_src_(.*)\.c.obj')
    # For back-compatible with make
    RE_TARGET_MAKE = re.compile(r'^LOAD .*?/xtensa-([^-]+)-elf/')

    for line in map_file:
        match_target = RE_TARGET.search(line)
        if match_target:
            return match_target.group(1)

        match_target = RE_TARGET_MAKE.search(line)
        if match_target:
            return match_target.group(1)

        line = line.strip()
        # There could be empty line(s) between the "Linker script and memory map" header and "LOAD lines". Therefore,
        # line stripping and length is checked as well. The "LOAD lines" are between START GROUP and END GROUP for
        # older MAP files.
        if not line.startswith(('LOAD', 'START GROUP', 'END GROUP')) and len(line) > 0:
            # This break is a failsafe to not process anything load_sections() might want to analyze.
            break

    raise RuntimeError('Target not detected')


def load_sections(map_file: TextIO) -> Dict:
    """ Load section size information from the MAP file.

    Returns a dict of 'sections', where each key is a section name and the value
    is a dict with details about this section, including a "sources" key which holds a list of source file line
    information for each symbol linked into the section.

    There are two kinds of lines:
        - symbol_only: [optional space]<sym_name>
        - full line: [optional space][optional sym_name] <address> <size> [optional file_info]
          If <sym_name> doesn't exist, ues the symbol name from the symbol_only line above
          If the line is the starting of a section, the <file> should be empty, otherwise if the line is for a source
          line, the <file> must exist, or the <sym_name> should be is no *fill*. This rule is used to tell sections from
          source lines.
    """

    # Check for lines which only contain the sym name (and rest is on following lines)
    RE_SYMBOL_ONLY_LINE = re.compile(r'^\s*(?P<sym_name>\S*)$')

    # Fast check to see if line is a potential source line before running the slower full regex against it
    RE_PRE_FILTER = re.compile(r'.*0x[\da-f]+\s*0x[\da-f]+.*')

    # source file line, ie
    # 0x0000000040080400       0xa4 /home/gus/esp/32/idf/examples/get-started/hello_world/build/esp32/libesp32.a(cpu_start.o)
    # cmake build system links some object files directly, not part of any archive, so make that part optional
    #  .xtensa.info   0x0000000000000000       0x38 CMakeFiles/hello_world.elf.dir/project_elf_src.c.obj
    #  *fill*         0x00000000400e2967        0x1
    RE_FULL_LINE = re.compile(r'\s*(?P<sym_name>\S*) +0x(?P<address>[\da-f]+) +0x(?P<size>[\da-f]+)\s*(?P<file>.*)$')

    # Extract archive and object_file from the file_info field
    # The object file extention (.obj or .o) is optional including the dot. This is necessary for some third-party
    # libraries. Since the dot is optional and the search gready the parsing of the object name must stop at ). Hence
    # the [^ )] part of the regex.
    RE_FILE = re.compile(r'((?P<archive>[^ ]+\.a)?\(?(?P<object_file>[^ )]+(\.(o|obj))?)\)?)')

    def dump_src_line(src: Dict) -> str:
        return '%s(%s) addr: 0x%08x, size: 0x%x+%d' % (src['sym_name'], src['file'], src['address'], src['size'], src['fill'])

    sections = {}  # type: Dict[Any, Dict]
    section = {}  # type: Dict[str, Any]
    sym_backup = ''
    for line in map_file:
        if line.strip() == 'Cross Reference Table':
            # Stop processing lines because we are at the next section in the map file
            break

        match_line = RE_SYMBOL_ONLY_LINE.match(line)
        if match_line:
            # In some cases the section name appears on the previous line, back it up in here
            sym_backup = match_line.group('sym_name')
            continue

        if not RE_PRE_FILTER.match(line):
            # Line does not match our quick check, so skip to next line
            continue

        match_line = RE_FULL_LINE.match(line)
        if not match_line:
            assert not sym_backup, 'Symbol only line must be followed by a line with address and size'
            continue

        name = match_line.group('sym_name') if match_line.group('sym_name') else sym_backup
        sym_backup = ''

        is_section = not match_line.group('file') and name != '*fill*'
        if is_section:
            # section

            section = {
                'name': name,
                'address': int(match_line.group('address'), 16),
                'size': int(match_line.group('size'), 16),
                'sources': [],
            }
            sections[name] = section

        else:
            # symbol
            if not section:
                continue

            # There are some source lines in rodata section doesn't actually take any space, but have size
            # Make size of those sections zero
            srcs = section['sources']  # type: List[Dict]
            if srcs:
                last_src = srcs[-1]
                if last_src['size'] > 0 and last_src['address'] == int(match_line.group('address'), 16):
                    if '.comment' != section['name'] and '.debug_str' != section['name'] and\
                            'rodata' not in last_src['sym_name']:

                        raise RuntimeError('Due to overlap with following lines, size of the line set to 0:\n    %s' % dump_src_line(last_src))

                    last_src['size'] = 0

            # Count the padding size into the last valid (size > 0) source in the section
            if name == '*fill*':
                for src in reversed(srcs):
                    if src['size'] > 0:
                        src['fill'] += int(match_line.group('size'), 16)
                        break
                continue

            # Extract archive and file information
            match_arch_and_file = RE_FILE.match(match_line.group('file'))
            assert match_arch_and_file, 'Archive and file information not found for "{}"'.format(match_line.group('file'))

            archive = match_arch_and_file.group('archive')
            if archive is None:
                # optional named group "archive" was not matched, so assign a value to it
                archive = '(exe)'

            file = match_arch_and_file.group('object_file')

            assert name
            source = {
                'size': int(match_line.group('size'), 16),
                'address': int(match_line.group('address'), 16),
                'archive': os.path.basename(archive),
                'object_file': os.path.basename(file),
                'sym_name': name,
                'fill': 0,  # padding size ofter the source
            }
            source['file'] = '%s:%s' % (source['archive'], source['object_file'])

            section['sources'].append(source)  # type: ignore

    # Validate the map file
    for section in sections.values():
        src_curr = {}  # type: Dict[str, Any]
        for src in section['sources']:
            if src['size'] == 0:
                continue

            expected_addr = src_curr['address'] + src_curr['size'] + src_curr['fill'] if src_curr else section['sources'][0]['address']
            if src['address'] != expected_addr:
                print('Warning: source line overlap:')
                print('    ' + dump_src_line(src_curr))
                print('    ' + dump_src_line(src))

            src_curr = src

    return sections


def check_target(target: str, map_file: TextIO) -> None:
    if target is None:
        raise RuntimeError('The target chip cannot be detected for {}. '
                           'Please report the issue.'.format(map_file.name))


def main() -> None:
    parser = argparse.ArgumentParser(description='idf_size - a tool to print size information from an IDF MAP file')

    parser.add_argument(
        '--json',
        help='Output results as JSON',
        action='store_true')

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
        help='Print output to the specified file instead of stdout')

    args = parser.parse_args()

    detected_target, segments, sections = load_map_data(args.map_file)
    args.map_file.close()
    check_target(detected_target, args.map_file)

    if args.another_map_file:
        with open(args.another_map_file, 'r') as f:
            detected_target_diff, segments_diff, sections_diff = load_map_data(f)
            check_target(detected_target_diff, f)
            if detected_target_diff != detected_target:
                print('WARNING: The target of the reference and other MAP files is {} and {}, respectively.'
                      ''.format(detected_target, detected_target_diff))
    else:
        segments_diff, sections_diff, detected_target_diff = {}, {}, ''

    if args.target is not None:
        if args.target != detected_target or (detected_target_diff and args.target != detected_target_diff):
            print('WARNING: The detected chip target overwritten to {} by command line argument!'.format(args.target))

        detected_target = args.target
        detected_target_diff = args.target

    output = ''

    if not args.json or not (args.archives or args.files or args.archive_details):
        output += get_summary(args.map_file.name, segments, sections, detected_target,
                              args.json,
                              args.another_map_file, segments_diff, sections_diff, detected_target_diff, not (args.archives or args.files))

    if args.archives:
        output += get_detailed_sizes(sections, 'archive', 'Archive File', args.json, sections_diff)
    if args.files:
        output += get_detailed_sizes(sections, 'file', 'Object File', args.json, sections_diff)

    if args.archive_details:
        output += get_archive_symbols(sections, args.archive_details, args.json, sections_diff)

    args.output_file.write(output)
    args.output_file.close()


class StructureForSummary(object):
    used_dram_data, used_dram_bss, used_dram_rodata, used_dram_other, used_dram, dram_total, dram_remain = (0, ) * 7

    used_dram_ratio = 0.
    used_iram_vectors, used_iram_text, used_iram_other, used_iram, iram_total, iram_remain = (0, ) * 6
    used_iram_ratio = 0.
    used_diram_data, used_diram_bss, used_diram_text, used_diram_vectors, used_diram_rodata, used_diram_other, diram_total, used_diram, diram_remain = (0, ) * 9
    used_diram_ratio = 0.
    used_flash_text, used_flash_rodata, used_flash_other, used_flash, total_size = (0, ) * 5

    def __sub__(self, rhs: 'StructureForSummary') -> 'StructureForSummary':
        assert isinstance(rhs, StructureForSummary)
        ret = self
        for key in StructureForSummary.get_required_items():
            setattr(ret, key, getattr(self, key) - getattr(rhs, key))

        return ret

    def get_dram_overflowed(self) -> bool:
        return self.used_dram_ratio > 1.0

    def get_iram_overflowed(self) -> bool:
        return self.used_iram_ratio > 1.0

    def get_diram_overflowed(self) -> bool:
        return self.used_diram_ratio > 1.0

    @classmethod
    def get_required_items(cls: Any) -> List:
        whole_list = list(filter(lambda x: not (x.startswith('__') or x.endswith('__') or callable(getattr(cls, x))), dir(cls)))
        return whole_list

    @staticmethod
    def get(segments: List, sections: List) -> 'StructureForSummary':

        def get_size(sections: Iterable) -> int:
            return sum([x.len for x in sections])

        def in_diram(x: MemRegions.Region) -> bool:
            return x.region.type in (MemRegions.DRAM_ID, MemRegions.IRAM_ID) and x.region.secondary_addr > 0

        def in_dram(x: MemRegions.Region) -> bool:
            return x.region.type == MemRegions.DRAM_ID and x.region.secondary_addr == 0  # type: ignore

        def in_iram(x: MemRegions.Region) -> bool:
            return x.region.type == MemRegions.IRAM_ID and x.region.secondary_addr == 0  # type: ignore

        r = StructureForSummary()

        diram_filter = filter(in_diram, segments)
        r.diram_total = int(get_size(diram_filter) / 2)

        dram_filter = filter(in_dram, segments)
        r.dram_total = get_size(dram_filter)
        iram_filter = filter(in_iram, segments)
        r.iram_total = get_size(iram_filter)

        def filter_in_section(sections: Iterable[MemRegions.Region], section_to_check: str) -> List[MemRegions.Region]:
            return list(filter(lambda x: LinkingSections.in_section(x.section, section_to_check), sections))  # type: ignore

        dram_sections = list(filter(in_dram, sections))
        iram_sections = list(filter(in_iram, sections))
        diram_sections = list(filter(in_diram, sections))
        flash_sections = filter_in_section(sections, 'flash')

        dram_data_list = filter_in_section(dram_sections, 'data')
        dram_bss_list = filter_in_section(dram_sections, 'bss')
        dram_rodata_list = filter_in_section(dram_sections, 'rodata')
        dram_other_list = [x for x in dram_sections if x not in dram_data_list + dram_bss_list + dram_rodata_list]

        iram_vectors_list = filter_in_section(iram_sections, 'vectors')
        iram_text_list = filter_in_section(iram_sections, 'text')
        iram_other_list = [x for x in iram_sections if x not in iram_vectors_list + iram_text_list]

        diram_vectors_list = filter_in_section(diram_sections, 'vectors')
        diram_data_list = filter_in_section(diram_sections, 'data')
        diram_bss_list = filter_in_section(diram_sections, 'bss')
        diram_text_list = filter_in_section(diram_sections, 'text')
        diram_rodata_list = filter_in_section(diram_sections, 'rodata')
        diram_other_list = [x for x in diram_sections if x not in diram_data_list + diram_bss_list + diram_text_list + diram_vectors_list + diram_rodata_list]

        flash_text_list = filter_in_section(flash_sections, 'text')
        flash_rodata_list = filter_in_section(flash_sections, 'rodata')
        flash_other_list = [x for x in flash_sections if x not in flash_text_list + flash_rodata_list]

        r.used_dram_data = get_size(dram_data_list)
        r.used_dram_bss = get_size(dram_bss_list)
        r.used_dram_rodata = get_size(dram_rodata_list)
        r.used_dram_other = get_size(dram_other_list)
        r.used_dram = r.used_dram_data + r.used_dram_bss + r.used_dram_other + r.used_dram_rodata
        try:
            r.used_dram_ratio = r.used_dram / r.dram_total
        except ZeroDivisionError:
            r.used_dram_ratio = float('nan') if r.used_dram != 0 else 0
        r.dram_remain = r.dram_total - r.used_dram

        r.used_iram_vectors = get_size((iram_vectors_list))
        r.used_iram_text = get_size((iram_text_list))
        r.used_iram_other = get_size((iram_other_list))
        r.used_iram = r.used_iram_vectors + r.used_iram_text + r.used_iram_other
        try:
            r.used_iram_ratio = r.used_iram / r.iram_total
        except ZeroDivisionError:
            r.used_iram_ratio = float('nan') if r.used_iram != 0 else 0
        r.iram_remain = r.iram_total - r.used_iram

        r.used_diram_data = get_size(diram_data_list)
        r.used_diram_bss = get_size(diram_bss_list)
        r.used_diram_text = get_size(diram_text_list)
        r.used_diram_vectors = get_size(diram_vectors_list)
        r.used_diram_rodata = get_size(diram_rodata_list)
        r.used_diram_other = get_size(diram_other_list)
        r.used_diram = r.used_diram_data + r.used_diram_bss + r.used_diram_text + r.used_diram_vectors + r.used_diram_other + r.used_diram_rodata
        try:
            r.used_diram_ratio = r.used_diram / r.diram_total
        except ZeroDivisionError:
            r.used_diram_ratio = float('nan') if r.used_diram != 0 else 0
        r.diram_remain = r.diram_total - r.used_diram

        r.used_flash_text = get_size(flash_text_list)
        r.used_flash_rodata = get_size(flash_rodata_list)

        r.used_flash_other = get_size(flash_other_list)
        r.used_flash = r.used_flash_text + r.used_flash_rodata + r.used_flash_other

        # The used DRAM BSS is counted into the "Used static DRAM" but not into the "Total image size"
        r.total_size = r.used_dram - r.used_dram_bss + r.used_iram + r.used_diram - r.used_diram_bss + r.used_flash
        return r

    def get_json_dic(self) -> collections.OrderedDict:
        ret = collections.OrderedDict([
            ('dram_data', self.used_dram_data),
            ('dram_bss', self.used_dram_bss),
            ('dram_rodata', self.used_dram_rodata),
            ('dram_other', self.used_dram_other),
            ('used_dram', self.used_dram),
            ('dram_total', self.dram_total),
            ('used_dram_ratio', self.used_dram_ratio if self.used_dram_ratio is not float('nan') else 0),
            ('dram_remain', self.dram_remain),

            ('iram_vectors', self.used_iram_vectors),
            ('iram_text', self.used_iram_text),
            ('iram_other', self.used_iram_other),
            ('used_iram', self.used_iram),
            ('iram_total', self.iram_total),
            ('used_iram_ratio', self.used_iram_ratio),
            ('iram_remain', self.iram_remain),

            ('diram_data', self.used_diram_data),
            ('diram_bss', self.used_diram_bss),
            ('diram_text', self.used_diram_text),
            ('diram_vectors', self.used_diram_vectors),
            ('diram_rodata', self.used_diram_rodata),
            ('diram_other', self.used_diram_other),
            ('diram_total', self.diram_total),
            ('used_diram', self.used_diram),
            ('used_diram_ratio', self.used_diram_ratio),
            ('diram_remain', self.diram_remain),

            ('flash_code', self.used_flash_text),
            ('flash_rodata', self.used_flash_rodata),
            ('flash_other', self.used_flash_other),
            ('used_flash_non_ram', self.used_flash),    # text/data in D/I RAM not included

            ('total_size', self.total_size)             # bss not included
        ])
        assert len(ret) == len(StructureForSummary.get_required_items())
        return ret


def get_structure_for_target(segments: Dict, sections: Dict, target: str) -> StructureForSummary:
    """
    Return StructureForSummary for specific target
    """
    mem_regions = MemRegions(target)
    segment_layout = mem_regions.fit_segments_into_regions(segments)
    section_layout = mem_regions.fit_sections_into_regions(LinkingSections.filter_sections(sections))
    current = StructureForSummary.get(segment_layout, section_layout)
    return current


def get_summary(path: str, segments: Dict, sections: Dict, target: str,
                as_json: bool=False,
                path_diff: str='', segments_diff: Optional[Dict]=None, sections_diff: Optional[Dict]=None,
                target_diff: str='', print_suggestions: bool=True) -> str:
    segments_diff = segments_diff or {}
    sections_diff = sections_diff or {}

    current = get_structure_for_target(segments, sections, target)

    if path_diff:
        diff_en = True
        mem_regions_diff = MemRegions(target_diff)
        segment_layout_diff = mem_regions_diff.fit_segments_into_regions(segments_diff)
        section_layout_diff = mem_regions_diff.fit_sections_into_regions(LinkingSections.filter_sections(sections_diff))
        reference = StructureForSummary.get(segment_layout_diff, section_layout_diff)
    else:
        diff_en = False
        reference = StructureForSummary()

    if as_json:
        current_json_dic = current.get_json_dic()
        if diff_en:
            reference_json_dic = reference.get_json_dic()
            diff_json_dic = collections.OrderedDict([
                (k, v - reference_json_dic[k]) for k, v in iteritems(current_json_dic)])
            output = format_json(collections.OrderedDict([('current', current_json_dic),
                                                          ('reference', reference_json_dic),
                                                          ('diff', diff_json_dic),
                                                          ]))
        else:
            output = format_json(current_json_dic)
    else:
        class LineDef(object):
            title = ''
            name = ''

            def __init__(self, title: str, name: str) -> None:
                self.title = title
                self.name = name

            def format_line(self) -> Tuple[str, str, str, str]:
                return (self.title + ': {%s:>7} bytes' % self.name,
                        '{%s:>7}' % self.name,
                        '{%s:+}' % self.name,
                        '')

        class HeadLineDef(LineDef):
            remain = ''
            ratio = ''
            total = ''
            warning_message = ''

            def __init__(self, title: str, name: str, remain: str, ratio: str, total: str, warning_message: str) -> None:
                super(HeadLineDef, self).__init__(title, name)
                self.remain = remain
                self.ratio = ratio
                self.total = total
                self.warning_message = warning_message

            def format_line(self) -> Tuple[str, str, str, str]:
                return ('%s: {%s:>7} bytes ({%s:>7} remain, {%s:.1%%} used)%s' % (self.title, self.name, self.remain, self.ratio, self.warning_message),
                        '{%s:>7}' % self.name,
                        '{%s:+}' % self.name,
                        '({%s:>+7} remain, {%s:>+7} total)' % (self.remain, self.total))

        class TotalLineDef(LineDef):

            def format_line(self) -> Tuple[str, str, str, str]:
                return (self.title + ': {%s:>7} bytes (.bin may be padded larger)' % self.name,
                        '{%s:>7}' % self.name,
                        '{%s:+}' % self.name,
                        '')

        warning_message = ' Overflow detected!' + (' You can run idf.py size-files for more information.' if print_suggestions else '')

        format_list = [
            HeadLineDef('Used static DRAM', 'used_dram', remain='dram_remain', ratio='used_dram_ratio', total='dram_total',
                        warning_message=warning_message if current.get_dram_overflowed() else ''),
            LineDef('      .data size', 'used_dram_data'),
            LineDef('      .bss  size', 'used_dram_bss'),
            LineDef('   .rodata  size', 'used_dram_rodata'),
            LineDef(' DRAM other size', 'used_dram_other'),

            HeadLineDef('Used static IRAM', 'used_iram', remain='iram_remain', ratio='used_iram_ratio', total='iram_total',
                        warning_message=warning_message if current.get_iram_overflowed() else ''),
            LineDef('      .text size', 'used_iram_text'),
            LineDef('   .vectors size', 'used_iram_vectors'),

            HeadLineDef('Used stat D/IRAM', 'used_diram', remain='diram_remain', ratio='used_diram_ratio', total='diram_total',
                        warning_message=warning_message if current.get_diram_overflowed() else ''),
            LineDef('      .data size', 'used_diram_data'),
            LineDef('      .bss  size', 'used_diram_bss'),
            LineDef('      .text size', 'used_diram_text'),
            LineDef('   .vectors size', 'used_diram_vectors'),
            LineDef('    .rodata size', 'used_diram_rodata'),
            LineDef('      other     ', 'used_diram_other'),

            LineDef('Used Flash size ', 'used_flash'),
            LineDef('      .text     ', 'used_flash_text'),
            LineDef('      .rodata   ', 'used_flash_rodata'),

            TotalLineDef('Total image size', 'total_size')
        ]

        def convert_to_fmt_dict(summary: StructureForSummary, suffix: str='') -> Dict:
            required_items = StructureForSummary.get_required_items()
            return dict([(key + suffix, getattr(summary, key)) for key in required_items])

        f_dic1 = convert_to_fmt_dict(current)
        if diff_en:
            f_dic2 = convert_to_fmt_dict(reference)
            f_dic_diff = convert_to_fmt_dict(current - reference)

        lf = '{:60}{:>15}{:>15} {}'  # Width for a, b, c, d columns

        def print_in_columns(a: str, b: Optional[str]='', c: Optional[str]='', d: Optional[str]='') -> str:
            return lf.format(a, b, c, d).rstrip() + os.linesep

        output = ''
        if diff_en:
            output += print_in_columns('<CURRENT> MAP file: ' + path)
            output += print_in_columns('<REFERENCE> MAP file: ' + path_diff)
            output += print_in_columns('Difference is counted as <CURRENT> - <REFERENCE>, ',
                                       'i.e. a positive number means that <CURRENT> is larger.')
            output += print_in_columns('Total sizes of <CURRENT>:', '<REFERENCE>', 'Difference', '')

            for line in format_list:
                if getattr(current, line.name) > 0 or getattr(reference, line.name) > 0 or line.name == 'total_size':
                    main_string_format, reference_format, sign_format, main_diff_format = line.format_line()
                    output += print_in_columns(
                        main_string_format.format(**f_dic1),
                        reference_format.format(**f_dic2),
                        sign_format.format(**f_dic_diff) if not sign_format.format(**f_dic_diff).startswith('+0') else '',
                        main_diff_format.format(**f_dic_diff))
        else:
            output += print_in_columns('Total sizes:')

            for line in format_list:
                if getattr(current, line.name) > 0 or line.name == 'total_size':
                    main_string_format, reference_format, sign_format, main_diff_format = line.format_line()
                    output += print_in_columns(main_string_format.format(**f_dic1))

    return output


def sort_dict(non_sort_list: List) -> List:
    '''
    sort with keeping the order data, bss, other, iram, diram, ram_st_total, flash_text, flash_rodata, flash_total
    '''
    start_of_other = 0
    props_sort = []  # type: List
    props_elem = ['.data', '.bss', 'other', 'iram', 'diram', 'ram_st_total', 'flash.text', 'flash.rodata', 'flash', 'flash_total']
    for i in props_elem:
        for j in non_sort_list:
            if i == 'other':
                # remembering where 'other' will start
                start_of_other = len(props_sort)
            elif i in j and j not in props_sort:
                props_sort.append(j)
    for j in non_sort_list:
        if j not in props_sort:
            # add all item that fit in other in dict
            props_sort.insert(start_of_other, j)
    return props_sort


class StructureForDetailedSizes(object):

    @staticmethod
    def sizes_by_key(sections: SectionDict, key: str, include_padding: Optional[bool]=False) -> Dict[str, Dict[str, int]]:
        """ Takes a dict of sections (from load_sections) and returns
        a dict keyed by 'key' with aggregate output size information.

        Key can be either "archive" (for per-archive data) or "file" (for per-file data) in the result.
        """
        result = {}  # type: Dict[str, Dict[str, int]]
        for _, section in iteritems(sections):
            for s in section['sources']:
                if not s[key] in result:
                    result[s[key]] = {}
                archive = result[s[key]]
                if not section['name'] in archive:
                    archive[section['name']] = 0
                archive[section['name']] += s['size']
                if include_padding:
                    archive[section['name']] += s['fill']
        return result

    @staticmethod
    def get(sections: SectionDict, by_key: str) -> collections.OrderedDict:
        """
        Get the detailed structure before using the filter to remove undesired sections,
        to show entries without desired sections
        """
        sizes = StructureForDetailedSizes.sizes_by_key(sections, by_key)
        for key_name in sizes:
            sizes[key_name] = LinkingSections.filter_sections(sizes[key_name])

        s = []
        for key, section_dict in sizes.items():
            ram_st_total = sum([x[1] for x in section_dict.items() if not LinkingSections.in_section(x[0], 'flash')])
            flash_total = sum([x[1] for x in section_dict.items() if not LinkingSections.in_section(x[0], 'bss')])  # type: int

            section_dict['ram_st_total'] = ram_st_total
            section_dict['flash_total'] = flash_total

            sorted_dict = sorted(section_dict.items(), key=lambda elem: elem[0])

            s.append((key, collections.OrderedDict(sorted_dict)))

        s = sorted(s, key=lambda elem: elem[0])
        # do a secondary sort in order to have consistent order (for diff-ing the output)
        s = sorted(s, key=lambda elem: elem[1]['flash_total'], reverse=True)

        return collections.OrderedDict(s)


def get_detailed_sizes(sections: Dict, key: str, header: str, as_json: bool=False, sections_diff: Dict=None) -> str:

    key_name_set = set()
    current = StructureForDetailedSizes.get(sections, key)
    for section_dict in current.values():
        key_name_set.update(section_dict.keys())

    if sections_diff:
        reference = StructureForDetailedSizes.get(sections_diff, key)
        for section_dict in reference.values():
            key_name_set.update(section_dict.keys())
        diff_en = True
    else:
        diff_en = False

    key_name_list = list(key_name_set)
    ordered_key_list, display_name_list = LinkingSections.get_display_name_order(key_name_list)
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
        def _get_header_format(disp_list: List=display_name_list) -> str:
            len_list = [len(x) for x in disp_list]
            len_list.insert(0, 24)
            return ' '.join(['{:>%d}' % x for x in len_list]) + os.linesep

        def _get_output(data: Dict[str, Dict[str, int]], selection: Collection, key_list: List=ordered_key_list, disp_list: List=display_name_list) -> str:
            header_format = _get_header_format(disp_list)
            output = header_format.format(header, *disp_list)

            for key, data_info in iteritems(data):
                if key not in selection:
                    continue

                try:
                    _, key = key.split(':', 1)
                    # print subheadings for key of format archive:file
                except ValueError:
                    # k remains the same
                    pass

                def get_section_size(section_dict: Dict) -> Callable[[str], int]:
                    return lambda x: section_dict.get(x, 0)

                section_size_list = map(get_section_size(section_dict=data_info), key_list)
                output += header_format.format(key[:24], *(section_size_list))
            return output

        def _get_header_format_diff(disp_list: List=display_name_list, columns: bool=False) -> str:
            if columns:
                len_list = (24, ) + (7, ) * 3 * len(disp_list)
                return '|'.join(['{:>%d}' % x for x in len_list]) + os.linesep

            len_list = (24, ) + (23, ) * len(disp_list)
            return ' '.join(['{:>%d}' % x for x in len_list]) + os.linesep

        def _get_output_diff(curr: Dict, ref: Dict, key_list: List=ordered_key_list, disp_list: List=display_name_list) -> str:
            # First header without Current/Ref/Diff columns
            header_format = _get_header_format_diff(columns=False)
            output = header_format.format(header, *disp_list)

            f_print = ('-' * 23, '') * len(key_list)
            f_print = f_print[0:len(key_list)]
            header_line = header_format.format('', *f_print)

            header_format = _get_header_format_diff(columns=True)
            f_print = ('<C>', '<R>', '<C>-<R>') * len(key_list)

            output += header_format.format('', *f_print)
            output += header_line

            for key, data_info in iteritems(curr):
                try:
                    v2 = ref[key]
                except KeyError:
                    continue

                try:
                    _, key = key.split(':', 1)
                    # print subheadings for key of format archive:file
                except ValueError:
                    # k remains the same
                    pass

                def _get_items(name: str, section_dict: Dict=data_info, section_dict_ref: Dict=v2) -> Tuple[str, str, str]:
                    a = section_dict.get(name, 0)
                    b = section_dict_ref.get(name, 0)
                    diff = a - b
                    # the sign is added here and not in header_format in order to be able to print empty strings
                    return (a or '', b or '', '' if diff == 0 else '{:+}'.format(diff))

                x = []  # type: List[str]
                for section in key_list:
                    x.extend(_get_items(section))

                output += header_format.format(key[:24], *(x))

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
    def get(archive: str, sections: Dict) -> Dict:
        interested_sections = LinkingSections.filter_sections(sections)

        result = dict([(t, {}) for t in interested_sections])  # type: Dict[str, Dict[str, int]]
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
            s = sorted(result[t].items(), key=lambda k_v: str(k_v[0]))
            # do a secondary sort in order to have consistent order (for diff-ing the output)
            s = sorted(s, key=lambda k_v: int(k_v[1]), reverse=True)
            section_symbols[t] = collections.OrderedDict(s)

        return section_symbols


def get_archive_symbols(sections: Dict, archive: str, as_json: bool=False, sections_diff: Dict=None) -> str:
    diff_en = bool(sections_diff)
    current = StructureForArchiveSymbols.get(archive, sections)
    reference = StructureForArchiveSymbols.get(archive, sections_diff) if sections_diff else {}

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
        def _get_item_pairs(name: str, section: collections.OrderedDict) -> collections.OrderedDict:
            return collections.OrderedDict([(key.replace(name + '.', ''), val) for key, val in iteritems(section)])

        def _get_max_len(symbols_dict: Dict) -> Tuple[int, int]:
            # the lists have 0 in them because max() doesn't work with empty lists
            names_max_len = 0
            numbers_max_len = 0
            for t, s in iteritems(symbols_dict):
                numbers_max_len = max([numbers_max_len, *[len(str(x)) for _, x in iteritems(s)]])
                names_max_len = max([names_max_len, *[len(x) for x in _get_item_pairs(t, s)]])

            return names_max_len, numbers_max_len

        def _get_output(section_symbols: Dict) -> str:
            output = ''
            names_max_len, numbers_max_len  = _get_max_len(section_symbols)
            for t, s in iteritems(section_symbols):
                output += '{}Symbols from section: {}{}'.format(os.linesep, t, os.linesep)
                item_pairs = _get_item_pairs(t, s)
                for key, val in iteritems(item_pairs):
                    output += ' '.join([('\t{:<%d} : {:>%d}\n' % (names_max_len,numbers_max_len)).format(key, val)])
                section_total = sum([val for _, val in iteritems(item_pairs)])
                output += 'Section total: {}{}'.format(section_total, os.linesep)
            return output

        output = '{}Symbols within the archive: {} (Not all symbols may be reported){}'.format(os.linesep, archive, os.linesep)
        if diff_en:

            def _generate_line_tuple(curr: collections.OrderedDict, ref: collections.OrderedDict, name: str) -> Tuple[str, int, int, str]:
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
