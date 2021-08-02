#
# Copyright 2021 Espressif Systems (Shanghai) CO., LTD
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

import hashlib
import os

from construct import (AlignedStruct, Bytes, Const, Container, GreedyRange, Int16ul, Int32ul, Padding, Pointer,
                       Sequence, Struct, this)

try:
    from typing import Optional
except ImportError:
    pass

# Following structs are based on spec
# https://refspecs.linuxfoundation.org/elf/elf.pdf
# and source code
# IDF_PATH/components/espcoredump/include_core_dump/elf.h

ElfIdentification = Struct(
    'EI_MAG' / Const(b'\x7fELF'),
    'EI_CLASS' / Const(b'\x01'),  # ELFCLASS32
    'EI_DATA' / Const(b'\x01'),  # ELFDATA2LSB
    'EI_VERSION' / Const(b'\x01'),  # EV_CURRENT
    Padding(9),
)

ElfHeader = Struct(
    'e_ident' / ElfIdentification,
    'e_type' / Int16ul,
    'e_machine' / Int16ul,
    'e_version' / Int32ul,
    'e_entry' / Int32ul,
    'e_phoff' / Int32ul,
    'e_shoff' / Int32ul,
    'e_flags' / Int32ul,
    'e_ehsize' / Int16ul,
    'e_phentsize' / Int16ul,
    'e_phnum' / Int16ul,
    'e_shentsize' / Int16ul,
    'e_shnum' / Int16ul,
    'e_shstrndx' / Int16ul,
)

SectionHeader = Struct(
    'sh_name' / Int32ul,
    'sh_type' / Int32ul,
    'sh_flags' / Int32ul,
    'sh_addr' / Int32ul,
    'sh_offset' / Int32ul,
    'sh_size' / Int32ul,
    'sh_link' / Int32ul,
    'sh_info' / Int32ul,
    'sh_addralign' / Int32ul,
    'sh_entsize' / Int32ul,
)

ProgramHeader = Struct(
    'p_type' / Int32ul,
    'p_offset' / Int32ul,
    'p_vaddr' / Int32ul,
    'p_paddr' / Int32ul,
    'p_filesz' / Int32ul,
    'p_memsz' / Int32ul,
    'p_flags' / Int32ul,
    'p_align' / Int32ul,
)

ElfHeaderTables = Struct(
    'elf_header' / ElfHeader,
    'program_headers' / Pointer(this.elf_header.e_phoff, ProgramHeader[this.elf_header.e_phnum]),
    'section_headers' / Pointer(this.elf_header.e_shoff, SectionHeader[this.elf_header.e_shnum]),
)

NoteSection = AlignedStruct(
    4,
    'namesz' / Int32ul,
    'descsz' / Int32ul,
    'type' / Int32ul,
    'name' / Bytes(this.namesz),
    'desc' / Bytes(this.descsz),
)

NoteSections = GreedyRange(NoteSection)


class ElfFile(object):
    """
    Elf class to a single elf file
    """

    SHN_UNDEF = 0x00
    SHT_PROGBITS = 0x01
    SHT_STRTAB = 0x03
    SHT_NOBITS = 0x08

    PT_LOAD = 0x01
    PT_NOTE = 0x04

    ET_CORE = 0x04

    EV_CURRENT = 0x01

    def __init__(self, elf_path=None, e_type=None, e_machine=None):
        # type: (Optional[str], Optional[int], Optional[int]) -> None
        self.e_type = e_type
        self.e_machine = e_machine

        self._struct = None  # type: Optional[Struct]
        self._model = None  # type: Optional[Container]

        self.sections = []  # type: list[ElfSection]
        self.load_segments = []  # type: list[ElfSegment]
        self.note_segments = []  # type: list[ElfNoteSegment]

        if elf_path and os.path.isfile(elf_path):
            self.read_elf(elf_path)

    def read_elf(self, elf_path):  # type: (str) -> None
        """
        Read elf file, also write to ``self.model``, ``self.program_headers``,
        ``self.section_headers``
        :param elf_path: elf file path
        :return: None
        """
        with open(elf_path, 'rb') as fr:
            elf_bytes = fr.read()
        header_tables = ElfHeaderTables.parse(elf_bytes)
        self.e_type = header_tables.elf_header.e_type
        self.e_machine = header_tables.elf_header.e_machine

        self._struct = self._generate_struct_from_headers(header_tables)
        self._model = self._struct.parse(elf_bytes)

        self.load_segments = [ElfSegment(seg.ph.p_vaddr,
                                         seg.data,
                                         seg.ph.p_flags) for seg in self._model.load_segments]
        self.note_segments = [ElfNoteSegment(seg.ph.p_vaddr,
                                             seg.data,
                                             seg.ph.p_flags) for seg in self._model.note_segments]
        self.sections = [ElfSection(self._parse_string_table(self._model.string_table, sec.sh.sh_name),
                                    sec.sh.sh_addr,
                                    sec.data,
                                    sec.sh.sh_flags) for sec in self._model.sections]

    @staticmethod
    def _parse_string_table(byte_str, offset):  # type: (bytes, int) -> str
        section_name_str = byte_str[offset:]
        string_end = section_name_str.find(0x00)

        if (string_end == -1):
            raise ValueError('Unable to get section name from section header string table')

        name = section_name_str[:string_end].decode('utf-8')

        return name

    def _generate_struct_from_headers(self, header_tables):  # type: (Container) -> Struct
        """
        Generate ``construct`` Struct for this file
        :param header_tables: contains elf_header, program_headers, section_headers
        :return: Struct of the whole file
        """
        elf_header = header_tables.elf_header
        program_headers = header_tables.program_headers
        section_headers = header_tables.section_headers
        assert program_headers or section_headers

        string_table_sh = None
        load_segment_subcons = []
        note_segment_subcons = []
        # Here we point back to make segments know their program headers
        for i, ph in enumerate(program_headers):
            args = [
                'ph' / Pointer(elf_header.e_phoff + i * ProgramHeader.sizeof(), ProgramHeader),
                'data' / Pointer(ph.p_offset, Bytes(ph.p_filesz)),
            ]
            if ph.p_vaddr == 0 and ph.p_type == self.PT_NOTE:
                args.append('note_secs' / Pointer(ph.p_offset, NoteSections))
                note_segment_subcons.append(Struct(*args))
            elif ph.p_vaddr != 0:
                load_segment_subcons.append(Struct(*args))

        section_subcons = []
        for i, sh in enumerate(section_headers):
            if sh.sh_type == self.SHT_STRTAB and i == elf_header.e_shstrndx:
                string_table_sh = sh
            elif sh.sh_addr != 0 and sh.sh_type == self.SHT_PROGBITS:
                section_subcons.append(Struct(
                    'sh' / Pointer(elf_header.e_shoff + i * SectionHeader.sizeof(), SectionHeader),
                    'data' / Pointer(sh.sh_offset, Bytes(sh.sh_size)),
                ))

        args = [
            'elf_header' / ElfHeader,
            'load_segments' / Sequence(*load_segment_subcons),
            'note_segments' / Sequence(*note_segment_subcons),
            'sections' / Sequence(*section_subcons),
        ]
        if string_table_sh is not None:
            args.append('string_table' / Pointer(string_table_sh.sh_offset, Bytes(string_table_sh.sh_size)))

        return Struct(*args)

    @property
    def sha256(self):  # type: () -> bytes
        """
        :return: SHA256 hash of the input ELF file
        """
        sha256 = hashlib.sha256()
        sha256.update(self._struct.build(self._model))  # type: ignore
        return sha256.digest()


class ElfSection(object):
    SHF_WRITE = 0x01
    SHF_ALLOC = 0x02
    SHF_EXECINSTR = 0x04
    SHF_MASKPROC = 0xf0000000

    def __init__(self, name, addr, data, flags):  # type: (str, int, bytes, int) -> None
        self.name = name
        self.addr = addr
        self.data = data
        self.flags = flags

    def attr_str(self):  # type: () -> str
        if self.flags & self.SHF_MASKPROC:
            return 'MS'

        res = 'R'
        res += 'W' if self.flags & self.SHF_WRITE else ' '
        res += 'X' if self.flags & self.SHF_EXECINSTR else ' '
        res += 'A' if self.flags & self.SHF_ALLOC else ' '
        return res

    def __repr__(self):  # type: () -> str
        return '{:>32} [Addr] 0x{:>08X}, [Size] 0x{:>08X} {:>4}' \
            .format(self.name, self.addr, len(self.data), self.attr_str())


class ElfSegment(object):
    PF_X = 0x01
    PF_W = 0x02
    PF_R = 0x04

    def __init__(self, addr, data, flags):  # type: (int, bytes, int) -> None
        self.addr = addr
        self.data = data
        self.flags = flags
        self.type = ElfFile.PT_LOAD

    def attr_str(self):  # type: () -> str
        res = ''
        res += 'R' if self.flags & self.PF_R else ' '
        res += 'W' if self.flags & self.PF_W else ' '
        res += 'E' if self.flags & self.PF_X else ' '
        return res

    @staticmethod
    def _type_str():  # type: () -> str
        return 'LOAD'

    def __repr__(self):  # type: () -> str
        return '{:>8} Addr 0x{:>08X}, Size 0x{:>08X} Flags {:4}' \
            .format(self._type_str(), self.addr, len(self.data), self.attr_str())


class ElfNoteSegment(ElfSegment):
    def __init__(self, addr, data, flags):  # type: (int, bytes, int) -> None
        super(ElfNoteSegment, self).__init__(addr, data, flags)
        self.type = ElfFile.PT_NOTE
        self.note_secs = NoteSections.parse(self.data)

    @staticmethod
    def _type_str():  # type: () -> str
        return 'NOTE'


TASK_STATUS_CORRECT = 0x00
TASK_STATUS_TCB_CORRUPTED = 0x01
TASK_STATUS_STACK_CORRUPTED = 0x02

EspTaskStatus = Struct(
    'task_index' / Int32ul,
    'task_flags' / Int32ul,
    'task_tcb_addr' / Int32ul,
    'task_stack_start' / Int32ul,
    'task_stack_len' / Int32ul,
    'task_name' / Bytes(16),
)


class ESPCoreDumpElfFile(ElfFile):
    PT_INFO = 8266
    PT_TASK_INFO = 678
    PT_EXTRA_INFO = 677

    CURR_TASK_MARKER = 0xdeadbeef

    # ELF file machine type
    EM_XTENSA = 0x5E
    EM_RISCV = 0xF3

    def __init__(self, elf_path=None, e_type=None, e_machine=None):
        # type: (Optional[str], Optional[int], Optional[int]) -> None
        _e_type = e_type or self.ET_CORE
        _e_machine = e_machine or self.EM_XTENSA
        super(ESPCoreDumpElfFile, self).__init__(elf_path, _e_type, _e_machine)

    def add_segment(self, addr, data, seg_type, flags):  # type: (int, bytes, int, int) -> None
        if seg_type != self.PT_NOTE:
            self.load_segments.append(ElfSegment(addr, data, flags))
        else:
            self.note_segments.append(ElfNoteSegment(addr, data, flags))

    def dump(self, output_path):  # type: (str) -> None
        """
        Dump self.model into file
        :param output_path: output file path
        :return: None
        """
        res = b''
        res += ElfHeader.build({
            'e_type': self.e_type,
            'e_machine': self.e_machine,
            'e_version': self.EV_CURRENT,
            'e_entry': 0,
            'e_phoff': ElfHeader.sizeof(),
            'e_shoff': 0,
            'e_flags': 0,
            'e_ehsize': ElfHeader.sizeof(),
            'e_phentsize': ProgramHeader.sizeof(),
            'e_phnum': len(self.load_segments) + len(self.note_segments),
            'e_shentsize': 0,
            'e_shnum': 0,
            'e_shstrndx': self.SHN_UNDEF,
        })

        offset = ElfHeader.sizeof() + (len(self.load_segments) + len(self.note_segments)) * ProgramHeader.sizeof()
        _segments = self.load_segments + self.note_segments  # type: ignore
        for seg in _segments:
            res += ProgramHeader.build({
                'p_type': seg.type,
                'p_offset': offset,
                'p_vaddr': seg.addr,
                'p_paddr': seg.addr,
                'p_filesz': len(seg.data),
                'p_memsz': len(seg.data),
                'p_flags': seg.flags,
                'p_align': 0,
            })
            offset += len(seg.data)

        for seg in _segments:
            res += seg.data

        with open(output_path, 'wb') as fw:
            fw.write(res)
