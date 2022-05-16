#!/usr/bin/env python
# coding=utf-8
#
# A script which parses ESP-IDF panic handler output (registers & stack dump),
# and then acts as a GDB server over stdin/stdout, presenting the information
# from the panic handler to GDB.
# This allows for generating backtraces out of raw stack dumps on architectures
# where backtracing on the target side is not possible.
#
# Note that the "act as a GDB server" approach is somewhat a hack.
# A much nicer solution would have been to convert the panic handler output
# into a core file, and point GDB to the core file.
# However, RISC-V baremetal GDB currently lacks core dump support.
#
# The approach is inspired by Cesanta's ESP8266 GDB server:
# https://github.com/cesanta/mongoose-os/blob/27777c8977/platforms/esp8266/tools/serve_core.py
#
# Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
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


import argparse
import binascii
import logging
import struct
import sys
from builtins import bytes
from collections import namedtuple

# Used for type annotations only. Silence linter warnings.
from pyparsing import (Combine, Group, Literal, OneOrMore, ParserElement,  # noqa: F401 # pylint: disable=unused-import
                       ParseResults, Word, nums, srange)

try:
    import typing  # noqa: F401 # pylint: disable=unused-import
except ImportError:
    pass

# pyparsing helper
hexnumber = srange('[0-9a-f]')


# List of registers to be passed to GDB, in the order GDB expects.
# The names should match those used in IDF panic handler.
# Registers not present in IDF panic handler output (like X0) will be assumed to be 0.
GDB_REGS_INFO_RISCV_ILP32 = [
    'X0', 'RA', 'SP', 'GP',
    'TP', 'T0', 'T1', 'T2',
    'S0/FP', 'S1', 'A0', 'A1',
    'A2', 'A3', 'A4', 'A5',
    'A6', 'A7', 'S2', 'S3',
    'S4', 'S5', 'S6', 'S7',
    'S8', 'S9', 'S10', 'S11',
    'T3', 'T4', 'T5', 'T6',
    'MEPC'
]


GDB_REGS_INFO = {
    'esp32c3': GDB_REGS_INFO_RISCV_ILP32,
    'esp32h2': GDB_REGS_INFO_RISCV_ILP32
}

PanicInfo = namedtuple('PanicInfo', 'core_id regs stack_base_addr stack_data')


def build_riscv_panic_output_parser():  # type: () -> typing.Any[typing.Type[ParserElement]]
    """Builds a parser for the panic handler output using pyparsing"""

    # We don't match the first line, since "Guru Meditation" will not be printed in case of an abort:
    # Guru Meditation Error: Core  0 panic'ed (Store access fault). Exception was unhandled.

    # Core  0 register dump:
    reg_dump_header = Group(Literal('Core') +
                            Word(nums)('core_id') +
                            Literal('register dump:'))('reg_dump_header')

    # MEPC    : 0x4200232c  RA      : 0x42009694  SP      : 0x3fc93a80  GP      : 0x3fc8b320
    reg_name = Word(srange('[A-Z_0-9/-]'))('name')
    hexnumber_with_0x = Combine(Literal('0x') + Word(hexnumber))
    reg_value = hexnumber_with_0x('value')
    reg_dump_one_reg = Group(reg_name + Literal(':') + reg_value)  # not named because there will be OneOrMore
    reg_dump_all_regs = Group(OneOrMore(reg_dump_one_reg))('regs')
    reg_dump = Group(reg_dump_header + reg_dump_all_regs)  # not named because there will be OneOrMore
    reg_dumps = Group(OneOrMore(reg_dump))('reg_dumps')

    # Stack memory:
    # 3fc93a80: 0x00000030 0x00000021 0x3fc8aedc 0x4200232a 0xa5a5a5a5 0xa5a5a5a5 0x3fc8aedc 0x420099b0
    stack_line = Group(Word(hexnumber)('base') + Literal(':') +
                       Group(OneOrMore(hexnumber_with_0x))('data'))
    stack_dump = Group(Literal('Stack memory:') +
                       Group(OneOrMore(stack_line))('lines'))('stack_dump')

    # Parser for the complete panic output:
    panic_output = reg_dumps + stack_dump
    return panic_output


def get_stack_addr_and_data(res):  # type: (ParseResults) -> typing.Tuple[int, bytes]
    """ Extract base address and bytes from the parsed stack dump """
    stack_base_addr = 0  # First reported address in the dump
    base_addr = 0  # keeps track of the address for the given line of the dump
    bytes_in_line = 0  # bytes of stack parsed on the previous line; used to validate the next base address
    stack_data = bytes(b'')  # accumulates all the dumped stack data
    for line in res.stack_dump.lines:
        # update and validate the base address
        prev_base_addr = base_addr
        base_addr = int(line.base, 16)
        if stack_base_addr == 0:
            stack_base_addr = base_addr
        else:
            assert base_addr == prev_base_addr + bytes_in_line

        # convert little-endian hex words to byte representation
        words = [int(w, 16) for w in line.data]
        line_data = bytes(b''.join([struct.pack('<I', w) for w in words]))
        bytes_in_line = len(line_data)

        # accumulate in the whole stack data
        stack_data += line_data

    return stack_base_addr, stack_data


def parse_idf_riscv_panic_output(panic_text):  # type: (str) -> PanicInfo
    """ Decode panic handler output from a file """
    panic_output = build_riscv_panic_output_parser()
    results = panic_output.searchString(panic_text)
    if len(results) != 1:
        raise ValueError("Couldn't parse panic handler output")
    res = results[0]

    if len(res.reg_dumps) > 1:
        raise NotImplementedError('Handling of multi-core register dumps not implemented')

    # Build a dict of register names/values
    rd = res.reg_dumps[0]
    core_id = int(rd.reg_dump_header.core_id)
    regs = dict()
    for reg in rd.regs:
        reg_value = int(reg.value, 16)
        regs[reg.name] = reg_value

    stack_base_addr, stack_data = get_stack_addr_and_data(res)

    return PanicInfo(core_id=core_id,
                     regs=regs,
                     stack_base_addr=stack_base_addr,
                     stack_data=stack_data)


PANIC_OUTPUT_PARSERS = {
    'esp32c3': parse_idf_riscv_panic_output,
    'esp32h2': parse_idf_riscv_panic_output
}


class GdbServer(object):
    def __init__(self, panic_info, target, log_file=None):  # type: (PanicInfo, str, str) -> None
        self.panic_info = panic_info
        self.in_stream = sys.stdin
        self.out_stream = sys.stdout
        self.reg_list = GDB_REGS_INFO[target]

        self.logger = logging.getLogger('GdbServer')
        if log_file:
            handler = logging.FileHandler(log_file, 'w+')
            self.logger.setLevel(logging.DEBUG)
            formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
            handler.setFormatter(formatter)
            self.logger.addHandler(handler)

    def run(self):  # type: () -> None
        """ Process GDB commands from stdin until GDB tells us to quit """
        buffer = ''
        while True:
            buffer += self.in_stream.read(1)
            if len(buffer) > 3 and buffer[-3] == '#':
                self._handle_command(buffer)
                buffer = ''

    def _handle_command(self, buffer):  # type: (str) -> None
        command = buffer[1:-3]  # ignore checksums
        # Acknowledge the command
        self.out_stream.write('+')
        self.out_stream.flush()
        self.logger.debug('Got command: %s', command)
        if command == '?':
            # report sigtrap as the stop reason; the exact reason doesn't matter for backtracing
            self._respond('T05')
        elif command.startswith('Hg') or command.startswith('Hc'):
            # Select thread command
            self._respond('OK')
        elif command == 'qfThreadInfo':
            # Get list of threads.
            # Only one thread for now, can be extended to show one thread for each core,
            # if we dump both cores (e.g. on an interrupt watchdog)
            self._respond('m1')
        elif command == 'qC':
            # That single thread is selected.
            self._respond('QC1')
        elif command == 'g':
            # Registers read
            self._respond_regs()
        elif command.startswith('m'):
            # Memory read
            addr, size = [int(v, 16) for v in command[1:].split(',')]
            self._respond_mem(addr, size)
        elif command.startswith('vKill') or command == 'k':
            # Quit
            self._respond('OK')
            raise SystemExit(0)
        else:
            # Empty response required for any unknown command
            self._respond('')

    def _respond(self, data):  # type: (str) -> None
        # calculate checksum
        data_bytes = bytes(data.encode('ascii'))  # bytes() for Py2 compatibility
        checksum = sum(data_bytes) & 0xff
        # format and write the response
        res = '${}#{:02x}'.format(data, checksum)
        self.logger.debug('Wrote: %s', res)
        self.out_stream.write(res)
        self.out_stream.flush()
        # get the result ('+' or '-')
        ret = self.in_stream.read(1)
        self.logger.debug('Response: %s', ret)
        if ret != '+':
            sys.stderr.write("GDB responded with '-' to {}".format(res))
            raise SystemExit(1)

    def _respond_regs(self):  # type: () -> None
        response = ''
        for reg_name in self.reg_list:
            # register values are reported as hexadecimal strings
            # in target byte order (i.e. LSB first for RISC-V)
            reg_val = self.panic_info.regs.get(reg_name, 0)
            reg_bytes = struct.pack('<L', reg_val)
            response += binascii.hexlify(reg_bytes).decode('ascii')
        self._respond(response)

    def _respond_mem(self, start_addr, size):  # type: (int, int) -> None
        stack_addr_min = self.panic_info.stack_base_addr
        stack_data = self.panic_info.stack_data
        stack_len = len(self.panic_info.stack_data)
        stack_addr_max = stack_addr_min + stack_len

        # For any memory address that is not on the stack, pretend the value is 0x00.
        # GDB should never ask us for program memory, it will be obtained from the ELF file.
        def in_stack(addr):  # type: (int) -> typing.Any[bool]
            return stack_addr_min <= addr < stack_addr_max

        result = ''
        for addr in range(start_addr, start_addr + size):
            if not in_stack(addr):
                result += '00'
            else:
                result += '{:02x}'.format(stack_data[addr - stack_addr_min])

        self._respond(result)


def main():  # type: () -> None
    parser = argparse.ArgumentParser()
    parser.add_argument('input_file', type=argparse.FileType('r'),
                        help='File containing the panic handler output')
    parser.add_argument('--target', choices=GDB_REGS_INFO.keys(),
                        help='Chip to use (determines the architecture)')
    parser.add_argument('--gdb-log', default=None,
                        help='If specified, the file for logging GDB server debug information')
    args = parser.parse_args()

    panic_info = PANIC_OUTPUT_PARSERS[args.target](args.input_file.read())

    server = GdbServer(panic_info, target=args.target, log_file=args.gdb_log)
    try:
        server.run()
    except KeyboardInterrupt:
        sys.exit(0)


if __name__ == '__main__':
    main()
