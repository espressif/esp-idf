#!/usr/bin/env python
#
# ESP32 core dump Utility

from __future__ import print_function
from __future__ import unicode_literals
from __future__ import division
from hashlib import sha256
import sys
try:
    from builtins import zip
    from builtins import str
    from builtins import range
    from past.utils import old_div
    from builtins import object
except ImportError:
    print('Import has failed probably because of the missing "future" package. Please install all the packages for '
          'interpreter {} from the $IDF_PATH/requirements.txt file.'.format(sys.executable))
    sys.exit(1)
import os
import argparse
import subprocess
import tempfile
import struct
import errno
import base64
import binascii
import logging
import re

idf_path = os.getenv('IDF_PATH')
if idf_path:
    sys.path.insert(0, os.path.join(idf_path, 'components', 'esptool_py', 'esptool'))

try:
    import esptool

except ImportError:
    print("esptool is not found! Set proper $IDF_PATH in environment.")
    sys.exit(2)

__version__ = "0.4-dev"

if os.name == 'nt':
    CLOSE_FDS = False
else:
    CLOSE_FDS = True

INVALID_CAUSE_VALUE = 0xFFFF
XCHAL_EXCCAUSE_NUM = 64

# Exception cause dictionary to get translation of exccause register
# From 4.4.1.5 table 4-64 Exception Causes of Xtensa
# Instruction Set Architecture (ISA) Reference Manual
xtensa_exception_cause_dict = {
    0: ("IllegalInstructionCause", "Illegal instruction"),
    1: ("SyscallCause", "SYSCALL instruction"),
    2: ("InstructionFetchErrorCause", "Processor internal physical address or data error during instruction fetch. (See EXCVADDR for more information)"),
    3: ("LoadStoreErrorCause", "Processor internal physical address or data error during load or store. (See EXCVADDR for more information)"),
    4: ("Level1InterruptCause", "Level-1 interrupt as indicated by set level-1 bits in the INTERRUPT register"),
    5: ("AllocaCause", "MOVSP instruction, if caller`s registers are not in the register file"),
    6: ("IntegerDivideByZeroCause", "QUOS: QUOU, REMS: or REMU divisor operand is zero"),
    8: ("PrivilegedCause", "Attempt to execute a privileged operation when CRING ? 0"),
    9: ("LoadStoreAlignmentCause", "Load or store to an unaligned address. (See EXCVADDR for more information)"),
    12: ("InstrPIFDataErrorCause", "PIF data error during instruction fetch. (See EXCVADDR for more information)"),
    13: ("LoadStorePIFDataErrorCause", "Synchronous PIF data error during LoadStore access. (See EXCVADDR for more information)"),
    14: ("InstrPIFAddrErrorCause", "PIF address error during instruction fetch. (See EXCVADDR for more information)"),
    15: ("LoadStorePIFAddrErrorCause", "Synchronous PIF address error during LoadStore access. (See EXCVADDR for more information)"),
    16: ("InstTLBMissCause", "Error during Instruction TLB refill. (See EXCVADDR for more information)"),
    17: ("InstTLBMultiHitCause", "Multiple instruction TLB entries matched. (See EXCVADDR for more information)"),
    18: ("InstFetchPrivilegeCause", "An instruction fetch referenced a virtual address at a ring level less than CRING. (See EXCVADDR for more information)"),
    20: ("InstFetchProhibitedCause", "An instruction fetch referenced a page mapped with an attribute that does not permit instruction fetch (EXCVADDR)."),
    24: ("LoadStoreTLBMissCause", "Error during TLB refill for a load or store. (See EXCVADDR for more information)"),
    25: ("LoadStoreTLBMultiHitCause", "Multiple TLB entries matched for a load or store. (See EXCVADDR for more information)"),
    26: ("LoadStorePrivilegeCause", "A load or store referenced a virtual address at a ring level less than CRING. (See EXCVADDR for more information)"),
    28: ("LoadProhibitedCause", "A load referenced a page mapped with an attribute that does not permit loads. (See EXCVADDR for more information)"),
    29: ("StoreProhibitedCause", "A store referenced a page mapped with an attribute that does not permit stores [Region Protection Option or MMU Option]."),
    32: ("Coprocessor0Disabled", "Coprocessor 0 instruction when cp0 disabled"),
    33: ("Coprocessor1Disabled", "Coprocessor 1 instruction when cp1 disabled"),
    34: ("Coprocessor2Disabled", "Coprocessor 2 instruction when cp2 disabled"),
    35: ("Coprocessor3Disabled", "Coprocessor 3 instruction when cp3 disabled"),
    36: ("Coprocessor4Disabled", "Coprocessor 4 instruction when cp4 disabled"),
    37: ("Coprocessor5Disabled", "Coprocessor 5 instruction when cp5 disabled"),
    38: ("Coprocessor6Disabled", "Coprocessor 6 instruction when cp6 disabled"),
    39: ("Coprocessor7Disabled", "Coprocessor 7 instruction when cp7 disabled"),
    INVALID_CAUSE_VALUE: ("InvalidCauseRegister", "Invalid EXCCAUSE register value or current task is broken and was skipped"),
    # ESP panic pseudo reasons
    XCHAL_EXCCAUSE_NUM + 0: ("UnknownException", "Unknown exception"),
    XCHAL_EXCCAUSE_NUM + 1: ("DebugException", "Unhandled debug exception"),
    XCHAL_EXCCAUSE_NUM + 2: ("DoubleException", "Double exception"),
    XCHAL_EXCCAUSE_NUM + 3: ("KernelException", "Unhandled kernel exception"),
    XCHAL_EXCCAUSE_NUM + 4: ("CoprocessorException", "Coprocessor exception"),
    XCHAL_EXCCAUSE_NUM + 5: ("InterruptWDTTimoutCPU0", "Interrupt wdt timeout on CPU0"),
    XCHAL_EXCCAUSE_NUM + 6: ("InterruptWDTTimoutCPU1", "Interrupt wdt timeout on CPU1"),
    XCHAL_EXCCAUSE_NUM + 7: ("CacheError", "Cache disabled but cached memory region accessed"),
}


class ESPCoreDumpError(RuntimeError):
    """Core dump runtime error class
    """
    def __init__(self, message):
        """Constructor for core dump error
        """
        super(ESPCoreDumpError, self).__init__(message)


class BinStruct(object):
    """Binary structure representation

       Subclasses must specify actual structure layout using 'fields' and 'format' members.
       For example, the following subclass represents structure with two fields:
       f1 of size 2 bytes and 4 bytes f2. Little endian.
        class SomeStruct(BinStruct):
            fields = ("f1",
                      "f2")
            format = "<HL"

        Then subclass can be used to initialize fields of underlaying structure and convert it to binary representation:
        f = open('some_struct.bin', 'wb')
        s = SomeStruct()
        s.f1 = 1
        s.f2 = 10
        f.write(s.dump())
        f.close()
    """

    def __init__(self, buf=None):
        """Base constructor for binary structure objects
        """
        if buf is None:
            buf = b'\0' * self.sizeof()
        fields = struct.unpack(self.__class__.format, buf[:self.sizeof()])
        self.__dict__.update(zip(self.__class__.fields, fields))

    def sizeof(self):
        """Returns the size of the structure represented by specific subclass
        """
        return struct.calcsize(self.__class__.format)

    def dump(self):
        """Returns binary representation of structure
        """
        keys = self.__class__.fields
        return struct.pack(self.__class__.format, *(self.__dict__[k] for k in keys))


class Elf32FileHeader(BinStruct):
    """ELF32 file header
    """
    fields = ("e_ident",
              "e_type",
              "e_machine",
              "e_version",
              "e_entry",
              "e_phoff",
              "e_shoff",
              "e_flags",
              "e_ehsize",
              "e_phentsize",
              "e_phnum",
              "e_shentsize",
              "e_shnum",
              "e_shstrndx")
    format = "<16sHHLLLLLHHHHHH"

    def __init__(self, buf=None):
        """Constructor for ELF32 file header structure
        """
        super(Elf32FileHeader, self).__init__(buf)
        if buf is None:
            # Fill in sane ELF header for LSB32
            self.e_ident = b"\x7fELF\1\1\1\0\0\0\0\0\0\0\0\0"
            self.e_version = ESPCoreDumpElfFile.EV_CURRENT
            self.e_ehsize = self.sizeof()


class Elf32ProgramHeader(BinStruct):
    """ELF32 program header
    """
    fields = ("p_type",
              "p_offset",
              "p_vaddr",
              "p_paddr",
              "p_filesz",
              "p_memsz",
              "p_flags",
              "p_align")
    format = "<LLLLLLLL"


class Elf32NoteDesc(object):
    """ELF32 note descriptor
    """
    def __init__(self, name, type, desc):
        """Constructor for ELF32 note descriptor
        """
        self.name = name
        self.type = type
        self.desc = desc

    def dump(self):
        """Returns binary representation of ELF32 note descriptor
        """
        nm_buf = bytearray(self.name, encoding='ascii') + b'\0'
        hdr = struct.pack("<LLL", len(nm_buf), len(self.desc), self.type)
        # pad for 4 byte alignment
        name = nm_buf + ((4 - len(nm_buf)) % 4) * b'\0'
        desc = self.desc + ((4 - len(self.desc)) % 4) * b'\0'
        return hdr + name + desc

    def read(self, data):
        """Reads ELF32 note descriptor
        """
        hdr_sz = struct.calcsize("<LLL")
        nm_len,desc_len,self.type = struct.unpack("<LLL", data[:hdr_sz])
        nm_len_a = nm_len + ((4 - nm_len) % 4)
        self.name = struct.unpack("<%ds" % (nm_len - 1), data[hdr_sz:hdr_sz + nm_len - 1])[0].decode('ascii')
        self.desc = data[hdr_sz + nm_len_a:hdr_sz + nm_len_a + desc_len]
        desc_len_a = desc_len + ((4 - desc_len) % 4)
        return hdr_sz + nm_len_a + desc_len_a


class XtensaPrStatus(BinStruct):
    """Xtensa program status structure"""
    fields = ("si_signo", "si_code", "si_errno",
              "pr_cursig",
              "pr_pad0",
              "pr_sigpend",
              "pr_sighold",
              "pr_pid",
              "pr_ppid",
              "pr_pgrp",
              "pr_sid",
              "pr_utime",
              "pr_stime",
              "pr_cutime",
              "pr_cstime")
    format = "<3LHHLLLLLLQQQQ"


class EspCoreDumpTaskStatus(BinStruct):
    """Core dump status structure"""
    # task status flags for note
    TASK_STATUS_CORRECT = 0x00
    TASK_STATUS_TCB_CORRUPTED = 0x01
    TASK_STATUS_STACK_CORRUPTED = 0x02
    fields = ("task_index",
              "task_flags",
              "task_tcb_addr",
              "task_stack_start",
              "task_stack_len",
              "task_name")
    format = "<LLLLL16s"


class ESPCoreDumpSegment(esptool.ImageSegment):
    """ Wrapper class for a program segment in core ELF file, has a segment
        type and flags as well as the common properties of an ImageSegment.
    """
    # segment flags
    PF_X = 0x1  # Execute
    PF_W = 0x2  # Write
    PF_R = 0x4  # Read

    def __init__(self, addr, data, type, flags):
        """Constructor for program segment
        """
        super(ESPCoreDumpSegment, self).__init__(addr, data)
        self.flags = flags
        self.type = type

    def __repr__(self):
        """Returns string representation of program segment
        """
        return "%s %s %s" % (self.type, self.attr_str(), super(ESPCoreDumpSegment, self).__repr__())

    def attr_str(self):
        """Returns string representation of program segment attributes
        """
        str = ''
        if self.flags & self.PF_R:
            str += 'R'
        else:
            str += ' '
        if self.flags & self.PF_W:
            str += 'W'
        else:
            str += ' '
        if self.flags & self.PF_X:
            str += 'X'
        else:
            str += ' '
        return str


class ESPCoreDumpSection(esptool.ELFSection):
    """ Wrapper class for a section in core ELF file, has a section
        flags as well as the common properties of an esptool.ELFSection.
    """
    # section flags
    SHF_WRITE       = 0x1
    SHF_ALLOC       = 0x2
    SHF_EXECINSTR   = 0x4

    def __init__(self, name, addr, data, flags):
        """Constructor for section
        """
        super(ESPCoreDumpSection, self).__init__(name, addr, data)
        self.flags = flags

    def __repr__(self):
        """Returns string representation of section
        """
        return "%s %s" % (super(ESPCoreDumpSection, self).__repr__(), self.attr_str())

    def attr_str(self):
        """Returns string representation of section attributes
        """
        str = "R"
        if self.flags & self.SHF_WRITE:
            str += 'W'
        else:
            str += ' '
        if self.flags & self.SHF_EXECINSTR:
            str += 'X'
        else:
            str += ' '
        if self.flags & self.SHF_ALLOC:
            str += 'A'
        else:
            str += ' '
        return str


class ESPCoreDumpElfFile(esptool.ELFFile):
    """ Wrapper class for core dump ELF file
    """
    # extra regs IDs used in EXTRA_INFO note
    REG_EXCCAUSE_IDX    = 0
    REG_EXCVADDR_IDX    = 1
    REG_EPC1_IDX        = 177
    REG_EPC2_IDX        = 178
    REG_EPC3_IDX        = 179
    REG_EPC4_IDX        = 180
    REG_EPC5_IDX        = 181
    REG_EPC6_IDX        = 182
    REG_EPC7_IDX        = 183
    REG_EPS2_IDX        = 194
    REG_EPS3_IDX        = 195
    REG_EPS4_IDX        = 196
    REG_EPS5_IDX        = 197
    REG_EPS6_IDX        = 198
    REG_EPS7_IDX        = 199
    # ELF file type
    ET_NONE             = 0x0  # No file type
    ET_REL              = 0x1  # Relocatable file
    ET_EXEC             = 0x2  # Executable file
    ET_DYN              = 0x3  # Shared object file
    ET_CORE             = 0x4  # Core file
    # ELF file version
    EV_NONE             = 0x0
    EV_CURRENT          = 0x1
    # ELF file machine type
    EM_NONE             = 0x0
    EM_XTENSA           = 0x5E
    # section types
    SEC_TYPE_PROGBITS   = 0x01
    SEC_TYPE_STRTAB     = 0x03
    # special section index
    SHN_UNDEF           = 0x0
    # program segment types
    PT_NULL             = 0x0
    PT_LOAD             = 0x1
    PT_DYNAMIC          = 0x2
    PT_INTERP           = 0x3
    PT_NOTE             = 0x4
    PT_SHLIB            = 0x5
    PT_PHDR             = 0x6

    def __init__(self, name=None):
        """Constructor for core dump ELF file
        """
        if name:
            super(ESPCoreDumpElfFile, self).__init__(name)
        else:
            self.sections = []
            self.program_segments = []
            self.aux_segments = []
            self.e_type = self.ET_NONE
            self.e_machine = self.EM_NONE

    def _read_elf_file(self, f):
        """Reads core dump from ELF file
        """
        # read the ELF file header
        LEN_FILE_HEADER = 0x34
        try:
            header = f.read(LEN_FILE_HEADER)
            (ident,type,machine,_version,
             self.entrypoint,phoff,shoff,_flags,
             _ehsize, phentsize,phnum,_shentsize,
             shnum,shstrndx) = struct.unpack("<16sHHLLLLLHHHHHH", header)
        except struct.error as e:
            raise ESPCoreDumpError("Failed to read a valid ELF header from %s: %s" % (f.name, e))

        if bytearray([ident[0]]) != b'\x7f' or ident[1:4] != b'ELF':
            raise ESPCoreDumpError("%s has invalid ELF magic header" % f.name)
        if machine != self.EM_XTENSA:
            raise ESPCoreDumpError("%s does not appear to be an Xtensa ELF file. e_machine=%04x" % (f.name, machine))
        self.e_type = type
        self.e_machine = machine
        self.sections = []
        self.program_segments = []
        self.aux_segments = []
        if shnum > 0:
            self._read_sections(f, shoff, shstrndx)
        if phnum > 0:
            self._read_program_segments(f, phoff, phentsize, phnum)

    def _read_sections(self, f, section_header_offs, shstrndx):
        """Reads core dump sections from ELF file
        """
        f.seek(section_header_offs)
        section_header = f.read()
        LEN_SEC_HEADER = 0x28
        if len(section_header) == 0:
            raise ESPCoreDumpError("No section header found at offset %04x in ELF file." % section_header_offs)
        if len(section_header) % LEN_SEC_HEADER != 0:
            logging.warning('Unexpected ELF section header length %04x is not mod-%02x' % (len(section_header),LEN_SEC_HEADER))

        # walk through the section header and extract all sections
        section_header_offsets = range(0, len(section_header), LEN_SEC_HEADER)

        def read_section_header(offs):
            name_offs,sec_type,flags,lma,sec_offs,size = struct.unpack_from("<LLLLLL", section_header[offs:])
            return (name_offs, sec_type, flags, lma, size, sec_offs)
        all_sections = [read_section_header(offs) for offs in section_header_offsets]
        prog_sections = [s for s in all_sections if s[1] == esptool.ELFFile.SEC_TYPE_PROGBITS]

        # search for the string table section
        if not shstrndx * LEN_SEC_HEADER in section_header_offsets:
            raise ESPCoreDumpError("ELF file has no STRTAB section at shstrndx %d" % shstrndx)
        _,sec_type,_,_,sec_size,sec_offs = read_section_header(shstrndx * LEN_SEC_HEADER)
        if sec_type != esptool.ELFFile.SEC_TYPE_STRTAB:
            logging.warning('ELF file has incorrect STRTAB section type 0x%02x' % sec_type)
        f.seek(sec_offs)
        string_table = f.read(sec_size)

        # build the real list of ELFSections by reading the actual section names from the
        # string table section, and actual data for each section from the ELF file itself
        def lookup_string(offs):
            raw = string_table[offs:]
            return raw[:raw.index(b'\x00')]

        def read_data(offs,size):
            f.seek(offs)
            return f.read(size)

        prog_sections = [ESPCoreDumpSection(lookup_string(n_offs), lma, read_data(offs, size), flags)
                         for (n_offs, _type, flags, lma, size, offs) in prog_sections if lma != 0]
        self.sections = prog_sections

    def _read_program_segments(self, f, seg_table_offs, entsz, num):
        """Reads core dump program segments from ELF file
        """
        f.seek(seg_table_offs)
        seg_table = f.read(entsz * num)
        LEN_SEG_HEADER = 0x20
        if len(seg_table) == 0:
            raise ESPCoreDumpError("No program header table found at offset %04x in ELF file." % seg_table_offs)
        if len(seg_table) % LEN_SEG_HEADER != 0:
            logging.warning('Unexpected ELF program header table length %04x is not mod-%02x' % (len(seg_table),LEN_SEG_HEADER))
        # walk through the program segment table and extract all segments
        seg_table_offs = range(0, len(seg_table), LEN_SEG_HEADER)

        def read_program_header(offs):
            type,offset,vaddr,_paddr,filesz,_memsz,flags,_align = struct.unpack_from("<LLLLLLLL", seg_table[offs:])
            return (type,offset,vaddr,filesz,flags)
        prog_segments = [read_program_header(offs) for offs in seg_table_offs]

        # build the real list of ImageSegment by reading actual data for each segment from the ELF file itself
        def read_data(offs,size):
            f.seek(offs)
            return f.read(size)

        # read loadable segments
        self.program_segments = [ESPCoreDumpSegment(vaddr, read_data(offset, filesz), type, flags)
                                 for (type, offset, vaddr, filesz,flags) in prog_segments if vaddr != 0]
        self.aux_segments = [ESPCoreDumpSegment(vaddr, read_data(offset, filesz), type, flags)
                             for (type, offset, vaddr, filesz, flags) in prog_segments if type == ESPCoreDumpElfFile.PT_NOTE and vaddr == 0]

    def add_program_segment(self, addr, data, type, flags):
        """Adds new program segment
        """
        # TODO: currently merging with existing segments is not supported
        data_sz = len(data)
        # check for overlapping and merge if needed
        if addr != 0 and data_sz != 0:
            for ps in self.program_segments:
                seg_len = len(ps.data)
                if addr >= ps.addr and addr < (ps.addr + seg_len):
                    raise ESPCoreDumpError("Can not add overlapping region [%x..%x] to ELF file. Conflict with existing [%x..%x]." %
                                           (addr, addr + data_sz - 1, ps.addr, ps.addr + seg_len - 1))
                if (addr + data_sz) > ps.addr and (addr + data_sz) <= (ps.addr + seg_len):
                    raise ESPCoreDumpError("Can not add overlapping region [%x..%x] to ELF file. Conflict with existing [%x..%x]." %
                                           (addr, addr + data_sz - 1, ps.addr, ps.addr + seg_len - 1))
        # append
        self.program_segments.append(ESPCoreDumpSegment(addr, data, type, flags))

    def add_aux_segment(self, data, type, flags):
        """Adds new note segment
        """
        self.aux_segments.append(ESPCoreDumpSegment(0, data, type, flags))

    def write_program_headers(self, f, off, segs):
        for seg in segs:
            phdr = Elf32ProgramHeader()
            phdr.p_type = seg.type
            phdr.p_offset = off
            phdr.p_vaddr = seg.addr
            phdr.p_paddr = phdr.p_vaddr  # TODO
            phdr.p_filesz = len(seg.data)
            phdr.p_memsz = phdr.p_filesz  # TODO
            phdr.p_flags = seg.flags
            phdr.p_align = 0  # TODO
            f.write(phdr.dump())
            off += phdr.p_filesz
        return off

    def dump(self, f):
        """Write core dump contents to file
        """
        # TODO: currently dumps only program segments.
        # dumping sections is not supported yet
        # write ELF header
        ehdr = Elf32FileHeader()
        ehdr.e_type = self.e_type
        ehdr.e_machine = self.e_machine
        ehdr.e_entry = 0
        ehdr.e_phoff = ehdr.sizeof()
        ehdr.e_shoff = 0
        ehdr.e_flags = 0
        ehdr.e_phentsize = Elf32ProgramHeader().sizeof()
        ehdr.e_phnum = len(self.program_segments) + len(self.aux_segments)
        ehdr.e_shentsize = 0
        ehdr.e_shnum = 0
        ehdr.e_shstrndx = self.SHN_UNDEF
        f.write(ehdr.dump())
        # write program header table
        cur_off = ehdr.e_ehsize + ehdr.e_phnum * ehdr.e_phentsize
        cur_off = self.write_program_headers(f, cur_off, self.program_segments)
        cur_off = self.write_program_headers(f, cur_off, self.aux_segments)
        # write program segments
        for segment in self.program_segments:
            f.write(segment.data)
        # write aux program segments
        for segment in self.aux_segments:
            f.write(segment.data)


class ESPCoreDumpLoaderError(ESPCoreDumpError):
    """Core dump loader error class
    """
    def __init__(self, message):
        """Constructor for core dump loader error
        """
        super(ESPCoreDumpLoaderError, self).__init__(message)


class ESPCoreDumpVersion(object):
    """Core dump version class
    """
    # This class contains all version-dependent params
    ESP_CORE_DUMP_CHIP_ESP32 = 0
    ESP_CORE_DUMP_CHIP_ESP32S2 = 2

    def __init__(self, version=None):
        """Constructor for core dump version
        """
        super(ESPCoreDumpVersion, self).__init__()
        if version is None:
            self.version = 0
        else:
            self.set_version(version)

    @staticmethod
    def make_dump_ver(maj, min):
        return (((maj & 0xFF) << 8) | ((min & 0xFF) << 0))

    def set_version(self, version):
        self.version = version

    @property
    def chip_ver(self):
        return ((self.version & 0xFFFF0000) >> 16)

    @property
    def dump_ver(self):
        return (self.version & 0x0000FFFF)

    @property
    def major(self):
        return ((self.version & 0x0000FF00) >> 8)

    @property
    def minor(self):
        return (self.version & 0x000000FF)


class ESPCoreDumpLoader(ESPCoreDumpVersion):
    """Core dump loader base class
    """
    # "legacy" stands for core dumps v0.1 (before IDF v4.1)
    ESP_COREDUMP_VERSION_BIN_V1 = ESPCoreDumpVersion.make_dump_ver(0, 1)
    ESP_COREDUMP_VERSION_BIN_V2 = ESPCoreDumpVersion.make_dump_ver(0, 2)
    ESP_COREDUMP_VERSION_ELF_CRC32 = ESPCoreDumpVersion.make_dump_ver(1, 0)
    ESP_COREDUMP_VERSION_ELF_SHA256 = ESPCoreDumpVersion.make_dump_ver(1, 1)
    ESP_CORE_DUMP_INFO_TYPE = 8266
    ESP_CORE_DUMP_TASK_INFO_TYPE = 678
    ESP_CORE_DUMP_EXTRA_INFO_TYPE = 677
    ESP_COREDUMP_CURR_TASK_MARKER = 0xdeadbeef
    ESP_COREDUMP_BIN_V1_HDR_FMT = '<4L'
    ESP_COREDUMP_BIN_V1_HDR_SZ = struct.calcsize(ESP_COREDUMP_BIN_V1_HDR_FMT)
    ESP_COREDUMP_HDR_FMT = '<5L'
    ESP_COREDUMP_HDR_SZ = struct.calcsize(ESP_COREDUMP_HDR_FMT)
    ESP_COREDUMP_TSK_HDR_FMT = '<3L'
    ESP_COREDUMP_TSK_HDR_SZ = struct.calcsize(ESP_COREDUMP_TSK_HDR_FMT)
    ESP_COREDUMP_MEM_SEG_HDR_FMT = '<2L'
    ESP_COREDUMP_MEM_SEG_HDR_SZ = struct.calcsize(ESP_COREDUMP_MEM_SEG_HDR_FMT)
    ESP_COREDUMP_NOTE_HDR_FMT = '<3L'
    ESP_COREDUMP_NOTE_HDR_SZ = struct.calcsize(ESP_COREDUMP_NOTE_HDR_FMT)
    ESP_COREDUMP_CRC_FMT = '<L'
    ESP_COREDUMP_CRC_SZ = struct.calcsize(ESP_COREDUMP_CRC_FMT)
    ESP_COREDUMP_SHA256_FMT = '32c'
    ESP_COREDUMP_SHA256_SZ = struct.calcsize(ESP_COREDUMP_SHA256_FMT)

    def __init__(self):
        """Base constructor for core dump loader
        """
        super(ESPCoreDumpLoader, self).__init__()
        self.fcore = None
        self.hdr = {}

    def _get_registers_from_stack(self, data, grows_down):
        """Returns list of registers (in GDB format) from xtensa stack frame
        """
        # from "gdb/xtensa-tdep.h"
        # typedef struct
        # {
        # 0    xtensa_elf_greg_t pc;
        # 1    xtensa_elf_greg_t ps;
        # 2    xtensa_elf_greg_t lbeg;
        # 3    xtensa_elf_greg_t lend;
        # 4    xtensa_elf_greg_t lcount;
        # 5    xtensa_elf_greg_t sar;
        # 6    xtensa_elf_greg_t windowstart;
        # 7    xtensa_elf_greg_t windowbase;
        # 8..63 xtensa_elf_greg_t reserved[8+48];
        # 64   xtensa_elf_greg_t ar[64];
        # } xtensa_elf_gregset_t;
        REG_PC_IDX = 0
        REG_PS_IDX = 1
        REG_LB_IDX = 2
        REG_LE_IDX = 3
        REG_LC_IDX = 4
        REG_SAR_IDX = 5
        # REG_WS_IDX = 6
        # REG_WB_IDX = 7
        REG_AR_START_IDX = 64
        # REG_AR_NUM = 64
        # FIXME: acc to xtensa_elf_gregset_t number of regs must be 128,
        # but gdb complanis when it less then 129
        REG_NUM = 129

        # XT_SOL_EXIT = 0
        XT_SOL_PC = 1
        XT_SOL_PS = 2
        # XT_SOL_NEXT = 3
        XT_SOL_AR_START = 4
        XT_SOL_AR_NUM = 4
        # XT_SOL_FRMSZ = 8

        XT_STK_EXIT = 0
        XT_STK_PC = 1
        XT_STK_PS = 2
        XT_STK_AR_START = 3
        XT_STK_AR_NUM = 16
        XT_STK_SAR = 19
        XT_STK_EXCCAUSE = 20
        XT_STK_EXCVADDR = 21
        XT_STK_LBEG = 22
        XT_STK_LEND = 23
        XT_STK_LCOUNT = 24
        XT_STK_FRMSZ = 25

        extra_regs = {ESPCoreDumpElfFile.REG_EPS2_IDX: 0, ESPCoreDumpElfFile.REG_EPS3_IDX: 0,
                      ESPCoreDumpElfFile.REG_EPS4_IDX: 0, ESPCoreDumpElfFile.REG_EPS5_IDX: 0,
                      ESPCoreDumpElfFile.REG_EPS6_IDX: 0, ESPCoreDumpElfFile.REG_EPS7_IDX: 0,
                      ESPCoreDumpElfFile.REG_EPC1_IDX: 0, ESPCoreDumpElfFile.REG_EPC2_IDX: 0,
                      ESPCoreDumpElfFile.REG_EPC3_IDX: 0, ESPCoreDumpElfFile.REG_EPC4_IDX: 0,
                      ESPCoreDumpElfFile.REG_EPC5_IDX: 0, ESPCoreDumpElfFile.REG_EPC6_IDX: 0,
                      ESPCoreDumpElfFile.REG_EPC7_IDX: 0}
        regs = [0] * REG_NUM
        # TODO: support for growing up stacks
        if not grows_down:
            raise ESPCoreDumpLoaderError("Growing up stacks are not supported for now!")
        ex_struct = "<%dL" % XT_STK_FRMSZ
        if len(data) < struct.calcsize(ex_struct):
            raise ESPCoreDumpLoaderError("Too small stack to keep frame: %d bytes!" % len(data))

        stack = struct.unpack(ex_struct, data[:struct.calcsize(ex_struct)])
        # Stack frame type indicator is always the first item
        rc = stack[XT_STK_EXIT]
        if rc != 0:
            regs[REG_PC_IDX] = stack[XT_STK_PC]
            regs[REG_PS_IDX] = stack[XT_STK_PS]
            for i in range(XT_STK_AR_NUM):
                regs[REG_AR_START_IDX + i] = stack[XT_STK_AR_START + i]
            regs[REG_SAR_IDX] = stack[XT_STK_SAR]
            regs[REG_LB_IDX] = stack[XT_STK_LBEG]
            regs[REG_LE_IDX] = stack[XT_STK_LEND]
            regs[REG_LC_IDX] = stack[XT_STK_LCOUNT]
            # FIXME: crashed and some running tasks (e.g. prvIdleTask) have EXCM bit set
            # and GDB can not unwind callstack properly (it implies not windowed call0)
            if regs[REG_PS_IDX] & (1 << 5):
                regs[REG_PS_IDX] &= ~(1 << 4)
            if stack[XT_STK_EXCCAUSE] in xtensa_exception_cause_dict:
                extra_regs[ESPCoreDumpElfFile.REG_EXCCAUSE_IDX] = stack[XT_STK_EXCCAUSE]
            else:
                extra_regs[ESPCoreDumpElfFile.REG_EXCCAUSE_IDX] = INVALID_CAUSE_VALUE
            extra_regs[ESPCoreDumpElfFile.REG_EXCVADDR_IDX] = stack[XT_STK_EXCVADDR]
        else:
            regs[REG_PC_IDX] = stack[XT_SOL_PC]
            regs[REG_PS_IDX] = stack[XT_SOL_PS]
            for i in range(XT_SOL_AR_NUM):
                regs[REG_AR_START_IDX + i] = stack[XT_SOL_AR_START + i]
            # nxt = stack[XT_SOL_NEXT]
        return regs,extra_regs

    def tcb_is_sane(self, tcb_addr, tcb_size):
        """Check tcb address if it is correct
        """
        return not (tcb_addr < 0x3ffae000 or (tcb_addr + tcb_size) > 0x40000000)

    def stack_is_sane(self, sp):
        """Check stack address if it is correct
        """
        return not(sp < 0x3ffae010 or sp > 0x3fffffff)

    def addr_is_fake(self, addr):
        """Check if address is in fake area
        """
        return ((addr < 0x3f3fffff and addr >= 0x20000000) or addr >= 0x80000000)

    def remove_tmp_file(self, fname):
        """Silently removes temporary file
        """
        try:
            os.remove(fname)
        except OSError as e:
            if e.errno != errno.ENOENT:
                logging.warning("Failed to remove temp file '%s' (%d)!" % (fname, e.errno))

    def cleanup(self):
        """Cleans up loader resources
        """
        if self.fcore:
            self.fcore.close()
            if self.fcore_name:
                self.remove_tmp_file(self.fcore_name)

    def _extract_elf_corefile(self, core_fname=None, off=0, exe_name=None):
        """ Reads the ELF formatted core dump image and parse it
        """
        core_off = off
        self.set_version(self.hdr['ver'])
        if self.dump_ver == self.ESP_COREDUMP_VERSION_ELF_CRC32:
            checksum_len = self.ESP_COREDUMP_CRC_SZ
        elif self.dump_ver == self.ESP_COREDUMP_VERSION_ELF_SHA256:
            checksum_len = self.ESP_COREDUMP_SHA256_SZ
        else:
            raise ESPCoreDumpLoaderError("Core dump version '%d' is not supported!" % self.dump_ver)
        core_elf = ESPCoreDumpElfFile()
        data = self.read_data(core_off, self.hdr['tot_len'] - checksum_len - self.ESP_COREDUMP_HDR_SZ)
        with open(core_fname, 'w+b') as fce:
            try:
                fce.write(data)
                fce.flush()
                fce.seek(0)
                core_elf._read_elf_file(fce)
                if exe_name:
                    exe_elf = ESPCoreDumpElfFile(exe_name)
                # Read note segments from core file which are belong to tasks (TCB or stack)
                for ns in core_elf.aux_segments:
                    if ns.type != ESPCoreDumpElfFile.PT_NOTE:
                        continue
                    note_read = 0
                    while note_read < len(ns.data):
                        note = Elf32NoteDesc("", 0, None)
                        note_read += note.read(ns.data[note_read:])
                        # Check for version info note
                        if 'ESP_CORE_DUMP_INFO' == note.name and note.type == self.ESP_CORE_DUMP_INFO_TYPE and exe_name:
                            app_sha256 = binascii.hexlify(exe_elf.sha256())
                            n_ver_len = struct.calcsize("<L")
                            n_sha256_len = self.ESP_COREDUMP_SHA256_SZ * 2  # SHA256 as hex string
                            n_ver,coredump_sha256 = struct.unpack("<L%ds" % (n_sha256_len), note.desc[:n_ver_len + n_sha256_len])
                            if coredump_sha256 != app_sha256 or ESPCoreDumpVersion(n_ver).dump_ver != self.dump_ver:
                                raise ESPCoreDumpError("Invalid application image for coredump: app_SHA256(%s) != coredump_SHA256(%s)." %
                                                       (app_sha256, coredump_sha256))
            except ESPCoreDumpError as e:
                logging.warning("Failed to extract ELF core dump image into file %s. (Reason: %s)" % (core_fname, e))
        return core_fname

    def _extract_bin_corefile(self, core_fname=None, rom_elf=None, off=0):
        """Creates core dump ELF file
        """
        core_off = off
        with open(core_fname, 'w+b') as fce:
            tcbsz_aligned = self.hdr['tcbsz']
            if tcbsz_aligned % 4:
                tcbsz_aligned = 4 * (old_div(tcbsz_aligned,4) + 1)
            core_elf = ESPCoreDumpElfFile()
            notes = b''
            core_dump_info_notes = b''
            task_info_notes = b''
            task_status = EspCoreDumpTaskStatus()
            for i in range(self.hdr['task_num']):
                task_status.task_index = i
                task_status.task_flags = EspCoreDumpTaskStatus.TASK_STATUS_CORRECT
                data = self.read_data(core_off, self.ESP_COREDUMP_TSK_HDR_SZ)
                tcb_addr,stack_top,stack_end = struct.unpack_from(self.ESP_COREDUMP_TSK_HDR_FMT, data)
                if stack_end > stack_top:
                    stack_len = stack_end - stack_top
                    stack_base = stack_top
                else:
                    stack_len = stack_top - stack_end
                    stack_base = stack_end
                stack_len_aligned = stack_len
                if stack_len_aligned % 4:
                    stack_len_aligned = 4 * (old_div(stack_len_aligned,4) + 1)

                core_off += self.ESP_COREDUMP_TSK_HDR_SZ
                logging.debug("Read TCB %d bytes @ 0x%x" % (tcbsz_aligned, tcb_addr))
                data = self.read_data(core_off, tcbsz_aligned)
                task_status.task_tcb_addr = tcb_addr
                try:
                    if self.tcb_is_sane(tcb_addr, tcbsz_aligned):
                        if self.hdr['tcbsz'] != tcbsz_aligned:
                            core_elf.add_program_segment(tcb_addr, data[:self.hdr['tcbsz'] - tcbsz_aligned],
                                                         ESPCoreDumpElfFile.PT_LOAD, ESPCoreDumpSegment.PF_R | ESPCoreDumpSegment.PF_W)
                        else:
                            core_elf.add_program_segment(tcb_addr, data, ESPCoreDumpElfFile.PT_LOAD, ESPCoreDumpSegment.PF_R | ESPCoreDumpSegment.PF_W)
                        # task_status.task_name = bytearray("%s\0" % task_name_str, encoding='ascii')
                    elif tcb_addr and self.addr_is_fake(tcb_addr):
                        task_status.task_flags |= EspCoreDumpTaskStatus.TASK_STATUS_TCB_CORRUPTED
                except ESPCoreDumpError as e:
                    logging.warning("Skip TCB %d bytes @ 0x%x. (Reason: %s)" % (tcbsz_aligned, tcb_addr, e))

                core_off += tcbsz_aligned
                logging.debug("Read stack %d bytes @ 0x%x" % (stack_len_aligned, stack_base))
                data = self.read_data(core_off, stack_len_aligned)
                if stack_len != stack_len_aligned:
                    data = data[:stack_len - stack_len_aligned]
                task_status.task_stack_start = stack_base
                task_status.task_stack_len = stack_len_aligned
                try:
                    if self.stack_is_sane(stack_base):
                        core_elf.add_program_segment(stack_base, data, ESPCoreDumpElfFile.PT_LOAD, ESPCoreDumpSegment.PF_R | ESPCoreDumpSegment.PF_W)
                    elif stack_base and self.addr_is_fake(stack_base):
                        task_status.task_flags |= EspCoreDumpTaskStatus.TASK_STATUS_STACK_CORRUPTED
                        core_elf.add_program_segment(stack_base, data, ESPCoreDumpElfFile.PT_LOAD, ESPCoreDumpSegment.PF_R | ESPCoreDumpSegment.PF_W)
                except ESPCoreDumpError as e:
                    logging.warning("Skip task's (%x) stack %d bytes @ 0x%x. (Reason: %s)" % (tcb_addr, stack_len_aligned, stack_base, e))
                core_off += stack_len_aligned
                try:
                    logging.debug("Stack start_end: 0x%x @ 0x%x" % (stack_top, stack_end))
                    task_regs,extra_regs = self._get_registers_from_stack(data, stack_end > stack_top)
                except Exception as e:
                    logging.error(e)
                    return None
                task_info_notes += Elf32NoteDesc("TASK_INFO", self.ESP_CORE_DUMP_TASK_INFO_TYPE, task_status.dump()).dump()
                prstatus = XtensaPrStatus()
                prstatus.pr_cursig = 0  # TODO: set sig only for current/failed task
                prstatus.pr_pid = tcb_addr
                note = Elf32NoteDesc("CORE", 1, prstatus.dump() + struct.pack("<%dL" % len(task_regs), *task_regs)).dump()
                notes += note
                if ESPCoreDumpElfFile.REG_EXCCAUSE_IDX in extra_regs and len(core_dump_info_notes) == 0:
                    # actually there will be only one such note - for crashed task
                    core_dump_info_notes += Elf32NoteDesc("ESP_CORE_DUMP_INFO", self.ESP_CORE_DUMP_INFO_TYPE, struct.pack("<L", self.hdr['ver'])).dump()
                    exc_regs = []
                    for reg_id in extra_regs:
                        exc_regs.extend([reg_id, extra_regs[reg_id]])
                    core_dump_info_notes += Elf32NoteDesc("EXTRA_INFO", self.ESP_CORE_DUMP_EXTRA_INFO_TYPE,
                                                          struct.pack("<%dL" % (1 + len(exc_regs)), tcb_addr, *exc_regs)).dump()
            self.set_version(self.hdr['ver'])
            if self.dump_ver == self.ESP_COREDUMP_VERSION_BIN_V2:
                for i in range(self.hdr['segs_num']):
                    data = self.read_data(core_off, self.ESP_COREDUMP_MEM_SEG_HDR_SZ)
                    core_off += self.ESP_COREDUMP_MEM_SEG_HDR_SZ
                    mem_start,mem_sz = struct.unpack_from(self.ESP_COREDUMP_MEM_SEG_HDR_FMT, data)
                    logging.debug("Read memory segment %d bytes @ 0x%x" % (mem_sz, mem_start))
                    data = self.read_data(core_off, mem_sz)
                    core_elf.add_program_segment(mem_start, data, ESPCoreDumpElfFile.PT_LOAD, ESPCoreDumpSegment.PF_R | ESPCoreDumpSegment.PF_W)
                    core_off += mem_sz
            # add notes
            try:
                core_elf.add_aux_segment(notes, ESPCoreDumpElfFile.PT_NOTE, 0)
            except ESPCoreDumpError as e:
                logging.warning("Skip NOTES segment %d bytes @ 0x%x. (Reason: %s)" % (len(notes), 0, e))
            # add core dump info notes
            try:
                core_elf.add_aux_segment(core_dump_info_notes, ESPCoreDumpElfFile.PT_NOTE, 0)
            except ESPCoreDumpError as e:
                logging.warning("Skip core dump info NOTES segment %d bytes @ 0x%x. (Reason: %s)" % (len(core_dump_info_notes), 0, e))
            try:
                core_elf.add_aux_segment(task_info_notes, ESPCoreDumpElfFile.PT_NOTE, 0)
            except ESPCoreDumpError as e:
                logging.warning("Skip failed tasks info NOTES segment %d bytes @ 0x%x. (Reason: %s)" % (len(task_info_notes), 0, e))
            # add ROM text sections
            if rom_elf:
                for ps in rom_elf.program_segments:
                    if (ps.flags & ESPCoreDumpSegment.PF_X) == 0:
                        continue
                    try:
                        core_elf.add_program_segment(ps.addr, ps.data, ESPCoreDumpElfFile.PT_LOAD, ps.flags)
                    except ESPCoreDumpError as e:
                        logging.warning("Skip ROM segment %d bytes @ 0x%x. (Reason: %s)" % (len(ps.data), ps.addr, e))
            # dump core ELF
            core_elf.e_type = ESPCoreDumpElfFile.ET_CORE
            core_elf.e_machine = ESPCoreDumpElfFile.EM_XTENSA
            core_elf.dump(fce)
        return core_fname

    def create_corefile(self, core_fname=None, exe_name=None, rom_elf=None, off=0):
        """Creates core dump ELF file
        """
        data = self.read_data(off, self.ESP_COREDUMP_HDR_SZ)
        vals = struct.unpack_from(self.ESP_COREDUMP_HDR_FMT, data)
        self.hdr = dict(zip(('tot_len', 'ver', 'task_num', 'tcbsz', 'segs_num'), vals))
        if not core_fname:
            fce = tempfile.NamedTemporaryFile(mode='w+b', delete=False)
            core_fname = fce.name
        self.set_version(self.hdr['ver'])
        if self.chip_ver == ESPCoreDumpVersion.ESP_CORE_DUMP_CHIP_ESP32S2 or self.chip_ver == ESPCoreDumpVersion.ESP_CORE_DUMP_CHIP_ESP32:
            if self.dump_ver == self.ESP_COREDUMP_VERSION_ELF_CRC32 or self.dump_ver == self.ESP_COREDUMP_VERSION_ELF_SHA256:
                return self._extract_elf_corefile(core_fname, off + self.ESP_COREDUMP_HDR_SZ, exe_name)
            elif self.dump_ver == self.ESP_COREDUMP_VERSION_BIN_V2:
                return self._extract_bin_corefile(core_fname, rom_elf, off + self.ESP_COREDUMP_HDR_SZ)
            elif self.dump_ver == self.ESP_COREDUMP_VERSION_BIN_V1:
                return self._extract_bin_corefile(core_fname, rom_elf, off + self.ESP_COREDUMP_BIN_V1_HDR_SZ)
            raise ESPCoreDumpLoaderError("Core dump version '0x%x' is not supported!" % (self.dump_ver))
        else:
            raise ESPCoreDumpLoaderError("Core dump chip '0x%x' is not supported!" % (self.chip_ver))

    def read_data(self, off, sz):
        """Reads data from raw core dump got from flash or UART
        """
        self.fcore.seek(off)
        data = self.fcore.read(sz)
        return data


class ESPCoreDumpFileLoader(ESPCoreDumpLoader):
    """Core dump file loader class
    """
    def __init__(self, path, b64=False):
        """Constructor for core dump file loader
        """
        super(ESPCoreDumpFileLoader, self).__init__()
        self.fcore = self._load_coredump(path, b64)

    def _load_coredump(self, path, b64):
        """Loads core dump from (raw binary or base64-encoded) file
        """
        logging.debug("Load core dump from '%s'", path)
        self.fcore_name = None
        if b64:
            fhnd,self.fcore_name = tempfile.mkstemp()
            fcore = os.fdopen(fhnd, 'wb')
            fb64 = open(path, 'rb')
            try:
                while True:
                    line = fb64.readline()
                    if len(line) == 0:
                        break
                    data = base64.standard_b64decode(line.rstrip(b'\r\n'))
                    fcore.write(data)
                fcore.close()
                fcore = open(self.fcore_name, 'rb')
            except Exception as e:
                if self.fcore_name:
                    self.remove_tmp_file(self.fcore_name)
                    raise e
            finally:
                fb64.close()
        else:
            fcore = open(path, 'rb')
        return fcore


class ESPCoreDumpFlashLoader(ESPCoreDumpLoader):
    """Core dump flash loader class
    """
    ESP_COREDUMP_FLASH_LEN_FMT    = '<L'
    ESP_COREDUMP_FLASH_LEN_SZ     = struct.calcsize(ESP_COREDUMP_FLASH_LEN_FMT)
    ESP_COREDUMP_PART_TABLE_OFF   = 0x8000

    def __init__(self, off, tool_path=None, chip='esp32', port=None, baud=None):
        """Constructor for core dump flash loader
        """
        super(ESPCoreDumpFlashLoader, self).__init__()
        self.port = port
        self.baud = baud
        self.chip = chip
        self.dump_sz = 0
        self.fcore = self._load_coredump(off)

    def get_tool_path(self, use_esptool=None):
        """Get tool path
        """
        if use_esptool:
            tool_path = os.path.join(idf_path, 'components', 'esptool_py', 'esptool') + os.path.sep
        else:
            tool_path = os.path.join(idf_path, 'components', 'partition_table') + os.path.sep
        return tool_path

    def get_core_dump_partition_info(self, part_off=None, tool_path=None):
        """Get core dump partition info using parttool
        """
        logging.info("Retrieving core dump partition offset and size...")
        if not tool_path:
            tool_path = self.get_tool_path(use_esptool=False)
        if not part_off:
            part_off = self.ESP_COREDUMP_PART_TABLE_OFF
        size = None
        offset = None
        try:
            tool_args = [sys.executable, tool_path + 'parttool.py', "-q", "--partition-table-offset", str(part_off)]
            if self.port:
                tool_args.extend(['--port', self.port])
            invoke_args = tool_args + ["get_partition_info", "--partition-type", "data", "--partition-subtype", "coredump", "--info", "offset", "size"]
            (offset_str, size_str) = subprocess.check_output(invoke_args).strip().split(b" ")
            size = int(size_str, 16)
            offset = int(offset_str, 16)
            logging.info("Core dump partition offset=%d, size=%d", offset, size)
        except subprocess.CalledProcessError as e:
            logging.error("parttool get partition info failed with err %d" % e.returncode)
            logging.debug("Command ran: '%s'" % e.cmd)
            logging.debug("Command out:")
            logging.debug(e.output)
            logging.error("Check if the coredump partition exists in partition table.")
            raise e
        return (offset, size)

    def invoke_parttool(self, tool_path=None):
        """Loads core dump from flash using parttool
        """
        part_tool_args = [sys.executable, tool_path + 'parttool.py']
        if self.port:
            part_tool_args.extend(['--port', self.port])
        part_tool_args.extend(['read_partition', '--partition-type', 'data', '--partition-subtype', 'coredump', '--output'])
        self.fcore_name = None
        f = tempfile.NamedTemporaryFile(mode='w+b', delete=False)
        try:
            part_tool_args.append(f.name)
            self.fcore_name = f.name
            # read core dump partition
            et_out = subprocess.check_output(part_tool_args)
            if len(et_out):
                logging.info(et_out.decode('utf-8'))
            self.dump_sz = self._read_core_dump_length(f)
            f.seek(self.dump_sz)
            # cut free space of the partition
            f.truncate()
            f.seek(0)
        except subprocess.CalledProcessError as e:
            logging.error("parttool script execution failed with err %d" % e.returncode)
            logging.debug("Command ran: '%s'" % e.cmd)
            logging.debug("Command out:")
            logging.debug(e.output)
            if self.fcore_name:
                f.close()
                self.remove_tmp_file(self.fcore_name)
            raise e

        return f

    def invoke_esptool(self, tool_path=None, off=None):
        """Loads core dump from flash using elftool
        """
        tool_args = [sys.executable, tool_path + 'esptool.py', '-c', self.chip]
        if self.port:
            tool_args.extend(['-p', self.port])
        if self.baud:
            tool_args.extend(['-b', str(self.baud)])
        f = tempfile.NamedTemporaryFile(mode='w+b', delete=False)
        self.fcore_name = None
        try:
            (part_offset, part_size) = self.get_core_dump_partition_info(tool_path='')
            if not off:
                off = part_offset  # set default offset if not specified
                logging.warning("The core dump image offset is not specified. Use partition offset: %d.", part_offset)
            if part_offset != off:
                logging.warning("Predefined image offset: %d does not match core dump partition offset: %d", off, part_offset)
            tool_args.extend(['read_flash', str(off), str(self.ESP_COREDUMP_FLASH_LEN_SZ)])
            tool_args.append(f.name)
            self.fcore_name = f.name
            # read core dump length
            et_out = subprocess.check_output(tool_args)
            if len(et_out):
                logging.info(et_out.decode('utf-8'))
            self.dump_sz = self._read_core_dump_length(f)
            if self.dump_sz == 0 or self.dump_sz > part_size:
                logging.error("Incorrect size of core dump image: %d, use partition size instead: %d", self.dump_sz, part_size)
                self.dump_sz = part_size
            # set actual size of core dump image and read it from flash
            tool_args[-2] = str(self.dump_sz)
            et_out = subprocess.check_output(tool_args)
            if len(et_out):
                logging.info(et_out.decode('utf-8'))
        except subprocess.CalledProcessError as e:
            logging.error("esptool script execution failed with err %d" % e.returncode)
            logging.debug("Command ran: '%s'" % e.cmd)
            logging.debug("Command out:")
            logging.debug(e.output)
            if self.fcore_name:
                f.close()
                self.remove_tmp_file(self.fcore_name)
            raise e
        return f

    def _load_coredump(self, off=None):
        """Loads core dump from flash using parttool or elftool (if offset is set)
        """
        tool_path = None
        try:
            if off:
                tool_path = ''
                logging.info("Invoke esptool to read image.")
                f = self.invoke_esptool(tool_path=tool_path, off=off)
            else:
                tool_path = ''
                logging.info("Invoke parttool to read image.")
                f = self.invoke_parttool(tool_path=tool_path)
        except subprocess.CalledProcessError as e:
            if len(e.output):
                logging.info(e.output)
            logging.warning("System path is not set. Try to use predefined path.")
            if off:
                tool_path = self.get_tool_path(use_esptool=True)
                f = self.invoke_esptool(tool_path=tool_path, off=off)
            else:
                tool_path = self.get_tool_path(use_esptool=False)
                f = self.invoke_parttool(tool_path=tool_path)
        return f

    def _read_core_dump_length(self, f):
        """Reads core dump length
        """
        data = f.read(self.ESP_COREDUMP_FLASH_LEN_SZ)
        tot_len, = struct.unpack_from(self.ESP_COREDUMP_FLASH_LEN_FMT, data)
        return tot_len

    def create_corefile(self, core_fname=None, exe_name=None, rom_elf=None):
        """Checks flash coredump data integrity and creates ELF file
        """
        data = self.read_data(0, self.ESP_COREDUMP_HDR_SZ)
        self.checksum_len = 0
        _,coredump_ver_data,_,_,_ = struct.unpack_from(self.ESP_COREDUMP_HDR_FMT, data)
        self.set_version(coredump_ver_data)
        if self.chip_ver != ESPCoreDumpVersion.ESP_CORE_DUMP_CHIP_ESP32S2 and self.chip_ver != ESPCoreDumpVersion.ESP_CORE_DUMP_CHIP_ESP32:
            raise ESPCoreDumpLoaderError("Invalid core dump chip version: '%s', should be <= '0x%x'" % (self.chip_ver, self.ESP_CORE_DUMP_CHIP_ESP32S2))
        if self.dump_ver == self.ESP_COREDUMP_VERSION_ELF_CRC32 or self.dump_ver == self.ESP_COREDUMP_VERSION_BIN_V1 \
                or self.dump_ver == self.ESP_COREDUMP_VERSION_BIN_V2:
            logging.debug("Dump size = %d, crc off = 0x%x", self.dump_sz, self.dump_sz - self.ESP_COREDUMP_CRC_SZ)
            data = self.read_data(self.dump_sz - self.ESP_COREDUMP_CRC_SZ, self.ESP_COREDUMP_CRC_SZ)
            dump_crc, = struct.unpack_from(self.ESP_COREDUMP_CRC_FMT, data)
            data = self.read_data(0, self.dump_sz - self.ESP_COREDUMP_CRC_SZ)
            data_crc = binascii.crc32(data) & 0xffffffff
            if dump_crc != data_crc:
                raise ESPCoreDumpLoaderError("Invalid core dump CRC %x, should be %x" % (data_crc, dump_crc))
        elif self.dump_ver == self.ESP_COREDUMP_VERSION_ELF_SHA256:
            dump_sha256 = self.read_data(self.dump_sz - self.ESP_COREDUMP_SHA256_SZ, self.ESP_COREDUMP_SHA256_SZ)
            data = self.read_data(0, self.dump_sz - self.ESP_COREDUMP_SHA256_SZ)
            data_sha256 = sha256(data)
            data_sha256_str = data_sha256.hexdigest()
            dump_sha256_str = binascii.hexlify(dump_sha256).decode('ascii')
            if dump_sha256_str != data_sha256_str:
                raise ESPCoreDumpLoaderError("Invalid core dump SHA256 '%s', should be '%s'" % (dump_sha256_str, data_sha256_str))
        return super(ESPCoreDumpFlashLoader, self).create_corefile(core_fname, exe_name)


class GDBMIOutRecordHandler(object):
    """GDB/MI output record handler base class
    """
    TAG = ''

    def __init__(self, f, verbose=False):
        """Base constructor for GDB/MI output record handler
        """
        self.verbose = verbose

    def execute(self, ln):
        """Base method to execute GDB/MI output record handler function
        """
        if self.verbose:
            logging.debug("%s.execute: [[%s]]" % (self.__class__.__name__, ln))


class GDBMIOutStreamHandler(GDBMIOutRecordHandler):
    """GDB/MI output stream handler class
    """
    def __init__(self, f, verbose=False):
        """Constructor for GDB/MI output stream handler
        """
        super(GDBMIOutStreamHandler, self).__init__(None, verbose)
        self.func = f

    def execute(self, ln):
        """Executes GDB/MI output stream handler function
        """
        GDBMIOutRecordHandler.execute(self, ln)
        if self.func:
            # remove TAG / quotes and replace c-string \n with actual NL
            self.func(ln[1:].strip('"').replace('\\n', '\n').replace('\\t', '\t'))


class GDBMIResultHandler(GDBMIOutRecordHandler):
    """GDB/MI result handler class
    """
    TAG = '^'
    RC_DONE = 'done'
    RC_RUNNING = 'running'
    RC_CONNECTED = 'connected'
    RC_ERROR = 'error'
    RC_EXIT = 'exit'

    def __init__(self, verbose=False):
        """Constructor for GDB/MI result handler
        """
        super(GDBMIResultHandler, self).__init__(None, verbose)
        self.result_class = ''
        self.result_str = ''

    def _parse_rc(self, ln, rc):
        """Parses result code
        """
        rc_str = "{0}{1}".format(self.TAG, rc)
        if not ln.startswith(rc_str):
            return False
        self.result_class = rc
        if len(ln) > len(rc_str):
            self.result_str = ln[len(rc_str):]
            if self.result_str.startswith(','):
                self.result_str = self.result_str[1:]
            else:
                logging.error("Invalid result format: '%s'" % ln)
        else:
            self.result_str = ''
        return True

    def execute(self, ln):
        """Executes GDB/MI result handler function
        """
        GDBMIOutRecordHandler.execute(self, ln)
        if self._parse_rc(ln, self.RC_DONE):
            return
        if self._parse_rc(ln, self.RC_RUNNING):
            return
        if self._parse_rc(ln, self.RC_CONNECTED):
            return
        if self._parse_rc(ln, self.RC_ERROR):
            return
        if self._parse_rc(ln, self.RC_EXIT):
            return
        logging.error("Unknown GDB/MI result: '%s'" % ln)


class GDBMIThreadListIdsHandler(GDBMIResultHandler):
    """GDB/MI thread-list-ids handler class
    """
    def __init__(self, verbose=False):
        """Constructor for GDB/MI result handler
        """
        super(GDBMIThreadListIdsHandler, self).__init__(verbose)
        self.threads = []
        self.current_thread = ''

    def execute(self, ln):
        """Executes GDB/MI thread-list-ids handler function
        """
        GDBMIResultHandler.execute(self, ln)
        if self.result_class != self.RC_DONE:
            return
        # simple parsing method
        result = re.search(r'thread-ids\s*=\s*\{([^\{\}]*)\}', self.result_str)
        if result:
            for tid in re.finditer(r'thread-id="(\d+)"', result.group(1)):
                self.threads.append(tid.group(1))
        result = re.search(r'current-thread-id="(\d+)"', self.result_str)
        if result:
            self.current_thread = result.group(1)


class GDBMIThreadSelectHandler(GDBMIResultHandler):
    """GDB/MI thread-select handler class
    """
    def execute(self, ln):
        """Executes GDB/MI thread-select handler function
        """
        GDBMIResultHandler.execute(self, ln)
        if self.result_class != self.RC_DONE:
            return


class GDBMIThreadInfoHandler(GDBMIResultHandler):
    """GDB/MI thread-info handler class
    """
    def __init__(self, verbose=False):
        """Constructor for GDB/MI result handler
        """
        super(GDBMIThreadInfoHandler, self).__init__(verbose)
        self.current = False
        self.id = ''
        self.target_id = ''
        self.details = ''
        self.name = ''
        self.frame = ''
        self.state = ''
        self.core = ''

    def execute(self, ln):
        """Executes GDB/MI thread-info  handler function
        """
        GDBMIResultHandler.execute(self, ln)
        if self.result_class != self.RC_DONE:
            return
        # simple parsing method
        result = re.search(r'id="(\d+)"', self.result_str)
        if result:
            self.id = result.group(1)
        result = re.search(r'current="\*"', self.result_str)
        if result:
            self.current = True
        result = re.search(r'target-id="([^"]+)"', self.result_str)
        if result:
            self.target_id = result.group(1)


class GDBMIDataEvalHandler(GDBMIResultHandler):
    """GDB/MI data-evaluate-expression handler class
    """
    def __init__(self, verbose=False):
        """Constructor for GDB/MI result handler
        """
        super(GDBMIDataEvalHandler, self).__init__(verbose)
        self.value = ''

    def execute(self, ln):
        """Executes GDB/MI data-evaluate-expression handler function
        """
        GDBMIResultHandler.execute(self, ln)
        if self.result_class != self.RC_DONE:
            return
        # simple parsing method
        if self.verbose:
            logging.debug("GDBMIDataEvalHandler: result '%s'", self.result_str)
        pos = 0
        r = re.compile(r'([a-zA-Z_]+)=(.+)\,')
        while True:
            m = r.search(self.result_str, pos=pos)
            if not m:
                break
            if m.group(1) == 'value':
                if self.verbose:
                    logging.debug("GDBMIDataEvalHandler: found value = '%s'", m.group(2))
                self.value = self.result.group(1)
                return
            pos = m.end(2) + 1
        res_str = self.result_str[pos:]
        res_str = res_str.replace(r'\"', '\'')
        m = re.search(r'value="([^"]+)"', res_str)
        if m:
            if self.verbose:
                logging.debug("GDBMIDataEvalHandler: found value = '%s'", m.group(1))
            self.value = m.group(1)


class GDBMIStreamConsoleHandler(GDBMIOutStreamHandler):
    """GDB/MI console stream handler class
    """
    TAG = '~'


def load_aux_elf(elf_path):
    """ Loads auxilary ELF file and composes GDB command to read its symbols
    """
    elf = None
    sym_cmd = ''
    if os.path.exists(elf_path):
        elf = ESPCoreDumpElfFile(elf_path)
        for s in elf.sections:
            if s.name == '.text':
                sym_cmd = 'add-symbol-file %s 0x%x' % (elf_path, s.addr)
    return (elf, sym_cmd)


def dbg_corefile(args):
    """ Command to load core dump from file or flash and run GDB debug session with it
    """
    global CLOSE_FDS
    loader = None
    rom_elf,rom_sym_cmd = load_aux_elf(args.rom_elf)
    if not args.core:
        loader = ESPCoreDumpFlashLoader(args.off, port=args.port, baud=args.baud)
        core_fname = loader.create_corefile(args.save_core, exe_name=args.prog, rom_elf=rom_elf)
        if not core_fname:
            logging.error("Failed to create corefile!")
            loader.cleanup()
            return
    else:
        core_fname = args.core
        if args.core_format and args.core_format != 'elf':
            loader = ESPCoreDumpFileLoader(core_fname, args.core_format == 'b64')
            core_fname = loader.create_corefile(args.save_core, exe_name=args.prog, rom_elf=rom_elf)
            if not core_fname:
                logging.error("Failed to create corefile!")
                loader.cleanup()
                return

    p = subprocess.Popen(bufsize=0,
                         args=[args.gdb,
                               '--nw',  # ignore .gdbinit
                               '--core=%s' % core_fname,  # core file,
                               '-ex', rom_sym_cmd,
                               args.prog
                               ],
                         stdin=None, stdout=None, stderr=None,
                         close_fds=CLOSE_FDS
                         )
    p.wait()

    if loader:
        if not args.core and not args.save_core:
            loader.remove_tmp_file(core_fname)
        loader.cleanup()
    print('Done!')


def info_corefile(args):
    """ Command to load core dump from file or flash and print it's data in user friendly form
    """
    global CLOSE_FDS

    def gdbmi_console_stream_handler(ln):
        sys.stdout.write(ln)
        sys.stdout.flush()

    def gdbmi_read2prompt(f, out_handlers=None):
        while True:
            ln = f.readline().decode('utf-8').rstrip(' \r\n')
            if ln == '(gdb)':
                break
            elif len(ln) == 0:
                break
            elif out_handlers:
                for h in out_handlers:
                    if ln.startswith(out_handlers[h].TAG):
                        out_handlers[h].execute(ln)
                        break

    def gdbmi_start(handlers, gdb_cmds):
        gdb_args = [args.gdb,
                    '--quiet',  # inhibit dumping info at start-up
                    '--nx',  # inhibit window interface
                    '--nw',  # ignore .gdbinit
                    '--interpreter=mi2',  # use GDB/MI v2
                    '--core=%s' % core_fname]  # core file
        for c in gdb_cmds:
            gdb_args += ['-ex', c]
        gdb_args.append(args.prog)
        p = subprocess.Popen(bufsize=0,
                             args=gdb_args,
                             stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                             close_fds=CLOSE_FDS)
        gdbmi_read2prompt(p.stdout, handlers)
        return p

    def gdbmi_cmd_exec(p, handlers, gdbmi_cmd):
        for t in handlers:
            handlers[t].result_class = None
        p.stdin.write(bytearray("%s\n" % gdbmi_cmd, encoding='utf-8'))
        gdbmi_read2prompt(p.stdout, handlers)
        if not handlers[GDBMIResultHandler.TAG].result_class or handlers[GDBMIResultHandler.TAG].result_class == GDBMIResultHandler.RC_EXIT:
            logging.error("GDB exited (%s / %s)!" % (handlers[GDBMIResultHandler.TAG].result_class, handlers[GDBMIResultHandler.TAG].result_str))
            p.wait()
            logging.error("Problem occured! GDB exited, restart it.")
            p = gdbmi_start(handlers, [])
        elif handlers[GDBMIResultHandler.TAG].result_class != GDBMIResultHandler.RC_DONE:
            logging.error("GDB/MI command failed (%s / %s)!" % (handlers[GDBMIResultHandler.TAG].result_class, handlers[GDBMIResultHandler.TAG].result_str))
        return p

    def gdbmi_getinfo(p, handlers, gdb_cmd):
        return gdbmi_cmd_exec(p, handlers, "-interpreter-exec console \"%s\"" % gdb_cmd)

    def gdbmi_get_thread_ids(p):
        handlers = {}
        result = GDBMIThreadListIdsHandler(verbose=False)
        handlers[GDBMIResultHandler.TAG] = result
        handlers[GDBMIStreamConsoleHandler.TAG] = GDBMIStreamConsoleHandler(None, verbose=False)
        p = gdbmi_cmd_exec(p, handlers, "-thread-list-ids")
        return p,result.threads,result.current_thread

    def gdbmi_switch_thread(p, thr_id):
        handlers = {}
        result = GDBMIThreadSelectHandler(verbose=False)
        handlers[GDBMIResultHandler.TAG] = result
        handlers[GDBMIStreamConsoleHandler.TAG] = GDBMIStreamConsoleHandler(None, verbose=False)
        return gdbmi_cmd_exec(p, handlers, "-thread-select %s" % thr_id)

    def gdbmi_get_thread_info(p, thr_id):
        handlers = {}
        result = GDBMIThreadInfoHandler(verbose=False)
        handlers[GDBMIResultHandler.TAG] = result
        handlers[GDBMIStreamConsoleHandler.TAG] = GDBMIStreamConsoleHandler(None, verbose=False)
        if thr_id:
            cmd = "-thread-info %s" % thr_id
        else:
            cmd = "-thread-info"
        p = gdbmi_cmd_exec(p, handlers, cmd)
        return p,result

    def gdbmi_data_evaluate_expression(p, expr):
        handlers = {}
        result = GDBMIDataEvalHandler(verbose=False)
        handlers[GDBMIResultHandler.TAG] = result
        handlers[GDBMIStreamConsoleHandler.TAG] = GDBMIStreamConsoleHandler(None, verbose=False)
        p = gdbmi_cmd_exec(p, handlers, "-data-evaluate-expression \"%s\"" % expr)
        return p,result

    def gdbmi_freertos_get_task_name(p, tcb_addr):
        p,res = gdbmi_data_evaluate_expression(p, "(char*)((TCB_t *)0x%x)->pcTaskName" % tcb_addr)
        result = re.match("0x[a-fA-F0-9]+[^']*'([^']*)'", res.value)
        if result:
            return p,result.group(1)
        return p,''

    def gdb2freertos_thread_id(gdb_thread_id):
        return int(gdb_thread_id.replace("process ", ""), 0)

    loader = None
    rom_elf,rom_sym_cmd = load_aux_elf(args.rom_elf)
    if not args.core:
        loader = ESPCoreDumpFlashLoader(args.off, port=args.port, baud=args.baud)
        core_fname = loader.create_corefile(args.save_core, exe_name=args.prog, rom_elf=rom_elf)
        if not core_fname:
            logging.error("Failed to create corefile!")
            loader.cleanup()
            return
    else:
        core_fname = args.core
        if args.core_format and args.core_format != 'elf':
            loader = ESPCoreDumpFileLoader(core_fname, args.core_format == 'b64')
            core_fname = loader.create_corefile(args.save_core, exe_name=args.prog, rom_elf=rom_elf)
            if not core_fname:
                logging.error("Failed to create corefile!")
                loader.cleanup()
                return

    exe_elf = ESPCoreDumpElfFile(args.prog)
    core_elf = ESPCoreDumpElfFile(core_fname)
    merged_segs = []
    core_segs = core_elf.program_segments
    for s in exe_elf.sections:
        merged = False
        for ps in core_segs:
            if ps.addr <= s.addr and ps.addr + len(ps.data) >= s.addr:
                # sec:    |XXXXXXXXXX|
                # seg: |...XXX.............|
                seg_addr = ps.addr
                if ps.addr + len(ps.data) <= s.addr + len(s.data):
                    # sec:        |XXXXXXXXXX|
                    # seg:    |XXXXXXXXXXX...|
                    # merged: |XXXXXXXXXXXXXX|
                    seg_len = len(s.data) + (s.addr - ps.addr)
                else:
                    # sec:        |XXXXXXXXXX|
                    # seg:    |XXXXXXXXXXXXXXXXX|
                    # merged: |XXXXXXXXXXXXXXXXX|
                    seg_len = len(ps.data)
                merged_segs.append((s.name, seg_addr, seg_len, s.attr_str(), True))
                core_segs.remove(ps)
                merged = True
            elif ps.addr >= s.addr and ps.addr <= s.addr + len(s.data):
                # sec:  |XXXXXXXXXX|
                # seg:  |...XXX.............|
                seg_addr = s.addr
                if (ps.addr + len(ps.data)) >= (s.addr + len(s.data)):
                    # sec:    |XXXXXXXXXX|
                    # seg:    |..XXXXXXXXXXX|
                    # merged: |XXXXXXXXXXXXX|
                    seg_len = len(s.data) + (ps.addr + len(ps.data)) - (s.addr + len(s.data))
                else:
                    # sec:    |XXXXXXXXXX|
                    # seg:      |XXXXXX|
                    # merged: |XXXXXXXXXX|
                    seg_len = len(s.data)
                merged_segs.append((s.name, seg_addr, seg_len, s.attr_str(), True))
                core_segs.remove(ps)
                merged = True

        if not merged:
            merged_segs.append((s.name, s.addr, len(s.data), s.attr_str(), False))

    handlers = {}
    handlers[GDBMIResultHandler.TAG] = GDBMIResultHandler(verbose=False)
    handlers[GDBMIStreamConsoleHandler.TAG] = GDBMIStreamConsoleHandler(None, verbose=False)
    p = gdbmi_start(handlers, [rom_sym_cmd])
    extra_note = None
    task_info = []
    for seg in core_elf.aux_segments:
        if seg.type != ESPCoreDumpElfFile.PT_NOTE:
            continue
        note_read = 0
        while note_read < len(seg.data):
            note = Elf32NoteDesc("", 0, None)
            note_read += note.read(seg.data[note_read:])
            if note.type == ESPCoreDumpLoader.ESP_CORE_DUMP_EXTRA_INFO_TYPE and 'EXTRA_INFO' in note.name:
                extra_note = note
            if note.type == ESPCoreDumpLoader.ESP_CORE_DUMP_TASK_INFO_TYPE and 'TASK_INFO' in note.name:
                task_info_struct = EspCoreDumpTaskStatus(buf=note.desc)
                task_info.append(task_info_struct)
    print("===============================================================")
    print("==================== ESP32 CORE DUMP START ====================")

    handlers[GDBMIResultHandler.TAG].result_class = None
    handlers[GDBMIStreamConsoleHandler.TAG].func = gdbmi_console_stream_handler
    if extra_note:
        extra_info = struct.unpack("<%dL" % (len(extra_note.desc) / struct.calcsize("<L")), extra_note.desc)
        if extra_info[0] == ESPCoreDumpLoader.ESP_COREDUMP_CURR_TASK_MARKER:
            print("\nCrashed task has been skipped.")
        else:
            p,task_name = gdbmi_freertos_get_task_name(p, extra_info[0])
            print("\nCrashed task handle: 0x%x, name: '%s', GDB name: 'process %d'" % (extra_info[0], task_name, extra_info[0]))
    print("\n================== CURRENT THREAD REGISTERS ===================")
    if extra_note:
        exccause = extra_info[1 + 2 * ESPCoreDumpElfFile.REG_EXCCAUSE_IDX + 1]
        exccause_str = xtensa_exception_cause_dict.get(exccause)
        if not exccause_str:
            exccause_str = ("Invalid EXCCAUSE code", "Invalid EXCAUSE description or not found.")
        print("exccause       0x%x (%s)" % (exccause, exccause_str[0]))
        print("excvaddr       0x%x" % extra_info[1 + 2 * ESPCoreDumpElfFile.REG_EXCVADDR_IDX + 1])

        # skip crashed_task_tcb, exccause, and excvaddr
        for i in range(5, len(extra_info), 2):
            if (extra_info[i] >= ESPCoreDumpElfFile.REG_EPC1_IDX and extra_info[i] <= ESPCoreDumpElfFile.REG_EPC7_IDX):
                print('epc%d           0x%x' % ((extra_info[i] - ESPCoreDumpElfFile.REG_EPC1_IDX + 1), extra_info[i + 1]))

        # skip crashed_task_tcb, exccause, and excvaddr
        for i in range(5, len(extra_info), 2):
            if (extra_info[i] >= ESPCoreDumpElfFile.REG_EPS2_IDX and extra_info[i] <= ESPCoreDumpElfFile.REG_EPS7_IDX):
                print('eps%d           0x%x' % ((extra_info[i] - ESPCoreDumpElfFile.REG_EPS2_IDX + 2), extra_info[i + 1]))
    else:
        print("Exception registers have not been found!")
    p = gdbmi_getinfo(p, handlers, "info registers")
    print("\n==================== CURRENT THREAD STACK =====================")
    p = gdbmi_getinfo(p, handlers, "bt")
    if task_info and task_info[0].task_flags != EspCoreDumpTaskStatus.TASK_STATUS_CORRECT:
        print("The current crashed task is corrupted.")
        print("Task #%d info: flags, tcb, stack (%x, %x, %x)." % (task_info[0].task_index,
              task_info[0].task_flags,
              task_info[0].task_tcb_addr,
              task_info[0].task_stack_start))
    print("\n======================== THREADS INFO =========================")
    p = gdbmi_getinfo(p, handlers, "info threads")
    # THREADS STACKS
    p,threads,cur_thread = gdbmi_get_thread_ids(p)
    print()
    for thr_id in threads:
        task_index = int(thr_id) - 1
        p = gdbmi_switch_thread(p, thr_id)
        p,thr_info_res = gdbmi_get_thread_info(p, thr_id)
        if not thr_info_res.target_id:
            print("WARNING: Unable to switch to thread %s\n" % thr_id)
            continue
        tcb_addr = gdb2freertos_thread_id(thr_info_res.target_id)
        p,task_name = gdbmi_freertos_get_task_name(p, tcb_addr)
        print("==================== THREAD %s (TCB: 0x%x, name: '%s') =====================" % (thr_id, tcb_addr, task_name))
        p = gdbmi_getinfo(p, handlers, "bt")
        if task_info and task_info[task_index].task_flags != EspCoreDumpTaskStatus.TASK_STATUS_CORRECT:
            print("The task '%s' is corrupted." % thr_id)
            print("Task #%d info: flags, tcb, stack (%x, %x, %x)." % (task_info[task_index].task_index,
                  task_info[task_index].task_flags,
                  task_info[task_index].task_tcb_addr,
                  task_info[task_index].task_stack_start))
        print()
    print("\n======================= ALL MEMORY REGIONS ========================")
    print("Name   Address   Size   Attrs")
    for ms in merged_segs:
        print("%s 0x%x 0x%x %s" % (ms[0], ms[1], ms[2], ms[3]))
    for cs in core_segs:
        # core dump exec segments are from ROM, other are belong to tasks (TCB or stack)
        if cs.flags & ESPCoreDumpSegment.PF_X:
            seg_name = 'rom.text'
        else:
            seg_name = 'tasks.data'
        print(".coredump.%s 0x%x 0x%x %s" % (seg_name, cs.addr, len(cs.data), cs.attr_str()))
    if args.print_mem:
        print("\n====================== CORE DUMP MEMORY CONTENTS ========================")
        for cs in core_elf.program_segments:
            # core dump exec segments are from ROM, other are belong to tasks (TCB or stack)
            if cs.flags & ESPCoreDumpSegment.PF_X:
                seg_name = 'rom.text'
            else:
                seg_name = 'tasks.data'
            print(".coredump.%s 0x%x 0x%x %s" % (seg_name, cs.addr, len(cs.data), cs.attr_str()))
            p = gdbmi_getinfo(p, handlers, "x/%dx 0x%x" % (old_div(len(cs.data),4), cs.addr))

    print("\n===================== ESP32 CORE DUMP END =====================")
    print("===============================================================")

    p.stdin.write(b'q\n')
    p.wait()
    p.stdin.close()
    p.stdout.close()

    if loader:
        if not args.core and not args.save_core:
            loader.remove_tmp_file(core_fname)
        loader.cleanup()
    print('Done!')


def main():
    parser = argparse.ArgumentParser(description='espcoredump.py v%s - ESP32 Core Dump Utility' % __version__, prog='espcoredump')

    parser.add_argument('--chip', '-c',
                        help='Target chip type',
                        choices=['auto', 'esp32'],
                        default=os.environ.get('ESPTOOL_CHIP', 'auto'))

    parser.add_argument(
        '--port', '-p',
        help='Serial port device',
        default=os.environ.get('ESPTOOL_PORT', esptool.ESPLoader.DEFAULT_PORT))

    parser.add_argument(
        '--baud', '-b',
        help='Serial port baud rate used when flashing/reading',
        type=int,
        default=os.environ.get('ESPTOOL_BAUD', esptool.ESPLoader.ESP_ROM_BAUD))

    subparsers = parser.add_subparsers(
        dest='operation',
        help='Run coredumper {command} -h for additional help')

    parser_debug_coredump = subparsers.add_parser(
        'dbg_corefile',
        help='Starts GDB debugging session with specified corefile')
    parser_debug_coredump.add_argument('--debug', '-d', help='Log level (0..3)', type=int, default=3)
    parser_debug_coredump.add_argument('--gdb', '-g', help='Path to gdb', default='xtensa-esp32-elf-gdb')
    parser_debug_coredump.add_argument('--core', '-c', help='Path to core dump file (if skipped core dump will be read from flash)', type=str)
    parser_debug_coredump.add_argument('--core-format', '-t', help='(elf, raw or b64). File specified with "-c" is an ELF ("elf"), '
                                                                   'raw (raw) or base64-encoded (b64) binary',
                                                                   choices=['b64', 'elf', 'raw'], type=str, default='elf')
    parser_debug_coredump.add_argument('--off', '-o', help='Ofsset of coredump partition in flash '
                                                           '(type "make partition_table" to see).', type=int, default=None)
    parser_debug_coredump.add_argument('--save-core', '-s', help='Save core to file. Othwerwise temporary core file will be deleted. '
                                                                 'Ignored with "-c"', type=str)
    parser_debug_coredump.add_argument('--rom-elf', '-r', help='Path to ROM ELF file.', type=str, default='esp32_rom.elf')
    parser_debug_coredump.add_argument('prog', help='Path to program\'s ELF binary', type=str)

    parser_info_coredump = subparsers.add_parser(
        'info_corefile',
        help='Print core dump info from file')
    parser_info_coredump.add_argument('--debug', '-d', help='Log level (0..3)', type=int, default=3)
    parser_info_coredump.add_argument('--gdb', '-g', help='Path to gdb', default='xtensa-esp32-elf-gdb')
    parser_info_coredump.add_argument('--core', '-c', help='Path to core dump file (if skipped core dump will be read from flash)', type=str)
    parser_info_coredump.add_argument('--core-format', '-t', help='(elf, raw or b64). File specified with "-c" is an ELF ("elf"), '
                                                                  'raw (raw) or base64-encoded (b64) binary',
                                                                  choices=['b64', 'elf', 'raw'], type=str, default='elf')
    parser_info_coredump.add_argument('--off', '-o', help='Offset of coredump partition in flash (type '
                                                          '"make partition_table" to see).', type=int, default=None)
    parser_info_coredump.add_argument('--save-core', '-s', help='Save core to file. Othwerwise temporary core file will be deleted. '
                                                                'Does not work with "-c"', type=str)
    parser_info_coredump.add_argument('--rom-elf', '-r', help='Path to ROM ELF file.', type=str, default='esp32_rom.elf')
    parser_info_coredump.add_argument('--print-mem', '-m', help='Print memory dump', action='store_true')
    parser_info_coredump.add_argument('prog', help='Path to program\'s ELF binary', type=str)

    # internal sanity check - every operation matches a module function of the same name
    for operation in subparsers.choices:
        assert operation in globals(), "%s should be a module function" % operation

    args = parser.parse_args()

    log_level = logging.CRITICAL
    if args.debug == 0:
        log_level = logging.CRITICAL
    elif args.debug == 1:
        log_level = logging.ERROR
    elif args.debug == 2:
        log_level = logging.WARNING
    elif args.debug == 3:
        log_level = logging.INFO
    else:
        log_level = logging.DEBUG
    logging.basicConfig(format='%(levelname)s: %(message)s', level=log_level)

    print('espcoredump.py v%s' % __version__)

    operation_func = globals()[args.operation]
    operation_func(args)


if __name__ == '__main__':
    try:
        main()
    except ESPCoreDumpError as e:
        print('\nA fatal error occurred: %s' % e)
        sys.exit(2)
