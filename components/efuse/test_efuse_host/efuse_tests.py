#!/usr/bin/env python
from __future__ import print_function, division
import unittest
import struct
import csv
import sys
import subprocess
import tempfile
import os
import StringIO
sys.path.append("..")
from efuse import *

'''
To run the test on local PC:
cd ~/esp/esp-idf/components/efuse/test_efuse_host/
 ./efuse_tests.py
'''


class CSVParserTests(unittest.TestCase):

    def test_general(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                       0,                     5,              Use for test name 1
name2,                   EFUSE_BLK3,                       5,                     4,              Use for test name 2
"""
        t = FuseTable.from_csv(csv)
        t.verify()
        
        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].efuse_block, 'EFUSE_BLK3')
        self.assertEqual(t[0].bit_start,   0)
        self.assertEqual(t[0].bit_count,   5)
        self.assertEqual(t[0].comment,   'Use for test name 1')
        
        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].efuse_block, 'EFUSE_BLK3')
        self.assertEqual(t[1].bit_start,   5)
        self.assertEqual(t[1].bit_count,   4)
        self.assertEqual(t[1].comment,   'Use for test name 2')

    def test_seq_bit_start1_fill(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                       ,                     5,
name2,                   EFUSE_BLK3,                       ,                     4,
"""
        t = FuseTable.from_csv(csv)
        t.verify()
        
        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].bit_start,   0)
        self.assertEqual(t[0].bit_count,   5)
        
        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].bit_start,   5)
        self.assertEqual(t[1].bit_count,   4)

    def test_seq_bit_start2_fill(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                       ,                     5,
name2,                   EFUSE_BLK2,                       ,                     4,
"""
        t = FuseTable.from_csv(csv)
        t.verify()
        
        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].bit_start,   0)
        self.assertEqual(t[0].bit_count,   5)
        
        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].bit_start,   0)
        self.assertEqual(t[1].bit_count,   4)

    def test_seq_bit_start3_fill(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                       ,                     5,
name2,                   EFUSE_BLK2,                       ,                     4,

name3,                   EFUSE_BLK2,                       5,                    4,
"""
        t = FuseTable.from_csv(csv)
        t.verify()
        
        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].bit_start,   0)
        self.assertEqual(t[0].bit_count,   5)
        
        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].bit_start,   0)
        self.assertEqual(t[1].bit_count,   4)
    
        self.assertEqual(t[2].field_name, 'name3')
        self.assertEqual(t[2].bit_start,   5)
        self.assertEqual(t[2].bit_count,   4)

    def test_seq_bit_start4_fill(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                       ,                     5,
name2,                   EFUSE_BLK2,                       ,                     4,
,                        EFUSE_BLK2,                       ,                     4,
name1,                   EFUSE_BLK3,                       ,                     5,
"""
        with self.assertRaisesRegexp(InputError, "Field names must be unique"):
            t = FuseTable.from_csv(csv)


    def test_seq_bit_start5_fill(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                       ,                     5,
name2,                   EFUSE_BLK2,                       ,                     4,
,                        EFUSE_BLK2,                       ,                     4,
name3,                   EFUSE_BLK3,                       5,                    5,
"""
        t = FuseTable.from_csv(csv)
        t.verify()
        
        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].bit_start,   0)
        self.assertEqual(t[0].bit_count,   5)
        
        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].bit_start,   0)
        self.assertEqual(t[1].bit_count,   4)
    
        self.assertEqual(t[2].field_name, 'name2')
        self.assertEqual(t[2].bit_start,   4)
        self.assertEqual(t[2].bit_count,   4)
        
        self.assertEqual(t[3].field_name, 'name3')
        self.assertEqual(t[3].bit_start,   5)
        self.assertEqual(t[3].bit_count,   5)
        
    def test_overlapping_bit_start_fail(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                     1,                     5,              Use for test name 1
name2,                   EFUSE_BLK3,                     5,                     4,              Use for test name 2
            """
        t = FuseTable.from_csv(csv)
        with self.assertRaisesRegexp(InputError, "overlap"):
            t.verify()

    def test_empty_field_name_fail(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
,                        EFUSE_BLK3,                       ,                     5,
name2,                   EFUSE_BLK2,                       ,                     4,
"""
        with self.assertRaisesRegexp(InputError, "missing field name"):
            t = FuseTable.from_csv(csv)
               
    def test_unique_field_name_fail(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                     0,                     5,              Use for test name 1
name1,                   EFUSE_BLK3,                     5,                     4,              Use for test name 2
            """
        with self.assertRaisesRegexp(InputError, "Field names must be unique"):
            t = FuseTable.from_csv(csv)
        
    def test_bit_count_empty_fail(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                     0,                     ,              Use for test name 1
name2,                   EFUSE_BLK3,                     5,                     4,              Use for test name 2
            """
        with self.assertRaisesRegexp(InputError, "empty"):
            t = FuseTable.from_csv(csv)

    def test_bit_start_num_fail(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                     k,                     5,              Use for test name 1
name2,                   EFUSE_BLK3,                     5,                     4,              Use for test name 2
            """
        with self.assertRaisesRegexp(InputError, "Invalid field value"):
            t = FuseTable.from_csv(csv)         

    def test_join_entry(self):       
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK2,                     0,                     6,              Use for test name 1
name2,                   EFUSE_BLK2,                     6,                     5,              Use for test name 2
name3,                   EFUSE_BLK3,                     20,                    5,              Use for test name 3
,                        EFUSE_BLK3,                     30,                    5,              Use for test name 3
name4,                   EFUSE_BLK2,                     30,                    5,              Use for test name 4
            """
        t = FuseTable.from_csv(csv)
        t.verify()
        
        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].efuse_block, 'EFUSE_BLK2')
        self.assertEqual(t[0].bit_start,   0)
        self.assertEqual(t[0].bit_count,   6)
        
        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].efuse_block, 'EFUSE_BLK2')
        self.assertEqual(t[1].bit_start,   6)
        self.assertEqual(t[1].bit_count,   5)
        
        self.assertEqual(t[2].field_name, 'name3')
        self.assertEqual(t[2].efuse_block, 'EFUSE_BLK3')
        self.assertEqual(t[2].bit_start,   20)
        self.assertEqual(t[2].bit_count,   5)
        
        self.assertEqual(t[3].field_name, 'name3')
        self.assertEqual(t[3].efuse_block, 'EFUSE_BLK3')
        self.assertEqual(t[3].bit_start,   30)
        self.assertEqual(t[3].bit_count,   5)  
        
        self.assertEqual(t[4].field_name, 'name4')
        self.assertEqual(t[4].efuse_block, 'EFUSE_BLK2')
        self.assertEqual(t[4].bit_start,   30)
        self.assertEqual(t[4].bit_count,   5)  

    def test_block_fail(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK5,                     0,                     5,              Use for test name 1
name2,                   EFUSE_BLK3,                     5,                     4,              Use for test name 2
            """
        with self.assertRaisesRegexp(InputError, "'efuse_block' should consist from EFUSE_BLK0..EFUSE_BLK3"):
            t = FuseTable.from_csv(csv)    

    def test_field_size_is_ok(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK0,                     0,                     224,            Use for test name 1
name2,                   EFUSE_BLK1,                     0,                     256,            Use for test name 2
            """
        t = FuseTable.from_csv(csv)
        t.verify()

    def test_field_blk0_size_is_more(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK0,                     1,                     224,            Use for test name 1
name2,                   EFUSE_BLK1,                     0,                     256,            Use for test name 2
            """
        t = FuseTable.from_csv(csv)    
        with self.assertRaisesRegexp(InputError, "The field is outside the boundaries"):
            t.verify()

    def test_field_blk1_size_is_more(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK0,                     0,                     224,            Use for test name 1
name2,                   EFUSE_BLK1,                     1,                     256,            Use for test name 2
            """
        t = FuseTable.from_csv(csv)    
        with self.assertRaisesRegexp(InputError, "The field is outside the boundaries"):
            t.verify()
                
class VerificationTests(unittest.TestCase):
    
    def test_bit_start_num_fail(self):
        csv = """
# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3),  bit_start(0..255),    bit_count,        comment
name1,                   EFUSE_BLK3,                     0,                     5,              Use for test name 1
name2,                   EFUSE_BLK3,                     5,                     4,              Use for test name 2
name1_1,                 EFUSE_BLK2,                     0,                     5,              Use for test name 1_1
name2_1,                 EFUSE_BLK2,                     5,                     4,              Use for test name 2_1
            """
        t = FuseTable.from_csv(csv)
        t.verify()
        
        self.assertEqual(t[0].field_name, 'name1')
        self.assertEqual(t[0].efuse_block, 'EFUSE_BLK3')
        self.assertEqual(t[0].bit_start,   0)
        self.assertEqual(t[0].bit_count,   5)
        
        self.assertEqual(t[1].field_name, 'name2')
        self.assertEqual(t[1].efuse_block, 'EFUSE_BLK3')
        self.assertEqual(t[1].bit_start,   5)
        self.assertEqual(t[1].bit_count,   4)
        
        self.assertEqual(t[2].field_name, 'name1_1')
        self.assertEqual(t[2].efuse_block, 'EFUSE_BLK2')
        self.assertEqual(t[2].bit_start,   0)
        self.assertEqual(t[2].bit_count,   5)
        
        self.assertEqual(t[3].field_name, 'name2_1')
        self.assertEqual(t[3].efuse_block, 'EFUSE_BLK2')
        self.assertEqual(t[3].bit_start,   5)
        self.assertEqual(t[3].bit_count,   4)  
                     
if __name__ =="__main__":
    unittest.main()
