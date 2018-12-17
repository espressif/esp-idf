#!/usr/bin/env python
from __future__ import print_function, division
import unittest
import struct
import csv
import sys
import subprocess
import tempfile
import os
import io
import re

try:
    import gen_esp32part
except ImportError:
    sys.path.append("..")
    import gen_esp32part

SIMPLE_CSV = """
# Name,Type,SubType,Offset,Size,Flags
factory,0,2,65536,1048576,
"""

LONGER_BINARY_TABLE = b""
# type 0x00, subtype 0x00,
# offset 64KB, size 1MB
LONGER_BINARY_TABLE += b"\xAA\x50\x00\x00" + \
                       b"\x00\x00\x01\x00" + \
                       b"\x00\x00\x10\x00" + \
                       b"factory\0" + (b"\0" * 8) + \
                       b"\x00\x00\x00\x00"
# type 0x01, subtype 0x20,
# offset 0x110000, size 128KB
LONGER_BINARY_TABLE += b"\xAA\x50\x01\x20" + \
                       b"\x00\x00\x11\x00" + \
                       b"\x00\x02\x00\x00" + \
                       b"data" + (b"\0" * 12) + \
                       b"\x00\x00\x00\x00"
# type 0x10, subtype 0x00,
# offset 0x150000, size 1MB
LONGER_BINARY_TABLE += b"\xAA\x50\x10\x00" + \
                       b"\x00\x00\x15\x00" + \
                       b"\x00\x10\x00\x00" + \
                       b"second" + (b"\0" * 10) + \
                       b"\x00\x00\x00\x00"
# MD5 checksum
LONGER_BINARY_TABLE += b"\xEB\xEB" + b"\xFF" * 14
LONGER_BINARY_TABLE += b'\xf9\xbd\x06\x1b\x45\x68\x6f\x86\x57\x1a\x2c\xd5\x2a\x1d\xa6\x5b'
# empty partition
LONGER_BINARY_TABLE += b"\xFF" * 32


def _strip_trailing_ffs(binary_table):
    """
    Strip all FFs down to the last 32 bytes (terminating entry)
    """
    while binary_table.endswith(b"\xFF" * 64):
        binary_table = binary_table[0:len(binary_table) - 32]
    return binary_table


class Py23TestCase(unittest.TestCase):

    def __init__(self, *args, **kwargs):
        super(Py23TestCase, self).__init__(*args, **kwargs)
        try:
            self.assertRaisesRegex
        except AttributeError:
            # assertRaisesRegexp is deprecated in Python3 but assertRaisesRegex doesn't exist in Python2
            # This fix is used in order to avoid using the alias from the six library
            self.assertRaisesRegex = self.assertRaisesRegexp


class CSVParserTests(Py23TestCase):

    def test_simple_partition(self):
        table = gen_esp32part.PartitionTable.from_csv(SIMPLE_CSV)
        self.assertEqual(len(table), 1)
        self.assertEqual(table[0].name, "factory")
        self.assertEqual(table[0].type, 0)
        self.assertEqual(table[0].subtype, 2)
        self.assertEqual(table[0].offset, 65536)
        self.assertEqual(table[0].size, 1048576)

    def test_require_type(self):
        csv = """
# Name,Type, SubType,Offset,Size
ihavenotype,
"""
        with self.assertRaisesRegex(gen_esp32part.InputError, "type"):
            gen_esp32part.PartitionTable.from_csv(csv)

    def test_type_subtype_names(self):
        csv_magicnumbers = """
# Name, Type, SubType, Offset, Size
myapp, 0, 0,,  0x100000
myota_0, 0, 0x10,, 0x100000
myota_1, 0, 0x11,, 0x100000
myota_15, 0, 0x1f,, 0x100000
mytest, 0, 0x20,, 0x100000
myota_status, 1, 0,, 0x100000
        """
        csv_nomagicnumbers = """
# Name, Type, SubType, Offset, Size
myapp, app, factory,, 0x100000
myota_0, app, ota_0,, 0x100000
myota_1, app, ota_1,, 0x100000
myota_15, app, ota_15,, 0x100000
mytest, app, test,, 0x100000
myota_status, data, ota,, 0x100000
"""
        # make two equivalent partition tables, one using
        # magic numbers and one using shortcuts. Ensure they match
        magic = gen_esp32part.PartitionTable.from_csv(csv_magicnumbers)
        magic.verify()
        nomagic = gen_esp32part.PartitionTable.from_csv(csv_nomagicnumbers)
        nomagic.verify()

        self.assertEqual(nomagic["myapp"].type, 0)
        self.assertEqual(nomagic["myapp"].subtype, 0)
        self.assertEqual(nomagic["myapp"], magic["myapp"])
        self.assertEqual(nomagic["myota_0"].type, 0)
        self.assertEqual(nomagic["myota_0"].subtype, 0x10)
        self.assertEqual(nomagic["myota_0"], magic["myota_0"])
        self.assertEqual(nomagic["myota_15"], magic["myota_15"])
        self.assertEqual(nomagic["mytest"], magic["mytest"])
        self.assertEqual(nomagic["myota_status"], magic["myota_status"])

        # self.assertEqual(nomagic.to_binary(), magic.to_binary())

    def test_unit_suffixes(self):
        csv = """
# Name, Type, Subtype, Offset, Size
one_megabyte, app, factory, 64k, 1M
"""
        t = gen_esp32part.PartitionTable.from_csv(csv)
        t.verify()
        self.assertEqual(t[0].offset, 64 * 1024)
        self.assertEqual(t[0].size, 1 * 1024 * 1024)

    def test_default_offsets(self):
        csv = """
# Name, Type, Subtype, Offset, Size
first, app, factory,, 1M
second, data, 0x15,, 1M
minidata, data, 0x40,, 32K
otherapp, app, factory,, 1M
        """
        t = gen_esp32part.PartitionTable.from_csv(csv)
        # 'first'
        self.assertEqual(t[0].offset, 0x010000)  # 64KB boundary as it's an app image
        self.assertEqual(t[0].size,   0x100000)  # Size specified in CSV
        # 'second'
        self.assertEqual(t[1].offset, 0x110000)  # prev offset+size
        self.assertEqual(t[1].size,   0x100000)  # Size specified in CSV
        # 'minidata'
        self.assertEqual(t[2].offset, 0x210000)
        # 'otherapp'
        self.assertEqual(t[3].offset, 0x220000)  # 64KB boundary as it's an app image

    def test_negative_size_to_offset(self):
        csv = """
# Name, Type, Subtype, Offset, Size
first, app, factory, 0x10000, -2M
second, data, 0x15,         ,  1M
        """
        t = gen_esp32part.PartitionTable.from_csv(csv)
        t.verify()
        # 'first'
        self.assertEqual(t[0].offset, 0x10000)  # in CSV
        self.assertEqual(t[0].size,   0x200000 - t[0].offset)  # Up to 2M
        # 'second'
        self.assertEqual(t[1].offset, 0x200000)  # prev offset+size

    def test_overlapping_offsets_fail(self):
        csv = """
first, app, factory, 0x100000, 2M
second, app, ota_0,  0x200000, 1M
"""
        with self.assertRaisesRegex(gen_esp32part.InputError, "overlap"):
            t = gen_esp32part.PartitionTable.from_csv(csv)
            t.verify()

    def test_unique_name_fail(self):
        csv = """
first, app, factory, 0x100000, 1M
first, app, ota_0,  0x200000, 1M
"""
        with self.assertRaisesRegex(gen_esp32part.InputError, "Partition names must be unique"):
            t = gen_esp32part.PartitionTable.from_csv(csv)
            t.verify()


class BinaryOutputTests(Py23TestCase):
    def test_binary_entry(self):
        csv = """
first, 0x30, 0xEE, 0x100400, 0x300000
"""
        t = gen_esp32part.PartitionTable.from_csv(csv)
        tb = _strip_trailing_ffs(t.to_binary())
        self.assertEqual(len(tb), 64 + 32)
        self.assertEqual(b'\xAA\x50', tb[0:2])  # magic
        self.assertEqual(b'\x30\xee', tb[2:4])  # type, subtype
        eo, es = struct.unpack("<LL", tb[4:12])
        self.assertEqual(eo, 0x100400)  # offset
        self.assertEqual(es, 0x300000)  # size
        self.assertEqual(b"\xEB\xEB" + b"\xFF" * 14, tb[32:48])
        self.assertEqual(b'\x43\x03\x3f\x33\x40\x87\x57\x51\x69\x83\x9b\x40\x61\xb1\x27\x26', tb[48:64])

    def test_multiple_entries(self):
        csv = """
first, 0x30, 0xEE, 0x100400, 0x300000
second,0x31, 0xEF,         , 0x100000
"""
        t = gen_esp32part.PartitionTable.from_csv(csv)
        tb = _strip_trailing_ffs(t.to_binary())
        self.assertEqual(len(tb), 96 + 32)
        self.assertEqual(b'\xAA\x50', tb[0:2])
        self.assertEqual(b'\xAA\x50', tb[32:34])

    def test_encrypted_flag(self):
        csv = """
# Name, Type, Subtype, Offset, Size, Flags
first, app, factory,, 1M, encrypted
"""
        t = gen_esp32part.PartitionTable.from_csv(csv)
        self.assertTrue(t[0].encrypted)
        tb = _strip_trailing_ffs(t.to_binary())
        tr = gen_esp32part.PartitionTable.from_binary(tb)
        self.assertTrue(tr[0].encrypted)


class BinaryParserTests(Py23TestCase):
    def test_parse_one_entry(self):
        # type 0x30, subtype 0xee,
        # offset 1MB, size 2MB
        entry = b"\xAA\x50\x30\xee" + \
                b"\x00\x00\x10\x00" + \
                b"\x00\x00\x20\x00" + \
                b"0123456789abc\0\0\0" + \
                b"\x00\x00\x00\x00" + \
                b"\xFF" * 32
        # verify that parsing 32 bytes as a table
        # or as a single Definition are the same thing
        t = gen_esp32part.PartitionTable.from_binary(entry)
        self.assertEqual(len(t), 1)
        t[0].verify()

        e = gen_esp32part.PartitionDefinition.from_binary(entry[:32])
        self.assertEqual(t[0], e)
        e.verify()

        self.assertEqual(e.type, 0x30)
        self.assertEqual(e.subtype, 0xEE)
        self.assertEqual(e.offset, 0x100000)
        self.assertEqual(e.size,   0x200000)
        self.assertEqual(e.name, "0123456789abc")

    def test_multiple_entries(self):
        t = gen_esp32part.PartitionTable.from_binary(LONGER_BINARY_TABLE)
        t.verify()

        self.assertEqual(3, len(t))
        self.assertEqual(t[0].type, gen_esp32part.APP_TYPE)
        self.assertEqual(t[0].name, "factory")

        self.assertEqual(t[1].type, gen_esp32part.DATA_TYPE)
        self.assertEqual(t[1].name, "data")

        self.assertEqual(t[2].type, 0x10)
        self.assertEqual(t[2].name, "second")

        round_trip = _strip_trailing_ffs(t.to_binary())
        self.assertEqual(round_trip, LONGER_BINARY_TABLE)

    def test_bad_magic(self):
        bad_magic = b"OHAI" + \
                    b"\x00\x00\x10\x00" + \
                    b"\x00\x00\x20\x00" + \
                    b"0123456789abc\0\0\0" + \
                    b"\x00\x00\x00\x00"
        with self.assertRaisesRegex(gen_esp32part.InputError, "Invalid magic bytes"):
            gen_esp32part.PartitionTable.from_binary(bad_magic)

    def test_bad_length(self):
        bad_length = b"OHAI" + \
                     b"\x00\x00\x10\x00" + \
                     b"\x00\x00\x20\x00" + \
                     b"0123456789"
        with self.assertRaisesRegex(gen_esp32part.InputError, "32 bytes"):
            gen_esp32part.PartitionTable.from_binary(bad_length)


class CSVOutputTests(Py23TestCase):

    def _readcsv(self, source_str):
        return list(csv.reader(source_str.split("\n")))

    def test_output_simple_formatting(self):
        table = gen_esp32part.PartitionTable.from_csv(SIMPLE_CSV)
        as_csv = table.to_csv(True)
        c = self._readcsv(as_csv)
        # first two lines should start with comments
        self.assertEqual(c[0][0][0], "#")
        self.assertEqual(c[1][0][0], "#")
        row = c[2]
        self.assertEqual(row[0], "factory")
        self.assertEqual(row[1], "0")
        self.assertEqual(row[2], "2")
        self.assertEqual(row[3], "0x10000")  # reformatted as hex
        self.assertEqual(row[4], "0x100000")  # also hex

        # round trip back to a PartitionTable and check is identical
        roundtrip = gen_esp32part.PartitionTable.from_csv(as_csv)
        self.assertEqual(roundtrip, table)

    def test_output_smart_formatting(self):
        table = gen_esp32part.PartitionTable.from_csv(SIMPLE_CSV)
        as_csv = table.to_csv(False)
        c = self._readcsv(as_csv)
        # first two lines should start with comments
        self.assertEqual(c[0][0][0], "#")
        self.assertEqual(c[1][0][0], "#")
        row = c[2]
        self.assertEqual(row[0], "factory")
        self.assertEqual(row[1], "app")
        self.assertEqual(row[2], "2")
        self.assertEqual(row[3], "0x10000")
        self.assertEqual(row[4], "1M")

        # round trip back to a PartitionTable and check is identical
        roundtrip = gen_esp32part.PartitionTable.from_csv(as_csv)
        self.assertEqual(roundtrip, table)


class CommandLineTests(Py23TestCase):

    def test_basic_cmdline(self):
        try:
            binpath = tempfile.mktemp()
            csvpath = tempfile.mktemp()

            # copy binary contents to temp file
            with open(binpath, 'wb') as f:
                f.write(LONGER_BINARY_TABLE)

            # run gen_esp32part.py to convert binary file to CSV
            output = subprocess.check_output([sys.executable, "../gen_esp32part.py",
                                              binpath, csvpath], stderr=subprocess.STDOUT)
            # reopen the CSV and check the generated binary is identical
            self.assertNotIn(b"WARNING", output)
            with open(csvpath, 'r') as f:
                from_csv = gen_esp32part.PartitionTable.from_csv(f.read())
            self.assertEqual(_strip_trailing_ffs(from_csv.to_binary()), LONGER_BINARY_TABLE)

            # run gen_esp32part.py to conver the CSV to binary again
            output = subprocess.check_output([sys.executable, "../gen_esp32part.py",
                                              csvpath, binpath], stderr=subprocess.STDOUT)
            self.assertNotIn(b"WARNING", output)
            # assert that file reads back as identical
            with open(binpath, 'rb') as f:
                binary_readback = f.read()
            binary_readback = _strip_trailing_ffs(binary_readback)
            self.assertEqual(binary_readback, LONGER_BINARY_TABLE)

        finally:
            for path in binpath, csvpath:
                try:
                    os.remove(path)
                except OSError:
                    pass


class VerificationTests(Py23TestCase):

    def test_bad_alignment(self):
        csv = """
# Name,Type, SubType,Offset,Size
app,app, factory, 32K, 1M
"""
        with self.assertRaisesRegex(gen_esp32part.ValidationError, r"Offset.+not aligned"):
            t = gen_esp32part.PartitionTable.from_csv(csv)
            t.verify()

    def test_warnings(self):
        try:
            sys.stderr = io.StringIO()  # capture stderr

            csv_1 = "app, 1, 2, 32K, 1M\n"
            gen_esp32part.PartitionTable.from_csv(csv_1).verify()
            self.assertIn("WARNING", sys.stderr.getvalue())
            self.assertIn("partition type", sys.stderr.getvalue())

            sys.stderr = io.StringIO()
            csv_2 = "ota_0, app, ota_1, , 1M\n"
            gen_esp32part.PartitionTable.from_csv(csv_2).verify()
            self.assertIn("WARNING", sys.stderr.getvalue())
            self.assertIn("partition subtype", sys.stderr.getvalue())

        finally:
            sys.stderr = sys.__stderr__


class PartToolTests(Py23TestCase):

    def _run_parttool(self, csvcontents, args, info):
        csvpath = tempfile.mktemp()
        with open(csvpath, "w") as f:
            f.write(csvcontents)
        try:
            output = subprocess.check_output([sys.executable, "../parttool.py"] + args.split(" ")
                                             + ["--partition-table-file", csvpath, "get_partition_info", "--info", info],
                                             stderr=subprocess.STDOUT)
            self.assertNotIn(b"WARNING", output)
            m = re.search(b"0x[0-9a-fA-F]+", output)
            return m.group(0) if m else ""
        finally:
            os.remove(csvpath)

    def test_find_basic(self):
        csv = """
nvs,      data, nvs,     0x9000,  0x4000
otadata,  data, ota,     0xd000,  0x2000
phy_init, data, phy,     0xf000,  0x1000
factory,  app, factory, 0x10000,  1M
        """

        def rpt(args, info):
            return self._run_parttool(csv, args, info)

        self.assertEqual(
            rpt("--partition-type=data --partition-subtype=nvs -q", "offset"), b"0x9000")
        self.assertEqual(
            rpt("--partition-type=data --partition-subtype=nvs -q", "size"), b"0x4000")
        self.assertEqual(
            rpt("--partition-name=otadata -q", "offset"), b"0xd000")
        self.assertEqual(
            rpt("--partition-boot-default -q", "offset"), b"0x10000")

    def test_fallback(self):
        csv = """
nvs,      data, nvs,     0x9000,  0x4000
otadata,  data, ota,     0xd000,  0x2000
phy_init, data, phy,     0xf000,  0x1000
ota_0,  app,    ota_0,   0x30000,  1M
ota_1,  app,    ota_1,          ,  1M
        """

        def rpt(args, info):
            return self._run_parttool(csv, args, info)

        self.assertEqual(
            rpt("--partition-type=app --partition-subtype=ota_1 -q", "offset"), b"0x130000")
        self.assertEqual(
            rpt("--partition-boot-default -q", "offset"), b"0x30000")  # ota_0
        csv_mod = csv.replace("ota_0", "ota_2")
        self.assertEqual(
            self._run_parttool(csv_mod, "--partition-boot-default -q", "offset"),
            b"0x130000")  # now default is ota_1


if __name__ == "__main__":
    unittest.main()
