#!/usr/bin/env python
#
# ESP32 core dump Utility

import sys
import os
import argparse
import subprocess
import tempfile
import struct
import array
import errno
import base64

idf_path = os.getenv('IDF_PATH')
if idf_path:
    sys.path.insert(0, os.path.join(idf_path, 'components', 'esptool_py', 'esptool'))

try:
    import esptool
except ImportError:
    print "Esptool is not found! Set proper $IDF_PATH in environment."
    sys.exit(2)

__version__ = "0.1-dev"

if os.name == 'nt':
    CLOSE_FDS = False
else:
    CLOSE_FDS = True


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
        keys =  self.__class__.fields
        return struct.pack(self.__class__.format, *(self.__dict__[k] for k in keys))

#     def __str__(self):
#         keys =  self.__class__.fields
#         return (self.__class__.__name__ + "({" +
#             ", ".join("%s:%r" % (k, self.__dict__[k]) for k in keys) +
#             "})")


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
            self.e_ident = "\x7fELF\1\1\1\0\0\0\0\0\0\0\0\0"
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
        self.name = bytearray(name, encoding='ascii') + b'\0'
        self.type = type
        self.desc = desc

    def dump(self):
        """Returns binary representation of ELF32 note descriptor
        """
        hdr = struct.pack("<LLL", len(self.name), len(self.desc), self.type)
        # pad for 4 byte alignment
        name = self.name + ((4 - len(self.name)) % 4) * b'\0'
        desc = self.desc + ((4 - len(self.desc)) % 4) * b'\0'
        return hdr + name + desc


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


class ESPCoreDumpSegment(esptool.ImageSegment):
    """ Wrapper class for a program segment in core ELF file, has a segment
        type and flags as well as the common properties of an ImageSegment. 
    """
    # segment flags
    PF_X = 0x1 # Execute
    PF_W = 0x2 # Write
    PF_R = 0x4 # Read

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
    # ELF file type
    ET_NONE             = 0x0 # No file type
    ET_REL              = 0x1 # Relocatable file
    ET_EXEC             = 0x2 # Executable file
    ET_DYN              = 0x3 # Shared object file
    ET_CORE             = 0x4 # Core file
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
            self.e_type = self.ET_NONE
            self.e_machine = self.EM_NONE

    def _read_elf_file(self, f):
        """Reads core dump from ELF file
        """
        # read the ELF file header
        LEN_FILE_HEADER = 0x34
        try:
            (ident,type,machine,_version,
             self.entrypoint,phoff,shoff,_flags,
             _ehsize, phentsize,phnum,_shentsize,
             shnum,shstrndx) = struct.unpack("<16sHHLLLLLHHHHHH", f.read(LEN_FILE_HEADER))
        except struct.error as e:
            raise ESPCoreDumpError("Failed to read a valid ELF header from %s: %s" % (self.name, e))

        if ident[0] != '\x7f' or ident[1:4] != 'ELF':
            raise ESPCoreDumpError("%s has invalid ELF magic header" % self.name)
        if machine != self.EM_XTENSA:
            raise ESPCoreDumpError("%s does not appear to be an Xtensa ELF file. e_machine=%04x" % (self.name, machine))
        self.e_type = type
        self.e_machine = machine
        if shnum > 0:
            self._read_sections(f, shoff, shstrndx)
        else:
            self.sections = []
            if phnum > 0:
                self._read_program_segments(f, phoff, phentsize, phnum)
            else:
                self.program_segments = []

    def _read_sections(self, f, section_header_offs, shstrndx):
        """Reads core dump sections from ELF file
        """
        f.seek(section_header_offs)
        section_header = f.read()
        LEN_SEC_HEADER = 0x28
        if len(section_header) == 0:
            raise ESPCoreDumpError("No section header found at offset %04x in ELF file." % section_header_offs)
        if len(section_header) % LEN_SEC_HEADER != 0:
            print 'WARNING: Unexpected ELF section header length %04x is not mod-%02x' % (len(section_header),LEN_SEC_HEADER)

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
            print 'WARNING: ELF file has incorrect STRTAB section type 0x%02x' % sec_type
        f.seek(sec_offs)
        string_table = f.read(sec_size)

        # build the real list of ELFSections by reading the actual section names from the
        # string table section, and actual data for each section from the ELF file itself
        def lookup_string(offs):
            raw = string_table[offs:]
            return raw[:raw.index('\x00')]

        def read_data(offs,size):
            f.seek(offs)
            return f.read(size)

        prog_sections = [ESPCoreDumpSection(lookup_string(n_offs), lma, read_data(offs, size), flags) for (n_offs, _type, flags, lma, size, offs) in prog_sections
                         if lma != 0]
        self.sections = prog_sections

    def _read_program_segments(self, f, seg_table_offs, entsz, num):
        """Reads core dump program segments from ELF file
        """
        f.seek(seg_table_offs)
        seg_table = f.read(entsz*num)
        LEN_SEG_HEADER = 0x20
        if len(seg_table) == 0:
            raise ESPCoreDumpError("No program header table found at offset %04x in ELF file." % seg_table_offs)
        if len(seg_table) % LEN_SEG_HEADER != 0:
            print 'WARNING: Unexpected ELF program header table length %04x is not mod-%02x' % (len(seg_table),LEN_SEG_HEADER)

        # walk through the program segment table and extract all segments
        seg_table_offs = range(0, len(seg_table), LEN_SEG_HEADER)

        def read_program_header(offs):
            type,offset,vaddr,_paddr,filesz,_memsz,flags,_align = struct.unpack_from("<LLLLLLLL", seg_table[offs:])
            return (type,offset,vaddr,filesz,flags)
        all_segments = [read_program_header(offs) for offs in seg_table_offs]
        prog_segments = [s for s in all_segments if s[0] == self.PT_LOAD]

        # build the real list of ImageSegment by reading actual data for each segment from the ELF file itself
        def read_data(offs,size):
            f.seek(offs)
            return f.read(size)

        prog_segments = [ESPCoreDumpSegment(vaddr, read_data(offset, filesz), type, flags) for (type, offset, vaddr, filesz,flags) in prog_segments
                         if vaddr != 0]
        self.program_segments = prog_segments

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
        ehdr.e_phnum = len(self.program_segments)
        ehdr.e_shentsize = 0
        ehdr.e_shnum = 0
        ehdr.e_shstrndx = self.SHN_UNDEF
        f.write(ehdr.dump())
        # write program header table
        cur_off = ehdr.e_ehsize + ehdr.e_phnum * ehdr.e_phentsize
        for i in range(len(self.program_segments)):
            phdr = Elf32ProgramHeader()
            phdr.p_type = self.program_segments[i].type
            phdr.p_offset = cur_off
            phdr.p_vaddr = self.program_segments[i].addr
            phdr.p_paddr = phdr.p_vaddr # TODO
            phdr.p_filesz = len(self.program_segments[i].data)
            phdr.p_memsz = phdr.p_filesz # TODO
            phdr.p_flags = self.program_segments[i].flags
            phdr.p_align = 0 # TODO
            f.write(phdr.dump())
            cur_off += phdr.p_filesz
        # write program segments
        for i in range(len(self.program_segments)):
            f.write(self.program_segments[i].data)


class ESPCoreDumpLoaderError(ESPCoreDumpError):
    """Core dump loader error class
    """
    def __init__(self, message):
        """Constructor for core dump loader error
        """
        super(ESPCoreDumpLoaderError, self).__init__(message)


class ESPCoreDumpLoader(object):
    """Core dump loader base class
    """
    ESP32_COREDUMP_HDR_FMT      = '<3L'
    ESP32_COREDUMP_HDR_SZ       = struct.calcsize(ESP32_COREDUMP_HDR_FMT)
    ESP32_COREDUMP_TSK_HDR_FMT  = '<3L'
    ESP32_COREDUMP_TSK_HDR_SZ   = struct.calcsize(ESP32_COREDUMP_TSK_HDR_FMT)
    
    def __init__(self):
        """Base constructor for core dump loader
        """
        self.fcore = None

    def _get_registers_from_stack(self, data, grows_down):
        """Returns list of registers (in GDB format) from xtensa stack frame
        """
        # from "gdb/xtensa-tdep.h"
        # typedef struct
        # {
        #0    xtensa_elf_greg_t pc;
        #1    xtensa_elf_greg_t ps;
        #2    xtensa_elf_greg_t lbeg;
        #3    xtensa_elf_greg_t lend;
        #4    xtensa_elf_greg_t lcount;
        #5    xtensa_elf_greg_t sar;
        #6    xtensa_elf_greg_t windowstart;
        #7    xtensa_elf_greg_t windowbase;
        #8..63 xtensa_elf_greg_t reserved[8+48];
        #64   xtensa_elf_greg_t ar[64];
        # } xtensa_elf_gregset_t;
        REG_PC_IDX=0
        REG_PS_IDX=1
        REG_LB_IDX=2
        REG_LE_IDX=3
        REG_LC_IDX=4
        REG_SAR_IDX=5
        REG_WS_IDX=6
        REG_WB_IDX=7
        REG_AR_START_IDX=64
        REG_AR_NUM=64
        # FIXME: acc to xtensa_elf_gregset_t number of regs must be 128, 
        # but gdb complanis when it less then 129
        REG_NUM=129

        XT_SOL_EXIT=0
        XT_SOL_PC=1
        XT_SOL_PS=2
        XT_SOL_NEXT=3
        XT_SOL_AR_START=4
        XT_SOL_AR_NUM=4
        XT_SOL_FRMSZ=8

        XT_STK_EXIT=0
        XT_STK_PC=1
        XT_STK_PS=2
        XT_STK_AR_START=3
        XT_STK_AR_NUM=16
        XT_STK_SAR=19
        XT_STK_EXCCAUSE=20
        XT_STK_EXCVADDR=21
        XT_STK_LBEG=22
        XT_STK_LEND=23
        XT_STK_LCOUNT=24
        XT_STK_FRMSZ=25

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
        else:
            regs[REG_PC_IDX] = stack[XT_SOL_PC]
            regs[REG_PS_IDX] = stack[XT_SOL_PS]
            for i in range(XT_SOL_AR_NUM):
                regs[REG_AR_START_IDX + i] = stack[XT_SOL_AR_START + i]
            nxt = stack[XT_SOL_NEXT]
             
        # TODO: remove magic hack with saved PC to get proper value
        regs[REG_PC_IDX] = ((regs[REG_PC_IDX] & 0x3FFFFFFF) | 0x40000000)
        if regs[REG_PC_IDX] & 0x80000000:
            regs[REG_PC_IDX] = (regs[REG_PC_IDX] & 0x3fffffff) | 0x40000000;
        if regs[REG_AR_START_IDX + 0] & 0x80000000:
            regs[REG_AR_START_IDX + 0] = (regs[REG_AR_START_IDX + 0] & 0x3fffffff) | 0x40000000;
        return regs

    def remove_tmp_file(self, fname):
        """Silently removes temporary file
        """
        try:
            os.remove(fname)
        except OSError as e:
            if e.errno != errno.ENOENT:
                print "Warning failed to remove temp file '%s' (%d)!" % (fname, e.errno)

    def cleanup(self):
        """Cleans up loader resources
        """
        if self.fcore:
            self.fcore.close()
            if self.fcore_name:
                self.remove_tmp_file(self.fcore_name)

    def create_corefile(self, core_fname=None, off=0):
        """Creates core dump ELF file
        """
        core_off = off
        data = self.read_data(core_off, self.ESP32_COREDUMP_HDR_SZ)
        tot_len,task_num,tcbsz = struct.unpack_from(self.ESP32_COREDUMP_HDR_FMT, data)
        tcbsz_aligned = tcbsz
        if tcbsz_aligned % 4:
            tcbsz_aligned = 4*(tcbsz_aligned/4 + 1)
        core_off += self.ESP32_COREDUMP_HDR_SZ
        core_elf = ESPCoreDumpElfFile()
        notes = b''
        for i in range(task_num):
            data = self.read_data(core_off, self.ESP32_COREDUMP_TSK_HDR_SZ)
            tcb_addr,stack_top,stack_end = struct.unpack_from(self.ESP32_COREDUMP_TSK_HDR_FMT, data)
            if stack_end > stack_top:
                stack_len = stack_end - stack_top
                stack_base = stack_top
            else:
                stack_len = stack_top - stack_end
                stack_base = stack_end
    
            stack_len_aligned = stack_len
            if stack_len_aligned % 4:
                stack_len_aligned = 4*(stack_len_aligned/4 + 1)
                
            core_off += self.ESP32_COREDUMP_TSK_HDR_SZ
            data = self.read_data(core_off, tcbsz_aligned)
            if tcbsz != tcbsz_aligned:
                core_elf.add_program_segment(tcb_addr, data[:tcbsz - tcbsz_aligned], ESPCoreDumpElfFile.PT_LOAD, ESPCoreDumpSegment.PF_R | ESPCoreDumpSegment.PF_W)
            else:
                core_elf.add_program_segment(tcb_addr, data, ESPCoreDumpElfFile.PT_LOAD, ESPCoreDumpSegment.PF_R | ESPCoreDumpSegment.PF_W)
            core_off += tcbsz_aligned
            data = self.read_data(core_off, stack_len_aligned)
            if stack_len != stack_len_aligned:
                data = data[:stack_len - stack_len_aligned]
            core_elf.add_program_segment(stack_base, data, ESPCoreDumpElfFile.PT_LOAD, ESPCoreDumpSegment.PF_R | ESPCoreDumpSegment.PF_W)
            core_off += stack_len_aligned
            try:
                task_regs = self._get_registers_from_stack(data, stack_end > stack_top)
            except Exception as e:
                print e
                return None
            prstatus = XtensaPrStatus()
            prstatus.pr_cursig = 0 # TODO: set sig only for current/failed task
            prstatus.pr_pid = i # TODO: use pid assigned by OS
            note = Elf32NoteDesc("CORE", 1, prstatus.dump() + struct.pack("<%dL" % len(task_regs), *task_regs)).dump()
            notes += note

        # add notes
        core_elf.add_program_segment(0, notes, ESPCoreDumpElfFile.PT_NOTE, 0)
    
        core_elf.e_type = ESPCoreDumpElfFile.ET_CORE
        core_elf.e_machine = ESPCoreDumpElfFile.EM_XTENSA
        if core_fname:
            fce = open(core_fname, 'wb')
        else:
            fhnd,core_fname = tempfile.mkstemp()
            fce = os.fdopen(fhnd, 'wb')
        core_elf.dump(fce)
        fce.close()
        return core_fname

    def read_data(self, off, sz):
        """Reads data from raw core dump got from flash or UART
        """
        self.fcore.seek(off)
        data = self.fcore.read(sz)
        return data


class ESPCoreDumpFileLoader(ESPCoreDumpLoader):
    """Core dump file loader class
    """
    def __init__(self, path, b64 = False):
        """Constructor for core dump file loader
        """
        super(ESPCoreDumpFileLoader, self).__init__()
        self.fcore = self._load_coredump(path, b64)

    def _load_coredump(self, path, b64):
        """Loads core dump from (raw binary or base64-encoded) file
        """
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
                    data = base64.standard_b64decode(line.rstrip('\r\n'))
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
    ESP32_COREDUMP_FLASH_MAGIC_START    = 0xE32C04ED
    ESP32_COREDUMP_FLASH_MAGIC_END      = 0xE32C04ED
    ESP32_COREDUMP_FLASH_MAGIC_FMT      = '<L'
    ESP32_COREDUMP_FLASH_MAGIC_SZ       = struct.calcsize(ESP32_COREDUMP_FLASH_MAGIC_FMT)
    ESP32_COREDUMP_FLASH_HDR_FMT        = '<4L'
    ESP32_COREDUMP_FLASH_HDR_SZ         = struct.calcsize(ESP32_COREDUMP_FLASH_HDR_FMT)

    def __init__(self, off, tool_path=None, chip='esp32', port=None, baud=None):
        """Constructor for core dump flash loader
        """
        super(ESPCoreDumpFlashLoader, self).__init__()
        if not tool_path:
            self.path = esptool.__file__
            _,e = os.path.splitext(self.path)
            if e == '.pyc':
                self.path = self.path[:-1]
        else:
            self.path =  tool_path
        self.port = port
        self.baud = baud
        self.chip = chip
        self.dump_sz = 0
        self.fcore = self._load_coredump(off)

    def _load_coredump(self, off):
        """Loads core dump from flash
        """
        tool_args = [sys.executable, self.path, '-c', self.chip]
        if self.port:
            tool_args.extend(['-p', self.port])
        if self.baud:
            tool_args.extend(['-b', str(self.baud)])
        tool_args.extend(['read_flash', str(off), str(self.ESP32_COREDUMP_FLASH_HDR_SZ), ''])

        self.fcore_name = None
        try:
            fhnd,self.fcore_name = tempfile.mkstemp()
            tool_args[-1] = self.fcore_name
            # read core dump length
            et_out = subprocess.check_output(tool_args)
            print et_out
            f = os.fdopen(fhnd, 'rb')
            self.dump_sz = self._read_core_dump_length(f)
            # read core dump
            tool_args[-2] = str(self. dump_sz)
            et_out = subprocess.check_output(tool_args)
            print et_out
        except subprocess.CalledProcessError as e: 
            print "esptool script execution failed with err %d" % e.returncode
            print "Command ran: '%s'" % e.cmd
            print "Command out:"
            print e.output
            if self.fcore_name:
                self.remove_tmp_file(self.fcore_name)
            raise e
        return f

    def _read_core_dump_length(self, f):
        """Reads core dump length
        """
        data = f.read(4*4)
        mag1,tot_len,task_num,tcbsz = struct.unpack_from(self.ESP32_COREDUMP_FLASH_HDR_FMT, data)
        if mag1 != self.ESP32_COREDUMP_FLASH_MAGIC_START:
            raise ESPCoreDumpLoaderError("Invalid start magic number!")
        return tot_len

    def create_corefile(self, core_fname=None):
        """Checks flash coredump data integrity and creates ELF file
        """
        data = self.read_data(0, self.ESP32_COREDUMP_FLASH_MAGIC_SZ)
        mag1, = struct.unpack_from(self.ESP32_COREDUMP_FLASH_MAGIC_FMT, data)
        if mag1 != self.ESP32_COREDUMP_FLASH_MAGIC_START:
            raise ESPCoreDumpLoaderError("Invalid start marker %x" % mag1)

        data = self.read_data(self.dump_sz-self.ESP32_COREDUMP_FLASH_MAGIC_SZ, self.ESP32_COREDUMP_FLASH_MAGIC_SZ)
        mag2, = struct.unpack_from(self.ESP32_COREDUMP_FLASH_MAGIC_FMT, data)
        if mag2 != self.ESP32_COREDUMP_FLASH_MAGIC_END:
            raise ESPCoreDumpLoaderError("Invalid end marker %x" % mag2)
        
        return super(ESPCoreDumpFlashLoader, self).create_corefile(core_fname, off=self.ESP32_COREDUMP_FLASH_MAGIC_SZ)


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
            print "%s.execute: [[%s]]" % (self.__class__.__name__, ln)


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
        self.result_class = None
        self.result_str = None

    def _parse_rc(self, ln, rc):
        """Parses result code
        """
        rc_str = "{0}{1}".format(self.TAG, rc)
        if ln.startswith(rc_str):
            self.result_class = rc
            sl = len(rc_str)
            if len(ln) > sl:
                self.result_str = ln[sl:]
                if self.result_str.startswith(','):
                    self.result_str = self.result_str[1:]
                else:
                    print "Invalid result format: '%s'" % ln
            else:
                self.result_str = ''
            return True
        return False

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
        print "Unknown GDB/MI result: '%s'" % ln


class GDBMIStreamConsoleHandler(GDBMIOutStreamHandler):
    """GDB/MI console stream handler class
    """
    TAG = '~'


def dbg_corefile(args):
    """ Command to load core dump from file or flash and run GDB debug session with it
    """
    global CLOSE_FDS
    loader = None
    if not args.core:
        loader = ESPCoreDumpFlashLoader(args.off, port=args.port)
        core_fname = loader.create_corefile(args.save_core)
        if not core_fname:
            print "Failed to create corefile!"
            loader.cleanup()
            return
    else:
        core_fname = args.core
        if args.core_format and args.core_format != 'elf':
            loader = ESPCoreDumpFileLoader(core_fname, args.core_format == 'b64')
            core_fname = loader.create_corefile(args.save_core)
            if not core_fname:
                print "Failed to create corefile!"
                loader.cleanup()
                return

    p = subprocess.Popen(
            bufsize = 0,
            args = [args.gdb,
                '--nw', # ignore .gdbinit
                '--core=%s' % core_fname, # core file
                args.prog],
            stdin = None, stdout = None, stderr = None,
            close_fds = CLOSE_FDS
            )
    p.wait()
    
    if loader:
        if not args.core and not args.save_core:
            loader.remove_tmp_file(core_fname)
        loader.cleanup()
    print 'Done!'


def info_corefile(args):
    """ Command to load core dump from file or flash and print it's data in user friendly form 
    """
    global CLOSE_FDS
    def gdbmi_console_stream_handler(ln):
        sys.stdout.write(ln)
        sys.stdout.flush()
    
    def gdbmi_read2prompt(f, out_handlers=None):
        while True:
            ln = f.readline().rstrip(' \r\n')
            if ln == '(gdb)':
                break
            elif len(ln) == 0:
                break
            elif out_handlers:
                for h in out_handlers:
                    if ln.startswith(out_handlers[h].TAG):
                        out_handlers[h].execute(ln)
                        break

    def gdbmi_start(handlers):
        p = subprocess.Popen(
                bufsize = 0,
                args = [args.gdb,
                    '--quiet', # inhibit dumping info at start-up
                    '--nx', # inhibit window interface
                    '--nw', # ignore .gdbinit
                    '--interpreter=mi2', # use GDB/MI v2
                    '--core=%s' % core_fname, # core file
                    args.prog],
                stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.STDOUT,
                close_fds = CLOSE_FDS
                )
        gdbmi_read2prompt(p.stdout, handlers)
        return p

    def gdbmi_getinfo(p, handlers, gdb_cmd):
        for t in handlers:
            handlers[t].result_class = None
        p.stdin.write("-interpreter-exec console \"%s\"\n" % gdb_cmd)
        gdbmi_read2prompt(p.stdout, handlers)
        if not handlers[GDBMIResultHandler.TAG].result_class or handlers[GDBMIResultHandler.TAG].result_class == GDBMIResultHandler.RC_EXIT:
            print "GDB exited (%s / %s)!" % (handlers[GDBMIResultHandler.TAG].result_class, handlers[GDBMIResultHandler.TAG].result_str)
            p.wait()
            print "Problem occured! GDB exited, restart it."
            p = gdbmi_start(handlers)
        elif handlers[GDBMIResultHandler.TAG].result_class != GDBMIResultHandler.RC_DONE:
            print "GDB/MI command failed (%s / %s)!" % (handlers[GDBMIResultHandler.TAG].result_class, handlers[GDBMIResultHandler.TAG].result_str)
        return p


    loader = None
    if not args.core:
        loader = ESPCoreDumpFlashLoader(args.off, port=args.port)
        core_fname = loader.create_corefile(args.save_core)
        if not core_fname:
            print "Failed to create corefile!"
            loader.cleanup()
            return
    else:
        core_fname = args.core
        if args.core_format and args.core_format != 'elf':
            loader = ESPCoreDumpFileLoader(core_fname, args.core_format == 'b64')
            core_fname = loader.create_corefile(args.save_core)
            if not core_fname:
                print "Failed to create corefile!"
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
    p = gdbmi_start(handlers)

    print "==============================================================="
    print "==================== ESP32 CORE DUMP START ===================="

    handlers[GDBMIResultHandler.TAG].result_class = None
    handlers[GDBMIStreamConsoleHandler.TAG].func = gdbmi_console_stream_handler
    print "\n================== CURRENT THREAD REGISTERS ==================="
    p = gdbmi_getinfo(p, handlers, "info registers")
    print "\n==================== CURRENT THREAD STACK ====================="
    p = gdbmi_getinfo(p, handlers, "bt")
    print "\n======================== THREADS INFO ========================="
    p = gdbmi_getinfo(p, handlers, "info threads")
    print "\n======================= ALL MEMORY REGIONS ========================"
    print "Name   Address   Size   Attrs"
    for ms in merged_segs:
        print "%s 0x%x 0x%x %s" % (ms[0], ms[1], ms[2], ms[3])
    for cs in core_segs:
        print ".coredump.tasks 0x%x 0x%x %s" % (cs.addr, len(cs.data), cs.attr_str())
    if args.print_mem:
        print "\n====================== CORE DUMP MEMORY CONTENTS ========================"
        for cs in core_elf.program_segments:
            print ".coredump.tasks 0x%x 0x%x %s" % (cs.addr, len(cs.data), cs.attr_str())
            p = gdbmi_getinfo(p, handlers, "x/%dx 0x%x" % (len(cs.data)/4, cs.addr))

    print "\n===================== ESP32 CORE DUMP END ====================="
    print "==============================================================="

    p.stdin.write('q\n')
    p.wait()
    p.stdin.close()
    p.stdout.close()
    
    if loader:
        if not args.core and not args.save_core:
            loader.remove_tmp_file(core_fname)
        loader.cleanup()
    print 'Done!'
        

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
    parser_debug_coredump.add_argument('--gdb', '-g', help='Path to gdb', default='xtensa-esp32-elf-gdb')
    parser_debug_coredump.add_argument('--core', '-c', help='Path to core dump file (if skipped core dump will be read from flash)', type=str)
    parser_debug_coredump.add_argument('--core-format', '-t', help='(elf, raw or b64). File specified with "-c" is an ELF ("elf"), raw (raw) or base64-encoded (b64) binary', type=str, default='elf')
    parser_debug_coredump.add_argument('--off', '-o', help='Ofsset of coredump partition in flash (type "make partition_table" to see).', type=int, default=0x110000)
    parser_debug_coredump.add_argument('--save-core', '-s', help='Save core to file. Othwerwise temporary core file will be deleted. Ignored with "-c"', type=str)
    parser_debug_coredump.add_argument('prog', help='Path to program\'s ELF binary', type=str)

    parser_info_coredump = subparsers.add_parser(
        'info_corefile',
        help='Print core dump info from file')
    parser_info_coredump.add_argument('--gdb', '-g', help='Path to gdb', default='xtensa-esp32-elf-gdb')
    parser_info_coredump.add_argument('--core', '-c', help='Path to core dump file (if skipped core dump will be read from flash)', type=str)
    parser_info_coredump.add_argument('--core-format', '-t', help='(elf, raw or b64). File specified with "-c" is an ELF ("elf"), raw (raw) or base64-encoded (b64) binary', type=str, default='elf')
    parser_info_coredump.add_argument('--off', '-o', help='Ofsset of coredump partition in flash (type "make partition_table" to see).', type=int, default=0x110000)
    parser_info_coredump.add_argument('--save-core', '-s', help='Save core to file. Othwerwise temporary core file will be deleted. Does not work with "-c"', type=str)
    parser_info_coredump.add_argument('--print-mem', '-m', help='Print memory dump', action='store_true')
    parser_info_coredump.add_argument('prog', help='Path to program\'s ELF binary', type=str)

    # internal sanity check - every operation matches a module function of the same name
    for operation in subparsers.choices.keys():
        assert operation in globals(), "%s should be a module function" % operation

    args = parser.parse_args()

    print 'espcoredump.py v%s' % __version__

    operation_func = globals()[args.operation]
    operation_func(args)


if __name__ == '__main__':
    try:
        main()
    except ESPCoreDumpError as e:
        print '\nA fatal error occurred: %s' % e
        sys.exit(2)
