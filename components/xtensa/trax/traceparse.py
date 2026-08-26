#!/usr/bin/env python
# coding=utf-8
#
# This script decodes Xtensa CPU trace dumps. It allows tracing the program
# execution at instruction level.
#
# Some trivia about the Xtensa CPU trace (TRAX):
# TRAX format mostly follows the IEEE-ISTO 5001-2003 (Nexus) standard.
# The following Nexus Program Trace messages are implemented by TRAX:
# - Indirect Branch Message
# - Syncronization Message
# - Indirect Branch with Synchronization Message
# - Correlation Message
# TRAX outputs compressed traces with 2 MSEO bits (LSB) and 6 MDO bits (MSB),
# packed into a byte. MSEO bits are used to split the stream into packets and messages,
# and MDO bits carry the actual data of the messages. Each message may contain multiple packets.
#
# This script can be used standalone, or loaded into GDB.
# When used standalone, it dumps the list of trace messages to stdout.
# When used from GDB, it also invokes GDB command to dump the list of assembly
# instructions corresponding to each of the messages.
#
# Standalone usage:
#   traceparse.py <dump_file>
#
# Usage from GDB:
#   xtensa-esp32-elf-gdb -n --batch program.elf -x gdbinit
# with the following gdbinit script:
#   set pagination off
#   set confirm off
#   add-symbol-file rom.elf <address of ROM .text section>
#   source traceparse.py
#   python parse_and_dump("/path/to/dump_file")
#
# Loading the ROM code is optional; if not loaded, disassembly for ROM sections of code
# will be missing.
#
###
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
from __future__ import print_function

import sys

# Check if loaded into GDB
try:
    assert gdb.__name__ == 'gdb'  # type: ignore
    WITH_GDB = True
except NameError:
    WITH_GDB = False

# MSEO bit masks:
MSEO_PKTEND = 1 << 0    # bit 0: indicates the last byte of a packet
MSEO_MSGEND = 1 << 1    # bit 1: indicates the last byte of the message

# Message types. The type is stored in the first 6 MDO bits or the first packet.
TVAL_INDBR = 4	        # Indirect branch
TVAL_INDBRSYNC = 12	    # Indirect branch w/ synchronisation
TVAL_SYNC = 9	        # Synchronisation msg
TVAL_CORR = 33	        # Correlation message


class TraxPacket(object):
    def __init__(self, data, truncated=False):
        self.data = data
        self.size_bytes = len(data)
        self.truncated = truncated

    def get_bits(self, start, count=0):
        """
        Extract data bits from the packet
        :param start: offset, in bits, of the part to be extracted
        :param count: number of bits to extract; if omitted or zero,
                      extracts until the end of the packet
        :return: integer containing the extracted bits
        """
        start_byte = start // 6
        if count <= 0:
            # all remaining bits
            count = len(self.data) * 6 - start
        bits_remaining = count
        result = 0
        shift = 0
        for i, b in enumerate(self.data[start_byte:]):
            # which bit in the byte is the starting bit
            if i == 0:
                # at start_byte: take the offset into account
                start_bit = 2 + (start % 6)
            else:
                # every other byte: start after MSEO bits
                start_bit = 2
            # how many bits do we need to copy from this byte
            cnt_bits = min(bits_remaining, 8 - start_bit)
            mask = (2 ** cnt_bits) - 1
            # take this many bits after the start_bit
            bits = (b >> start_bit) & mask
            # add these bits to the result
            result |= bits << shift
            # update the remaining bit count
            shift += cnt_bits
            bits_remaining -= cnt_bits
            if bits_remaining == 0:
                break
        return result

    def __str__(self):
        return '%d byte packet%s' % (self.size_bytes, ' (truncated)' if self.truncated else '')


class TraxMessage(object):
    def __init__(self, packets, truncated=False):
        """
        Create and parse a TRAX message from packets
        :param packets: list of TraxPacket objects, must not be empty
        :param truncated: whether the message was truncated in the stream
        """
        assert len(packets) > 0
        self.packets = packets
        self.truncated = truncated
        if truncated:
            self.msg_type = None
        else:
            self.msg_type = self._get_type()

        # Start and end of the instruction range corresponding to this message
        self.pc_start = 0   # inclusive
        self.pc_end = 0     # not inclusive
        self.pc_target = 0      # PC of the next range
        self.is_exception = False   # whether the message indicates an exception
        self.is_correlation = False     # whether this is a correlation message

        # message-specific fields
        self.icnt = 0
        self.uaddr = 0
        self.dcont = 0

        # decode the fields
        if not truncated:
            self._decode()

    def _get_type(self):
        """
        :return: Message type, one of TVAL_XXX values
        """
        return self.packets[0].get_bits(0, 6)

    def _decode(self):
        """ Parse the packets and fill in the message-specific fields """
        if self.msg_type == TVAL_INDBR:
            self.icnt = self.packets[0].get_bits(7, -1)
            self.btype = self.packets[0].get_bits(6, 1)
            self.uaddr = self.packets[1].get_bits(0)
            self.is_exception = self.btype > 0
        elif self.msg_type == TVAL_INDBRSYNC:
            self.icnt = self.packets[0].get_bits(8, -1)
            self.btype = self.packets[0].get_bits(7, 1)
            self.pc_target = self.packets[1].get_bits(0)
            self.dcont = self.packets[0].get_bits(6, 1)
            self.is_exception = self.btype > 0
        elif self.msg_type == TVAL_SYNC:
            self.icnt = self.packets[0].get_bits(7, -1)
            self.dcont = self.packets[0].get_bits(6, 1)
            self.pc_target = self.packets[1].get_bits(0)
        elif self.msg_type == TVAL_CORR:
            self.icnt = self.packets[0].get_bits(12, -1)
            self.is_correlation = True
        else:
            raise NotImplementedError('Unknown message type (%d)' % self.msg_type)

    def process_forward(self, cur_pc):
        """
        Given the target PC known from the previous message, determine
        the PC range corresponding to the current message.
        :param cur_pc: previous known PC
        :return: target PC after the current message
        """
        assert not self.truncated

        next_pc = cur_pc
        if self.msg_type == TVAL_INDBR:
            next_pc = cur_pc ^ self.uaddr
            self.pc_target = next_pc
            self.pc_start = cur_pc
            self.pc_end = self.pc_start + self.icnt + 1
        if self.msg_type == TVAL_INDBRSYNC:
            next_pc = self.pc_target
            self.pc_start = cur_pc
            self.pc_end = cur_pc + self.icnt + 1
        if self.msg_type == TVAL_SYNC:
            next_pc = self.pc_target
            self.pc_start = next_pc - self.icnt
            self.pc_end = next_pc + 1
        if self.msg_type == TVAL_CORR:
            pass
        return next_pc

    def process_backward(self, cur_pc):
        """
        Given the address of the PC known from the _next_ message, determine
        the PC range corresponding to the current message.
        :param cur_pc: next known PC
        :return: target PC of the _previous_ message
        """
        assert not self.truncated
        # Backward pass is only used to resolve addresses of messages
        # up to the first SYNC/INDBRSYNC message.
        # SYNC/INDBRSYNC messages are only handled in the forward pass.
        assert self.msg_type != TVAL_INDBRSYNC
        assert self.msg_type != TVAL_SYNC

        prev_pc = cur_pc
        self.pc_target = cur_pc
        if self.msg_type == TVAL_INDBR:
            prev_pc ^= self.uaddr
            self.pc_start = prev_pc
            self.pc_end = prev_pc + self.icnt + 1
        if self.msg_type == TVAL_CORR:
            pass
        return prev_pc

    def __str__(self):
        desc = 'Unknown (%d)' % self.msg_type
        extra = ''
        if self.truncated:
            desc = 'Truncated'
        if self.msg_type == TVAL_INDBR:
            desc = 'Indirect branch'
            extra = ', icnt=%d, uaddr=0x%x, exc=%d' % (self.icnt, self.uaddr, self.is_exception)
        if self.msg_type == TVAL_INDBRSYNC:
            desc = 'Indirect branch w/sync'
            extra = ', icnt=%d, dcont=%d, exc=%d' % (self.icnt, self.dcont, self.is_exception)
        if self.msg_type == TVAL_SYNC:
            desc = 'Synchronization'
            extra = ', icnt=%d, dcont=%d' % (self.icnt, self.dcont)
        if self.msg_type == TVAL_CORR:
            desc = 'Correlation'
            extra = ', icnt=%d' % self.icnt
        return '%s message, %d packets, PC range 0x%08x - 0x%08x, target PC 0x%08x' % (
            desc, len(self.packets), self.pc_start, self.pc_end, self.pc_target) + extra


def load_messages(data):
    """
    Decodes TRAX data and resolves PC ranges.
    :param data: input data, bytes
    :return: list of TraxMessage objects
    """
    messages = []
    packets = []
    packet_start = 0
    msg_cnt = 0
    pkt_cnt = 0

    # Iterate over the input data, splitting bytes into packets and messages
    for i, b in enumerate(data):
        if (b & MSEO_MSGEND) and not (b & MSEO_PKTEND):
            raise AssertionError('Invalid MSEO bits in b=0x%x. Not a TRAX dump?' % b)

        if b & MSEO_PKTEND:
            pkt_cnt += 1
            packets.append(TraxPacket(data[packet_start:i + 1], packet_start == 0))
            packet_start = i + 1

        if b & MSEO_MSGEND:
            msg_cnt += 1
            try:
                messages.append(TraxMessage(packets, len(messages) == 0))
            except NotImplementedError as e:
                sys.stderr.write('Failed to parse message #%03d (at %d bytes): %s\n' % (msg_cnt, i, str(e)))
            packets = []

    # Resolve PC ranges of messages.
    # Forward pass: skip messages until a message with known PC,
    # i.e. a SYNC/INDBRSYNC message. Process all messages following it.
    pc = 0
    first_sync_index = -1
    for i, m in enumerate(messages):
        if pc == 0 and m.pc_target == 0:
            continue
        if first_sync_index < 0:
            first_sync_index = i
        pc = m.process_forward(pc)

    # Now process the skipped messages in the reverse direction,
    # starting from the first message with known PC.
    pc = messages[first_sync_index].pc_start
    for m in reversed(messages[0:first_sync_index]):
        if m.truncated:
            break
        pc = m.process_backward(pc)

    return messages


def parse_and_dump(filename, disassemble=WITH_GDB):
    """
    Decode TRAX data from a file, print out the messages.
    :param filename: file to load the dump from
    :param disassemble: if True, print disassembly of PC ranges
    """
    with open(filename, 'rb') as f:
        data = f.read()

    messages = load_messages(data)
    sys.stderr.write('Loaded %d messages in %d bytes\n' % (len(messages), len(data)))

    for i, m in enumerate(messages):
        if m.truncated:
            continue
        print('%04d: %s' % (i, str(m)))
        if m.is_exception:
            print('*** Exception occurred ***')
        if disassemble and WITH_GDB:
            try:
                gdb.execute('disassemble 0x%08x, 0x%08x' % (m.pc_start, m.pc_end))  # noqa: F821
            except gdb.MemoryError:  # noqa: F821
                print('Failed to disassemble from 0x%08x to 0x%08x' % (m.pc_start, m.pc_end))


def main():
    if len(sys.argv) < 2:
        sys.stderr.write('Usage: %s <dump_file>\n')
        raise SystemExit(1)

    parse_and_dump(sys.argv[1])


if __name__ == '__main__' and not WITH_GDB:
    main()
