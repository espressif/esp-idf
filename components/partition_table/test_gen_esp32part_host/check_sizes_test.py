#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from __future__ import division, print_function

import io
import sys
import unittest

from test_utils import Py23TestCase

try:
    from typing import IO
except ImportError:
    pass  # only needed for type annotations

try:
    import check_sizes
except ImportError:
    sys.path.append('..')
    import check_sizes

try:
    import gen_esp32part
except ImportError:
    sys.path.append('..')
    import gen_esp32part


BASE_CSV = """
# Name,   Type, SubType, Offset,   Size, Flags
# Note: if you have increased the bootloader size, make sure to update the offsets to avoid overlap
nvs,      data, nvs,     ,        0x4000,
otadata,  data, ota,     ,        0x2000,
phy_init, data, phy,     ,        0x1000,

"""


def gen_table(factory_appsize=None, ota0_appsize=None, ota1_appsize=None):  # type: (str, str, str) -> io.BytesIO
    """ generate a partition table binary with up to 3 app partitions with the specified sizes. """
    csv = BASE_CSV
    if factory_appsize:
        csv += 'factory, app, factory, , {}\n'.format(factory_appsize)
    if ota0_appsize:
        csv += 'ota0, app, ota_0, , {}\n'.format(ota0_appsize)
    if ota1_appsize:
        csv += 'ota1, app, ota_1, , {}\n'.format(ota1_appsize)

    table = gen_esp32part.PartitionTable.from_csv(csv)
    return io.BytesIO(table.to_binary())


def fake_file(kilobytes, name):  # type: (int, str) -> IO
    result = io.BytesIO(b'\xEE' * kilobytes * 1024)
    result.name = name
    return result


class TestAppSizes(Py23TestCase):

    def test_sizes_ok(self):  # type: () -> None
        pt = gen_table('1M')
        app = fake_file(512, 'test.bin')
        check_sizes.check_partition('app', None, pt, app)

    def test_single_part_too_small(self):  # type: () -> None
        pt = gen_table('1M')
        app = fake_file(1500, 'too_big.bin')
        with self.assertRaises(SystemExit) as e:
            check_sizes.check_partition('app', None, pt, app)
        self.assertIn('too small', str(e.exception))
        self.assertIn('too_big.bin', str(e.exception))

    def test_all_parts_too_small(self):  # type: () -> None
        pt = gen_table('500K', '500K', '500K')
        app = fake_file(501, 'just_too_big.bin')
        with self.assertRaises(SystemExit) as e:
            check_sizes.check_partition('app', None, pt, app)
        self.assertIn('All', str(e.exception))
        self.assertIn('too small', str(e.exception))
        self.assertIn('just_too_big.bin', str(e.exception))

    def test_one_part_too_small_warning(self):  # type: () -> None
        pt = gen_table('500K', '1M', '1M')
        app = fake_file(501, 'big.bin')
        # this will print a warning, no easy way to verify it looks correct
        check_sizes.check_partition('app', None, pt, app)


class TestBootloaderSizes(Py23TestCase):

    def test_sizes_ok(self):  # type: () -> None
        bootloader = fake_file(25, 'test.bin')
        check_sizes.check_bootloader(0x8000, 0x1000, bootloader)

    def test_bootloader_too_big_default(self):  # type: () -> None
        bootloader = fake_file(40, 'test.bin')
        with self.assertRaises(SystemExit) as e:
            check_sizes.check_bootloader(0x8000, 0x1000, bootloader)
        self.assertIn('overlap', str(e.exception))
        # move the partition table offset up, it will pass
        check_sizes.check_bootloader(0xb000, 0x1000, bootloader)


if __name__ == '__main__':
    unittest.main()
