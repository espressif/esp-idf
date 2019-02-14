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

from __future__ import division, print_function
from builtins import int, range, bytes
from io import open
import sys
import argparse
import binascii
import random
import struct
import os
import array
import csv
import zlib
import codecs
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend

VERSION1_PRINT = "v1 - Multipage Blob Support Disabled"
VERSION2_PRINT = "v2 - Multipage Blob Support Enabled"


""" Class for standard NVS page structure """


class Page(object):
    PAGE_PARAMS = {
        "max_size": 4096,
        "max_old_blob_size": 1984,
        "max_new_blob_size": 4000,
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
    BLOB_DATA = 0x42
    BLOB_IDX = 0x48

    # Few Page constants
    HEADER_SIZE = 32
    BITMAPARRAY_OFFSET = 32
    BITMAPARRAY_SIZE_IN_BYTES = 32
    FIRST_ENTRY_OFFSET = 64
    SINGLE_ENTRY_SIZE = 32
    CHUNK_ANY = 0xFF
    ACTIVE = 0xFFFFFFFE
    FULL = 0xFFFFFFFC
    VERSION1 = 0xFF
    VERSION2 = 0xFE

    def __init__(self, page_num, is_rsrv_page=False):
        self.entry_num = 0
        self.is_encrypt = False
        self.encr_key = None
        self.bitmap_array = array.array('B')
        self.version = Page.VERSION2
        self.page_buf = bytearray(b'\xff') * Page.PAGE_PARAMS["max_size"]
        if not is_rsrv_page:
            self.bitmap_array = self.create_bitmap_array()
            self.set_header(page_num)

    def set_header(self, page_num):
        global page_header

        # set page state to active
        page_header = bytearray(b'\xff') * 32
        page_state_active_seq = Page.ACTIVE
        struct.pack_into('<I', page_header, 0,  page_state_active_seq)
        # set page sequence number
        struct.pack_into('<I', page_header, 4, page_num)
        # set version
        if version == Page.VERSION2:
            page_header[8] = Page.VERSION2
        elif version == Page.VERSION1:
            page_header[8] = Page.VERSION1
        # set header's CRC
        crc_data = bytes(page_header[4:28])
        crc = zlib.crc32(crc_data, 0xFFFFFFFF)
        struct.pack_into('<I', page_header, 28, crc & 0xFFFFFFFF)
        self.page_buf[0:len(page_header)] = page_header

    def create_bitmap_array(self):
        bitarray = array.array('B')
        charsize = 32  # bitmaparray has 256 bits, hence 32 bytes
        fill = 255  # Fill all 8 bits with 1's
        bitarray.extend((fill,) * charsize)
        return bitarray

    def write_bitmaparray(self):
        bitnum = self.entry_num * 2
        byte_idx = bitnum // 8  # Find byte index in the array
        bit_offset = bitnum & 7  # Find bit offset in given byte index
        mask = ~(1 << bit_offset)
        self.bitmap_array[byte_idx] &= mask
        start_idx = Page.BITMAPARRAY_OFFSET
        end_idx = Page.BITMAPARRAY_OFFSET + Page.BITMAPARRAY_SIZE_IN_BYTES
        self.page_buf[start_idx:end_idx] = self.bitmap_array

    def encrypt_entry(self, data_arr, tweak_arr, encr_key):
        # Encrypt 32 bytes of data using AES-XTS encryption
        backend = default_backend()
        plain_text = codecs.decode(data_arr, 'hex')
        tweak = codecs.decode(tweak_arr, 'hex')

        cipher = Cipher(algorithms.AES(encr_key), modes.XTS(tweak), backend=backend)
        encryptor = cipher.encryptor()
        encrypted_data = encryptor.update(plain_text)

        return encrypted_data

    def reverse_hexbytes(self, addr_tmp):
        addr = []
        reversed_bytes = ""
        for i in range(0, len(addr_tmp), 2):
            addr.append(addr_tmp[i:i + 2])
        reversed_bytes = "".join(reversed(addr))

        return reversed_bytes

    def encrypt_data(self, data_input, no_of_entries, nvs_obj):
        # Set values needed for encryption and encrypt data byte wise
        encr_data_to_write = bytearray()
        data_len_needed = 64  # in hex
        tweak_len_needed = 32  # in hex
        init_tweak_val = '0'
        init_data_val = 'f'
        tweak_tmp = ''
        encr_key_input = None

        # Extract encryption key and tweak key from given key input
        if len(self.encr_key) == key_len_needed:
            encr_key_input = self.encr_key
        else:
            encr_key_input = codecs.decode(self.encr_key, 'hex')

        rel_addr = nvs_obj.page_num * Page.PAGE_PARAMS["max_size"] + Page.FIRST_ENTRY_OFFSET

        if not isinstance(data_input, bytearray):
            byte_arr = bytearray(b'\xff') * 32
            byte_arr[0:len(data_input)] = data_input
            data_input = byte_arr

        data_input = binascii.hexlify(data_input)

        entry_no = self.entry_num
        start_idx = 0
        end_idx = start_idx + 64

        for _ in range(0, no_of_entries):
            # Set tweak value
            offset = entry_no * Page.SINGLE_ENTRY_SIZE
            addr = hex(rel_addr + offset)[2:]
            addr_len = len(addr)
            if addr_len > 2:
                if not addr_len % 2:
                    addr_tmp = addr
                    tweak_tmp = self.reverse_hexbytes(addr_tmp)
                    tweak_val = tweak_tmp + (init_tweak_val * (tweak_len_needed - (len(tweak_tmp))))
                else:
                    addr_tmp = init_tweak_val + addr
                    tweak_tmp = self.reverse_hexbytes(addr_tmp)
                    tweak_val = tweak_tmp + (init_tweak_val * (tweak_len_needed - (len(tweak_tmp))))
            else:
                tweak_val = addr + (init_tweak_val * (tweak_len_needed - len(addr)))

            # Encrypt data
            data_bytes = data_input[start_idx:end_idx]
            if type(data_bytes) == bytes:
                data_bytes = data_bytes.decode()

            data_val = data_bytes + (init_data_val * (data_len_needed - len(data_bytes)))
            encr_data_ret = self.encrypt_entry(data_val, tweak_val, encr_key_input)
            encr_data_to_write = encr_data_to_write + encr_data_ret
            # Update values for encrypting next set of data bytes
            start_idx = end_idx
            end_idx = start_idx + 64
            entry_no += 1

        return encr_data_to_write

    def write_entry_to_buf(self, data, entrycount,nvs_obj):
        encr_data = bytearray()

        if self.is_encrypt:
            encr_data_ret = self.encrypt_data(data, entrycount,nvs_obj)
            encr_data[0:len(encr_data_ret)] = encr_data_ret
            data = encr_data

        data_offset = Page.FIRST_ENTRY_OFFSET + (Page.SINGLE_ENTRY_SIZE * self.entry_num)
        start_idx = data_offset
        end_idx = data_offset + len(data)
        self.page_buf[start_idx:end_idx]  = data

        # Set bitmap array for entries in current page
        for i in range(0, entrycount):
            self.write_bitmaparray()
            self.entry_num += 1

    def set_crc_header(self, entry_struct):
        crc_data = bytearray(b'28')
        crc_data[0:4] = entry_struct[0:4]
        crc_data[4:28] = entry_struct[8:32]
        crc_data = bytes(crc_data)
        crc = zlib.crc32(crc_data, 0xFFFFFFFF)
        struct.pack_into('<I', entry_struct, 4, crc & 0xFFFFFFFF)
        return entry_struct

    def write_varlen_binary_data(self, entry_struct, ns_index, key, data, data_size, total_entry_count, encoding, nvs_obj):
        chunk_start = 0
        chunk_count = 0
        chunk_index = Page.CHUNK_ANY
        offset = 0
        remaining_size = data_size
        tailroom = None

        while True:
            chunk_size = 0

            # Get the size available in current page
            tailroom = (Page.PAGE_PARAMS["max_entries"] - self.entry_num - 1) * Page.SINGLE_ENTRY_SIZE
            assert tailroom >= 0, "Page overflow!!"

            # Split the binary data into two and store a chunk of available size onto curr page
            if tailroom < remaining_size:
                chunk_size = tailroom
            else:
                chunk_size = remaining_size

            remaining_size = remaining_size - chunk_size

            # Change type of data to BLOB_DATA
            entry_struct[1] = Page.BLOB_DATA

            # Calculate no. of entries data chunk will require
            datachunk_rounded_size = (chunk_size + 31) & ~31
            datachunk_entry_count = datachunk_rounded_size // 32
            datachunk_total_entry_count = datachunk_entry_count + 1  # +1 for the entry header

            # Set Span
            entry_struct[2] = datachunk_total_entry_count

            # Update the chunkIndex
            chunk_index = chunk_start + chunk_count
            entry_struct[3] = chunk_index

            # Set data chunk
            data_chunk = data[offset:offset + chunk_size]

            # Compute CRC of data chunk
            struct.pack_into('<H', entry_struct, 24, chunk_size)

            if type(data) != bytes:
                data_chunk = bytes(data_chunk, encoding='utf8')

            crc = zlib.crc32(data_chunk, 0xFFFFFFFF)
            struct.pack_into('<I', entry_struct, 28, crc & 0xFFFFFFFF)

            # compute crc of entry header
            entry_struct = self.set_crc_header(entry_struct)

            # write entry header
            self.write_entry_to_buf(entry_struct, 1,nvs_obj)
            # write actual data
            self.write_entry_to_buf(data_chunk, datachunk_entry_count,nvs_obj)

            chunk_count = chunk_count + 1

            if remaining_size or (tailroom - chunk_size) < Page.SINGLE_ENTRY_SIZE:
                if page_header[0:4] != Page.FULL:
                    page_state_full_seq = Page.FULL
                    struct.pack_into('<I', page_header, 0, page_state_full_seq)
                nvs_obj.create_new_page()
                self = nvs_obj.cur_page

            offset = offset + chunk_size

            # All chunks are stored, now store the index
            if not remaining_size:
                # Initialise data field to 0xff
                data_array = bytearray(b'\xff') * 8
                entry_struct[24:32] = data_array

                # change type of data to BLOB_IDX
                entry_struct[1] = Page.BLOB_IDX

                # Set Span
                entry_struct[2] = 1

                # Update the chunkIndex
                chunk_index = Page.CHUNK_ANY
                entry_struct[3] = chunk_index

                struct.pack_into('<I', entry_struct, 24, data_size)
                entry_struct[28] = chunk_count
                entry_struct[29] = chunk_start

                # compute crc of entry header
                entry_struct = self.set_crc_header(entry_struct)

                # write last entry
                self.write_entry_to_buf(entry_struct, 1,nvs_obj)
                break

        return entry_struct

    def write_single_page_entry(self, entry_struct, data, datalen, data_entry_count, nvs_obj):
        # compute CRC of data
        struct.pack_into('<H', entry_struct, 24, datalen)

        if type(data) != bytes:
            data = bytes(data, encoding='utf8')

        crc = zlib.crc32(data, 0xFFFFFFFF)
        struct.pack_into('<I', entry_struct, 28, crc & 0xFFFFFFFF)

        # compute crc of entry header
        entry_struct = self.set_crc_header(entry_struct)

        # write entry header
        self.write_entry_to_buf(entry_struct, 1, nvs_obj)
        # write actual data
        self.write_entry_to_buf(data, data_entry_count, nvs_obj)

    """
    Low-level function to write variable length data into page buffer. Data should be formatted
    according to encoding specified.
    """
    def write_varlen_data(self, key, data, encoding, ns_index,nvs_obj):
        # Set size of data
        datalen = len(data)

        if datalen > Page.PAGE_PARAMS["max_old_blob_size"]:
            if version == Page.VERSION1:
                raise InputError("Version %s\n%s: Size exceeds max allowed length." % (VERSION1_PRINT,key))
            else:
                if encoding == "string":
                    raise InputError("Version %s\n%s: Size exceeds max allowed length." % (VERSION2_PRINT,key))

        # Calculate no. of entries data will require
        rounded_size = (datalen + 31) & ~31
        data_entry_count = rounded_size // 32
        total_entry_count = data_entry_count + 1  # +1 for the entry header

        # Check if page is already full and new page is needed to be created right away
        if self.entry_num >= Page.PAGE_PARAMS["max_entries"]:
            raise PageFullError()
        elif (self.entry_num + total_entry_count) >= Page.PAGE_PARAMS["max_entries"]:
            if not (version == Page.VERSION2 and encoding in ["hex2bin", "binary", "base64"]):
                raise PageFullError()

        # Entry header
        entry_struct = bytearray(b'\xff') * 32
        # Set Namespace Index
        entry_struct[0] = ns_index
        # Set Span
        if version == Page.VERSION2:
            if encoding == "string":
                entry_struct[2] = data_entry_count + 1
            # Set Chunk Index
            chunk_index = Page.CHUNK_ANY
            entry_struct[3] = chunk_index
        else:
            entry_struct[2] = data_entry_count + 1

        # set key
        key_array = b'\x00' * 16
        entry_struct[8:24] = key_array
        entry_struct[8:8 + len(key)] = key.encode()

        # set Type
        if encoding == "string":
            entry_struct[1] = Page.SZ
        elif encoding in ["hex2bin", "binary", "base64"]:
            entry_struct[1] = Page.BLOB

        if version == Page.VERSION2 and (encoding in ["hex2bin", "binary", "base64"]):
            entry_struct = self.write_varlen_binary_data(entry_struct,ns_index,key,data,
                                                         datalen,total_entry_count, encoding, nvs_obj)
        else:
            self.write_single_page_entry(entry_struct, data, datalen, data_entry_count, nvs_obj)

    """ Low-level function to write data of primitive type into page buffer. """
    def write_primitive_data(self, key, data, encoding, ns_index,nvs_obj):
        # Check if entry exceeds max number of entries allowed per page
        if self.entry_num >= Page.PAGE_PARAMS["max_entries"]:
            raise PageFullError()

        entry_struct = bytearray(b'\xff') * 32
        entry_struct[0] = ns_index  # namespace index
        entry_struct[2] = 0x01  # Span
        chunk_index = Page.CHUNK_ANY
        entry_struct[3] = chunk_index

        # write key
        key_array = b'\x00' * 16
        entry_struct[8:24] = key_array
        entry_struct[8:8 + len(key)] = key.encode()

        if encoding == "u8":
            entry_struct[1] = Page.U8
            struct.pack_into('<B', entry_struct, 24, data)
        elif encoding == "i8":
            entry_struct[1] = Page.I8
            struct.pack_into('<b', entry_struct, 24, data)
        elif encoding == "u16":
            entry_struct[1] = Page.U16
            struct.pack_into('<H', entry_struct, 24, data)
        elif encoding == "u32":
            entry_struct[1] = Page.U32
            struct.pack_into('<I', entry_struct, 24, data)
        elif encoding == "i32":
            entry_struct[1] = Page.I32
            struct.pack_into('<i', entry_struct, 24, data)

        # Compute CRC
        crc_data = bytearray(b'28')
        crc_data[0:4] = entry_struct[0:4]
        crc_data[4:28] = entry_struct[8:32]
        crc_data = bytes(crc_data)
        crc = zlib.crc32(crc_data, 0xFFFFFFFF)
        struct.pack_into('<I', entry_struct, 4, crc & 0xFFFFFFFF)

        # write to file
        self.write_entry_to_buf(entry_struct, 1,nvs_obj)

    """ Get page buffer data of a given page """
    def get_data(self):
        return self.page_buf


"""
NVS class encapsulates all NVS specific operations to create a binary with given key-value pairs.
Binary can later be flashed onto device via a flashing utility.
"""


class NVS(object):
    def __init__(self, fout, input_size):
        self.size = input_size
        self.namespace_idx = 0
        self.page_num = -1
        self.pages = []
        self.cur_page = self.create_new_page()
        self.fout = fout

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if exc_type is None and exc_value is None:
            # Create pages for remaining available size
            while True:
                try:
                    self.create_new_page()
                except InsufficientSizeError:
                    self.size = None
                    # Creating the last reserved page
                    self.create_new_page(is_rsrv_page=True)
                    break

            result = self.get_binary_data()
            if version == Page.VERSION1:
                print("Version: ", VERSION1_PRINT)
            else:
                print("Version: ", VERSION2_PRINT)
            self.fout.write(result)

    def create_new_page(self, is_rsrv_page=False):
        # Update available size as each page is created
        if self.size == 0:
            raise InsufficientSizeError("Size parameter is is less than the size of data in csv.Please increase size.")
        if not is_rsrv_page:
            self.size = self.size - Page.PAGE_PARAMS["max_size"]
        self.page_num += 1
        new_page = Page(self.page_num, is_rsrv_page)
        new_page.version = version
        new_page.is_encrypt = is_encrypt_data
        if new_page.is_encrypt:
            new_page.encr_key = key_input
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
            self.cur_page.write_primitive_data(key, self.namespace_idx, "u8", 0,self)
        except PageFullError:
            new_page = self.create_new_page()
            new_page.write_primitive_data(key, self.namespace_idx, "u8", 0,self)

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

        if encoding == "base64":
            value = binascii.a2b_base64(value)

        if encoding == "string":
            if type(value) == bytes:
                value = value.decode()
            value += '\0'

        encoding = encoding.lower()
        varlen_encodings = ["string", "binary", "hex2bin", "base64"]
        primitive_encodings = ["u8", "i8", "u16", "u32", "i32"]

        if encoding in varlen_encodings:
            try:
                self.cur_page.write_varlen_data(key, value, encoding, self.namespace_idx,self)
            except PageFullError:
                new_page = self.create_new_page()
                new_page.write_varlen_data(key, value, encoding, self.namespace_idx,self)
        elif encoding in primitive_encodings:
            try:
                self.cur_page.write_primitive_data(key, int(value), encoding, self.namespace_idx,self)
            except PageFullError:
                new_page = self.create_new_page()
                new_page.write_primitive_data(key, int(value), encoding, self.namespace_idx,self)
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


class InsufficientSizeError(RuntimeError):
    """
    Represents error when NVS Partition size given is insufficient
    to accomodate the data in the given csv file
    """
    def __init__(self, e):
        super(InsufficientSizeError, self).__init__(e)


def nvs_open(result_obj, input_size):
    """ Wrapper to create and NVS class object. This object can later be used to set key-value pairs

    :param result_obj: File/Stream object to dump resultant binary. If data is to be dumped into memory, one way is to use BytesIO object
    :param input_size: Size of Partition
    :return: NVS class instance
    """
    return NVS(result_obj, input_size)


def write_entry(nvs_instance, key, datatype, encoding, value):
    """ Wrapper to set key-value pair in NVS format

    :param nvs_instance: Instance of an NVS class returned by nvs_open()
    :param key: Key of the data
    :param datatype: Data type. Valid values are "file", "data" and "namespace"
    :param encoding: Data encoding. Valid values are "u8", "i8", "u16", "u32", "i32", "string", "binary", "hex2bin" and "base64"
    :param value: Data value in ascii encoded string format for "data" datatype and filepath for "file" datatype
    :return: None
    """

    if datatype == "file":
        abs_file_path = value
        if os.path.isabs(value) is False:
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


def check_input_args(input_filename=None, output_filename=None, input_part_size=None, is_key_gen=None,
                     encrypt_mode=None, key_file=None, version_no=None, print_arg_str=None, print_encrypt_arg_str=None):

    global version, is_encrypt_data, input_size, key_gen

    version = version_no
    is_encrypt_data = encrypt_mode
    key_gen = is_key_gen
    input_size = input_part_size

    if is_encrypt_data.lower() == 'true':
        is_encrypt_data = True
    elif is_encrypt_data.lower() == 'false':
        is_encrypt_data = False

    if version == 'v1':
        version = Page.VERSION1
    elif version == 'v2':
        version = Page.VERSION2

    if key_gen.lower() == 'true':
        key_gen = True
    elif key_gen.lower() == 'false':
        key_gen = False

    if key_gen:
        if all(arg is not None for arg in [input_filename, output_filename, input_size]):
            if not is_encrypt_data:
                sys.exit("--encrypt argument is missing or set to false.")
        elif any(arg is not None for arg in [input_filename, output_filename, input_size]):
            sys.exit(print_arg_str)
    else:
        if not input_size:
            if not all(arg is not None for arg in [input_filename, output_filename]):
                sys.exit(print_arg_str)

    if is_encrypt_data and not key_gen and not key_file:
        sys.exit(print_encrypt_arg_str)

    if is_encrypt_data and key_gen and key_file:
        sys.exit(print_encrypt_arg_str)

    if not is_encrypt_data and key_file:
        sys.exit("Invalid. Cannot give --keyfile as --encrypt is set to false.")

    if input_size:
        # Set size
        input_size = int(input_size, 0)

        if input_size % 4096 != 0:
            sys.exit("Size of partition must be multiple of 4096")

        # Update size as a page needs to be reserved of size 4KB
        input_size = input_size - Page.PAGE_PARAMS["max_size"]

        if input_size < (2 * Page.PAGE_PARAMS["max_size"]):
            sys.exit("Minimum NVS partition size needed is 0x3000 bytes.")


def nvs_part_gen(input_filename=None, output_filename=None, input_part_size=None, is_key_gen=None, encrypt_mode=None, key_file=None, version_no=None):
    """ Wrapper to generate nvs partition binary

    :param input_filename: Name of input file containing data
    :param output_filename: Name of output file to store generated binary
    :param input_part_size: Size of partition in bytes (must be multiple of 4096)
    :param is_key_gen: Enable encryption key generation in encryption mode
    :param encrypt_mode: Enable/Disable encryption mode
    :param key_file: Input file having encryption keys in encryption mode
    :param version_no: Format Version number
    :return: None
    """

    global key_input, key_len_needed

    key_len_needed = 64
    key_input = bytearray()

    if key_gen:
        key_input = ''.join(random.choice('0123456789abcdef') for _ in range(128)).strip()
    elif key_file:
        with open(key_file, 'rb') as key_f:
            key_input = key_f.read(64)

    if all(arg is not None for arg in [input_filename, output_filename, input_size]):
        input_file = open(input_filename, 'rt', encoding='utf8')
        output_file = open(output_filename, 'wb')

        with nvs_open(output_file, input_size) as nvs_obj:
            reader = csv.DictReader(input_file, delimiter=',')
            for row in reader:
                try:
                    write_entry(nvs_obj, row["key"], row["type"], row["encoding"], row["value"])
                except (InputError) as e:
                    print(e)
                    input_file.close()
                    output_file.close()
                    sys.exit(-2)

        input_file.close()
        output_file.close()

    if key_gen:
        keys_page_buf = bytearray(b'\xff') * Page.PAGE_PARAMS["max_size"]
        key_bytes = bytearray()
        if len(key_input) == key_len_needed:
            key_bytes = key_input
        else:
            key_bytes = codecs.decode(key_input, 'hex')
        key_len = len(key_bytes)
        keys_page_buf[0:key_len] = key_bytes
        crc_data = keys_page_buf[0:key_len]
        crc_data = bytes(crc_data)
        crc = zlib.crc32(crc_data, 0xFFFFFFFF)
        struct.pack_into('<I', keys_page_buf, key_len,  crc & 0xFFFFFFFF)
        with open("encryption_keys.bin",'wb') as output_keys_file:
            output_keys_file.write(keys_page_buf)

    print("Binary created.")


def main():
    parser = argparse.ArgumentParser(description="ESP32 NVS partition generation utility")
    nvs_part_gen_group = parser.add_argument_group('To generate NVS partition')
    nvs_part_gen_group.add_argument(
        "--input",
        help="Path to CSV file to parse.",
        default=None)

    nvs_part_gen_group.add_argument(
        "--output",
        help='Path to output converted binary file.',
        default=None)

    nvs_part_gen_group.add_argument(
        "--size",
        help='Size of NVS Partition in bytes (must be multiple of 4096)')

    nvs_part_gen_group.add_argument(
        "--version",
        help='Set version. Default: v2',
        choices=['v1','v2'],
        default='v2',
        type=str.lower)

    keygen_action = nvs_part_gen_group.add_argument(
        "--keygen",
        help='Generate keys for encryption. Creates an `encryption_keys.bin` file. Default: false',
        choices=['true','false'],
        default='false',
        type=str.lower)

    nvs_part_gen_group.add_argument(
        "--encrypt",
        help='Set encryption mode. Default: false',
        choices=['true','false'],
        default='false',
        type=str.lower)

    nvs_part_gen_group.add_argument(
        "--keyfile",
        help='File having key for encryption (Applicable only if encryption mode is true)',
        default=None)

    key_gen_group = parser.add_argument_group('To generate encryption keys')
    key_gen_group._group_actions.append(keygen_action)

    args = parser.parse_args()
    input_filename = args.input
    output_filename = args.output
    part_size = args.size
    version_no = args.version
    is_key_gen = args.keygen
    is_encrypt_data = args.encrypt
    key_file = args.keyfile

    print_arg_str = "Invalid.\nTo generate nvs partition binary --input, --output and --size arguments are mandatory.\n \
        To generate encryption keys --keygen argument is mandatory."
    print_encrypt_arg_str = "Missing parameter. Enter --keyfile or --keygen."

    check_input_args(input_filename,output_filename, part_size, is_key_gen, is_encrypt_data, key_file, version_no, print_arg_str, print_encrypt_arg_str)
    nvs_part_gen(input_filename, output_filename, part_size, is_key_gen, is_encrypt_data, key_file, version_no)


if __name__ == "__main__":
    main()
