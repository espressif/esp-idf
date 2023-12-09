#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import binascii
import json
import sys
from typing import Any, Dict, List, Union

from nvs_parser import NVS_Entry, NVS_Partition, nvs_const


class NVS_Logger:
    ansi = {
        'red': '\033[31m',
        'green': '\033[32m',
        'yellow': '\033[33m',
        'blue': '\033[34m',
        'cyan': '\033[36m',
        'bold': '\033[1m',
        'clear': '\033[0m',
    }

    def __init__(self, *, color: str = 'auto', out_format: str = 'text'):
        self.color = color == 'always' or (color == 'auto' and sys.stdout.isatty())
        self.output_format = out_format

    def set_color(self, color: str) -> None:
        self.color = color == 'always' or (color == 'auto' and sys.stdout.isatty())

    def set_format(self, out_format: str) -> None:
        self.output_format = out_format

    def info(self, *args, **kwargs) -> None:  # type: ignore
        kwargs['file'] = kwargs.get(
            'file', sys.stdout
        )  # Set default output to be stdout, but can be overwritten
        print(*args, **kwargs)

    def error(self, *args, **kwargs) -> None:  # type: ignore
        kwargs['file'] = kwargs.get(
            'file', sys.stderr
        )  # Set default output to be stderr, but can be overwritten
        print(*args, **kwargs)

    def red(self, text: str) -> str:
        if self.color:
            return NVS_Logger.ansi['red'] + text + NVS_Logger.ansi['clear']
        return text

    def green(self, text: str) -> str:
        if self.color:
            return NVS_Logger.ansi['green'] + text + NVS_Logger.ansi['clear']
        return text

    def yellow(self, text: str) -> str:
        if self.color:
            return NVS_Logger.ansi['yellow'] + text + NVS_Logger.ansi['clear']
        return text

    def blue(self, text: str) -> str:
        if self.color:
            return NVS_Logger.ansi['blue'] + text + NVS_Logger.ansi['clear']
        return text

    def cyan(self, text: str) -> str:
        if self.color:
            return NVS_Logger.ansi['cyan'] + text + NVS_Logger.ansi['clear']
        return text

    def bold(self, text: str) -> str:
        if self.color:
            return NVS_Logger.ansi['bold'] + text + NVS_Logger.ansi['clear']
        return text


nvs_log = NVS_Logger()


def storage_stats(nvs_partition: NVS_Partition) -> None:
    global_stats = {
        'written_entries': 0,
        'free_entries': 0,
        'erased_entries': 0,
        'invalid_entries': 0,
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

        nvs_log.info(nvs_log.bold(f'Page {page.header["status"]}'))
        nvs_log.info('  Found entries:')
        nvs_log.info(f'    Written: {written_e: 5d}')
        nvs_log.info(f'    Erased:  {erased_e: 5d}')
        nvs_log.info(f'    Empty:   {free_e: 5d}')
        nvs_log.info(f'    Invalid: {invalid_e: 5d}')
        nvs_log.info(f'    Total:   {written_e + free_e + erased_e + invalid_e: 5d}')
        nvs_log.info()
        global_stats['written_entries'] += written_e
        global_stats['erased_entries'] += erased_e
        global_stats['free_entries'] += free_e
        global_stats['invalid_entries'] += invalid_e

    nvs_log.info(nvs_log.bold('Global'))
    nvs_log.info('  Config:')
    nvs_log.info(f'    Page size:   {nvs_const.page_size: 5d}')
    nvs_log.info(f'    Entry size:  {nvs_const.entry_size: 5d}')
    nvs_log.info(f'    Total pages: {len(nvs_partition.pages): 5d}')
    nvs_log.info('  Entries:')
    nvs_log.info(f'    Written: {global_stats["written_entries"]: 5d}')
    nvs_log.info(f'    Erased:  {global_stats["erased_entries"]: 5d}')
    nvs_log.info(f'    Empty:   {global_stats["free_entries"]: 5d}')
    nvs_log.info(f'    Invalid: {global_stats["invalid_entries"]: 5d}')
    nvs_log.info(f'    Total:   {sum([global_stats[key] for key in global_stats]): 5d}')
    nvs_log.info()


def dump_everything(nvs_partition: NVS_Partition, written_only: bool = False) -> None:
    for page in nvs_partition.pages:
        # Print page header
        if page.is_empty:
            nvs_log.info(
                nvs_log.bold(f'Page Empty, Page address: 0x{page.start_address:x}')
            )
        else:
            if (
                page.header['crc']['original'] == page.header['crc']['computed']
            ):  # Color CRC32
                crc = nvs_log.green(f'{page.header["crc"]["original"]: >8x}')
            else:
                crc = nvs_log.red(f'{page.header["crc"]["original"]: >8x}')
            nvs_log.info(
                nvs_log.bold(
                    f'Page no. {page.header["page_index"]}'
                    + f', Status: {page.header["status"]}'
                    + f', Version: {page.header["version"]}'
                    + f', CRC32: {crc}'
                )
                + nvs_log.bold(f', Page address: 0x{page.start_address:x}')
            )

        nvs_log.info(nvs_log.bold(' Entry state bitmap: '), end='')
        for x in page.raw_entry_state_bitmap:
            nvs_log.info(f'{x:02x} ', end='')
        nvs_log.info()

        # Dump entries
        empty_entries = []
        for entry in page.entries:
            # Skip non-written entries if needed
            if written_only and not entry.state == 'Written':
                continue

            # Compress all empty entries
            if (
                entry.state == 'Empty' and entry.is_empty
            ):  # Gather all subsequent empty entries
                empty_entries.append(entry)
                continue
            else:
                # Print the empty entries
                if len(empty_entries) >= 3:  # There is enough entries to compress
                    nvs_log.info(
                        nvs_log.bold(f' {empty_entries[0].index:03d}.'), 'Empty'
                    )
                    nvs_log.info(nvs_log.bold(' ...'))
                    nvs_log.info(
                        nvs_log.bold(f' {empty_entries[-1].index:03d}.'), 'Empty'
                    )
                else:  # No need for compression
                    for e in empty_entries:
                        nvs_log.info(nvs_log.bold(f' {e.index:03d}.'), 'Empty')
                empty_entries.clear()

            # Dump a single entry
            status = entry.state
            if status == 'Written':
                status = nvs_log.green(f'{status: <7}')
            elif status == 'Erased':
                status = nvs_log.red(f'{status: <7}')

            crc = ''
            if (
                entry.metadata['crc']['original'] == entry.metadata['crc']['computed']
            ):  # Color CRC32
                crc = nvs_log.green(f'{entry.metadata["crc"]["original"]: >8x}')
            else:
                crc = nvs_log.red(f'{entry.metadata["crc"]["original"]: >8x}')

            nvs_log.info(
                nvs_log.bold(f' {entry.index:03d}.')
                + ' '
                + status
                + f', Namespace Index: {entry.metadata["namespace"]:03d}'
                + f', Type: {entry.metadata["type"]:<10}'
                + f', Span: {entry.metadata["span"]:03d}'
                + f', Chunk Index: {entry.metadata["chunk_index"]:03d}'
                + f', CRC32: {crc}'
                + f' | {entry.key} : ',
                end='',
            )

            if entry.metadata['type'] not in [
                'string',
                'blob_data',
                'blob_index',
                'blob',
            ]:  # Entry is non-variable length
                if entry.data is not None:
                    nvs_log.info(entry.data['value'])
                else:
                    nvs_log.info(entry.data)  # None
            else:
                if entry.metadata['type'] == 'blob_index':
                    nvs_log.info(
                        f'Size={entry.data["size"]}'
                        + f', ChunkCount={entry.data["chunk_count"]}'
                        + f', ChunkStart={entry.data["chunk_start"]}'
                    )
                else:
                    if (
                        entry.metadata['crc']['data_original']
                        == entry.metadata['crc']['data_computed']
                    ):  # Color CRC32
                        crc = nvs_log.green(
                            f'{entry.metadata["crc"]["data_original"]:x}'
                        )
                    else:
                        crc = nvs_log.red(f'{entry.metadata["crc"]["data_original"]:x}')
                    nvs_log.info(f'Size={entry.data["size"]}, CRC32={crc}')

            # Dump all children entries
            if entry.metadata['span'] != 1:
                for i, data in enumerate(entry.children):
                    nvs_log.info(
                        f'{"": >6}0x{(i*nvs_const.entry_size):03x}  {data.dump_raw()}'
                    )

        # Dump trailing empty entries
        if len(empty_entries) >= 3:
            nvs_log.info(nvs_log.bold(f' {empty_entries[0].index:03d}.'), 'Empty')
            nvs_log.info(nvs_log.bold(' ...'))
            nvs_log.info(nvs_log.bold(f' {empty_entries[-1].index:03d}.'), 'Empty')
        else:
            for e in empty_entries:
                nvs_log.info(nvs_log.bold(f' {e.index:03d}.'), 'Empty')
        empty_entries.clear()
        nvs_log.info()


def dump_written_entries(nvs_partition: NVS_Partition) -> None:
    dump_everything(nvs_partition, True)


def list_namespaces(nvs_partition: NVS_Partition) -> None:
    # Gather namespaces
    ns = {}
    for page in nvs_partition.pages:
        for entry in page.entries:
            if entry.state == 'Written' and entry.metadata['namespace'] == 0:
                ns[entry.data['value']] = entry.key

    # Print found namespaces
    nvs_log.info(nvs_log.bold(f'Index : Namespace'))
    for ns_index in sorted(ns):
        nvs_log.info(f' {ns_index:03d}  :', nvs_log.cyan(ns[ns_index]))


def dump_key_value_pairs(nvs_partition: NVS_Partition) -> None:
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
            nvs_log.info(nvs_log.bold('Page Empty'))
        else:
            nvs_log.info(
                nvs_log.bold(
                    f'Page no. {page.header["page_index"]}'
                    + f', Status: {page.header["status"]}'
                )
            )

        # Print entries
        for entry in page.entries:
            if (
                entry.state == 'Written' and entry.metadata['namespace'] != 0
            ):  # Ignore non-written entries
                chunk_index = ''
                data = ''
                if entry.metadata['type'] not in [
                    'string',
                    'blob_data',
                    'blob_index',
                    'blob',
                ]:  # Non-variable length entry
                    data = entry.data['value']
                elif entry.metadata['type'] == 'blob_index':
                    continue
                else:  # Variable length entries
                    tmp = b''
                    for e in entry.children:  # Merge all children entries
                        tmp += bytes(e.raw)
                    tmp = tmp[: entry.data['size']]  # Discard padding
                    if entry.metadata['type'] == 'blob_data':
                        if entry.metadata['chunk_index'] >= 128:  # Get real chunk index
                            chunk_index = f'[{entry.metadata["chunk_index"] - 128}]'
                        else:
                            chunk_index = f'[{entry.metadata["chunk_index"]}]'
                    data = str(tmp)

                if entry.metadata['namespace'] not in ns:
                    continue
                else:
                    nvs_log.info(
                        ' '
                        + nvs_log.cyan(ns[entry.metadata['namespace']])
                        + ':'
                        + nvs_log.yellow(entry.key)
                        + f'{chunk_index} = {data}'
                    )
        nvs_log.info()


def dump_written_blobs(nvs_partition: NVS_Partition) -> None:
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
                    blobs[f'{entry.metadata["namespace"]:03d}{entry.key}'] = [entry] + [
                        empty_entry
                    ] * entry.data['chunk_count']
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
                if (
                    entry.state == 'Written'
                    and entry.metadata['type'] != 'blob_index'
                    and entry.metadata['namespace']
                    == blobs[key][0].metadata['namespace']
                    and entry.key == blobs[key][0].key
                ):
                    blobs[key][
                        1
                        + entry.metadata['chunk_index']
                        - blobs[key][0].data['chunk_start']
                    ] = entry

        blob_index = blobs[key][0]
        blob_chunks = blobs[key][1:]

        # Print blob info
        nvs_log.info(
            nvs_log.cyan(
                ns.get(
                    blob_index.metadata['namespace'], blob_index.metadata['namespace']
                )
            )
            + ':'
            + nvs_log.yellow(blob_index.key)
            + ' - '
            + f'Type: Blob (Version 2), '
            + f'Size: {blob_index.data["size"]}'
        )

        # Print blob data
        raw_entries = []
        for kid in blob_chunks:  # Gather all chunk entries
            if kid is empty_entry:
                raw_entries += [empty_entry]
            else:
                raw_entries += kid.children

        for i, entry in enumerate(raw_entries):
            if entry is empty_entry:
                nvs_log.info(nvs_log.yellow(f'  {"":->63} Missing data {"":-<64}'))
            else:
                nvs_log.info(
                    f'  0x{(i * nvs_const.entry_size):05x}  {entry.dump_raw()}'
                )
        nvs_log.info()

    # Dump strings
    for string in strings:
        nvs_log.info(
            nvs_log.cyan(
                ns.get(string.metadata['namespace'], string.metadata['namespace'])
            )
            + ':'
            + nvs_log.yellow(string.key)
            + ' - '
            + 'Type: String, '
            + f'Size: {string.data["size"]}'
        )
        for i, entry in enumerate(string.children):
            nvs_log.info(f'  0x{(i * nvs_const.entry_size):05x}  {entry.dump_raw()}')
        nvs_log.info()

    # Dump legacy blobs
    for blob in legacy_blobs:
        nvs_log.info(
            nvs_log.cyan(ns.get(blob.metadata['namespace'], blob.metadata['namespace']))
            + ':'
            + nvs_log.yellow(blob.key)
            + ' - '
            + 'Type: Blob (Version 1), '
            + f'Size: {blob.data["size"]}'
        )
        for i, entry in enumerate(blob.children):
            nvs_log.info(f'  0x{(i * nvs_const.entry_size):05x}  {entry.dump_raw()}')
        nvs_log.info()


def print_json(nvs: NVS_Partition) -> None:
    class NVSEncoder(json.JSONEncoder):
        def default(self, obj: Any) -> Union[Any, Dict[str, Any], str]:
            if hasattr(obj, 'toJSON'):
                return obj.toJSON()
            if isinstance(obj, bytearray):
                return binascii.b2a_base64(obj, newline=False).decode(
                    'ascii'
                )  # Binary to Base64 ASCII representation
            return json.JSONEncoder.default(self, obj)

    print(json.dumps(nvs.toJSON(), cls=NVSEncoder, indent=2))
