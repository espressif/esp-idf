#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import unicode_literals

import filecmp
import hashlib
import os
import random
import struct
import sys
import tempfile
import time
import unittest
from functools import partial
from io import open
from itertools import chain

import pexpect

current_dir = os.path.dirname(os.path.realpath(__file__))
mkuf2_dir = os.path.abspath(os.path.join(current_dir, '..'))
mkuf2_path = os.path.join(mkuf2_dir, 'mkuf2.py')

try:
    import mkuf2
except ImportError:
    sys.path.append(mkuf2_dir)
    import mkuf2


class UF2Block(object):
    def __init__(self, bs):
        self.length = len(bs)

        # See https://github.com/microsoft/uf2 for the format
        first_part = '<' + 'I' * 8
        # payload is between
        last_part = '<I'

        first_part_len = struct.calcsize(first_part)
        last_part_len = struct.calcsize(last_part)

        (self.magicStart0, self.magicStart1, self.flags, self.targetAddr, self.payloadSize, self.blockNo,
            self.numBlocks, self.familyID) = struct.unpack(first_part, bs[:first_part_len])

        self.data = bs[first_part_len:-last_part_len]

        (self.magicEnd, ) = struct.unpack(last_part, bs[-last_part_len:])

    def __len__(self):
        return self.length


class UF2BlockReader(object):
    def __init__(self, f_name):
        self.f_name = f_name

    def get(self):
        with open(self.f_name, 'rb') as f:
            for chunk in iter(partial(f.read, mkuf2.UF2Writer.UF2_BLOCK_SIZE), b''):
                yield UF2Block(chunk)


class BinaryWriter(object):
    def __init__(self, f_name):
        self.f_name = f_name

    def append(self, data):
        # File is reopened several times in order to make sure that won't left open
        with open(self.f_name, 'ab') as f:
            f.write(data)


class BinaryTester(unittest.TestCase):

    def generate_binary(self, size):
        with tempfile.NamedTemporaryFile(delete=False) as f:
            self.addCleanup(os.unlink, f.name)
            for _ in range(size):
                f.write(struct.pack('B', random.randrange(0, 1 << 8)))
            return f.name

    @staticmethod
    def generate_chipID():
        return random.randrange(0, 1 << 32)

    def generate_uf2(self, chip_id, iter_addr_offset_tuples, chunk_size=None):
        of_name = self.generate_binary(0)

        com_args = [mkuf2_path, 'write',
                    '-o', of_name,
                    '--chip-id', hex(chip_id)]
        com_args += [] if chunk_size is None else ['--chunk-size', str(chunk_size)]
        file_args = list(chain(*[(str(addr), f) for addr, f in iter_addr_offset_tuples]))

        p = pexpect.spawn(sys.executable, com_args + file_args, timeout=20)
        self.addCleanup(p.terminate, force=True)

        exp_list = ['Adding {} at {}'.format(f, hex(addr)) for addr, f in iter_addr_offset_tuples]
        exp_list += ['"{}" has been written.'.format(of_name)]
        for e in exp_list:
            p.expect_exact(e)

        # Do non-blocking wait instead of the blocking p.wait():
        for _ in range(10):
            if not p.isalive():
                break
            time.sleep(0.5)
        # else: will be terminated during cleanup

        return of_name

    def process_blocks(self, uf2block, expected_chip_id):
        flags = mkuf2.UF2Writer.UF2_FLAG_FAMILYID_PRESENT | mkuf2.UF2Writer.UF2_FLAG_MD5_PRESENT

        parsed_binaries = []

        block_list = []  # collect block numbers here
        total_blocks = set()  # collect total block numbers here
        for block in UF2BlockReader(uf2block).get():
            if block.blockNo == 0:
                # new file has been detected
                base_addr = block.targetAddr
                current_addr = base_addr
                binary_writer = BinaryWriter(self.generate_binary(0))

            self.assertEqual(len(block), mkuf2.UF2Writer.UF2_BLOCK_SIZE)
            self.assertEqual(block.magicStart0, mkuf2.UF2Writer.UF2_FIRST_MAGIC)
            self.assertEqual(block.magicStart1, mkuf2.UF2Writer.UF2_SECOND_MAGIC)
            self.assertEqual(block.flags & flags, flags)

            self.assertEqual(len(block.data), mkuf2.UF2Writer.UF2_DATA_SIZE)
            payload = block.data[:block.payloadSize]
            md5_obj = hashlib.md5(payload)
            md5_part = block.data[block.payloadSize:block.payloadSize + mkuf2.UF2Writer.UF2_MD5_PART_SIZE]
            address, length = struct.unpack('<II', md5_part[:-md5_obj.digest_size])
            md5sum = md5_part[-md5_obj.digest_size:]
            self.assertEqual(address, block.targetAddr)
            self.assertEqual(length, block.payloadSize)
            self.assertEqual(md5sum, md5_obj.digest())

            self.assertEqual(block.familyID, expected_chip_id)
            self.assertEqual(block.magicEnd, mkuf2.UF2Writer.UF2_FINAL_MAGIC)

            self.assertEqual(current_addr, block.targetAddr)
            binary_writer.append(payload)

            block_list.append(block.blockNo)
            total_blocks.add(block.numBlocks)
            if block.blockNo == block.numBlocks - 1:
                self.assertEqual(block_list, list(range(block.numBlocks)))
                # we have found all blocks and in the right order
                self.assertEqual(total_blocks, {block.numBlocks})  # numBlocks are the same in all the blocks
                del block_list[:]
                total_blocks.clear()

                parsed_binaries += [(base_addr, binary_writer.f_name)]

            current_addr += block.payloadSize
        return parsed_binaries

    def common(self, t, chunk_size=None):
        chip_id = self.generate_chipID()
        parsed_t = self.process_blocks(self.generate_uf2(chip_id, t, chunk_size), chip_id)

        self.assertEqual(len(t), len(parsed_t))
        for (orig_addr, orig_fname), (addr, fname) in zip(t, parsed_t):
            self.assertEqual(orig_addr, addr)
            self.assertTrue(filecmp.cmp(orig_fname, fname))

    def test_simple(self):
        self.common([(0, self.generate_binary(1))])

    def test_more_files(self):
        self.common([(100, self.generate_binary(1)), (200, self.generate_binary(1))])

    def test_larger_files(self):
        self.common([(0x10, self.generate_binary(6)), (0x20, self.generate_binary(8))])

    def test_boundaries(self):
        self.common([(0x100, self.generate_binary(mkuf2.UF2Writer.UF2_DATA_SIZE)),
                     (0x200, self.generate_binary(mkuf2.UF2Writer.UF2_DATA_SIZE + 1)),
                     (0x300, self.generate_binary(mkuf2.UF2Writer.UF2_DATA_SIZE - 1))])

    def test_files_with_more_blocks(self):
        self.common([(0x100, self.generate_binary(3 * mkuf2.UF2Writer.UF2_DATA_SIZE)),
                     (0x200, self.generate_binary(2 * mkuf2.UF2Writer.UF2_DATA_SIZE + 1)),
                     (0x300, self.generate_binary(2 * mkuf2.UF2Writer.UF2_DATA_SIZE - 1))])

    def test_very_large_files(self):
        self.common([(0x100, self.generate_binary(20 * mkuf2.UF2Writer.UF2_DATA_SIZE + 5)),
                     (0x10000, self.generate_binary(50 * mkuf2.UF2Writer.UF2_DATA_SIZE + 100)),
                     (0x100000, self.generate_binary(100 * mkuf2.UF2Writer.UF2_DATA_SIZE))])

    def test_chunk_size(self):
        chunk_size = 256
        self.common([(0x100, self.generate_binary(chunk_size)),
                     (0x200, self.generate_binary(chunk_size + 1)),
                     (0x300, self.generate_binary(chunk_size - 1))],
                    chunk_size)


if __name__ == '__main__':
    unittest.main()
