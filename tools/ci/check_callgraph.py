#!/usr/bin/env python
#
# Based on cally.py (https://github.com/chaudron/cally/), Copyright 2018, Eelco Chaudron
# Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

import argparse
import os
import re
from functools import partial

import elftools
from elftools.elf import elffile

try:
    from typing import BinaryIO, Callable, Dict, Generator, List, Optional, Tuple
except ImportError:
    pass

FUNCTION_REGEX = re.compile(
    r'^;; Function (?P<mangle>.*)\s+\((?P<function>\S+)(,.*)?\).*$'
)
CALL_REGEX = re.compile(r'^.*\(call.*"(?P<target>.*)".*$')
SYMBOL_REF_REGEX = re.compile(r'^.*\(symbol_ref[^()]*\("(?P<target>.*)"\).*$')


class RtlFunction(object):
    def __init__(self, name, rtl_filename, tu_filename):
        self.name = name
        self.rtl_filename = rtl_filename
        self.tu_filename = tu_filename
        self.calls = list()  # type: List[str]
        self.refs = list()  # type: List[str]
        self.sym = None


class SectionAddressRange(object):
    def __init__(self, name, addr, size):  # type: (str, int, int) -> None
        self.name = name
        self.low = addr
        self.high = addr + size

    def __str__(self):
        return '{}: 0x{:08x} - 0x{:08x}'.format(self.name, self.low, self.high)

    def contains_address(self, addr):
        return self.low <= addr < self.high


TARGET_SECTIONS = {
    'esp32': [
        SectionAddressRange('.rom.text', 0x40000000, 0x70000),
        SectionAddressRange('.rom.rodata', 0x3ff96000, 0x9018)
    ],
    'esp32s2': [
        SectionAddressRange('.rom.text', 0x40000000, 0x1bed0),
        SectionAddressRange('.rom.rodata', 0x3ffac600, 0x392c)
    ],
    'esp32s3': [
        SectionAddressRange('.rom.text', 0x40000000, 0x568d0),
        SectionAddressRange('.rom.rodata', 0x3ff071c0, 0x8e30)
    ]
}  # type: Dict[str, List[SectionAddressRange]]


class Symbol(object):
    def __init__(self, name, addr, local, filename, section):  # type: (str, int, bool, Optional[str], Optional[str]) -> None
        self.name = name
        self.addr = addr
        self.local = local
        self.filename = filename
        self.section = section
        self.refers_to = list()  # type: List[Symbol]
        self.referred_from = list()  # type: List[Symbol]

    def __str__(self):
        return '{} @0x{:08x} [{}]{} {}'.format(
            self.name,
            self.addr,
            self.section or 'unknown',
            ' (local)' if self.local else '',
            self.filename
        )


class Reference(object):
    def __init__(self, from_sym, to_sym):  # type: (Symbol, Symbol) -> None
        self.from_sym = from_sym
        self.to_sym = to_sym

    def __str__(self):
        return '{} @0x{:08x} ({}) -> {} @0x{:08x} ({})'.format(
            self.from_sym.name,
            self.from_sym.addr,
            self.from_sym.section,
            self.to_sym.name,
            self.to_sym.addr,
            self.to_sym.section
        )


class ElfInfo(object):
    def __init__(self, elf_file):  # type: (BinaryIO) -> None
        self.elf_file = elf_file
        self.elf_obj = elffile.ELFFile(self.elf_file)
        self.section_ranges = self._load_sections()
        self.symbols = self._load_symbols()

    def _load_symbols(self):  # type: () -> List[Symbol]
        symbols = []
        for s in self.elf_obj.iter_sections():
            if not isinstance(s, elftools.elf.sections.SymbolTableSection):
                continue
            filename = None
            for sym in s.iter_symbols():
                sym_type = sym.entry['st_info']['type']
                if sym_type == 'STT_FILE':
                    filename = sym.name
                if sym_type in ['STT_NOTYPE', 'STT_FUNC', 'STT_OBJECT']:
                    local = sym.entry['st_info']['bind'] == 'STB_LOCAL'
                    addr = sym.entry['st_value']
                    symbols.append(
                        Symbol(
                            sym.name,
                            addr,
                            local,
                            filename if local else None,
                            self.section_for_addr(addr),
                        )
                    )
        return symbols

    def _load_sections(self):  # type: () -> List[SectionAddressRange]
        result = []
        for segment in self.elf_obj.iter_segments():
            if segment['p_type'] == 'PT_LOAD':
                for section in self.elf_obj.iter_sections():
                    if not segment.section_in_segment(section):
                        continue
                    result.append(
                        SectionAddressRange(
                            section.name, section['sh_addr'], section['sh_size']
                        )
                    )

        target = os.environ.get('IDF_TARGET')
        if target in TARGET_SECTIONS:
            result += TARGET_SECTIONS[target]

        return result

    def symbols_by_name(self, name):  # type: (str) -> List[Symbol]
        res = []
        for sym in self.symbols:
            if sym.name == name:
                res.append(sym)
        return res

    def section_for_addr(self, sym_addr):  # type: (int) -> Optional[str]
        for sar in self.section_ranges:
            if sar.contains_address(sym_addr):
                return sar.name
        return None


def load_rtl_file(rtl_filename, tu_filename, functions):  # type: (str, str, List[RtlFunction]) -> None
    last_function = None  # type: Optional[RtlFunction]
    for line in open(rtl_filename):
        # Find function definition
        match = re.match(FUNCTION_REGEX, line)
        if match:
            function_name = match.group('function')
            last_function = RtlFunction(function_name, rtl_filename, tu_filename)
            functions.append(last_function)
            continue

        if last_function:
            # Find direct function calls
            match = re.match(CALL_REGEX, line)
            if match:
                target = match.group('target')
                if target not in last_function.calls:
                    last_function.calls.append(target)
                continue

            # Find symbol references
            match = re.match(SYMBOL_REF_REGEX, line)
            if match:
                target = match.group('target')
                if target not in last_function.refs:
                    last_function.refs.append(target)
                continue


def rtl_filename_matches_sym_filename(rtl_filename, symbol_filename):  # type: (str, str) -> bool
    # Symbol file names (from ELF debug info) are short source file names, without path: "cpu_start.c".
    # RTL file names are paths relative to the build directory, e.g.:
    # "build/esp-idf/esp_system/CMakeFiles/__idf_esp_system.dir/port/cpu_start.c.234r.expand"
    #
    # The check below may give a false positive if there are two files with the same name in
    # different directories. This doesn't seem to happen in IDF now, but if it does happen,
    # an assert in find_symbol_by_rtl_func should catch this.
    #
    # If this becomes and issue, consider also loading the .map file and using it to figure out
    # which object file was used as the source of each symbol. Names of the object files and RTL files
    # should be much easier to match.
    return os.path.basename(rtl_filename).startswith(symbol_filename)


class SymbolNotFound(RuntimeError):
    pass


def find_symbol_by_name(name, elfinfo, local_func_matcher):  # type: (str, ElfInfo, Callable[[Symbol], bool]) -> Optional[Symbol]
    """
    Find an ELF symbol for the given name.
    local_func_matcher is a callback function which checks is the candidate local symbol is suitable.
    """
    syms = elfinfo.symbols_by_name(name)
    if not syms:
        return None
    if len(syms) == 1:
        return syms[0]
    else:
        # There are multiple symbols with a given name. Find the best fit.
        local_candidate = None
        global_candidate = None
        for sym in syms:
            if not sym.local:
                assert not global_candidate  # can't have two global symbols with the same name
                global_candidate = sym
            elif local_func_matcher(sym):
                assert not local_candidate  # can't have two symbols with the same name in a single file
                local_candidate = sym

        # If two symbols with the same name are defined, a global and a local one,
        # prefer the local symbol as the reference target.
        return local_candidate or global_candidate


def match_local_source_func(rtl_filename, sym):  # type: (str, Symbol) -> bool
    """
    Helper for match_rtl_funcs_to_symbols, checks if local symbol sym is a good candidate for the
    reference source (caller), based on the RTL file name.
    """
    assert sym.filename  # should be set for local functions
    return rtl_filename_matches_sym_filename(rtl_filename, sym.filename)


def match_local_target_func(rtl_filename, sym_from, sym):  # type: (str, Symbol, Symbol) -> bool
    """
    Helper for match_rtl_funcs_to_symbols, checks if local symbol sym is a good candidate for the
    reference target (callee or referenced data), based on RTL filename of the source symbol
    and the source symbol itself.
    """
    assert sym.filename  # should be set for local functions
    if sym_from.local:
        # local symbol referencing another local symbol
        return sym_from.filename == sym.filename
    else:
        # global symbol referencing a local symbol;
        # source filename is not known, use RTL filename as a hint
        return rtl_filename_matches_sym_filename(rtl_filename, sym.filename)


def match_rtl_funcs_to_symbols(rtl_functions, elfinfo):  # type: (List[RtlFunction], ElfInfo) -> Tuple[List[Symbol], List[Reference]]
    symbols = []  # type: List[Symbol]
    refs = []  # type: List[Reference]

    # General idea:
    # - iterate over RTL functions.
    #   - for each RTL function, find the corresponding symbol
    #   - iterate over the functions and variables referenced from this RTL function
    #     - find symbols corresponding to the references
    #     - record every pair (sym_from, sym_to) as a Reference object

    for source_rtl_func in rtl_functions:
        maybe_sym_from = find_symbol_by_name(source_rtl_func.name, elfinfo, partial(match_local_source_func, source_rtl_func.rtl_filename))
        if maybe_sym_from is None:
            # RTL references a symbol, but the symbol is not defined in the generated object file.
            # This means that the symbol was likely removed (or not included) at link time.
            # There is nothing we can do to check section placement in this case.
            continue
        sym_from = maybe_sym_from

        if sym_from not in symbols:
            symbols.append(sym_from)

        for target_rtl_func_name in source_rtl_func.calls + source_rtl_func.refs:
            if '*.LC' in target_rtl_func_name:  # skip local labels
                continue

            maybe_sym_to = find_symbol_by_name(target_rtl_func_name, elfinfo, partial(match_local_target_func, source_rtl_func.rtl_filename, sym_from))
            if not maybe_sym_to:
                # This may happen for a extern reference in the RTL file, if the reference was later removed
                # by one of the optimization passes, and the external definition got garbage-collected.
                # TODO: consider adding some sanity check that we are here not because of some bug in
                # find_symbol_by_name?..
                continue
            sym_to = maybe_sym_to

            sym_from.refers_to.append(sym_to)
            sym_to.referred_from.append(sym_from)
            refs.append(Reference(sym_from, sym_to))
            if sym_to not in symbols:
                symbols.append(sym_to)

    return symbols, refs


def get_symbols_and_refs(rtl_list, elf_file):  # type: (List[str], BinaryIO) -> Tuple[List[Symbol], List[Reference]]
    elfinfo = ElfInfo(elf_file)

    rtl_functions = []  # type: List[RtlFunction]
    for file_name in rtl_list:
        load_rtl_file(file_name, file_name, rtl_functions)

    return match_rtl_funcs_to_symbols(rtl_functions, elfinfo)


def list_refs_from_to_sections(refs, from_sections, to_sections):  # type: (List[Reference], List[str], List[str]) -> int
    found = 0
    for ref in refs:
        if (not from_sections or ref.from_sym.section in from_sections) and \
           (not to_sections or ref.to_sym.section in to_sections):
            print(str(ref))
            found += 1
    return found


def find_files_recursive(root_path, ext):  # type: (str, str) -> Generator[str, None, None]
    for root, _, files in os.walk(root_path):
        for basename in files:
            if basename.endswith(ext):
                filename = os.path.join(root, basename)
                yield filename


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--rtl-list',
        help='File with the list of RTL files',
        type=argparse.FileType('r'),
    )
    parser.add_argument(
        '--rtl-dir', help='Directory where to look for RTL files, recursively'
    )
    parser.add_argument(
        '--elf-file',
        required=True,
        help='Program ELF file',
        type=argparse.FileType('rb'),
    )
    action_sub = parser.add_subparsers(dest='action')
    find_refs_parser = action_sub.add_parser(
        'find-refs',
        help='List the references coming from a given list of source sections'
             'to a given list of target sections.',
    )
    find_refs_parser.add_argument(
        '--from-sections', help='comma-separated list of source sections'
    )
    find_refs_parser.add_argument(
        '--to-sections', help='comma-separated list of target sections'
    )
    find_refs_parser.add_argument(
        '--exit-code',
        action='store_true',
        help='If set, exits with non-zero code when any references found',
    )
    action_sub.add_parser(
        'all-refs',
        help='Print the list of all references',
    )

    parser.parse_args()
    args = parser.parse_args()
    if args.rtl_list:
        with open(args.rtl_list, 'r') as rtl_list_file:
            rtl_list = [line.strip for line in rtl_list_file]
    else:
        if not args.rtl_dir:
            raise RuntimeError('Either --rtl-list or --rtl-dir must be specified')
        rtl_list = list(find_files_recursive(args.rtl_dir, '.expand'))

    if not rtl_list:
        raise RuntimeError('No RTL files specified')

    _, refs = get_symbols_and_refs(rtl_list, args.elf_file)

    if args.action == 'find-refs':
        from_sections = args.from_sections.split(',') if args.from_sections else []
        to_sections = args.to_sections.split(',') if args.to_sections else []
        found = list_refs_from_to_sections(
            refs, from_sections, to_sections
        )
        if args.exit_code and found:
            raise SystemExit(1)
    elif args.action == 'all-refs':
        for r in refs:
            print(str(r))


if __name__ == '__main__':
    main()
