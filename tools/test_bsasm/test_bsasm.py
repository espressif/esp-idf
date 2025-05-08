#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import os
import re
import shlex
import struct
import subprocess
import tempfile
import unittest
from typing import Any
from typing import List
from typing import Tuple
from typing import TypedDict


class DecompInstOp(TypedDict, total=False):
    val: int
    op: str
    src: int
    tgt: int
    end_val: int
    ctr_add: int
    ctr_set: int


class DecompInst(TypedDict, total=False):
    mux_val: List[int]
    opcode: DecompInstOp
    read_in: int
    wr_out: int
    mux_rel: int
    ctr_src_sel: int
    lut_src_sel: int


class UnpackedBinary(TypedDict, total=False):
    binary_ver: int
    hw_rev: int
    hdr_len: int
    inst_ct: int
    lut_size_words: int
    lut_width_bits: int
    prefetch: int
    trailing_bits: int
    eof_on: int
    padding: int
    inst: List[DecompInst]
    lut: List[int]


current_dir = os.path.dirname(os.path.realpath(__file__))
bsasm_path = os.path.join(current_dir, '..', 'bsasm.py')


class TestAssembler(unittest.TestCase):
    def bit_from_inst(self, data: Tuple[int, int], off: int) -> int:
        return data[off // 8] & (1 << (off & 0x7))

    # returns a field from a list of bytes. Starts at bit off, field is of size length.
    def bits_from_inst(self, data: Tuple[int, int], off: int, length: int) -> int:
        ret = 0
        for i in range(0, length):
            if self.bit_from_inst(data, off + i) != 0:
                ret |= 1 << i
        return ret

    # returns a decomposed instruction
    def decode_inst(self, data: Tuple[int, int]) -> DecompInst:
        ret: DecompInst = {}
        mux_val = []
        for i in range(0, 32):
            mux_val.append(self.bits_from_inst(data, i * 7, 7))
        ret['mux_val'] = mux_val
        opcode = self.bits_from_inst(data, 224, 26)
        op: DecompInstOp = {'val': opcode}
        if opcode & (1 << 25):
            op['op'] = 'LOOPB' if (opcode & (1 << 24)) else 'LOOPA'
            op['tgt'] = (opcode >> 21) & 7
            op['end_val'] = (opcode >> 5) & 0xFFFF
            op['ctr_add'] = opcode & 31
        else:
            sub = (opcode >> 16) & 0x1F
            if sub == 1 or sub == 2:
                op['op'] = 'IF' if (sub == 1) else 'IFN'
                op['src'] = opcode & 0x7F
                op['tgt'] = (opcode >> 21) & 7
            else:
                fl = 'B' if (opcode & (1 << 24)) else 'A'
                hl = (opcode >> 22) & 3
                if hl == 3:
                    pass  # don't add HL
                elif hl == 2:
                    fl += 'H'
                elif hl == 1:
                    fl += 'L'
                elif hl == 0:
                    fl += 'XX'  # shouldn't happen

                if sub == 0:
                    op['op'] = 'ADD' + fl
                    op['ctr_add'] = opcode & 0xFFFF
                elif sub == 3:
                    op['op'] = 'LDCTD' + fl
                    op['ctr_set'] = opcode & 0xFFFF
                elif sub == 4:
                    op['op'] = 'LDCTI' + fl
                elif sub == 5:
                    op['op'] = 'ADDCTI' + fl
        ret['opcode'] = op
        ret['read_in'] = self.bits_from_inst(data, 250, 2)
        ret['wr_out'] = self.bits_from_inst(data, 252, 2)
        ret['mux_rel'] = self.bits_from_inst(data, 254, 1)
        ret['ctr_src_sel'] = self.bits_from_inst(data, 255, 1)
        ret['lut_src_sel'] = self.bits_from_inst(data, 256, 1)
        return ret

    # returns a decomposed binary
    def unpack_binary(self, filename: str) -> UnpackedBinary:
        with open(filename, mode='rb') as f:
            data = f.read()
            ud = struct.unpack('<BBBBHBBHBB', data[:12])
            unpacked: UnpackedBinary = {}
            unpacked['binary_ver'] = ud[0]
            unpacked['hw_rev'] = ud[1]
            unpacked['hdr_len'] = ud[2]
            unpacked['inst_ct'] = ud[3]
            unpacked['lut_size_words'] = ud[4]
            unpacked['lut_width_bits'] = ud[5]
            unpacked['prefetch'] = ud[6]
            unpacked['trailing_bits'] = ud[7]
            unpacked['eof_on'] = ud[8]
            unpacked['padding'] = ud[9]
            off = unpacked['hdr_len'] * 4
            inst = []
            for insno in range(0, unpacked['inst_ct']):
                inst.append(self.decode_inst(struct.unpack_from('B' * 36, data, off)))
                off += 36
            unpacked['inst'] = inst
            unpacked['lut'] = list(
                struct.unpack_from('<' + 'L' * unpacked['lut_size_words'], data, off)
            )
            return unpacked

    def compare(self, out: Any, js: Any, base: str) -> None:
        self.assertEqual(type(out), type(js), ' Diverging types between json and decoded obj: ' + base)
        if type(js) == dict:
            for k in js:
                self.assertTrue(k in out, ' Key not found in decoded output: ' + base + '.' + k)
                self.compare(out[k], js[k], base + '.' + k)
        elif type(js) == list:
            for k in range(0, len(js)):
                self.compare(out[k], js[k], f'{base}[{k}]')
        else:
            self.assertEqual(js, out, f'Items different: {base} (json {js} decoded {out})')

    def test_examples(self) -> None:
        testfiles = []
        for f in os.listdir(os.path.join(current_dir, 'testcases')):
            if f.endswith('.bsasm'):
                testfiles.append(os.path.join(current_dir, 'testcases', f))
        for f in testfiles:
            print(f'Testing {f}...')
            # Extract testing options in the form '#test: key = value'
            cmdlineopts = []
            should_fail = False
            pattern = r'^\s*#\s*test:\s*([^\s=]+)\s*=\s*(.*)\s*$'
            with open(f) as tf:
                for line in tf:
                    match = re.match(pattern, line)
                    if match:
                        if match.group(1) == 'should_fail':
                            if match.group(2) in ['1', 'true', 'True', 'TRUE']:
                                should_fail = True
                        elif match.group(1) == 'cmdlineopts':
                            cmdlineopts = shlex.split(match.group(2))
                        else:
                            self.fail(f'Unknown test option: {match.group(0)}')
            # Generate temp filename and assemble
            with tempfile.NamedTemporaryFile(delete=False) as f_out:
                self.addCleanup(os.unlink, f_out.name)
            args = [bsasm_path, f, f_out.name]
            args.extend(cmdlineopts)
            p = subprocess.run(args, timeout=10)
            if not should_fail:
                self.assertEqual(p.returncode, 0)
            else:
                print('Note: THE TEST EXPECTED BSASM TO ERROR OUT. If there\'s error text above, that is expected.')
                self.assertNotEqual(p.returncode, 0)

            if not should_fail:
                b = self.unpack_binary(f_out.name)
                jsfn = f[:-6] + '.json'
                try:
                    with open(jsfn) as out_desc_f:
                        out_desc = json.load(out_desc_f)
                    # We were able to open the JSON file. See if the keys in it match up with the ones in the decoded fields.
                    self.compare(b, out_desc, '')
                except FileNotFoundError:
                    print(f'File not found: {jsfn}. Printing out decoded contents instead.')
                    print(json.dumps(b, indent=4))


if __name__ == '__main__':
    unittest.main()
