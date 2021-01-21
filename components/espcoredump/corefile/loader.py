#
# Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

import base64
import binascii
import hashlib
import logging
import os
import subprocess
import sys
import tempfile

from construct import AlignedStruct, Bytes, GreedyRange, Int32ul, Padding, Struct, abs_, this

from . import ESPCoreDumpLoaderError, _ArchMethodsBase, _TargetMethodsBase
from .elf import (TASK_STATUS_CORRECT, TASK_STATUS_TCB_CORRUPTED, ElfFile, ElfSegment, ESPCoreDumpElfFile,
                  EspTaskStatus, NoteSection)
from .xtensa import _ArchMethodsXtensa, _TargetMethodsESP32

IDF_PATH = os.getenv('IDF_PATH')
PARTTOOL_PY = os.path.join(IDF_PATH, 'components', 'partition_table', 'parttool.py')
ESPTOOL_PY = os.path.join(IDF_PATH, 'components', 'esptool_py', 'esptool', 'esptool.py')

# Following structs are based on source code
# components/espcoredump/include_core_dump/esp_core_dump_priv.h

EspCoreDumpV1Header = Struct(
    'tot_len' / Int32ul,
    'ver' / Int32ul,
    'task_num' / Int32ul,
    'tcbsz' / Int32ul,
)

EspCoreDumpV2Header = Struct(
    'tot_len' / Int32ul,
    'ver' / Int32ul,
    'task_num' / Int32ul,
    'tcbsz' / Int32ul,
    'segs_num' / Int32ul,
)

CRC = Int32ul
SHA256 = Bytes(32)

TaskHeader = Struct(
    'tcb_addr' / Int32ul,
    'stack_top' / Int32ul,
    'stack_end' / Int32ul,
)

MemSegmentHeader = Struct(
    'mem_start' / Int32ul,
    'mem_sz' / Int32ul,
    'data' / Bytes(this.mem_sz),
)


class EspCoreDumpVersion(object):
    """Core dump version class
    """
    # This class contains all version-dependent params
    ESP32 = 0
    ESP32S2 = 2

    XTENSA_CHIPS = [ESP32, ESP32S2]

    ESP_COREDUMP_TARGETS = XTENSA_CHIPS

    def __init__(self, version=None):
        """Constructor for core dump version
        """
        super(EspCoreDumpVersion, self).__init__()
        if version is None:
            self.version = 0
        else:
            self.set_version(version)

    @staticmethod
    def make_dump_ver(major, minor):
        return ((major & 0xFF) << 8) | ((minor & 0xFF) << 0)

    def set_version(self, version):
        self.version = version

    @property
    def chip_ver(self):
        return (self.version & 0xFFFF0000) >> 16

    @property
    def dump_ver(self):
        return self.version & 0x0000FFFF

    @property
    def major(self):
        return (self.version & 0x0000FF00) >> 8

    @property
    def minor(self):
        return self.version & 0x000000FF


class EspCoreDumpLoader(EspCoreDumpVersion):
    # "legacy" stands for core dumps v0.1 (before IDF v4.1)
    BIN_V1 = EspCoreDumpVersion.make_dump_ver(0, 1)
    BIN_V2 = EspCoreDumpVersion.make_dump_ver(0, 2)
    ELF_CRC32 = EspCoreDumpVersion.make_dump_ver(1, 0)
    ELF_SHA256 = EspCoreDumpVersion.make_dump_ver(1, 1)

    def __init__(self):
        super(EspCoreDumpLoader, self).__init__()
        self.core_src_file = None
        self.core_src_struct = None
        self.core_src = None

        self.core_elf_file = None

        self.header = None
        self.header_struct = EspCoreDumpV1Header
        self.checksum_struct = CRC

        # These two method classes will be assigned in ``reload_coredump``
        self.target_method_cls = _TargetMethodsBase
        self.arch_method_cls = _ArchMethodsBase

        self._temp_files = []

    def __del__(self):
        if self.core_src_file:
            self.core_src_file.close()
        if self.core_elf_file:
            self.core_elf_file.close()
        for f in self._temp_files:
            try:
                os.remove(f)
            except OSError:
                pass

    def _create_temp_file(self):
        t = tempfile.NamedTemporaryFile('wb', delete=False)
        self._temp_files.append(t.name)
        return t

    def _reload_coredump(self):
        with open(self.core_src_file.name, 'rb') as fr:
            coredump_bytes = fr.read()

        _header = EspCoreDumpV1Header.parse(coredump_bytes)  # first we use V1 format to get version
        self.set_version(_header.ver)
        if self.dump_ver == self.ELF_CRC32:
            self.checksum_struct = CRC
            self.header_struct = EspCoreDumpV2Header
        elif self.dump_ver == self.ELF_SHA256:
            self.checksum_struct = SHA256
            self.header_struct = EspCoreDumpV2Header
        elif self.dump_ver == self.BIN_V1:
            self.checksum_struct = CRC
            self.header_struct = EspCoreDumpV1Header
        elif self.dump_ver == self.BIN_V2:
            self.checksum_struct = CRC
            self.header_struct = EspCoreDumpV2Header
        else:
            raise ESPCoreDumpLoaderError('Core dump version "0x%x" is not supported!' % self.dump_ver)

        self.core_src_struct = Struct(
            'header' / self.header_struct,
            'data' / Bytes(this.header.tot_len - self.header_struct.sizeof() - self.checksum_struct.sizeof()),
            'checksum' / self.checksum_struct,
        )
        self.core_src = self.core_src_struct.parse(coredump_bytes)

        # Reload header if header struct changes after parsing
        if self.header_struct != EspCoreDumpV1Header:
            self.header = EspCoreDumpV2Header.parse(coredump_bytes)

        if self.chip_ver in self.ESP_COREDUMP_TARGETS:
            if self.chip_ver == self.ESP32:
                self.target_method_cls = _TargetMethodsESP32

            if self.chip_ver in self.XTENSA_CHIPS:
                self.arch_method_cls = _ArchMethodsXtensa
        else:
            raise ESPCoreDumpLoaderError('Core dump chip "0x%x" is not supported!' % self.chip_ver)

    def _validate_dump_file(self):
        if self.chip_ver not in self.ESP_COREDUMP_TARGETS:
            raise ESPCoreDumpLoaderError('Invalid core dump chip version: "{}", should be <= "0x{:X}"'
                                         .format(self.chip_ver, self.ESP32S2))

        if self.checksum_struct == CRC:
            self._crc_validate()
        elif self.checksum_struct == SHA256:
            self._sha256_validate()

    def _crc_validate(self):
        data_crc = binascii.crc32(EspCoreDumpV2Header.build(self.core_src.header) + self.core_src.data) & 0xffffffff
        if data_crc != self.core_src.checksum:
            raise ESPCoreDumpLoaderError('Invalid core dump CRC %x, should be %x' % (data_crc, self.core_src.crc))

    def _sha256_validate(self):
        data_sha256 = hashlib.sha256(EspCoreDumpV2Header.build(self.core_src.header) + self.core_src.data)
        data_sha256_str = data_sha256.hexdigest()
        sha256_str = binascii.hexlify(self.core_src.checksum).decode('ascii')
        if data_sha256_str != sha256_str:
            raise ESPCoreDumpLoaderError('Invalid core dump SHA256 "{}", should be "{}"'
                                         .format(data_sha256_str, sha256_str))

    def create_corefile(self, exe_name=None):  # type: (str) -> None
        """
        Creates core dump ELF file
        """
        self._validate_dump_file()
        self.core_elf_file = self._create_temp_file()

        if self.dump_ver in [self.ELF_CRC32,
                             self.ELF_SHA256]:
            self._extract_elf_corefile(exe_name)
        elif self.dump_ver in [self.BIN_V1,
                               self.BIN_V2]:
            self._extract_bin_corefile()
        else:
            raise NotImplementedError

    def _extract_elf_corefile(self, exe_name=None):
        """
        Reads the ELF formatted core dump image and parse it
        """
        self.core_elf_file.write(self.core_src.data)
        # Need to be closed before read. Otherwise the result will be wrong
        self.core_elf_file.close()

        core_elf = ESPCoreDumpElfFile(self.core_elf_file.name)

        # Read note segments from core file which are belong to tasks (TCB or stack)
        for seg in core_elf.note_segments:
            for note_sec in seg.note_secs:
                # Check for version info note
                if note_sec.name == 'ESP_CORE_DUMP_INFO' \
                        and note_sec.type == ESPCoreDumpElfFile.PT_INFO \
                        and exe_name:
                    exe_elf = ElfFile(exe_name)
                    app_sha256 = binascii.hexlify(exe_elf.sha256)
                    coredump_sha256_struct = Struct(
                        'ver' / Int32ul,
                        'sha256' / Bytes(64)  # SHA256 as hex string
                    )
                    coredump_sha256 = coredump_sha256_struct.parse(note_sec.desc[:coredump_sha256_struct.sizeof()])
                    if coredump_sha256.sha256 != app_sha256:
                        raise ESPCoreDumpLoaderError(
                            'Invalid application image for coredump: coredump SHA256({}) != app SHA256({}).'
                            .format(coredump_sha256, app_sha256))
                    if coredump_sha256.ver != self.version:
                        raise ESPCoreDumpLoaderError(
                            'Invalid application image for coredump: coredump SHA256 version({}) != app SHA256 version({}).'
                            .format(coredump_sha256.ver, self.version))

    @staticmethod
    def _get_aligned_size(size, align_with=4):
        if size % align_with:
            return align_with * (size // align_with + 1)
        return size

    @staticmethod
    def _build_note_section(name, sec_type, desc):
        name = bytearray(name, encoding='ascii') + b'\0'
        return NoteSection.build({
            'namesz': len(name),
            'descsz': len(desc),
            'type': sec_type,
            'name': name,
            'desc': desc,
        })

    def _extract_bin_corefile(self):
        """
        Creates core dump ELF file
        """
        tcbsz_aligned = self._get_aligned_size(self.header.tcbsz)

        coredump_data_struct = Struct(
            'tasks' / GreedyRange(
                AlignedStruct(
                    4,
                    'task_header' / TaskHeader,
                    'tcb' / Bytes(self.header.tcbsz),
                    'stack' / Bytes(abs_(this.task_header.stack_top - this.task_header.stack_end)),
                )
            ),
            'mem_seg_headers' / MemSegmentHeader[self.core_src.header.segs_num]
        )

        core_elf = ESPCoreDumpElfFile()
        notes = b''
        core_dump_info_notes = b''
        task_info_notes = b''

        coredump_data = coredump_data_struct.parse(self.core_src.data)
        for i, task in enumerate(coredump_data.tasks):
            stack_len_aligned = self._get_aligned_size(abs(task.task_header.stack_top - task.task_header.stack_end))
            task_status_kwargs = {
                'task_index': i,
                'task_flags': TASK_STATUS_CORRECT,
                'task_tcb_addr': task.task_header.tcb_addr,
                'task_stack_start': min(task.task_header.stack_top, task.task_header.stack_end),
                'task_stack_len': stack_len_aligned,
                'task_name': Padding(16).build({})  # currently we don't have task_name, keep it as padding
            }

            # Write TCB
            try:
                if self.target_method_cls.tcb_is_sane(task.task_header.tcb_addr, tcbsz_aligned):
                    core_elf.add_segment(task.task_header.tcb_addr,
                                         task.tcb,
                                         ElfFile.PT_LOAD,
                                         ElfSegment.PF_R | ElfSegment.PF_W)
                elif task.task_header.tcb_addr and self.target_method_cls.addr_is_fake(task.task_header.tcb_addr):
                    task_status_kwargs['task_flags'] |= TASK_STATUS_TCB_CORRUPTED
            except ESPCoreDumpLoaderError as e:
                logging.warning('Skip TCB {} bytes @ 0x{:x}. (Reason: {})'
                                .format(tcbsz_aligned, task.task_header.tcb_addr, e))

            # Write stack
            try:
                if self.target_method_cls.stack_is_sane(task_status_kwargs['task_stack_start']):
                    core_elf.add_segment(task_status_kwargs['task_stack_start'],
                                         task.stack,
                                         ElfFile.PT_LOAD,
                                         ElfSegment.PF_R | ElfSegment.PF_W)
                elif task_status_kwargs['task_stack_start'] \
                        and self.target_method_cls.addr_is_fake(task_status_kwargs['task_stack_start']):
                    task_status_kwargs['task_flags'] |= TASK_STATUS_TCB_CORRUPTED
                    core_elf.add_segment(task_status_kwargs['task_stack_start'],
                                         task.stack,
                                         ElfFile.PT_LOAD,
                                         ElfSegment.PF_R | ElfSegment.PF_W)
            except ESPCoreDumpLoaderError as e:
                logging.warning('Skip task\'s ({:x}) stack {} bytes @ 0x{:x}. (Reason: {})'
                                .format(task_status_kwargs['tcb_addr'],
                                        task_status_kwargs['stack_len_aligned'],
                                        task_status_kwargs['stack_base'],
                                        e))

            try:
                logging.debug('Stack start_end: 0x{:x} @ 0x{:x}'
                              .format(task.task_header.stack_top, task.task_header.stack_end))
                task_regs, extra_regs = self.arch_method_cls.get_registers_from_stack(
                    task.stack,
                    task.task_header.stack_end > task.task_header.stack_top
                )
            except Exception as e:
                raise ESPCoreDumpLoaderError(str(e))

            task_info_notes += self._build_note_section('TASK_INFO',
                                                        ESPCoreDumpElfFile.PT_TASK_INFO,
                                                        EspTaskStatus.build(task_status_kwargs))
            notes += self._build_note_section('CORE',
                                              ElfFile.PT_LOAD,
                                              self.arch_method_cls.build_prstatus_data(task.task_header.tcb_addr,
                                                                                       task_regs))

            if extra_regs and len(core_dump_info_notes) == 0:
                # actually there will be only one such note - for crashed task
                core_dump_info_notes += self._build_note_section('ESP_CORE_DUMP_INFO',
                                                                 ESPCoreDumpElfFile.PT_INFO,
                                                                 Int32ul.build(self.header.ver))

                exc_regs = []
                for reg_id in extra_regs:
                    exc_regs.extend([reg_id, extra_regs[reg_id]])
                _regs = [task.task_header.tcb_addr] + exc_regs
                core_dump_info_notes += self._build_note_section(
                    'EXTRA_INFO',
                    ESPCoreDumpElfFile.PT_EXTRA_INFO,
                    Int32ul[1 + len(exc_regs)].build(_regs)
                )

        if self.dump_ver == self.BIN_V2:
            for header in coredump_data.mem_seg_headers:
                logging.debug('Read memory segment {} bytes @ 0x{:x}'.format(header.mem_sz, header.mem_start))
                core_elf.add_segment(header.mem_start, header.data, ElfFile.PT_LOAD, ElfSegment.PF_R | ElfSegment.PF_W)

        # add notes
        try:
            core_elf.add_segment(0, notes, ElfFile.PT_NOTE, 0)
        except ESPCoreDumpLoaderError as e:
            logging.warning('Skip NOTES segment {:d} bytes @ 0x{:x}. (Reason: {})'.format(len(notes), 0, e))
        # add core dump info notes
        try:
            core_elf.add_segment(0, core_dump_info_notes, ElfFile.PT_NOTE, 0)
        except ESPCoreDumpLoaderError as e:
            logging.warning('Skip core dump info NOTES segment {:d} bytes @ 0x{:x}. (Reason: {})'
                            .format(len(core_dump_info_notes), 0, e))
        try:
            core_elf.add_segment(0, task_info_notes, ElfFile.PT_NOTE, 0)
        except ESPCoreDumpLoaderError as e:
            logging.warning('Skip failed tasks info NOTES segment {:d} bytes @ 0x{:x}. (Reason: {})'
                            .format(len(task_info_notes), 0, e))
        # dump core ELF
        core_elf.e_type = ElfFile.ET_CORE
        core_elf.e_machine = ESPCoreDumpElfFile.EM_XTENSA
        core_elf.dump(self.core_elf_file.name)


class ESPCoreDumpFlashLoader(EspCoreDumpLoader):
    ESP_COREDUMP_PART_TABLE_OFF = 0x8000

    def __init__(self, offset, target='esp32', port=None, baud=None):
        super(ESPCoreDumpFlashLoader, self).__init__()
        self.port = port
        self.baud = baud
        self.target = target

        self._get_coredump(offset)
        self._reload_coredump()

    def _get_coredump(self, off):
        """
        Loads core dump from flash using parttool or elftool (if offset is set)
        """
        try:
            if off:
                logging.info('Invoke esptool to read image.')
                self._invoke_esptool(off=off)
            else:
                logging.info('Invoke parttool to read image.')
                self._invoke_parttool()
        except subprocess.CalledProcessError as e:
            if e.output:
                logging.info(e.output)
            logging.error('Error during the subprocess execution')
        else:
            # Need to be closed before read. Otherwise the result will be wrong
            self.core_src_file.close()

    def _invoke_esptool(self, off=None):
        """
        Loads core dump from flash using elftool
        """
        tool_args = [sys.executable, ESPTOOL_PY, '-c', self.target]
        if self.port:
            tool_args.extend(['-p', self.port])
        if self.baud:
            tool_args.extend(['-b', str(self.baud)])

        self.core_src_file = self._create_temp_file()
        try:
            (part_offset, part_size) = self._get_core_dump_partition_info()
            if not off:
                off = part_offset  # set default offset if not specified
                logging.warning('The core dump image offset is not specified. Use partition offset: %d.', part_offset)
            if part_offset != off:
                logging.warning('Predefined image offset: %d does not match core dump partition offset: %d', off,
                                part_offset)

            # Here we use V1 format to locate the size
            tool_args.extend(['read_flash', str(off), str(EspCoreDumpV1Header.sizeof())])
            tool_args.append(self.core_src_file.name)

            # read core dump length
            et_out = subprocess.check_output(tool_args)
            if et_out:
                logging.info(et_out.decode('utf-8'))

            header = EspCoreDumpV1Header.parse(open(self.core_src_file.name, 'rb').read())
            if not header or not 0 < header.tot_len <= part_size:
                logging.error('Incorrect size of core dump image: {}, use partition size instead: {}'
                              .format(header.tot_len, part_size))
                coredump_len = part_size
            else:
                coredump_len = header.tot_len
            # set actual size of core dump image and read it from flash
            tool_args[-2] = str(coredump_len)
            et_out = subprocess.check_output(tool_args)
            if et_out:
                logging.info(et_out.decode('utf-8'))
        except subprocess.CalledProcessError as e:
            logging.error('esptool script execution failed with err %d', e.returncode)
            logging.debug("Command ran: '%s'", e.cmd)
            logging.debug('Command out:')
            logging.debug(e.output)
            raise e

    def _invoke_parttool(self):
        """
        Loads core dump from flash using parttool
        """
        tool_args = [sys.executable, PARTTOOL_PY]
        if self.port:
            tool_args.extend(['--port', self.port])
        tool_args.extend(['read_partition', '--partition-type', 'data', '--partition-subtype', 'coredump', '--output'])

        self.core_src_file = self._create_temp_file()
        try:
            tool_args.append(self.core_src_file.name)
            # read core dump partition
            et_out = subprocess.check_output(tool_args)
            if et_out:
                logging.info(et_out.decode('utf-8'))
        except subprocess.CalledProcessError as e:
            logging.error('parttool script execution failed with err %d', e.returncode)
            logging.debug("Command ran: '%s'", e.cmd)
            logging.debug('Command out:')
            logging.debug(e.output)
            raise e

    def _get_core_dump_partition_info(self, part_off=None):
        """
        Get core dump partition info using parttool
        """
        logging.info('Retrieving core dump partition offset and size...')
        if not part_off:
            part_off = self.ESP_COREDUMP_PART_TABLE_OFF
        try:
            tool_args = [sys.executable, PARTTOOL_PY, '-q', '--partition-table-offset', str(part_off)]
            if self.port:
                tool_args.extend(['--port', self.port])
            invoke_args = tool_args + ['get_partition_info', '--partition-type', 'data',
                                       '--partition-subtype', 'coredump',
                                       '--info', 'offset', 'size']
            res = subprocess.check_output(invoke_args).strip()
            (offset_str, size_str) = res.rsplit(b'\n')[-1].split(b' ')
            size = int(size_str, 16)
            offset = int(offset_str, 16)
            logging.info('Core dump partition offset=%d, size=%d', offset, size)
        except subprocess.CalledProcessError as e:
            logging.error('parttool get partition info failed with err %d', e.returncode)
            logging.debug("Command ran: '%s'", e.cmd)
            logging.debug('Command out:')
            logging.debug(e.output)
            logging.error('Check if the coredump partition exists in partition table.')
            raise e
        return offset, size


class ESPCoreDumpFileLoader(EspCoreDumpLoader):
    def __init__(self, path, is_b64=False):
        super(ESPCoreDumpFileLoader, self).__init__()
        self.is_b64 = is_b64

        self._get_coredump(path)
        self._reload_coredump()

    def _get_coredump(self, path):
        """
        Loads core dump from (raw binary or base64-encoded) file
        """
        logging.debug('Load core dump from "%s", %s format', path, 'b64' if self.is_b64 else 'raw')
        if not self.is_b64:
            self.core_src_file = open(path, mode='rb')
        else:
            self.core_src_file = self._create_temp_file()
            with open(path, 'rb') as fb64:
                while True:
                    line = fb64.readline()
                    if len(line) == 0:
                        break
                    data = base64.standard_b64decode(line.rstrip(b'\r\n'))
                    self.core_src_file.write(data)
                self.core_src_file.flush()
                self.core_src_file.seek(0)
