# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
from __future__ import unicode_literals

import textwrap

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_examples_cbor(dut: Dut) -> None:

    dut.expect(r'example: encoded buffer size \d+')
    dut.expect('example: convert CBOR to JSON')
    parsed_info = dut.expect(r'\[\{"chip":"(\w+)","unicore":(\w+),"ip":\[(\d+),(\d+),(\d+),(\d+)\]\},'
                             r'3.1400001049041748'
                             r',"simple\(99\)","2019-07-10 09:00:00\+0000","undefined"\]')
    dut.expect('example: decode CBOR manually')

    dut.expect(textwrap.dedent(r'''
                                Array\[\s+
                                  Map{{\s+
                                    chip\s+
                                    {}\s+
                                    unicore\s+
                                    {}\s+
                                    ip\s+
                                    Array\[\s+
                                      {}\s+
                                      {}\s+
                                      {}\s+
                                      {}\s+
                                    \]\s+
                                  }}\s+
                                  3.14\s+
                                  simple\(99\)\s+
                                  2019-07-10 09:00:00\+0000\s+
                                  undefined\s+
                                \]'''.format(parsed_info[1].decode(), parsed_info[2].decode(), parsed_info[3].decode(),
                                             parsed_info[4].decode(), parsed_info[5].decode(),parsed_info[6].decode())).replace('{', r'\{').replace('}', r'\}'))
