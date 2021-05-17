#!/usr/bin/env python
#
# ESP-IDF Core Dump Utility

import argparse
import logging
import os
import subprocess
import sys
from shutil import copyfile

from construct import GreedyRange, Int32ul, Struct
from corefile import RISCV_TARGETS, SUPPORTED_TARGETS, XTENSA_TARGETS, __version__, xtensa
from corefile.elf import TASK_STATUS_CORRECT, ElfFile, ElfSegment, ESPCoreDumpElfFile, EspTaskStatus
from corefile.gdb import EspGDB
from corefile.loader import ESPCoreDumpFileLoader, ESPCoreDumpFlashLoader
from pygdbmi.gdbcontroller import DEFAULT_GDB_TIMEOUT_SEC

try:
    from typing import Optional, Tuple
except ImportError:
    # Only used for type annotations
    pass

IDF_PATH = os.getenv('IDF_PATH')
if not IDF_PATH:
    sys.stderr.write('IDF_PATH is not found! Set proper IDF_PATH in environment.\n')
    sys.exit(2)

sys.path.insert(0, os.path.join(IDF_PATH, 'components', 'esptool_py', 'esptool'))
try:
    import esptool
except ImportError:
    sys.stderr.write('esptool is not found!\n')
    sys.exit(2)

if os.name == 'nt':
    CLOSE_FDS = False
else:
    CLOSE_FDS = True


def load_aux_elf(elf_path):  # type: (str) -> str
    """
    Loads auxiliary ELF file and composes GDB command to read its symbols.
    """
    sym_cmd = ''
    if os.path.exists(elf_path):
        elf = ElfFile(elf_path)
        for s in elf.sections:
            if s.name == '.text':
                sym_cmd = 'add-symbol-file %s 0x%x' % (elf_path, s.addr)
    return sym_cmd


def get_core_dump_elf(e_machine=ESPCoreDumpFileLoader.ESP32):
    # type: (int) -> Tuple[str, Optional[str], Optional[list[str]]]
    loader = None
    core_filename = None
    target = None
    temp_files = None

    if not args.core:
        # Core file not specified, try to read core dump from flash.
        loader = ESPCoreDumpFlashLoader(args.off, args.chip, port=args.port, baud=args.baud)
    elif args.core_format != 'elf':
        # Core file specified, but not yet in ELF format. Convert it from raw or base64 into ELF.
        loader = ESPCoreDumpFileLoader(args.core, args.core_format == 'b64')
    else:
        # Core file is already in the ELF format
        core_filename = args.core

    # Load/convert the core file
    if loader:
        loader.create_corefile(exe_name=args.prog, e_machine=e_machine)
        core_filename = loader.core_elf_file
        if args.save_core:
            # We got asked to save the core file, make a copy
            copyfile(loader.core_elf_file, args.save_core)
        target = loader.target
        temp_files = loader.temp_files

    return core_filename, target, temp_files  # type: ignore


def get_target():  # type: () -> str
    if args.chip != 'auto':
        return args.chip  # type: ignore

    inst = esptool.ESPLoader.detect_chip(args.port, args.baud)
    return inst.CHIP_NAME.lower().replace('-', '')  # type: ignore


def get_gdb_path(target=None):  # type: (Optional[str]) -> str
    if args.gdb:
        return args.gdb  # type: ignore

    if target is None:
        target = get_target()

    if target in XTENSA_TARGETS:
        # For some reason, xtensa-esp32s2-elf-gdb will report some issue.
        # Use xtensa-esp32-elf-gdb instead.
        return 'xtensa-esp32-elf-gdb'
    if target in RISCV_TARGETS:
        return 'riscv32-esp-elf-gdb'
    raise ValueError('Invalid value: {}. For now we only support {}'.format(target, SUPPORTED_TARGETS))


def get_rom_elf_path(target=None):  # type: (Optional[str]) -> str
    if args.rom_elf:
        return args.rom_elf  # type: ignore

    if target is None:
        target = get_target()

    return '{}_rom.elf'.format(target)


def dbg_corefile():  # type: () -> Optional[list[str]]
    """
    Command to load core dump from file or flash and run GDB debug session with it
    """
    exe_elf = ESPCoreDumpElfFile(args.prog)
    core_elf_path, target, temp_files = get_core_dump_elf(e_machine=exe_elf.e_machine)

    rom_elf_path = get_rom_elf_path(target)
    rom_sym_cmd = load_aux_elf(rom_elf_path)

    gdb_tool = get_gdb_path(target)
    p = subprocess.Popen(bufsize=0,
                         args=[gdb_tool,
                               '--nw',  # ignore .gdbinit
                               '--core=%s' % core_elf_path,  # core file,
                               '-ex', rom_sym_cmd,
                               args.prog],
                         stdin=None, stdout=None, stderr=None,
                         close_fds=CLOSE_FDS)
    p.wait()
    print('Done!')
    return temp_files


def info_corefile():  # type: () -> Optional[list[str]]
    """
    Command to load core dump from file or flash and print it's data in user friendly form
    """
    exe_elf = ESPCoreDumpElfFile(args.prog)
    core_elf_path, target, temp_files = get_core_dump_elf(e_machine=exe_elf.e_machine)
    core_elf = ESPCoreDumpElfFile(core_elf_path)

    if exe_elf.e_machine != core_elf.e_machine:
        raise ValueError('The arch should be the same between core elf and exe elf')

    extra_note = None
    task_info = []
    for seg in core_elf.note_segments:
        for note_sec in seg.note_secs:
            if note_sec.type == ESPCoreDumpElfFile.PT_EXTRA_INFO and 'EXTRA_INFO' in note_sec.name.decode('ascii'):
                extra_note = note_sec
            if note_sec.type == ESPCoreDumpElfFile.PT_TASK_INFO and 'TASK_INFO' in note_sec.name.decode('ascii'):
                task_info_struct = EspTaskStatus.parse(note_sec.desc)
                task_info.append(task_info_struct)
    print('===============================================================')
    print('==================== ESP32 CORE DUMP START ====================')
    rom_elf_path = get_rom_elf_path(target)
    rom_sym_cmd = load_aux_elf(rom_elf_path)

    gdb_tool = get_gdb_path(target)
    gdb = EspGDB(gdb_tool, [rom_sym_cmd], core_elf_path, args.prog, timeout_sec=args.gdb_timeout_sec)

    extra_info = None
    if extra_note:
        extra_info = Struct('regs' / GreedyRange(Int32ul)).parse(extra_note.desc).regs
        marker = extra_info[0]
        if marker == ESPCoreDumpElfFile.CURR_TASK_MARKER:
            print('\nCrashed task has been skipped.')
        else:
            task_name = gdb.get_freertos_task_name(marker)
            print("\nCrashed task handle: 0x%x, name: '%s', GDB name: 'process %d'" % (marker, task_name, marker))
    print('\n================== CURRENT THREAD REGISTERS ===================')
    # Only xtensa have exception registers
    if exe_elf.e_machine == ESPCoreDumpElfFile.EM_XTENSA:
        if extra_note and extra_info:
            xtensa.print_exc_regs_info(extra_info)
        else:
            print('Exception registers have not been found!')
    print(gdb.run_cmd('info registers'))
    print('\n==================== CURRENT THREAD STACK =====================')
    print(gdb.run_cmd('bt'))
    if task_info and task_info[0].task_flags != TASK_STATUS_CORRECT:
        print('The current crashed task is corrupted.')
        print('Task #%d info: flags, tcb, stack (%x, %x, %x).' % (task_info[0].task_index,
                                                                  task_info[0].task_flags,
                                                                  task_info[0].task_tcb_addr,
                                                                  task_info[0].task_stack_start))
    print('\n======================== THREADS INFO =========================')
    print(gdb.run_cmd('info threads'))
    # THREADS STACKS
    threads, _ = gdb.get_thread_info()
    for thr in threads:
        thr_id = int(thr['id'])
        tcb_addr = gdb.gdb2freertos_thread_id(thr['target-id'])
        task_index = int(thr_id) - 1
        task_name = gdb.get_freertos_task_name(tcb_addr)
        gdb.switch_thread(thr_id)
        print('\n==================== THREAD {} (TCB: 0x{:x}, name: \'{}\') ====================='
              .format(thr_id, tcb_addr, task_name))
        print(gdb.run_cmd('bt'))
        if task_info and task_info[task_index].task_flags != TASK_STATUS_CORRECT:
            print("The task '%s' is corrupted." % thr_id)
            print('Task #%d info: flags, tcb, stack (%x, %x, %x).' % (task_info[task_index].task_index,
                                                                      task_info[task_index].task_flags,
                                                                      task_info[task_index].task_tcb_addr,
                                                                      task_info[task_index].task_stack_start))
    print('\n\n======================= ALL MEMORY REGIONS ========================')
    print('Name   Address   Size   Attrs')
    merged_segs = []
    core_segs = core_elf.load_segments
    for sec in exe_elf.sections:
        merged = False
        for seg in core_segs:
            if seg.addr <= sec.addr <= seg.addr + len(seg.data):
                # sec:    |XXXXXXXXXX|
                # seg: |...XXX.............|
                seg_addr = seg.addr
                if seg.addr + len(seg.data) <= sec.addr + len(sec.data):
                    # sec:        |XXXXXXXXXX|
                    # seg:    |XXXXXXXXXXX...|
                    # merged: |XXXXXXXXXXXXXX|
                    seg_len = len(sec.data) + (sec.addr - seg.addr)
                else:
                    # sec:        |XXXXXXXXXX|
                    # seg:    |XXXXXXXXXXXXXXXXX|
                    # merged: |XXXXXXXXXXXXXXXXX|
                    seg_len = len(seg.data)
                merged_segs.append((sec.name, seg_addr, seg_len, sec.attr_str(), True))
                core_segs.remove(seg)
                merged = True
            elif sec.addr <= seg.addr <= sec.addr + len(sec.data):
                # sec:  |XXXXXXXXXX|
                # seg:  |...XXX.............|
                seg_addr = sec.addr
                if (seg.addr + len(seg.data)) >= (sec.addr + len(sec.data)):
                    # sec:    |XXXXXXXXXX|
                    # seg:    |..XXXXXXXXXXX|
                    # merged: |XXXXXXXXXXXXX|
                    seg_len = len(sec.data) + (seg.addr + len(seg.data)) - (sec.addr + len(sec.data))
                else:
                    # sec:    |XXXXXXXXXX|
                    # seg:      |XXXXXX|
                    # merged: |XXXXXXXXXX|
                    seg_len = len(sec.data)
                merged_segs.append((sec.name, seg_addr, seg_len, sec.attr_str(), True))
                core_segs.remove(seg)
                merged = True

        if not merged:
            merged_segs.append((sec.name, sec.addr, len(sec.data), sec.attr_str(), False))

    for ms in merged_segs:
        print('%s 0x%x 0x%x %s' % (ms[0], ms[1], ms[2], ms[3]))

    for cs in core_segs:
        # core dump exec segments are from ROM, other are belong to tasks (TCB or stack)
        if cs.flags & ElfSegment.PF_X:
            seg_name = 'rom.text'
        else:
            seg_name = 'tasks.data'
        print('.coredump.%s 0x%x 0x%x %s' % (seg_name, cs.addr, len(cs.data), cs.attr_str()))
    if args.print_mem:
        print('\n====================== CORE DUMP MEMORY CONTENTS ========================')
        for cs in core_elf.load_segments:
            # core dump exec segments are from ROM, other are belong to tasks (TCB or stack)
            if cs.flags & ElfSegment.PF_X:
                seg_name = 'rom.text'
            else:
                seg_name = 'tasks.data'
            print('.coredump.%s 0x%x 0x%x %s' % (seg_name, cs.addr, len(cs.data), cs.attr_str()))
            print(gdb.run_cmd('x/%dx 0x%x' % (len(cs.data) // 4, cs.addr)))

    print('\n===================== ESP32 CORE DUMP END =====================')
    print('===============================================================')

    del gdb
    print('Done!')
    return temp_files


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='espcoredump.py v%s - ESP32 Core Dump Utility' % __version__)
    parser.add_argument('--chip', default=os.environ.get('ESPTOOL_CHIP', 'auto'),
                        choices=['auto'] + SUPPORTED_TARGETS,
                        help='Target chip type')
    parser.add_argument('--port', '-p', default=os.environ.get('ESPTOOL_PORT', esptool.ESPLoader.DEFAULT_PORT),
                        help='Serial port device')
    parser.add_argument('--baud', '-b', type=int,
                        default=os.environ.get('ESPTOOL_BAUD', esptool.ESPLoader.ESP_ROM_BAUD),
                        help='Serial port baud rate used when flashing/reading')
    parser.add_argument('--gdb-timeout-sec', type=int, default=DEFAULT_GDB_TIMEOUT_SEC,
                        help='Overwrite the default internal delay for gdb responses')

    common_args = argparse.ArgumentParser(add_help=False)
    common_args.add_argument('--debug', '-d', type=int, default=3,
                             help='Log level (0..3)')
    common_args.add_argument('--gdb', '-g',
                             help='Path to gdb')
    common_args.add_argument('--core', '-c',
                             help='Path to core dump file (if skipped core dump will be read from flash)')
    common_args.add_argument('--core-format', '-t', choices=['b64', 'elf', 'raw'], default='elf',
                             help='File specified with "-c" is an ELF ("elf"), '
                                  'raw (raw) or base64-encoded (b64) binary')
    common_args.add_argument('--off', '-o', type=int,
                             help='Offset of coredump partition in flash (type "make partition_table" to see).')
    common_args.add_argument('--save-core', '-s',
                             help='Save core to file. Otherwise temporary core file will be deleted. '
                                  'Does not work with "-c"', )
    common_args.add_argument('--rom-elf', '-r',
                             help='Path to ROM ELF file. Will use "<target>_rom.elf" if not specified')
    common_args.add_argument('prog', help='Path to program\'s ELF binary')

    operations = parser.add_subparsers(dest='operation')

    operations.add_parser('dbg_corefile', parents=[common_args],
                          help='Starts GDB debugging session with specified corefile')

    info_coredump = operations.add_parser('info_corefile', parents=[common_args],
                                          help='Print core dump info from file')
    info_coredump.add_argument('--print-mem', '-m', action='store_true',
                               help='Print memory dump')

    args = parser.parse_args()

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
    temp_core_files = None
    try:
        if args.operation == 'info_corefile':
            temp_core_files = info_corefile()
        elif args.operation == 'dbg_corefile':
            temp_core_files = dbg_corefile()
        else:
            raise ValueError('Please specify action, should be info_corefile or dbg_corefile')
    finally:
        if temp_core_files:
            for f in temp_core_files:
                try:
                    os.remove(f)
                except OSError:
                    pass
