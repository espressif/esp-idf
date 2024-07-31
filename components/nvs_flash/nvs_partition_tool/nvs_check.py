#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from typing import Dict, List

from nvs_logger import NVS_Logger
from nvs_parser import nvs_const
from nvs_parser import NVS_Entry
from nvs_parser import NVS_Page
from nvs_parser import NVS_Partition


EMPTY_ENTRY = NVS_Entry(-1, bytearray(32), 'Erased')

used_namespaces: Dict[int, None] = {}
found_namespaces: Dict[int, str] = {}
blobs: Dict = {}
blob_chunks: List[NVS_Entry] = []


def check_partition_size(nvs_partition: NVS_Partition, nvs_log: NVS_Logger) -> None:
    if len(nvs_partition.pages) < 3:
        nvs_log.info(
            nvs_log.yellow(
                'NVS Partition must contain 3 pages (sectors) at least to function properly!'
            )
        )


def check_empty_page_present(nvs_partition: NVS_Partition, nvs_log: NVS_Logger) -> None:
    if not any(page.header['status'] == 'Empty' for page in nvs_partition.pages):
        nvs_log.info(
            nvs_log.red(
                '''No free (empty) page found in the NVS partition,
at least one free page is required for proper function!'''
            )
        )
        nvs_log.info(nvs_log.red('NVS partition possibly truncated?\n'))


def check_empty_page_content(nvs_page: NVS_Page, nvs_log: NVS_Logger) -> None:
    nvs_log.info(nvs_log.cyan(f'Page {nvs_page.header["status"]}'))

    if nvs_page.raw_entry_state_bitmap != bytearray({0xFF}) * nvs_const.entry_size:
        nvs_log.info(
            nvs_log.red(
                'The page is reported as Empty but its entry state bitmap is not empty!'
            )
        )

    if any([not e.is_empty for e in nvs_page.entries]):
        nvs_log.info(
            nvs_log.red('The page is reported as Empty but there are data written!')
        )


def check_page_crc(nvs_page: NVS_Page, nvs_log: NVS_Logger) -> None:
    if nvs_page.header['crc']['original'] == nvs_page.header['crc']['computed']:
        nvs_log.info(
            nvs_log.cyan(f'Page no. {nvs_page.header["page_index"]}'), '\tCRC32: OK'
        )
    else:
        nvs_log.info(
            nvs_log.cyan(f'Page no. {nvs_page.header["page_index"]}'),
            f'Original CRC32:',
            nvs_log.red(f'{nvs_page.header["crc"]["original"]:x}'),
            f'Generated CRC32:',
            nvs_log.green(f'{nvs_page.header["crc"]["computed"]:x}'),
        )


def identify_entry_duplicates(entry: NVS_Entry, seen_written_entires: Dict[str, list[NVS_Entry]]) -> Dict[str, list[NVS_Entry]]:
    if entry.state == 'Written':
        if entry.key in seen_written_entires:
            seen_written_entires[entry.key].append(entry)
        else:
            seen_written_entires[entry.key] = [entry]
    return seen_written_entires


def check_page_entries(nvs_page: NVS_Page, nvs_log: NVS_Logger) -> Dict[str, list[NVS_Entry]]:
    seen_written_entires: Dict[str, list[NVS_Entry]] = {}

    for entry in nvs_page.entries:
        # entry: NVS_Entry

        # Entries stored in 'page.entries' are primitive data types, blob indexes or string/blob data

        # Variable length values themselves occupy whole 32 bytes (therefore metadata values are meaningless)
        # and are stored in as entries inside string/blob data entry 'entry.children' list

        # Duplicate entry check (1) - same key, different index - find duplicates
        seen_written_entires = identify_entry_duplicates(entry, seen_written_entires)

        # Entry state check - doesn't check variable length values (metadata such as state are meaningless as all 32 bytes are pure data)
        if entry.is_empty:
            if entry.state == 'Written':
                nvs_log.info(
                    nvs_log.red(
                        f' Entry #{entry.index:03d} is reported as Written but it is empty!'
                    )
                )
                continue
            elif entry.state == 'Erased':
                nvs_log.info(
                    nvs_log.yellow(
                        f' Entry #{entry.index:03d} is reported as Erased but it is empty! (Only entries reported as Empty should be empty)'
                    )
                )

        if entry.state == 'Written':
            # Entry CRC32 check
            if (
                entry.metadata['crc']['original']
                != entry.metadata['crc']['computed']
            ):
                nvs_log.info(
                    nvs_log.red(
                        f' Entry #{entry.index:03d} {entry.key} has wrong CRC32!{"": <5}'
                    ),
                    f'Written:',
                    nvs_log.red(f'{entry.metadata["crc"]["original"]:x}'),
                    f'Generated:',
                    nvs_log.green(f'{entry.metadata["crc"]["computed"]:x}'),
                )

            # Entry children CRC32 check
            if (
                entry.metadata['span'] > 1
                and (entry.metadata['crc']['data_original'] != entry.metadata['crc']['data_computed'])
            ):
                nvs_log.info(
                    nvs_log.red(
                        f' Entry #{entry.index:03d} {entry.key} data (string, blob) has wrong CRC32!'
                    ),
                    f'Written:',
                    nvs_log.red(f'{entry.metadata["crc"]["data_original"]:x}'),
                    f'Generated:',
                    nvs_log.green(f'{entry.metadata["crc"]["data_computed"]:x}'),
                )

            # Entry type check
            if entry.metadata['type'] not in [
                nvs_const.item_type[key] for key in nvs_const.item_type
            ]:
                nvs_log.info(
                    nvs_log.yellow(
                        f' Type of entry #{entry.index:03d} {entry.key} is unrecognized!'
                    ),
                    f'Type: {entry.metadata["type"]}',
                )

            # Span check
            if (
                entry.index + entry.metadata['span'] - 1
                >= int(nvs_const.page_size / nvs_const.entry_size) - 2
            ):
                nvs_log.info(
                    nvs_log.red(
                        f' Variable length entry #{entry.index:03d} {entry.key} is out of bounds!'
                    )
                )
            # Spanned entry state checks
            elif entry.metadata['span'] > 1:
                parent_state = entry.state
                for kid in entry.children:
                    if parent_state != kid.state:
                        nvs_log.info(
                            nvs_log.yellow(' Inconsistent data state!'),
                            f'Entry #{entry.index:03d} {entry.key} state: {parent_state},',
                            f'Data entry #{kid.index:03d} {entry.key} state: {kid.state}',
                        )

            # Gather blobs & namespaces
            if entry.metadata['type'] == 'blob_index':
                blobs[f'{entry.metadata["namespace"]:03d}{entry.key}'] = [entry] + [
                    EMPTY_ENTRY
                ] * entry.data['chunk_count']
            elif entry.metadata['type'] == 'blob_data':
                blob_chunks.append(entry)

            if entry.metadata['namespace'] == 0:
                found_namespaces[entry.data['value']] = entry.key
            else:
                used_namespaces[entry.metadata['namespace']] = None

    return seen_written_entires


def filter_entry_duplicates(seen_written_entires: Dict[str, list[NVS_Entry]]) -> List[List[NVS_Entry]]:
    duplicate_entries_list = [seen_written_entires[key] for key in seen_written_entires if len(seen_written_entires[key]) > 1]
    return duplicate_entries_list


def print_entry_duplicates(page: NVS_Page, duplicate_entries_list: List[List[NVS_Entry]], nvs_log: NVS_Logger) -> None:
    for duplicate_entries in duplicate_entries_list:
        # duplicate_entries: list[NVS_Entry]
        nvs_log.info(
            nvs_log.red(
                f'''Entry key {duplicate_entries[0].key} on page no. {page.header["page_index"]}
with status {page.header["status"]} is used by the following entries:'''
            )
        )
        for entry in duplicate_entries:
            nvs_log.info(
                nvs_log.red(
                    f'Entry #{entry.index:03d} {entry.key} is a duplicate!'
                )
            )


def assemble_blobs(nvs_log: NVS_Logger) -> None:
    for chunk in blob_chunks:
        # chunk: NVS_Entry
        parent = blobs.get(
            f'{chunk.metadata["namespace"]:03d}{chunk.key}', [EMPTY_ENTRY]
        )[0]
        # Blob chunk without blob index check
        if parent is EMPTY_ENTRY:
            nvs_log.info(
                nvs_log.red(f'Blob {chunk.key} chunk has no blob index!'),
                f'Namespace index: {chunk.metadata["namespace"]:03d}',
                f'[{found_namespaces.get(chunk.metadata["namespace"], "undefined")}],',
                f'Chunk Index: {chunk.metadata["chunk_index"]:03d}',
            )
        else:
            blob_key = f'{chunk.metadata["namespace"]:03d}{chunk.key}'
            chunk_index = chunk.metadata['chunk_index'] - parent.data['chunk_start']
            blobs[blob_key][chunk_index + 1] = chunk

    # return blobs


def check_blob_data(nvs_log: NVS_Logger) -> None:
    for blob_key in blobs:
        blob_index = blobs[blob_key][0]
        blob_chunks = blobs[blob_key][1:]
        blob_size = blob_index.data['size']

        for i, chunk in enumerate(blob_chunks):
            # chunk: NVS_Entry
            # Blob missing chunk check
            if chunk is EMPTY_ENTRY:
                nvs_log.info(
                    nvs_log.red(f'Blob {blob_index.key} is missing a chunk!'),
                    f'Namespace index: {blob_index.metadata["namespace"]:03d}',
                    f'[{found_namespaces.get(blob_index.metadata["namespace"], "undefined")}],',
                    f'Chunk Index: {i:03d}',
                )
            else:
                blob_size -= len(chunk.children) * nvs_const.entry_size

        # Blob missing data check
        if blob_size > 0:
            nvs_log.info(
                nvs_log.red(f'Blob {blob_index.key} is missing {blob_size} B of data!'),
                f'Namespace index: {blob_index.metadata["namespace"]:03d}',
            )


def check_blobs(nvs_log: NVS_Logger) -> None:
    # Assemble blobs
    assemble_blobs(nvs_log)
    # Blob data check
    check_blob_data(nvs_log)


def check_namespaces(nvs_log: NVS_Logger) -> None:
    # Undefined namespace index check
    for used_ns in used_namespaces:
        key = found_namespaces.pop(used_ns, '')
        if key == '':
            nvs_log.info(
                nvs_log.red('Undefined namespace index!'),
                f'Namespace index: {used_ns:03d}',
                f'[undefined]',
            )

    # Unused namespace index check
    for unused_ns in found_namespaces:
        nvs_log.info(
            nvs_log.yellow('Found unused namespace.'),
            f'Namespace index: {unused_ns:03d}',
            f'[{found_namespaces[unused_ns]}]',
        )


def integrity_check(nvs_partition: NVS_Partition, nvs_log: NVS_Logger) -> None:
    # Partition size check
    check_partition_size(nvs_partition, nvs_log)

    # Free/empty page check
    check_empty_page_present(nvs_partition, nvs_log)

    for page in nvs_partition.pages:
        # page: NVS_Page

        # Print page header
        if page.header['status'] == 'Empty':
            # Check if page is truly empty
            check_empty_page_content(page, nvs_log)
        else:
            # Check page header CRC32
            check_page_crc(page, nvs_log)

        # Check all entries
        seen_written_entires = check_page_entries(page, nvs_log)

        # Duplicate entry check (2) - same key, different index - print duplicates
        duplicates = filter_entry_duplicates(seen_written_entires)
        # Print duplicate entries
        print_entry_duplicates(page, duplicates, nvs_log)

    nvs_log.info()  # Empty line

    # Blob checks
    check_blobs(nvs_log)

    # Namespace checks
    check_namespaces(nvs_log)
