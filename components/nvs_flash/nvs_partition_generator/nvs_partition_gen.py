#!/usr/bin/env python
#
# esp-idf NVS partition generation tool. Tool helps in generating NVS-compatible
# partition binary, with key-value pair entries provided via a CSV file.
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import argparse
import array
import binascii
import codecs
import csv
import datetime
import distutils.dir_util
import os
import random
import struct
import sys
import textwrap
import zlib
from io import open

try:
    from cryptography.hazmat.backends import default_backend
    from cryptography.hazmat.primitives import hashes, hmac
    from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
except ImportError:
    print('The cryptography package is not installed.'
          'Please refer to the Get Started section of the ESP-IDF Programming Guide for '
          'setting up the required packages.')
    raise

VERSION1_PRINT = 'V1 - Multipage Blob Support Disabled'
VERSION2_PRINT = 'V2 - Multipage Blob Support Enabled'


def reverse_hexbytes(addr_tmp):
    addr = []
    reversed_bytes = ''
    for i in range(0, len(addr_tmp), 2):
        addr.append(addr_tmp[i:i + 2])
    reversed_bytes = ''.join(reversed(addr))

    return reversed_bytes


def desc_format(*args):
    desc = ''
    for arg in args:
        desc += textwrap.fill(replace_whitespace=False, text=arg) + '\n'
    return desc


""" Class for standard NVS page structure """


class Page(object):
    # Item type codes
    U8   = 0x01
    I8   = 0x11
    U16  = 0x02
    I16  = 0x12
    U32  = 0x04
    I32  = 0x14
    U64  = 0x08
    I64  = 0x18
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

    PAGE_PARAMS = {
        'max_size': 4096,
        'max_blob_size': {VERSION1: 1984, VERSION2: 4000},
        'max_entries': 126
    }

    def __init__(self, page_num, version, is_rsrv_page=False):
        self.entry_num = 0
        self.bitmap_array = array.array('B')
        self.version = version
        self.page_buf = bytearray(b'\xff') * Page.PAGE_PARAMS['max_size']
        if not is_rsrv_page:
            self.bitmap_array = self.create_bitmap_array()
            self.set_header(page_num, version)

    def set_header(self, page_num, version):
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
        # Encrypt 32 bytes of data using XTS-AES encryption
        backend = default_backend()
        plain_text = codecs.decode(data_arr, 'hex')
        tweak = codecs.decode(tweak_arr, 'hex')

        cipher = Cipher(algorithms.AES(encr_key), modes.XTS(tweak), backend=backend)
        encryptor = cipher.encryptor()
        encrypted_data = encryptor.update(plain_text)

        return encrypted_data

    def encrypt_data(self, data_input, no_of_entries, nvs_obj):
        # Set values needed for encryption and encrypt data byte wise
        encr_data_to_write = bytearray()
        data_len_needed = 64  # in hex
        tweak_len_needed = 32  # in hex
        key_len_needed = 64
        init_tweak_val = '0'
        init_data_val = 'f'
        tweak_tmp = ''
        encr_key_input = None

        # Extract encryption key and tweak key from given key input
        if len(nvs_obj.encr_key) == key_len_needed:
            encr_key_input = nvs_obj.encr_key
        else:
            encr_key_input = codecs.decode(nvs_obj.encr_key, 'hex')

        rel_addr = nvs_obj.page_num * Page.PAGE_PARAMS['max_size'] + Page.FIRST_ENTRY_OFFSET

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
                else:
                    addr_tmp = init_tweak_val + addr
                tweak_tmp = reverse_hexbytes(addr_tmp)
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

        if nvs_obj.encrypt:
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
            tailroom = (Page.PAGE_PARAMS['max_entries'] - self.entry_num - 1) * Page.SINGLE_ENTRY_SIZE
            assert tailroom >= 0, 'Page overflow!!'

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

        max_blob_size = Page.PAGE_PARAMS['max_blob_size'][self.version]
        # V2 blob size limit only applies to strings
        blob_limit_applies = self.version == Page.VERSION1 or encoding == 'string'

        if blob_limit_applies and datalen > max_blob_size:
            raise InputError(' Input File: Size (%d) exceeds max allowed length `%s` bytes for key `%s`.'
                             % (datalen, max_blob_size, key))

        # Calculate no. of entries data will require
        rounded_size = (datalen + 31) & ~31
        data_entry_count = rounded_size // 32
        total_entry_count = data_entry_count + 1  # +1 for the entry header

        # Check if page is already full and new page is needed to be created right away
        if self.entry_num >= Page.PAGE_PARAMS['max_entries']:
            raise PageFullError()
        elif (self.entry_num + total_entry_count) >= Page.PAGE_PARAMS['max_entries']:
            if not (self.version == Page.VERSION2 and encoding in ['hex2bin', 'binary', 'base64']):
                raise PageFullError()

        # Entry header
        entry_struct = bytearray(b'\xff') * 32
        # Set Namespace Index
        entry_struct[0] = ns_index
        # Set Span
        if self.version == Page.VERSION2:
            if encoding == 'string':
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
        if encoding == 'string':
            entry_struct[1] = Page.SZ
        elif encoding in ['hex2bin', 'binary', 'base64']:
            entry_struct[1] = Page.BLOB

        if self.version == Page.VERSION2 and (encoding in ['hex2bin', 'binary', 'base64']):
            entry_struct = self.write_varlen_binary_data(entry_struct,ns_index,key,data,
                                                         datalen,total_entry_count, encoding, nvs_obj)
        else:
            self.write_single_page_entry(entry_struct, data, datalen, data_entry_count, nvs_obj)

    """ Low-level function to write data of primitive type into page buffer. """
    def write_primitive_data(self, key, data, encoding, ns_index,nvs_obj):
        # Check if entry exceeds max number of entries allowed per page
        if self.entry_num >= Page.PAGE_PARAMS['max_entries']:
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

        if encoding == 'u8':
            entry_struct[1] = Page.U8
            struct.pack_into('<B', entry_struct, 24, data)
        elif encoding == 'i8':
            entry_struct[1] = Page.I8
            struct.pack_into('<b', entry_struct, 24, data)
        elif encoding == 'u16':
            entry_struct[1] = Page.U16
            struct.pack_into('<H', entry_struct, 24, data)
        elif encoding == 'i16':
            entry_struct[1] = Page.I16
            struct.pack_into('<h', entry_struct, 24, data)
        elif encoding == 'u32':
            entry_struct[1] = Page.U32
            struct.pack_into('<I', entry_struct, 24, data)
        elif encoding == 'i32':
            entry_struct[1] = Page.I32
            struct.pack_into('<i', entry_struct, 24, data)
        elif encoding == 'u64':
            entry_struct[1] = Page.U64
            struct.pack_into('<Q', entry_struct, 24, data)
        elif encoding == 'i64':
            entry_struct[1] = Page.I64
            struct.pack_into('<q', entry_struct, 24, data)

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
    def __init__(self, fout, input_size, version, encrypt=False, key_input=None):
        self.size = input_size
        self.encrypt = encrypt
        self.encr_key = None
        self.namespace_idx = 0
        self.page_num = -1
        self.pages = []
        self.version = version
        self.fout = fout
        if self.encrypt:
            self.encr_key = key_input
        self.cur_page = self.create_new_page(version)

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
            self.fout.write(result)

    def create_new_page(self, version=None, is_rsrv_page=False):
        # Set previous page state to FULL before creating new page
        if self.pages:
            curr_page_state = struct.unpack('<I', self.cur_page.page_buf[0:4])[0]
            if curr_page_state == Page.ACTIVE:
                page_state_full_seq = Page.FULL
                struct.pack_into('<I', self.cur_page.page_buf, 0, page_state_full_seq)
        # Set version for NVS binary generated
        version = self.version
        # Update available size as each page is created
        if self.size == 0:
            raise InsufficientSizeError('Error: Size parameter is less than the size of data in csv.Please increase size.')
        if not is_rsrv_page:
            self.size = self.size - Page.PAGE_PARAMS['max_size']
        self.page_num += 1
        # Set version for each page and page header
        new_page = Page(self.page_num, version, is_rsrv_page)
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
            self.cur_page.write_primitive_data(key, self.namespace_idx, 'u8', 0,self)
        except PageFullError:
            new_page = self.create_new_page()
            new_page.write_primitive_data(key, self.namespace_idx, 'u8', 0,self)

    """
    Write key-value pair. Function accepts value in the form of ascii character and converts
    it into appropriate format before calling Page class's functions to write entry into NVS format.
    Function handles PageFullError and creates a new page and re-invokes the function on a new page.
    We don't have to guard re-invocation with try-except since no entry can span multiple pages.
    """
    def write_entry(self, key, value, encoding):
        # Encoding-specific handling
        if encoding == 'hex2bin':
            value = value.strip()
            if len(value) % 2 != 0:
                raise InputError('%s: Invalid data length. Should be multiple of 2.' % key)
            value = binascii.a2b_hex(value)
        elif encoding == 'base64':
            value = binascii.a2b_base64(value)
        elif encoding == 'string':
            if type(value) == bytes:
                value = value.decode()
            value += '\0'

        encoding = encoding.lower()
        varlen_encodings = {'string', 'binary', 'hex2bin', 'base64'}
        primitive_encodings = {'u8', 'i8', 'u16', 'i16', 'u32', 'i32', 'u64', 'i64'}

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
            raise InputError('%s: Unsupported encoding' % encoding)

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
        print('\nError:')
        super(InputError, self).__init__(e)


class InsufficientSizeError(RuntimeError):
    """
    Represents error when NVS Partition size given is insufficient
    to accomodate the data in the given csv file
    """
    def __init__(self, e):
        super(InsufficientSizeError, self).__init__(e)


def nvs_open(result_obj, input_size, version=None, is_encrypt=False, key=None):
    """ Wrapper to create and NVS class object. This object can later be used to set key-value pairs

    :param result_obj: File/Stream object to dump resultant binary. If data is to be dumped into memory, one way is to use BytesIO object
    :param input_size: Size of Partition
    :return: NVS class instance
    """
    return NVS(result_obj, input_size, version, encrypt=is_encrypt, key_input=key)


def write_entry(nvs_instance, key, datatype, encoding, value):
    """ Wrapper to set key-value pair in NVS format

    :param nvs_instance: Instance of an NVS class returned by nvs_open()
    :param key: Key of the data
    :param datatype: Data type. Valid values are "file", "data" and "namespace"
    :param encoding: Data encoding. Valid values are "u8", "i8", "u16", "i16", "u32", "i32", "u64", "i64", "string", "binary", "hex2bin" and "base64"
    :param value: Data value in ascii encoded string format for "data" datatype and filepath for "file" datatype
    :return: None
    """

    if datatype == 'file':
        abs_file_path = value
        if os.path.isabs(value) is False:
            script_dir = os.getcwd()
            abs_file_path = os.path.join(script_dir, value)

        with open(abs_file_path, 'rb') as f:
            value = f.read()

    if datatype == 'namespace':
        nvs_instance.write_namespace(key)
    else:
        nvs_instance.write_entry(key, value, encoding)


def nvs_close(nvs_instance):
    """ Wrapper to finish writing to NVS and write data to file/stream object provided to nvs_open method

    :param nvs_instance: Instance of NVS class returned by nvs_open()
    :return: None
    """
    nvs_instance.__exit__(None, None, None)


def check_size(size):
    '''
    Checks for input partition size
    :param size: Input partition size
    '''
    try:
        # Set size
        input_size = int(size, 0)
        if input_size % 4096 != 0:
            sys.exit('Size of partition must be multiple of 4096')

        # Update size as a page needs to be reserved of size 4KB
        input_size = input_size - Page.PAGE_PARAMS['max_size']

        if input_size < (2 * Page.PAGE_PARAMS['max_size']):
            sys.exit('Minimum NVS partition size needed is 0x3000 bytes.')
        return input_size
    except Exception as e:
        print(e)
        sys.exit(0)


def set_target_filepath(outdir, filepath):
    '''
    Set target file path: <outdir>/<filepath>
    :param outdir: Target output dir to store files
    :param filepath: Path of target file
    '''
    bin_ext = '.bin'
    # Expand if tilde(~) provided in path
    outdir = os.path.expanduser(outdir)

    if filepath:
        key_file_name, ext  = os.path.splitext(filepath)
        if not ext:
            filepath = key_file_name + bin_ext
        elif bin_ext not in ext:
            sys.exit('Error: `%s`. Only `%s` extension allowed.' % (filepath, bin_ext))

    # Create dir if does not exist
    if not (os.path.isdir(outdir)):
        distutils.dir_util.mkpath(outdir)

    filedir, filename = os.path.split(filepath)
    filedir = os.path.join(outdir,filedir,'')
    if filedir and not os.path.isdir(filedir):
        distutils.dir_util.mkpath(filedir)

    if os.path.isabs(filepath):
        if not outdir == os.getcwd():
            print('\nWarning: `%s` \n\t==> absolute path given so outdir is ignored for this file.' % filepath)
        # Set to empty as outdir is ignored here
        outdir = ''

    # Set full path - outdir + filename
    filepath = os.path.join(outdir, '') + filepath

    return outdir, filepath


def encrypt(args):
    '''
    Generate encrypted NVS Partition
    :param args: Command line arguments given
    '''
    key = None
    bin_ext = '.bin'

    check_size(args.size)
    if (args.keygen is False) and (not args.inputkey):
        sys.exit('Error. --keygen or --inputkey argument needed.')
    elif args.keygen and args.inputkey:
        sys.exit('Error. --keygen and --inputkey both are not allowed.')
    elif not args.keygen and args.keyfile:
        print('\nWarning:','--inputkey argument is given. --keyfile argument will be ignored...')

    if args.inputkey:
        # Check if key file has .bin extension
        filename, ext = os.path.splitext(args.inputkey)
        if bin_ext not in ext:
            sys.exit('Error: `%s`. Only `%s` extension allowed.' % (args.inputkey, bin_ext))
        key = bytearray()
        with open(args.inputkey, 'rb') as key_f:
            key = key_f.read(64)

    # Generate encrypted NVS Partition
    generate(args, is_encr_enabled=True, encr_key=key)


def decrypt_data(data_input, decr_key, page_num, entry_no, entry_size):
    '''
    Decrypt NVS data entry
    '''
    page_max_size = 4096
    first_entry_offset = 64
    init_tweak_val = '0'
    tweak_len_needed = 32  # in hex
    tweak_tmp = ''

    data_input = binascii.hexlify(data_input)
    rel_addr = page_num * page_max_size + first_entry_offset

    # Set tweak value
    offset = entry_no * entry_size
    addr = hex(rel_addr + offset)[2:]
    addr_len = len(addr)
    if addr_len > 2:
        if not addr_len % 2:
            addr_tmp = addr
        else:
            addr_tmp = init_tweak_val + addr
        tweak_tmp = reverse_hexbytes(addr_tmp)
        tweak_val = tweak_tmp + (init_tweak_val * (tweak_len_needed - (len(tweak_tmp))))
    else:
        tweak_val = addr + (init_tweak_val * (tweak_len_needed - len(addr)))

    if type(data_input) == bytes:
        data_input = data_input.decode()

    # Decrypt 32 bytes of data using XTS-AES decryption
    backend = default_backend()
    plain_text = codecs.decode(data_input, 'hex')
    tweak = codecs.decode(tweak_val, 'hex')
    cipher = Cipher(algorithms.AES(decr_key), modes.XTS(tweak), backend=backend)
    decryptor = cipher.decryptor()
    decrypted_data = decryptor.update(plain_text)

    return decrypted_data


def decrypt(args):
    '''
    Decrypt encrypted NVS Partition
    :param args: Command line arguments given
    '''
    bin_ext = '.bin'
    nvs_read_bytes = 32
    decrypted_entry_no = 0
    file_entry_no = 0
    page_num = 0
    page_max_size = 4096
    start_entry_offset = 0
    empty_data_entry = bytearray(b'\xff') * nvs_read_bytes

    # Check if key file has .bin extension
    input_files = [args.input, args.key, args.output]
    for filepath in input_files:
        filename, ext = os.path.splitext(filepath)
        if bin_ext not in ext:
            sys.exit('Error: `%s`. Only `%s` extension allowed.' % (filepath, bin_ext))
    with open(args.key,'rb') as decr_key_file:
        decr_key = decr_key_file.read(64)

    args.outdir, args.output = set_target_filepath(args.outdir, args.output)

    output_buf = bytearray(b'\xff')

    with open(args.input, 'rb') as input_file, open(args.output,'wb') as output_file:
        while True:
            if file_entry_no == 128:
                decrypted_entry_no = 0
                file_entry_no = 0
                page_num += 1
            data_entry = input_file.read(nvs_read_bytes)
            if not data_entry:
                break
            if data_entry != empty_data_entry and file_entry_no not in [0,1]:
                data_entry = decrypt_data(data_entry, decr_key, page_num, decrypted_entry_no, nvs_read_bytes)
                decrypted_entry_no += 1
            write_entry_no = ((page_num * page_max_size) + file_entry_no)
            start_idx = start_entry_offset + (write_entry_no  * nvs_read_bytes)
            end_idx = nvs_read_bytes
            output_buf[start_idx:end_idx] = data_entry
            file_entry_no += 1
            start_entry_offset += nvs_read_bytes
        output_file.write(output_buf)

    print('\nCreated NVS decrypted binary: ===>', args.output)


def generate_key(args):
    '''
    Generate encryption keys
    :param args: Command line arguments given
    '''
    page_max_size = 4096
    keys_dir = 'keys'
    output_keyfile = None
    bin_ext = '.bin'

    if not args.keyfile:
        timestamp = datetime.datetime.now().strftime('%m-%d_%H-%M')
        args.keyfile = 'keys-' + timestamp + bin_ext

    keys_outdir = os.path.join(args.outdir,keys_dir, '')
    # Create keys/ dir in <outdir> if does not exist
    if not (os.path.isdir(keys_outdir)):
        distutils.dir_util.mkpath(keys_outdir)
    keys_outdir, output_keyfile = set_target_filepath(keys_outdir, args.keyfile)

    keys_buf = bytearray(b'\xff') * page_max_size
    key = ''

    if args.key_protect_hmac:
        HMAC_EKEY_SEED_ELEMENT = b'\x5A\x5A\xBE\xAE'
        HMAC_TKEY_SEED_ELEMENT = b'\xA5\xA5\xDE\xCE'
        hmac_key = b''

        if args.kp_hmac_keygen:
            hmac_key_str = ''.join(random.choice('0123456789abcdef') for _ in range(64)).strip()
            hmac_key = codecs.decode(hmac_key_str, 'hex')
            hmac_keyfile = ''

            if args.kp_hmac_keyfile:
                hmac_keyfile = args.kp_hmac_keyfile
            else:
                hmac_keyfile = 'hmac-' + args.keyfile

            hmac_keyfile = set_target_filepath(keys_outdir, hmac_keyfile)[1]
            with open(hmac_keyfile, 'wb') as hmac_key_file:
                hmac_key_file.write(hmac_key)
        else:
            if not args.kp_hmac_inputkey:
                raise RuntimeError('HMAC Key input file (HMAC-based encryption scheme) missing!')

            with open(args.kp_hmac_inputkey, 'rb') as input_keys_file:
                hmac_key = input_keys_file.read()

        ekey_seed = HMAC_EKEY_SEED_ELEMENT * 8
        h_e = hmac.HMAC(hmac_key, hashes.SHA256())
        h_e.update(ekey_seed)
        e_key = h_e.finalize()

        tkey_seed = HMAC_TKEY_SEED_ELEMENT * 8
        h_t = hmac.HMAC(hmac_key, hashes.SHA256())
        h_t.update(tkey_seed)
        t_key = h_t.finalize()

        encr_key_bytes = e_key + t_key
        key_len = len(encr_key_bytes)
        key = f"{int.from_bytes(encr_key_bytes, 'big'):x}"

        keys_buf[0:key_len] = encr_key_bytes
        crc_data = keys_buf[0:key_len]
        crc_data = bytes(crc_data)
        crc = zlib.crc32(crc_data, 0xFFFFFFFF)
        struct.pack_into('<I', keys_buf, key_len,  crc & 0xFFFFFFFF)
    else:
        key = ''.join(random.choice('0123456789abcdef') for _ in range(128)).strip()
        encr_key_bytes = codecs.decode(key, 'hex')
        key_len = len(encr_key_bytes)

        keys_buf[0:key_len] = encr_key_bytes
        crc_data = keys_buf[0:key_len]
        crc_data = bytes(crc_data)
        crc = zlib.crc32(crc_data, 0xFFFFFFFF)
        struct.pack_into('<I', keys_buf, key_len,  crc & 0xFFFFFFFF)

    with open(output_keyfile, 'wb') as output_keys_file:
        output_keys_file.write(keys_buf)

    print('\nCreated encryption keys: ===> ', output_keyfile)

    return key


def generate(args, is_encr_enabled=False, encr_key=None):
    '''
    Generate NVS Partition
    :param args: Command line arguments given
    :param is_encr_enabled: Encryption enabled/disabled
    :param encr_key: Key to encrypt NVS partition
    '''
    is_dir_new = False
    bin_ext = '.bin'

    input_size = check_size(args.size)
    if args.version == 1:
        args.version = Page.VERSION1
    elif args.version == 2:
        args.version = Page.VERSION2

    # Check if key file has .bin extension
    filename, ext = os.path.splitext(args.output)
    if bin_ext not in ext:
        sys.exit('Error: `%s`. Only `.bin` extension allowed.' % args.output)
    args.outdir, args.output = set_target_filepath(args.outdir, args.output)

    if is_encr_enabled and not encr_key:
        encr_key = generate_key(args)

    with open(args.input, 'rt', encoding='utf8') as input_file,\
            open(args.output, 'wb') as output_file,\
            nvs_open(output_file, input_size, args.version, is_encrypt=is_encr_enabled, key=encr_key) as nvs_obj:
        # Comments are skipped
        reader = csv.DictReader(filter(lambda row: row[0] != '#',input_file), delimiter=',')
        if nvs_obj.version == Page.VERSION1:
            version_set = VERSION1_PRINT
        else:
            version_set = VERSION2_PRINT
        print('\nCreating NVS binary with version:', version_set)

        for row in reader:
            try:
                max_key_len = 15
                if len(row['key']) > max_key_len:
                    raise InputError('Length of key `%s` should be <= 15 characters.' % row['key'])
                write_entry(nvs_obj, row['key'], row['type'], row['encoding'], row['value'])
            except InputError as e:
                print(e)
                filedir, filename = os.path.split(args.output)
                if filename:
                    print('\nWarning: NVS binary not created...')
                    os.remove(args.output)
                if is_dir_new and not filedir == os.getcwd():
                        print('\nWarning: Output dir not created...')
                        os.rmdir(filedir)
                sys.exit(-2)

    print('\nCreated NVS binary: ===>', args.output)


def main():
    parser = argparse.ArgumentParser(description=desc_format('ESP NVS partition generation utility'), formatter_class=argparse.RawTextHelpFormatter)
    subparser = parser.add_subparsers(title='Commands',
                                      dest='command',
                                      help=desc_format('Run nvs_partition_gen.py {command} -h for additional help'))

    parser_gen = subparser.add_parser('generate',
                                      help=desc_format('Generate NVS partition'),
                                      formatter_class=argparse.RawTextHelpFormatter)
    parser_gen.set_defaults(func=generate)
    parser_gen.add_argument('input',
                            default=None,
                            help=desc_format('Path to CSV file to parse'))
    parser_gen.add_argument('output',
                            default=None,
                            help=desc_format('Path to output NVS binary file'))
    parser_gen.add_argument('size',
                            default=None,
                            help=desc_format('Size of NVS partition in bytes (must be multiple of 4096)'))
    parser_gen.add_argument('--version',
                            choices=[1,2],
                            default=2,
                            type=int,
                            help=desc_format(
                                'Set multipage blob version.',
                                'Version 1 - Multipage blob support disabled.',
                                'Version 2 - Multipage blob support enabled.',
                                'Default: Version 2'))
    parser_gen.add_argument('--outdir',
                            default=os.getcwd(),
                            help=desc_format('Output directory to store files created (Default: current directory)'))
    parser_gen_key = subparser.add_parser('generate-key',
                                          help=desc_format('Generate keys for encryption'),
                                          formatter_class=argparse.RawTextHelpFormatter)
    parser_gen_key.set_defaults(func=generate_key)
    parser_gen_key.add_argument('--key_protect_hmac',
                                action='store_true',
                                help=desc_format(
                                    'If set, the NVS encryption key protection scheme based on HMAC',
                                    'peripheral is used; else the default scheme based on Flash Encryption',
                                    'is used'))
    parser_gen_key.add_argument('--kp_hmac_keygen',
                                action='store_true',
                                help=desc_format('Generate the HMAC key for HMAC-based encryption scheme'))
    parser_gen_key.add_argument('--kp_hmac_keyfile',
                                default=None,
                                help=desc_format('Path to output HMAC key file'))
    parser_gen_key.add_argument('--kp_hmac_inputkey',
                                default=None,
                                help=desc_format('File having the HMAC key for generating the NVS encryption keys'))
    parser_gen_key.add_argument('--keyfile',
                                default=None,
                                help=desc_format('Path to output encryption keys file'))
    parser_gen_key.add_argument('--outdir',
                                default=os.getcwd(),
                                help=desc_format('Output directory to store files created. (Default: current directory)'))
    parser_encr = subparser.add_parser('encrypt',
                                       help=desc_format('Generate NVS encrypted partition'),
                                       formatter_class=argparse.RawTextHelpFormatter)
    parser_encr.set_defaults(func=encrypt)
    parser_encr.add_argument('input',
                             default=None,
                             help=desc_format('Path to CSV file to parse'))
    parser_encr.add_argument('output',
                             default=None,
                             help=desc_format('Path to output NVS binary file'))
    parser_encr.add_argument('size',
                             default=None,
                             help=desc_format('Size of NVS partition in bytes (must be multiple of 4096)'))
    parser_encr.add_argument('--version',
                             choices=[1,2],
                             default=2,
                             type=int,
                             help=desc_format(
                                 'Set multipage blob version.',
                                 'Version 1 - Multipage blob support disabled.',
                                 'Version 2 - Multipage blob support enabled.',
                                 'Default: Version 2'))
    parser_encr.add_argument('--keygen',
                             action='store_true',
                             help=desc_format('Generates key for encrypting NVS partition'))
    parser_encr.add_argument('--keyfile',
                             default=None,
                             help=desc_format('Path to output encryption keys file'))
    parser_encr.add_argument('--inputkey',
                             default=None,
                             help=desc_format('File having key for encrypting NVS partition'))
    parser_encr.add_argument('--outdir',
                             default=os.getcwd(),
                             help=desc_format('Output directory to store files created. (Default: current directory)'))
    parser_encr.add_argument('--key_protect_hmac',
                             action='store_true',
                             help=desc_format(
                                 'If set, the NVS encryption key protection scheme based on HMAC',
                                 'peripheral is used; else the default scheme based on Flash Encryption',
                                 'is used'))
    parser_encr.add_argument('--kp_hmac_keygen',
                             action='store_true',
                             help=desc_format('Generate the HMAC key for HMAC-based encryption scheme'))
    parser_encr.add_argument('--kp_hmac_keyfile',
                             default=None,
                             help=desc_format('Path to output HMAC key file'))
    parser_encr.add_argument('--kp_hmac_inputkey',
                             default=None,
                             help=desc_format('File having the HMAC key for generating the NVS encryption keys'))
    parser_decr = subparser.add_parser('decrypt',
                                       help=desc_format('Decrypt NVS encrypted partition'),
                                       formatter_class=argparse.RawTextHelpFormatter)
    parser_decr.set_defaults(func=decrypt)
    parser_decr.add_argument('input',
                             default=None,
                             help=desc_format('Path to encrypted NVS partition file to parse'))
    parser_decr.add_argument('key',
                             default=None,
                             help=desc_format('Path to file having keys for decryption'))
    parser_decr.add_argument('output',
                             default=None,
                             help='Path to output decrypted binary file')
    parser_decr.add_argument('--outdir',
                             default=os.getcwd(),
                             help=desc_format('Output directory to store files created. (Default: current directory)'))
    args = parser.parse_args()

    args.func(args)


if __name__ == '__main__':
    main()
