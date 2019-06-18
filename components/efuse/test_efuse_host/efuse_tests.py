#!/usr/bin/env python
from __future__ import print_function, division
import unittest
import sys

try:
    import efuse_table_gen
except ImportError:
    sys.path.append("..")
    import efuse_table_gen


'''
To run the test on local PC:
cd ~/esp/esp-idf/components/efuse/test_efuse_host/
 ./efuse_tests.py
'''


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

    def test_general(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                       0,                     5,              Use for test name 1
name2,                   EFUSE_BLK3,                       5,                     4,              Use for test name 2
"""
        t = efuse_table_gen.FuseTable.from_csv(csv)
        t.verify()

        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].efuse_block, 'EFUSE_BLK3')
        self.assertEqual(t[0].bit_start, 0)
        self.assertEqual(t[0].bit_count, 5)
        self.assertEqual(t[0].comment, 'Use for test name 1')

        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].efuse_block, 'EFUSE_BLK3')
        self.assertEqual(t[1].bit_start, 5)
        self.assertEqual(t[1].bit_count, 4)
        self.assertEqual(t[1].comment, 'Use for test name 2')

    def test_seq_bit_start1_fill(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                       ,                     5,
name2,                   EFUSE_BLK3,                       ,                     4,
"""
        t = efuse_table_gen.FuseTable.from_csv(csv)
        t.verify()

        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].bit_start, 0)
        self.assertEqual(t[0].bit_count, 5)

        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].bit_start, 5)
        self.assertEqual(t[1].bit_count, 4)

    def test_seq_bit_start2_fill(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                       ,                     5,
name2,                   EFUSE_BLK2,                       ,                     4,
"""
        t = efuse_table_gen.FuseTable.from_csv(csv)
        t.verify()

        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].bit_start, 0)
        self.assertEqual(t[0].bit_count, 5)

        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].bit_start, 0)
        self.assertEqual(t[1].bit_count, 4)

    def test_seq_bit_start3_fill(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                       ,                     5,
name2,                   EFUSE_BLK2,                       ,                     4,

name3,                   EFUSE_BLK2,                       5,                    4,
"""
        t = efuse_table_gen.FuseTable.from_csv(csv)
        t.verify()

        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].bit_start, 0)
        self.assertEqual(t[0].bit_count, 5)

        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].bit_start, 0)
        self.assertEqual(t[1].bit_count, 4)

        self.assertEqual(t[2].field_name, 'name3')
        self.assertEqual(t[2].bit_start, 5)
        self.assertEqual(t[2].bit_count, 4)

    def test_seq_bit_start4_fill(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                       ,                     5,
name2,                   EFUSE_BLK2,                       ,                     4,
,                        EFUSE_BLK2,                       ,                     4,
name1,                   EFUSE_BLK3,                       ,                     5,
"""
        with self.assertRaisesRegex(efuse_table_gen.InputError, "Field names must be unique"):
            efuse_table_gen.FuseTable.from_csv(csv)

    def test_seq_bit_start5_fill(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                       ,                     5,
name2,                   EFUSE_BLK2,                       ,                     4,
,                        EFUSE_BLK2,                       ,                     4,
name3,                   EFUSE_BLK3,                       5,                    5,
"""
        t = efuse_table_gen.FuseTable.from_csv(csv)
        t.verify()

        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].bit_start, 0)
        self.assertEqual(t[0].bit_count, 5)

        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].bit_start, 0)
        self.assertEqual(t[1].bit_count, 4)

        self.assertEqual(t[2].field_name, 'name2')
        self.assertEqual(t[2].bit_start, 4)
        self.assertEqual(t[2].bit_count, 4)

        self.assertEqual(t[3].field_name, 'name3')
        self.assertEqual(t[3].bit_start, 5)
        self.assertEqual(t[3].bit_count, 5)

    def test_overlapping_bit_start_fail(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                     1,                     5,              Use for test name 1
name2,                   EFUSE_BLK3,                     5,                     4,              Use for test name 2
            """
        t = efuse_table_gen.FuseTable.from_csv(csv)
        with self.assertRaisesRegex(efuse_table_gen.InputError, "overlap"):
            t.verify()

    def test_empty_field_name_fail(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
,                        EFUSE_BLK3,                       ,                     5,
name2,                   EFUSE_BLK2,                       ,                     4,
"""
        with self.assertRaisesRegex(efuse_table_gen.InputError, "missing field name"):
            efuse_table_gen.FuseTable.from_csv(csv)

    def test_unique_field_name_fail(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                     0,                     5,              Use for test name 1
name1,                   EFUSE_BLK3,                     5,                     4,              Use for test name 2
            """
        with self.assertRaisesRegex(efuse_table_gen.InputError, "Field names must be unique"):
            efuse_table_gen.FuseTable.from_csv(csv)

    def test_bit_count_empty_fail(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                     0,                     ,              Use for test name 1
name2,                   EFUSE_BLK3,                     5,                     4,              Use for test name 2
            """
        with self.assertRaisesRegex(efuse_table_gen.InputError, "empty"):
            efuse_table_gen.FuseTable.from_csv(csv)

    def test_bit_start_num_fail(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                     k,                     5,              Use for test name 1
name2,                   EFUSE_BLK3,                     5,                     4,              Use for test name 2
            """
        with self.assertRaisesRegex(efuse_table_gen.InputError, "Invalid field value"):
            efuse_table_gen.FuseTable.from_csv(csv)

    def test_join_entry(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK2,                     0,                     6,              Use for test name 1
name2,                   EFUSE_BLK2,                     6,                     5,              Use for test name 2
name3,                   EFUSE_BLK3,                     20,                    5,              Use for test name 3
,                        EFUSE_BLK3,                     30,                    5,              Use for test name 3
name4,                   EFUSE_BLK2,                     30,                    5,              Use for test name 4
            """
        t = efuse_table_gen.FuseTable.from_csv(csv)
        t.verify()

        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].efuse_block, 'EFUSE_BLK2')
        self.assertEqual(t[0].bit_start, 0)
        self.assertEqual(t[0].bit_count, 6)

        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].efuse_block, 'EFUSE_BLK2')
        self.assertEqual(t[1].bit_start, 6)
        self.assertEqual(t[1].bit_count, 5)

        self.assertEqual(t[2].field_name, 'name3')
        self.assertEqual(t[2].efuse_block, 'EFUSE_BLK3')
        self.assertEqual(t[2].bit_start, 20)
        self.assertEqual(t[2].bit_count, 5)

        self.assertEqual(t[3].field_name, 'name3')
        self.assertEqual(t[3].efuse_block, 'EFUSE_BLK3')
        self.assertEqual(t[3].bit_start, 30)
        self.assertEqual(t[3].bit_count, 5)

        self.assertEqual(t[4].field_name, 'name4')
        self.assertEqual(t[4].efuse_block, 'EFUSE_BLK2')
        self.assertEqual(t[4].bit_start, 30)
        self.assertEqual(t[4].bit_count, 5)

    def test_block_fail(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK5,                     0,                     5,              Use for test name 1
name2,                   EFUSE_BLK3,                     5,                     4,              Use for test name 2
            """
        with self.assertRaisesRegex(efuse_table_gen.InputError, "'efuse_block' should be one of EFUSE_BLK0..EFUSE_BLK3"):
            efuse_table_gen.FuseTable.from_csv(csv)

    def test_field_size_is_ok(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK0,                     0,                     224,            Use for test name 1
name2,                   EFUSE_BLK1,                     0,                     256,            Use for test name 2
            """
        efuse_table_gen.max_blk_len = 256
        t = efuse_table_gen.FuseTable.from_csv(csv)
        t.verify()

    def test_field_blk3_size_is_more(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                     190,                   1,            Use for test name 1
name2,                   EFUSE_BLK3,                     191,                   5,            Use for test name 2
            """
        efuse_table_gen.max_blk_len = 192
        t = efuse_table_gen.FuseTable.from_csv(csv)
        with self.assertRaisesRegex(efuse_table_gen.InputError, "The field is outside the boundaries"):
            t.verify()

    def test_field_blk1_size_is_more(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK0,                     0,                     224,            Use for test name 1
name2,                   EFUSE_BLK1,                     1,                     256,            Use for test name 2
            """
        t = efuse_table_gen.FuseTable.from_csv(csv)
        with self.assertRaisesRegex(efuse_table_gen.InputError, "The field is outside the boundaries"):
            t.verify()


class VerificationTests(Py23TestCase):

    def test_general(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                     0,                     5,              Use for test name 1
name2,                   EFUSE_BLK3,                     5,                     4,              Use for test name 2
name1_1,                 EFUSE_BLK2,                     0,                     5,              Use for test name 1_1
name2_1,                 EFUSE_BLK2,                     5,                     4,              Use for test name 2_1
            """
        t = efuse_table_gen.FuseTable.from_csv(csv)
        t.verify()

        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].efuse_block, 'EFUSE_BLK3')
        self.assertEqual(t[0].bit_start, 0)
        self.assertEqual(t[0].bit_count, 5)

        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].efuse_block, 'EFUSE_BLK3')
        self.assertEqual(t[1].bit_start, 5)
        self.assertEqual(t[1].bit_count, 4)

        self.assertEqual(t[2].field_name, 'name1_1')
        self.assertEqual(t[2].efuse_block, 'EFUSE_BLK2')
        self.assertEqual(t[2].bit_start, 0)
        self.assertEqual(t[2].bit_count, 5)

        self.assertEqual(t[3].field_name, 'name2_1')
        self.assertEqual(t[3].efuse_block, 'EFUSE_BLK2')
        self.assertEqual(t[3].bit_start, 5)
        self.assertEqual(t[3].bit_count, 4)

    def test_custom_use_only_BLK3(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                     0,                     5,              Use for test name 1
name2,                   EFUSE_BLK2,                     5,                     4,              Use for test name 2
            """
        t = efuse_table_gen.FuseTable.from_csv(csv)
        with self.assertRaisesRegex(efuse_table_gen.ValidationError, "custom_table should use only EFUSE_BLK3"):
            t.verify("custom_table")

    def test_common_and_custom_table_use_the_same_bits(self):
        csv_common = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                     0,                     5,              Use for test name 1
name2,                   EFUSE_BLK2,                     5,                     4,              Use for test name 2
                     """
        common_table = efuse_table_gen.FuseTable.from_csv(csv_common)
        common_table.verify("common_table")
        two_tables = common_table

        csv_custom = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name3,                   EFUSE_BLK3,                     20,                    5,              Use for test name 1
name4,                   EFUSE_BLK3,                      4,                    1,              Use for test name 2
            """
        custom_table = efuse_table_gen.FuseTable.from_csv(csv_custom)
        custom_table.verify("custom_table")

        two_tables += custom_table
        with self.assertRaisesRegex(efuse_table_gen.InputError, "overlaps"):
            two_tables.verify()


if __name__ == "__main__":
    unittest.main()
