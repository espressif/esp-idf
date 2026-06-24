#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import binascii
import json
import sys
from typing import Any

from esp_pylib.logger import log
from nvs_parser import NVS_Entry
from nvs_parser import NVS_Partition
from nvs_parser import nvs_const


class NVS_Logger:
    """Rich-based logger for NVS partition dump and integrity-check output."""

    def __init__(self, *, color: str = 'auto', out_format: str = 'text'):
        self.output_format = out_format
        self.set_color(color)

    def set_color(self, color: str) -> None:
        no_color = color == 'never' or (color == 'auto' and not sys.stdout.isatty())
        self._color_enabled = not no_color

    def set_format(self, out_format: str) -> None:
        self.output_format = out_format

    def info(self, *args: Any, **kwargs: Any) -> None:
        log.print(*args, **kwargs)

    def error(self, *args: Any, **kwargs: Any) -> None:
        log.err(*args)

    def warn(self, message: str) -> None:
        log.warn(message)

    def err(self, message: str) -> None:
        log.err(message)

    def _style(self, text: str, style: str) -> str:
        if self._color_enabled:
            return f'[{style}]{text}[/{style}]'
        return text

    def red(self, text: str) -> str:
        return self._style(text, 'red')

    def green(self, text: str) -> str:
        return self._style(text, 'green')

    def yellow(self, text: str) -> str:
        return self._style(text, 'yellow')

    def blue(self, text: str) -> str:
        return self._style(text, 'blue')

    def cyan(self, text: str) -> str:
        return self._style(text, 'cyan')

    def bold(self, text: str) -> str:
        return self._style(text, 'bold')


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
            nvs_log.info(nvs_log.bold(f'Page Empty, Page address: 0x{page.start_address:x}'))
        else:
            if page.header['crc']['original'] == page.header['crc']['computed']:  # Color CRC32
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
            if entry.state == 'Empty' and entry.is_empty:  # Gather all subsequent empty entries
                empty_entries.append(entry)
                continue
            else:
                # Print the empty entries
                if len(empty_entries) >= 3:  # There is enough entries to compress
                    nvs_log.info(nvs_log.bold(f' {empty_entries[0].index:03d}.'), 'Empty')
                    nvs_log.info(nvs_log.bold(' ...'))
                    nvs_log.info(nvs_log.bold(f' {empty_entries[-1].index:03d}.'), 'Empty')
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
            if entry.metadata['crc']['original'] == entry.metadata['crc']['computed']:  # Color CRC32
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
                + f' | {entry.key}: ',
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
                    if entry.metadata['crc']['data_original'] == entry.metadata['crc']['data_computed']:  # Color CRC32
                        crc = nvs_log.green(f'{entry.metadata["crc"]["data_original"]:x}')
                    else:
                        crc = nvs_log.red(f'{entry.metadata["crc"]["data_original"]:x}')
                    nvs_log.info(f'Size={entry.data["size"]}, CRC32={crc}')

            # Dump all children entries
            if entry.metadata['span'] != 1:
                for i, data in enumerate(entry.children):
                    nvs_log.info(f'{"": >6}0x{(i * nvs_const.entry_size):03x}  {data.dump_raw()}')

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
    nvs_log.info(nvs_log.bold('Index: Namespace'))
    for ns_index in sorted(ns):
        nvs_log.info(f' {ns_index:03d}:', nvs_log.cyan(ns[ns_index]))


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
            nvs_log.info(nvs_log.bold(f'Page no. {page.header["page_index"]}' + f', Status: {page.header["status"]}'))

        # Print entries
        for entry in page.entries:
            if entry.state == 'Written' and entry.metadata['namespace'] != 0:  # Ignore non-written entries
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
                    elif entry.metadata['type'] == 'string':
                        data = str(tmp, 'utf-8')
                    else:
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
    blobs: dict = {}
    strings: list[NVS_Entry] = []
    legacy_blobs: list[NVS_Entry] = []
    ns = {}
    empty_entry = NVS_Entry(-1, bytearray(32), 'Erased')

    # Gather namespaces, blob indexes and legacy blobs
    for page in nvs_partition.pages:
        for entry in page.entries:
            if entry.state == 'Written':
                if entry.metadata['type'] == 'blob_index':
                    blobs[f'{entry.metadata["namespace"]:03d}{entry.key}'] = [entry] + [empty_entry] * entry.data[
                        'chunk_count'
                    ]
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
                    and entry.metadata['namespace'] == blobs[key][0].metadata['namespace']
                    and entry.key == blobs[key][0].key
                ):
                    blobs[key][1 + entry.metadata['chunk_index'] - blobs[key][0].data['chunk_start']] = entry

        blob_index = blobs[key][0]
        blob_chunks = blobs[key][1:]

        # Print blob info
        nvs_log.info(
            nvs_log.cyan(ns.get(blob_index.metadata['namespace'], blob_index.metadata['namespace']))
            + ':'
            + nvs_log.yellow(blob_index.key)
            + ' - '
            + 'Type: Blob (Version 2), '
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
                nvs_log.info(f'  0x{(i * nvs_const.entry_size):05x}  {entry.dump_raw()}')
        nvs_log.info()

    # Dump strings
    for string in strings:
        nvs_log.info(
            nvs_log.cyan(ns.get(string.metadata['namespace'], string.metadata['namespace']))
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
        def default(self, obj: Any) -> Any | dict[str, Any] | str:
            if hasattr(obj, 'toJSON'):
                return obj.toJSON()
            if isinstance(obj, bytearray):
                return binascii.b2a_base64(obj, newline=False).decode('ascii')  # Binary to Base64 ASCII representation
            return json.JSONEncoder.default(self, obj)

    # Machine-readable JSON on stdout (not via Rich Console — pytest/capture-friendly).
    print(json.dumps(nvs.toJSON(), cls=NVSEncoder, indent=2))


def print_minimal_json(nvs_partition: NVS_Partition) -> None:
    # Get namespace list
    ns = {}
    for page in nvs_partition.pages:
        for entry in page.entries:
            if entry.state == 'Written' and entry.metadata['namespace'] == 0:
                ns[entry.data['value']] = entry.key

    # Prepare key-value pairs for JSON output
    key_value_pairs = []
    for page in nvs_partition.pages:
        for entry in page.entries:
            if entry.state == 'Written' and entry.metadata['namespace'] != 0:
                data = ''
                entry_type = entry.metadata['type']
                if entry_type not in [
                    'string',
                    'blob_data',
                    'blob_index',
                    'blob',
                ]:  # Non-variable length entry
                    data = entry.data['value']
                elif entry_type == 'blob_index':
                    continue
                else:  # Variable length entries
                    tmp = b''
                    for e in entry.children:  # Merge all children entries
                        tmp += bytes(e.raw)
                    tmp = tmp[: entry.data['size']]  # Discard padding
                    if entry_type == 'string':
                        data = str(tmp.rstrip(b'\x00'), 'utf-8')
                    else:
                        data = binascii.b2a_base64(tmp, newline=False).decode('ascii')

                if entry.metadata['namespace'] in ns:
                    key_value_pairs.append(
                        {
                            'namespace': ns[entry.metadata['namespace']],
                            'key': entry.key,
                            'encoding': entry_type,  # Add type of data
                            'data': data,  # Ensure data ends with a newline
                            'state': entry.state,
                            'is_empty': entry.is_empty if hasattr(entry, 'is_empty') else None,
                        }
                    )
    print(json.dumps(key_value_pairs, indent=4))
