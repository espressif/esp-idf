#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import sys
import traceback
from typing import Any, Dict, List
from zlib import crc32


class Logger:
    ansi = {
        'red': '\033[31m',
        'green': '\033[32m',
        'yellow': '\033[33m',
        'blue': '\033[34m',
        'cyan': '\033[36m',
        'bold': '\033[1m',
        'clear': '\033[0m'
    }

    def __init__(self, *, color:str='auto'):
        self.color = color == 'always' or (color == 'auto' and sys.stdout.isatty())

    def set_color(self, color:str) -> None:
        self.color = color == 'always' or (color == 'auto' and sys.stdout.isatty())

    def info(self, *args, **kwargs) -> None:  # type: ignore
        kwargs['file'] = kwargs.get('file', sys.stdout)  # Set default output to be stdout, but can be overwritten
        print(*args, **kwargs)

    def error(self, *args, **kwargs) -> None:  # type: ignore
        kwargs['file'] = kwargs.get('file', sys.stderr)  # Set default output to be stderr, but can be overwritten
        print(*args, **kwargs)

    def red(self, text:str) -> str:
        if self.color:
            return Logger.ansi['red'] + text + Logger.ansi['clear']
        return text

    def green(self, text:str) -> str:
        if self.color:
            return Logger.ansi['green'] + text + Logger.ansi['clear']
        return text

    def yellow(self, text:str) -> str:
        if self.color:
            return Logger.ansi['yellow'] + text + Logger.ansi['clear']
        return text

    def blue(self, text:str) -> str:
        if self.color:
            return Logger.ansi['blue'] + text + Logger.ansi['clear']
        return text

    def cyan(self, text:str) -> str:
        if self.color:
            return Logger.ansi['cyan'] + text + Logger.ansi['clear']
        return text

    def bold(self, text:str) -> str:
        if self.color:
            return Logger.ansi['bold'] + text + Logger.ansi['clear']
        return text


# Constants
class Constants:
    class ConstantError(AttributeError):
        pass

    def __init__(self) -> None:
        self.page_size  = 4096
        self.entry_size = 32
        self.item_type = {
            0x01: 'uint8_t',
            0x11: 'int8_t',
            0x02: 'uint16_t',
            0x12: 'int16_t',
            0x04: 'uint32_t',
            0x14: 'int32_t',
            0x08: 'uint64_t',
            0x18: 'int64_t',
            0x21: 'string',
            0x41: 'blob',
            0x42: 'blob_data',
            0x48: 'blob_index',
        }
        self.page_status = {
            0xffffffff: 'Empty',
            0xfffffffe: 'Active',
            0xfffffffc: 'Full',
            0xfffffff8: 'Freeing',
        }
        self.entry_status = {
            0b11: 'Empty',
            0b10: 'Written',
            0b00: 'Erased',
        }

    def __setattr__(self, key:str, val:Any) -> None:
        if self.__dict__.get(key, None) is None:
            self.__dict__[key] = val
        else:
            raise Constants.ConstantError('Cannot change a constant!')


const = Constants()
log = Logger()


class NotAlignedError(ValueError):
    pass


class NVS_Partition:
    def __init__(self, name:str, raw_data:bytearray):
        if len(raw_data) % const.page_size != 0:
            raise NotAlignedError(f'Given partition data is not aligned to page size ({len(raw_data)} % {const.page_size} = {len(raw_data)%const.page_size})')

        # Divide partition into pages
        self.name = name
        self.pages = []
        for i in range(0, len(raw_data), const.page_size):
            self.pages.append(NVS_Page(raw_data[i:i + const.page_size], i))


class NVS_Page:
    def __init__(self, page_data:bytearray, address:int):
        if len(page_data) != const.page_size:
            raise NotAlignedError(f'Size of given page does not match page size ({len(page_data)} != {const.page_size})')

        # Initialize class
        self.is_empty      = page_data[0:const.entry_size] == bytearray({0xff}) * const.entry_size
        self.start_address = address
        self.raw_header    = page_data[0:const.entry_size]
        self.raw_entry_state_bitmap = page_data[const.entry_size:2 * const.entry_size]
        self.entries       = []

        # Load header
        self.header:Dict[str, Any] = {
            'status': const.page_status.get(int.from_bytes(page_data[0:4], byteorder='little'), 'Invalid'),
            'page_index': int.from_bytes(page_data[4:8], byteorder='little'),
            'version': 256 - page_data[8],
            'crc':{
                'original': int.from_bytes(page_data[28:32], byteorder='little'),
                'computed': crc32(page_data[4:28], 0xFFFFFFFF)
            }
        }

        # Load entry state bitmap
        entry_states = []
        for c in self.raw_entry_state_bitmap:
            for index in range(0, 8, 2):
                entry_states.append(const.entry_status.get((c >> index) & 3, 'Invalid'))
        entry_states = entry_states[:-2]

        # Load entries
        i = 2
        while i < int(const.page_size / const.entry_size):  # Loop through every entry
            span = page_data[(i * const.entry_size) + 2]
            if span in [0xff, 0]:  # 'Default' span length to prevent span overflow
                span = 1

            # Load an entry
            entry = NVS_Entry(i - 2, page_data[i * const.entry_size:(i + 1) * const.entry_size], entry_states[i - 2])
            self.entries.append(entry)

            # Load all children entries
            if span != 1:
                for span_idx in range(1, span):
                    page_addr = i + span_idx
                    entry_idx = page_addr - 2
                    if page_addr * const.entry_size >= const.page_size:
                        break
                    child_entry = NVS_Entry(entry_idx, page_data[page_addr * const.entry_size:(page_addr + 1) * const.entry_size], entry_states[entry_idx])
                    entry.child_assign(child_entry)
                entry.compute_crc()
            i += span


class NVS_Entry:
    def __init__(self, index:int, entry_data:bytearray, entry_state:str):
        if len(entry_data) != const.entry_size:
            raise NotAlignedError(f'Given entry is not aligned to entry size ({len(entry_data)} % {const.entry_size} = {len(entry_data)%const.entry_size})')

        def item_convert(i_type:int, data:bytearray) -> Dict:
            byte_size_mask = 0x0f
            number_sign_mask = 0xf0
            fixed_entry_length_threshold = 0x20  # Fixed length entry type number is always smaller than this
            if i_type in const.item_type:
                # Deal with non variable length entries
                if i_type < fixed_entry_length_threshold:
                    size = i_type & byte_size_mask
                    num = int.from_bytes(data[:size], byteorder='little', signed=bool(i_type & number_sign_mask))
                    return {'value':num}

                # Deal with variable length entries
                if const.item_type[i_type] in ['string', 'blob_data', 'blob']:
                    size = int.from_bytes(data[:2], byteorder='little')
                    crc = int.from_bytes(data[4:8], byteorder='little')
                    return {'value': [size, crc],
                            'size': size,
                            'crc': crc}
                if const.item_type[i_type] == 'blob_index':
                    size = int.from_bytes(data[:4], byteorder='little')
                    chunk_count = data[4]
                    chunk_start = data[5]
                    return {'value': [size, chunk_count, chunk_start],
                            'size': size,
                            'chunk_count': chunk_count,
                            'chunk_start': chunk_start}
            return {'value': log.red('Cannot parse')}

        def key_decode(data:bytearray) -> str:
            decoded = ''
            for n in data:
                char = chr(n)
                if char.isprintable():
                    decoded += char
            return decoded

        self.raw = entry_data
        self.state = entry_state
        self.is_empty = self.raw == bytearray({0xff}) * const.entry_size
        self.index = index

        namespace       = self.raw[0]
        entry_type      = self.raw[1]
        span            = self.raw[2]
        chunk_index     = self.raw[3]
        crc             = self.raw[4:8]
        key             = self.raw[8:24]
        data            = self.raw[24:32]
        raw_without_crc = self.raw[:4] + self.raw[8:32]
        self.metadata: Dict[str, Any] = {
            'namespace': namespace,
            'type': const.item_type.get(entry_type, f'0x{entry_type:02x}'),
            'span': span,
            'chunk_index': chunk_index,
            'crc':{
                'original': int.from_bytes(crc, byteorder='little'),
                'computed': crc32(raw_without_crc,  0xFFFFFFFF),
                'data_original': int.from_bytes(data[-4:], byteorder='little'),
                'data_computed': 0
            }
        }
        self.children: List['NVS_Entry'] = []
        self.key = key_decode(key)
        self.data = item_convert(entry_type, data)

    def dump_raw(self) -> str:
        hex_bytes = ''
        decoded = ''
        for i, c in enumerate(self.raw):
            middle_index = int(len(self.raw) / 2)
            if i == middle_index:  # Add a space in the middle
                hex_bytes += ' '
                decoded += ' '
            hex_bytes += f'{c:02x} '
            decoded += chr(c) if chr(c).isprintable() else '.'
        return hex_bytes + ' ' + decoded

    def child_assign(self, entry:'NVS_Entry') -> None:
        if not isinstance(entry, type(self)):
            raise ValueError('You can assign only NVS_Entry')
        self.children.append(entry)

    def compute_crc(self) -> None:
        if self.metadata['span'] == 1:
            return

        # Merge entries into one buffer
        children_data = bytearray()
        for entry in self.children:
            children_data += entry.raw
        children_data = children_data[:self.data['size']]  # Discard padding

        self.metadata['crc']['data_computed'] = crc32(children_data, 0xFFFFFFFF)


def storage_stats(nvs_partition:NVS_Partition) -> None:
    global_stats = {
        'written_entries': 0,
        'free_entries': 0,
        'erased_entries': 0,
        'invalid_entries': 0
    }

    for page in nvs_partition.pages:
        written_e = 0
        free_e = 0
        erased_e = 0
        invalid_e = 0
        for entry in page.entries:
            if entry.state == 'Written':
                written_e += 1
            elif entry.state == 'Empty':
                free_e += 1
            elif entry.state == 'Erased':
                erased_e += 1
            else:
                invalid_e += 1

        log.info(log.bold(f'Page {page.header["status"]}'))
        log.info('  Found entries:')
        log.info(f'    Written: {written_e: 5d}')
        log.info(f'    Erased:  {erased_e: 5d}')
        log.info(f'    Empty:   {free_e: 5d}')
        log.info(f'    Invalid: {invalid_e: 5d}')
        log.info(f'    Total:   {written_e + free_e + erased_e + invalid_e: 5d}')
        log.info()
        global_stats['written_entries'] += written_e
        global_stats['erased_entries'] += erased_e
        global_stats['free_entries'] += free_e
        global_stats['invalid_entries'] += invalid_e

    log.info(log.bold('Global'))
    log.info('  Config:')
    log.info(f'    Page size:   {const.page_size: 5d}')
    log.info(f'    Entry size:  {const.entry_size: 5d}')
    log.info(f'    Total pages: {len(nvs_partition.pages): 5d}')
    log.info('  Entries:')
    log.info(f'    Written: {global_stats["written_entries"]: 5d}')
    log.info(f'    Erased:  {global_stats["erased_entries"]: 5d}')
    log.info(f'    Empty:   {global_stats["free_entries"]: 5d}')
    log.info(f'    Invalid: {global_stats["invalid_entries"]: 5d}')
    log.info(f'    Total:   {sum([global_stats[key] for key in global_stats]): 5d}')
    log.info()


def dump_everything(nvs_partition:NVS_Partition, written_only:bool=False) -> None:
    for page in nvs_partition.pages:
        # Print page header
        if page.is_empty:
            log.info(log.bold(f'Page Empty, Page address: 0x{page.start_address:x}'))
        else:
            if page.header['crc']['original'] == page.header['crc']['computed']:  # Color CRC32
                crc = log.green(f'{page.header["crc"]["original"]: >8x}')
            else:
                crc = log.red(f'{page.header["crc"]["original"]: >8x}')
            log.info(log.bold(f'Page no. {page.header["page_index"]}'
                              + f', Status: {page.header["status"]}'
                              + f', Version: {page.header["version"]}'
                              + f', CRC32: {crc}')
                     + log.bold(f', Page address: 0x{page.start_address:x}'))

        log.info(log.bold(' Entry state bitmap: '), end='')
        for x in page.raw_entry_state_bitmap:
            log.info(f'{x:02x} ', end='')
        log.info()

        # Dump entries
        empty_entries = []
        for entry in page.entries:
            # Skip non-written entries if needed
            if written_only and not entry.state == 'Written':
                continue

            # Compress all empty entries
            if entry.state == 'Empty' and entry.is_empty:  # Gather all subsequent empty entries
                empty_entries.append(entry)
                continue
            else:
                # Print the empty entries
                if len(empty_entries) >= 3:  # There is enough entries to compress
                    log.info(log.bold(f' {empty_entries[0].index:03d}.'), 'Empty')
                    log.info(log.bold(' ...'))
                    log.info(log.bold(f' {empty_entries[-1].index:03d}.'), 'Empty')
                else:  # No need for compression
                    for e in empty_entries:
                        log.info(log.bold(f' {e.index:03d}.'), 'Empty')
                empty_entries.clear()

            # Dump a single entry
            status = entry.state
            if status == 'Written':
                status = log.green(f'{status: <7}')
            elif status == 'Erased':
                status = log.red(f'{status: <7}')

            crc = ''
            if entry.metadata['crc']['original'] == entry.metadata['crc']['computed']:  # Color CRC32
                crc = log.green(f'{entry.metadata["crc"]["original"]: >8x}')
            else:
                crc = log.red(f'{entry.metadata["crc"]["original"]: >8x}')

            log.info(log.bold(f' {entry.index:03d}.')
                     + ' ' + status
                     + f', Namespace Index: {entry.metadata["namespace"]:03d}'
                     + f', Type: {entry.metadata["type"]:<10}'
                     + f', Span: {entry.metadata["span"]:03d}'
                     + f', Chunk Index: {entry.metadata["chunk_index"]:03d}'
                     + f', CRC32: {crc}'
                     + f' | {entry.key} : ', end='')

            if entry.metadata['type'] not in ['string', 'blob_data', 'blob_index', 'blob']:  # Entry is non-variable length
                log.info(entry.data['value'])
            else:
                if entry.metadata['type'] == 'blob_index':
                    log.info(f'Size={entry.data["size"]}'
                             + f', ChunkCount={entry.data["chunk_count"]}'
                             + f', ChunkStart={entry.data["chunk_start"]}')
                else:
                    if entry.metadata['crc']['data_original'] == entry.metadata['crc']['data_computed']:  # Color CRC32
                        crc = log.green(f'{entry.metadata["crc"]["data_original"]:x}')
                    else:
                        crc = log.red(f'{entry.metadata["crc"]["data_original"]:x}')
                    log.info(f'Size={entry.data["size"]}, CRC32={crc}')

            # Dump all children entries
            if entry.metadata['span'] != 1:
                for i, data in enumerate(entry.children):
                    log.info(f'{"": >6}0x{(i*const.entry_size):03x}  {data.dump_raw()}')

        # Dump trailing empty entries
        if len(empty_entries) >= 3:
            log.info(log.bold(f' {empty_entries[0].index:03d}.'), 'Empty')
            log.info(log.bold(' ...'))
            log.info(log.bold(f' {empty_entries[-1].index:03d}.'), 'Empty')
        else:
            for e in empty_entries:
                log.info(log.bold(f' {e.index:03d}.'), 'Empty')
        empty_entries.clear()
        log.info()


def dump_written_entries(nvs_partition:NVS_Partition) -> None:
    dump_everything(nvs_partition, True)


def list_namespaces(nvs_partition:NVS_Partition) -> None:
    # Gather namespaces
    ns = {}
    for page in nvs_partition.pages:
        for entry in page.entries:
            if entry.state == 'Written' and entry.metadata['namespace'] == 0:
                ns[entry.data['value']] = entry.key

    # Print found namespaces
    log.info(log.bold(f'Index : Namespace'))
    for ns_index in sorted(ns):
        log.info(f' {ns_index:03d}  :', log.cyan(ns[ns_index]))


def dump_key_value_pairs(nvs_partition:NVS_Partition) -> None:
    # Get namespace list
    ns = {}
    for page in nvs_partition.pages:
        for entry in page.entries:
            if entry.state == 'Written' and entry.metadata['namespace'] == 0:
                ns[entry.data['value']] = entry.key

    # Print key-value pairs
    for page in nvs_partition.pages:
        # Print page header
        if page.is_empty:
            log.info(log.bold('Page Empty'))
        else:
            log.info(log.bold(f'Page no. {page.header["page_index"]}'
                              + f', Status: {page.header["status"]}'))

        # Print entries
        for entry in page.entries:
            if entry.state == 'Written' and entry.metadata['namespace'] != 0:  # Ignore non-written entries
                chunk_index = ''
                data = ''
                if entry.metadata['type'] not in ['string', 'blob_data', 'blob_index', 'blob']:  # Non-variable length entry
                    data = entry.data['value']
                elif entry.metadata['type'] == 'blob_index':
                    continue
                else:  # Variable length entries
                    tmp = b''
                    for e in entry.children:  # Merge all children entries
                        tmp += bytes(e.raw)
                    tmp = tmp[:entry.data['size']]  # Discard padding
                    if entry.metadata['type'] == 'blob_data':
                        if entry.metadata['chunk_index'] >= 128:  # Get real chunk index
                            chunk_index = f'[{entry.metadata["chunk_index"] - 128}]'
                        else:
                            chunk_index = f'[{entry.metadata["chunk_index"]}]'
                    data = str(tmp)

                if entry.metadata['namespace'] not in ns:
                    continue
                else:
                    log.info(' '
                             + log.cyan(ns[entry.metadata['namespace']])
                             + ':'
                             + log.yellow(entry.key)
                             + f'{chunk_index} = {data}')
        log.info()


def dump_written_blobs(nvs_partition:NVS_Partition) -> None:
    blobs: Dict = {}
    strings: List[NVS_Entry] = []
    legacy_blobs: List[NVS_Entry] = []
    ns = {}
    empty_entry = NVS_Entry(-1, bytearray(32), 'Erased')

    # Gather namespaces, blob indexes and legacy blobs
    for page in nvs_partition.pages:
        for entry in page.entries:
            if entry.state == 'Written':
                if entry.metadata['type'] == 'blob_index':
                    blobs[f'{entry.metadata["namespace"]:03d}{entry.key}'] = [entry] + [empty_entry] * entry.data['chunk_count']
                elif entry.metadata['type'] == 'blob':
                    legacy_blobs.append(entry)
                elif entry.metadata['type'] == 'string':
                    strings.append(entry)
                elif entry.metadata['namespace'] == 0:
                    ns[entry.data['value']] = entry.key

    # Dump blobs
    for key in blobs:
        for page in nvs_partition.pages:
            for entry in page.entries:
                # Gather all blob chunks
                if entry.state == 'Written'                                               \
                   and entry.metadata['type'] != 'blob_index'                             \
                   and entry.metadata['namespace'] == blobs[key][0].metadata['namespace'] \
                   and entry.key == blobs[key][0].key:
                    blobs[key][1 + entry.metadata['chunk_index'] - blobs[key][0].data['chunk_start']] = entry

        blob_index = blobs[key][0]
        blob_chunks = blobs[key][1:]

        # Print blob info
        log.info(log.cyan(ns.get(blob_index.metadata['namespace'], blob_index.metadata['namespace']))
                 + ':'
                 + log.yellow(blob_index.key)
                 + ' - '
                 + f'Type: Blob (Version 2), '
                 + f'Size: {blob_index.data["size"]}')

        # Print blob data
        raw_entries = []
        for kid in blob_chunks:  # Gather all chunk entries
            if kid is empty_entry:
                raw_entries += [empty_entry]
            else:
                raw_entries += kid.children

        for i, entry in enumerate(raw_entries):
            if entry is empty_entry:
                log.info(log.yellow(f'  {"":->63} Missing data {"":-<64}'))
            else:
                log.info(f'  0x{(i * const.entry_size):05x}  {entry.dump_raw()}')
        log.info()

    # Dump strings
    for string in strings:
        log.info(log.cyan(ns.get(string.metadata['namespace'], string.metadata['namespace']))
                 + ':'
                 + log.yellow(string.key)
                 + ' - '
                 + 'Type: String, '
                 + f'Size: {string.data["size"]}')
        for i, entry in enumerate(string.children):
            log.info(f'  0x{(i * const.entry_size):05x}  {entry.dump_raw()}')
        log.info()

    # Dump legacy blobs
    for blob in legacy_blobs:
        log.info(log.cyan(ns.get(blob.metadata['namespace'], blob.metadata['namespace']))
                 + ':'
                 + log.yellow(blob.key)
                 + ' - '
                 + 'Type: Blob (Version 1), '
                 + f'Size: {blob.data["size"]}')
        for i, entry in enumerate(blob.children):
            log.info(f'  0x{(i * const.entry_size):05x}  {entry.dump_raw()}')
        log.info()


def integrity_check(nvs_partition:NVS_Partition) -> None:
    used_namespaces: Dict[int, None] = {}
    found_namespaces: Dict[int, str] = {}
    blobs:Dict = {}
    blob_chunks: List[NVS_Entry] = []
    empty_entry = NVS_Entry(-1, bytearray(32), 'Erased')

    # Partition size
    if len(nvs_partition.pages) < 3:
        log.info(log.yellow('Partition has to have at least 3 pages to function properly!'))

    for page in nvs_partition.pages:
        # Print page header
        if page.header['status'] == 'Empty':
            log.info(log.cyan('Page Empty'))

            # Check if page is truly empty
            if page.raw_entry_state_bitmap != bytearray({0xff}) * const.entry_size:
                log.info(log.red('Page is reported as empty but entry state bitmap is not empty!'))
            if any([not e.is_empty for e in page.entries]):
                log.info(log.red('Page is reported as emtpy but there is written data!'))
        else:
            # Check page header CRC32
            if page.header['crc']['original'] == page.header['crc']['computed']:
                log.info(log.cyan(f'Page no. {page.header["page_index"]}'))
            else:
                log.info(log.cyan(f'Page no. {page.header["page_index"]}'),
                         f'Written CRC32:',
                         log.red(f'{page.header["crc"]["original"]:x}'),
                         f'Generated CRC32:',
                         log.green(f'{page.header["crc"]["computed"]:x}'))

        # Check all entries
        for entry in page.entries:
            # Entry state check
            if entry.is_empty:
                if entry.state == 'Written':
                    log.info(log.red(f' Entry #{entry.index:03d} is reported as written but is empty!'))
                    continue
                elif entry.state == 'Erased':
                    log.info(log.yellow(f' Entry #{entry.index:03d} is reported as erased but is empty!'))

            if entry.state == 'Written':
                # Entry CRC32 check
                if entry.metadata['crc']['original'] != entry.metadata['crc']['computed']:
                    log.info(log.red(f' Entry #{entry.index:03d} {entry.key} has wrong CRC32!{"": <5}'),
                             f'Written:',
                             log.red(f'{entry.metadata["crc"]["original"]:x}'),
                             f'Generated:',
                             log.green(f'{entry.metadata["crc"]["computed"]:x}'))

                # Entry children CRC32 check
                if entry.metadata['span'] > 1 and entry.metadata['crc']['data_original'] != entry.metadata['crc']['data_computed']:
                    log.info(log.red(f' Entry #{entry.index:03d} {entry.key} data has wrong CRC32!'),
                             f'Written:',
                             log.red(f'{entry.metadata["crc"]["original"]:x}'),
                             f'Generated:',
                             log.green(f'{entry.metadata["crc"]["computed"]:x}'))

                # Entry type check
                if entry.metadata['type'] not in [const.item_type[key] for key in const.item_type]:
                    log.info(log.yellow(f' Type of entry #{entry.index:03d} {entry.key} is unrecognized!'),
                             f'Type: {entry.metadata["type"]}')

                # Span check
                if entry.index + entry.metadata['span'] - 1 >= int(const.page_size / const.entry_size) - 2:
                    log.info(log.red(f' Variable length entry #{entry.index:03d} {entry.key} is out of bounds!'))
                # Spanned entry state checks
                elif entry.metadata['span'] > 1:
                    parent_state = entry.state
                    for kid in entry.children:
                        if parent_state != kid.state:
                            log.info(log.yellow(' Inconsistent data state!'),
                                     f'Entry #{entry.index:03d} {entry.key} state: {parent_state},',
                                     f'Data entry #{kid.index:03d} {entry.key} state: {kid.state}')

                # Gather blobs & namespaces
                if entry.metadata['type'] == 'blob_index':
                    blobs[f'{entry.metadata["namespace"]:03d}{entry.key}'] = [entry] + [empty_entry] * entry.data['chunk_count']
                elif entry.metadata['type'] == 'blob_data':
                    blob_chunks.append(entry)

                if entry.metadata['namespace'] == 0:
                    found_namespaces[entry.data['value']] = entry.key
                else:
                    used_namespaces[entry.metadata['namespace']] = None
    log.info()

    # Assemble blobs
    for chunk in blob_chunks:
        parent = blobs.get(f'{chunk.metadata["namespace"]:03d}{chunk.key}', [empty_entry])[0]
        if parent is empty_entry:
            log.info(log.red(f'Blob {chunk.key} chunk has no blob index!'),
                     f'Namespace index: {chunk.metadata["namespace"]:03d}',
                     f'[{found_namespaces.get(chunk.metadata["namespace"], "undefined")}],',
                     f'Chunk Index: {chunk.metadata["chunk_index"]:03d}')
        else:
            blob_key = f'{chunk.metadata["namespace"]:03d}{chunk.key}'
            chunk_index = chunk.metadata['chunk_index'] - parent.data['chunk_start']
            blobs[blob_key][chunk_index + 1] = chunk

    # Check all blobs
    for blob_key in blobs:
        blob_index = blobs[blob_key][0]
        blob_chunks = blobs[blob_key][1:]
        blob_size = blob_index.data['size']

        # Blob data check
        for i, chunk in enumerate(blob_chunks):
            if chunk is empty_entry:
                log.info(log.red(f'Blob {blob_index.key} is missing a chunk!'),
                         f'Namespace index: {blob_index.metadata["namespace"]:03d}',
                         f'[{found_namespaces.get(blob_index.metadata["namespace"], "undefined")}],',
                         f'Chunk Index: {i:03d}')
            else:
                blob_size -= len(chunk.children) * const.entry_size

        if blob_size > 0:
            log.info(log.red(f'Blob {blob_index.key} is missing {blob_size} B of data!'),
                     f'Namespace index: {blob_index.metadata["namespace"]:03d}')

    # Namespaces
    for used_ns in used_namespaces:
        key = found_namespaces.pop(used_ns, '')
        if key == '':
            log.info(log.red('Undefined namespace index!'),
                     f'Namespace index: {used_ns:03d}',
                     f'[undefined]')

    for unused_ns in found_namespaces:
        log.info(log.yellow('Found unused namespace.'),
                 f'Namespace index: {unused_ns:03d}',
                 f'[{found_namespaces[unused_ns]}]')


def program_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description='Parse NVS partition')
    parser.add_argument('file', help='Path to dumped NVS partition')
    parser.add_argument('-i', '--integrity-check', action='store_true',
                        help='Check partition for potential errors')
    tmp = {'all': 'Print everything',
           'written': 'Print only currently written entries',
           'minimal': 'Print only namespace:key=value pairs',
           'namespaces': 'List all written namespaces',
           'blobs': 'Print all blobs and strings',
           'storage_info': 'Print storage related information (free/used entries, etc)',
           'none': 'Do not print anything (if you only want to do integrity check)'}
    parser.add_argument('-d', '--dump', choices=tmp, default='all', metavar='type',
                        help=f"type={{{str(list(tmp.keys()))[1:-1]}}} : {' ; '.join([f'{opt} - {tmp[opt]}' for opt in tmp])}")
    parser.add_argument('--color', choices=['never', 'auto', 'always'], default='auto', help='Enable color (ANSI)')
    return parser.parse_args()


def main() -> None:
    args = program_args()

    if const.entry_size != 32:
        raise ValueError(f'Entry size is not 32 B! This is currently non negotiable.')

    log.set_color(args.color)

    try:
        with open(args.file, 'rb') as f:
            partition = f.read()
    except IndexError:
        log.error('No file given')
        raise
    except FileNotFoundError:
        log.error('Bad filename')
        raise

    nvs = NVS_Partition(args.file.split('/')[-1], bytearray(partition))

    def noop(_:NVS_Partition) -> None:
        pass

    def not_implemented(_:NVS_Partition) -> None:
        raise RuntimeError(f'{args.dump} is not implemented')

    cmds = {
        'all': dump_everything,
        'written': dump_written_entries,
        'minimal': dump_key_value_pairs,
        'namespaces': list_namespaces,
        'blobs': dump_written_blobs,
        'storage_info': storage_stats,
        'none': noop,
    }
    cmds.get(args.dump, not_implemented)(nvs)  # type: ignore

    if args.integrity_check:
        log.info()
        integrity_check(nvs)


if __name__ == '__main__':
    try:
        main()
    except ValueError:
        traceback.print_exc(file=sys.stderr)
        sys.exit(1)
    except Constants.ConstantError:
        traceback.print_exc(file=sys.stderr)
        sys.exit(1)
