#!/usr/bin/env python
#
# esp-idf NVS partition generation tool. Tool helps in generating NVS-compatible
# partition binary, with key-value pair entries provided via a CSV file.
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
import sys
import argparse
import binascii
import getopt
import struct
import os
import array
import csv
import zlib
from os import path

""" Class for standard NVS page structure """
class Page(object):
    PAGE_PARAMS = {
        "max_size": 4096,
        "max_blob_size": 1984,
        "max_entries": 126
    }

    # Item type codes
    U8   = 0x01
    I8   = 0x11
    U16  = 0x02
    I16  = 0x12
    U32  = 0x04
    I32  = 0x14
    SZ   = 0x21
    BLOB = 0x41

    # Few Page constants
    HEADER_SIZE = 32
    BITMAPARRAY_OFFSET = 32
    BITMAPARRAY_SIZE_IN_BYTES = 32
    FIRST_ENTRY_OFFSET = 64
    SINGLE_ENTRY_SIZE = 32

    def __init__(self, page_num):
        self.entry_num = 0
        self.bitmap_array = array.array('B')
        self.page_buf = bytearray(b'\xff')*Page.PAGE_PARAMS["max_size"]
        self.bitmap_array = self.create_bitmap_array()
        self.set_header(page_num)

    def set_header(self, page_num):
        # set page state to active
        page_header= bytearray(b'\xff')*32
        page_state_active_seq = 0xFFFFFFFE
        page_header[0:4] = struct.pack('<I', page_state_active_seq)
        # set page sequence number
        page_header[4:8] = struct.pack('<I', page_num)
        # set header's CRC
        crc_data = page_header[4:28]
        crc = zlib.crc32(buffer(crc_data), 0xFFFFFFFF)
        page_header[28:32] = struct.pack('<I', crc & 0xFFFFFFFF)
        self.page_buf[0:len(page_header)] = page_header

    def create_bitmap_array(self):
        bitarray = array.array('B')
        charsize = 32 # bitmaparray has 256 bits, hence 32 bytes
        fill = 255 # Fill all 8 bits with 1's
        bitarray.extend((fill,) * charsize)
        return bitarray

    def write_bitmaparray(self):
        bitnum = self.entry_num * 2
        byte_idx = bitnum / 8 # Find byte index in the array
        bit_offset = bitnum & 7 # Find bit offset in given byte index
        mask = ~(1 << bit_offset)
        self.bitmap_array[byte_idx] &= mask
        start_idx = Page.BITMAPARRAY_OFFSET
        end_idx = Page.BITMAPARRAY_OFFSET + Page.BITMAPARRAY_SIZE_IN_BYTES
        self.page_buf[start_idx:end_idx] = self.bitmap_array

    def write_entry_to_buf(self, data, entrycount):
        data_offset = Page.FIRST_ENTRY_OFFSET + (Page.SINGLE_ENTRY_SIZE * self.entry_num)
        start_idx = data_offset
        end_idx = data_offset + len(data)
        self.page_buf[start_idx:end_idx]  = data

        # Set bitmap array for entries in current page
        for i in range(0, entrycount):
            self.write_bitmaparray()
            self.entry_num += 1

    """
    Low-level function to write variable length data into page buffer. Data should be formatted
    according to encoding specified.
    """
    def write_varlen_data(self, key, data, encoding, ns_index):
        datalen = len(data)
        if datalen > Page.PAGE_PARAMS["max_blob_size"]:
            raise InputError("%s: Size exceeds max allowed length." % key)

        # Calculate no. of entries data will require
        rounded_size = (datalen + 31) & ~31
        data_entry_count = rounded_size / 32
        total_entry_count = data_entry_count + 1 # +1 for the entry header

        # Check if page is already full and new page is needed to be created right away
        if (self.entry_num + total_entry_count) >= Page.PAGE_PARAMS["max_entries"]:
            raise PageFullError()

        # Entry header
        entry_struct = bytearray('\xff')*32
        entry_struct[0] = ns_index # namespace index
        entry_struct[2] = data_entry_count + 1 # Span

        # set key
        key_array = bytearray('\x00')*16
        entry_struct[8:24] = key_array
        entry_struct[8:8 + len(key)] = key

        # set Type
        if encoding == "string":
            entry_struct[1] = Page.SZ
        elif encoding == "hex2bin" or encoding == "binary":
            entry_struct[1] = Page.BLOB

        # compute CRC of data
        entry_struct[24:26] = struct.pack('<H', datalen)
        crc = zlib.crc32(data, 0xFFFFFFFF)
        entry_struct[28:32] = struct.pack('<I', crc & 0xFFFFFFFF)

        # compute crc of entry header
        crc_data = bytearray(28)
        crc_data[0:4] = entry_struct[0:4]
        crc_data[4:28] = entry_struct[8:32]
        crc = zlib.crc32(buffer(crc_data), 0xFFFFFFFF)
        entry_struct[4:8] = struct.pack('<I', crc & 0xFFFFFFFF)

        # write entry header
        self.write_entry_to_buf(entry_struct, 1)
        # write actual data
        self.write_entry_to_buf(data, data_entry_count)

    """ Low-level function to write data of primitive type into page buffer. """
    def write_primitive_data(self, key, data, encoding, ns_index):
        # Check if entry exceeds max number of entries allowed per page
        if self.entry_num >= Page.PAGE_PARAMS["max_entries"]:
            raise PageFullError()

        entry_struct = bytearray('\xff')*32
        entry_struct[0] = ns_index # namespace index
        entry_struct[2] = 0x01 # Span

        # write key
        key_array = bytearray('\x00')*16
        entry_struct[8:24] = key_array
        entry_struct[8:8 + len(key)] = key

        if encoding == "u8":
            entry_struct[1] = Page.U8
            entry_struct[24] = struct.pack('<B', data)
        elif encoding == "i8":
            entry_struct[1] = Page.I8
            entry_struct[24] = struct.pack('<b', data)
        elif encoding == "u16":
            entry_struct[1] = Page.U16
            entry_struct[24:26] = struct.pack('<H', data)
        elif encoding == "u32":
            entry_struct[1] = Page.U32
            entry_struct[24:28] = struct.pack('<I', data)
        elif encoding == "i32":
            entry_struct[1] = Page.I32
            entry_struct[24:28] = struct.pack('<i', data)

        # Compute CRC
        crc_data = bytearray(28)
        crc_data[0:4] = entry_struct[0:4]
        crc_data[4:28] = entry_struct[8:32]
        crc = zlib.crc32(buffer(crc_data), 0xFFFFFFFF)
        entry_struct[4:8] = struct.pack('<I', crc & 0xFFFFFFFF)

        # write to file
        self.write_entry_to_buf(entry_struct, 1)

    """ Get page buffer data of a given page """
    def get_data(self):
        return self.page_buf

"""
NVS class encapsulates all NVS specific operations to create a binary with given key-value pairs. Binary can later be flashed onto device via a flashing utility.
"""
class NVS(object):
    def __init__(self, fout):
        self.namespace_idx = 0
        self.page_num = -1
        self.pages = []
        self.cur_page = self.create_new_page()
        self.fout = fout

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if exc_type == None and exc_value == None:
            result = self.get_binary_data()
            self.fout.write(result)

    def create_new_page(self):
        self.page_num += 1
        new_page = Page(self.page_num)
        self.pages.append(new_page)
        self.cur_page = new_page
        return new_page

    """
    Write namespace entry and subsequently increase namespace count so that all upcoming entries
    will be mapped to a new namespace.
    """
    def write_namespace(self, key):
        self.namespace_idx += 1
        try:
            self.cur_page.write_primitive_data(key, self.namespace_idx, "u8", 0)
        except PageFullError:
            new_page = self.create_new_page()
            new_page.write_primitive_data(key, self.namespace_idx, "u8", 0)
            pass

    """
    Write key-value pair. Function accepts value in the form of ascii character and converts
    it into appropriate format before calling Page class's functions to write entry into NVS format.
    Function handles PageFullError and creates a new page and re-invokes the function on a new page.
    We don't have to guard re-invocation with try-except since no entry can span multiple pages.
    """
    def write_entry(self, key, value, encoding):
        if encoding == "hex2bin":
            if len(value) % 2 != 0:
                raise InputError("%s: Invalid data length. Should be multiple of 2." % key)
            value = binascii.a2b_hex(value)

        if encoding == "string":
            value += '\0'

        encoding = encoding.lower()
        varlen_encodings = ["string", "binary", "hex2bin"]
        primitive_encodings = ["u8", "i8", "u16", "u32", "i32"]
        if encoding in varlen_encodings:
            try:
                self.cur_page.write_varlen_data(key, value, encoding, self.namespace_idx)
            except PageFullError:
                new_page = self.create_new_page()
                new_page.write_varlen_data(key, value, encoding, self.namespace_idx)
                pass
        elif encoding in primitive_encodings:
            try:
                self.cur_page.write_primitive_data(key, int(value), encoding, self.namespace_idx)
            except PageFullError:
                new_page = self.create_new_page()
                new_page.write_primitive_data(key, int(value), encoding, self.namespace_idx)
                sys.exc_clear()
                pass
        else:
            raise InputError("%s: Unsupported encoding" % encoding)

    """ Return accumulated data of all pages """
    def get_binary_data(self):
        data = bytearray()
        for page in self.pages:
            data += page.get_data()
        return data

class PageFullError(RuntimeError):
    """
    Represents error when current page doesn't have sufficient entries left
    to accommodate current request
    """
    def __init__(self):
       super(PageFullError, self).__init__()

class InputError(RuntimeError):
    """
    Represents error on the input
    """
    def __init__(self, e):
        super(InputError, self).__init__(e)

def nvs_open(result_obj):
    """ Wrapper to create and NVS class object. This object can later be used to set key-value pairs

    :param result_obj: File/Stream object to dump resultant binary. If data is to be dumped into memory, one way is to use BytesIO object
    :return: NVS class instance
    """
    return NVS(result_obj)

def write_entry(nvs_instance, key, datatype, encoding, value):
    """ Wrapper to set key-value pair in NVS format

    :param nvs_instance: Instance of an NVS class returned by nvs_open()
    :param key: Key of the data
    :param datatype: Data type. Valid values are "file", "data" and "namespace"
    :param encoding: Data encoding. Valid values are "u8", "i8", "u16", "u32", "i32", "string", "binary" and "hex2bin"
    :param value: Data value in ascii encoded string format for "data" datatype and filepath for "file" datatype
    :return: None
    """
    if datatype == "file":
        abs_file_path = value
        if os.path.isabs(value) == False:
            script_dir = os.path.dirname(__file__)
            abs_file_path = os.path.join(script_dir, value)
        with open(abs_file_path, 'rb') as f:
            value = f.read()

    if datatype == "namespace":
        nvs_instance.write_namespace(key)
    else:
        nvs_instance.write_entry(key, value, encoding)

def nvs_close(nvs_instance):
    """ Wrapper to finish writing to NVS and write data to file/stream object provided to nvs_open method

    :param nvs_instance: Instance of NVS class returned by nvs_open()
    :return: None
    """
    nvs_instance.__exit__(None, None, None)

def nvs_part_gen(input_filename=None, output_filename=None):
    input_file = open(input_filename, 'rb')
    output_file = open(output_filename, 'wb')

    with nvs_open(output_file) as nvs_obj:
        reader = csv.DictReader(input_file, delimiter=',')
        for row in reader:
            try:
                write_entry(nvs_obj, row["key"], row["type"], row["encoding"], row["value"])
            except InputError as e:
                print(e)
                input_file.close()
                output_file.close()
                exit(-2)

    input_file.close()
    output_file.close()

def main():
    parser = argparse.ArgumentParser(description="ESP32 NVS partition generation utility")
    parser.add_argument(
            "input",
            help="Path to CSV file to parse. Will use stdin if omitted",
            default=sys.stdin)

    parser.add_argument(
            "output",
            help='Path to output converted binary file. Will use stdout if omitted',
            default=sys.stdout)

    args = parser.parse_args()
    input_filename = args.input
    output_filename = args.output
    nvs_part_gen(input_filename, output_filename)



if __name__ == "__main__":
    main()
