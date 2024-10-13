#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from typing import Dict
from typing import List
from typing import Optional
from typing import Set

from nvs_logger import NVS_Logger
from nvs_parser import nvs_const
from nvs_parser import NVS_Entry
from nvs_parser import NVS_Page
from nvs_parser import NVS_Partition


EMPTY_ENTRY = NVS_Entry(-1, bytearray(32), 'Erased')

used_namespaces: Dict[int, Optional[str]] = {}
found_namespaces: Dict[int, str] = {}
blobs: Dict = {}
blob_chunks: List[NVS_Entry] = []


def check_partition_size(nvs_partition: NVS_Partition, nvs_log: NVS_Logger) -> bool:
    if len(nvs_partition.pages) < 3:
        nvs_log.info(
            nvs_log.yellow(
                'NVS Partition must contain 3 pages (sectors) at least to function properly!'
            )
        )
        return False
    return True


def check_empty_page_present(nvs_partition: NVS_Partition, nvs_log: NVS_Logger) -> bool:
    if not any(page.header['status'] == 'Empty' for page in nvs_partition.pages):
        nvs_log.info(
            nvs_log.red(
                '''No free (empty) page found in the NVS partition,
at least one free page is required for proper function!'''
            )
        )
        nvs_log.info(nvs_log.red('NVS partition possibly truncated?\n'))
        return False
    return True


def check_empty_page_content(nvs_page: NVS_Page, nvs_log: NVS_Logger) -> bool:
    result = True
    nvs_log.info(nvs_log.cyan(f'Page {nvs_page.header["status"]}'))

    if nvs_page.raw_entry_state_bitmap != bytearray({0xFF}) * nvs_const.entry_size:
        result = False
        nvs_log.info(
            nvs_log.red(
                'The page is reported as Empty but its entry state bitmap is not empty!'
            )
        )

    if any([not e.is_empty for e in nvs_page.entries]):
        result = False
        nvs_log.info(
            nvs_log.red('The page is reported as Empty but there are data written!')
        )

    return result


def check_page_crc(nvs_page: NVS_Page, nvs_log: NVS_Logger) -> bool:
    if nvs_page.header['crc']['original'] == nvs_page.header['crc']['computed']:
        nvs_log.info(
            nvs_log.cyan(f'Page no. {nvs_page.header["page_index"]}'), '\tCRC32: OK'
        )
        return True
    else:
        nvs_log.info(
            nvs_log.cyan(f'Page no. {nvs_page.header["page_index"]}'),
            f'Original CRC32:',
            nvs_log.red(f'{nvs_page.header["crc"]["original"]:x}'),
            f'Generated CRC32:',
            nvs_log.green(f'{nvs_page.header["crc"]["computed"]:x}'),
        )
        return False


def identify_entry_duplicates(entry: NVS_Entry, seen_written_entires: Dict[str, List[NVS_Entry]]) -> Dict[str, List[NVS_Entry]]:
    if entry.state == 'Written':
        if entry.key in seen_written_entires:
            seen_written_entires[entry.key].append(entry)
        else:
            seen_written_entires[entry.key] = [entry]
    return seen_written_entires


def check_page_entries(nvs_page: NVS_Page, nvs_log: NVS_Logger) -> Dict[str, List[NVS_Entry]]:
    seen_written_entires: Dict[str, List[NVS_Entry]] = {}

    for entry in nvs_page.entries:
        # entry: NVS_Entry
        entry.page = nvs_page

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


def filter_namespaces_fake_duplicates(duplicate_entries_dict: Dict[str, List[NVS_Entry]]) -> Dict[str, List[NVS_Entry]]:
    new_duplicate_entries_dict: Dict[str, List[NVS_Entry]] = {}
    for key, duplicate_entries in duplicate_entries_dict.items():
        seen_entries: List[NVS_Entry] = []
        entry_same_namespace_collisions_list: Set[NVS_Entry] = set()

        # Search through the "duplicates" and see if there are real duplicates
        # E.g. the key can be the same if the namespace is different
        for entry in duplicate_entries:
            if entry.metadata['type'] in nvs_const.item_type.values():

                entry_same_namespace_collisions = set()
                for other_entry in seen_entries:
                    if entry.metadata['namespace'] == other_entry.metadata['namespace']:
                        entry_same_namespace_collisions.add(entry)
                        entry_same_namespace_collisions.add(other_entry)

                if len(entry_same_namespace_collisions) != 0:
                    entry_same_namespace_collisions_list.update(entry_same_namespace_collisions)
                seen_entries.append(entry)

        # Catch real duplicates
        new_duplicate_entries: List[NVS_Entry] = []
        if len(seen_entries) > 1:
            for entry in seen_entries:
                if entry in entry_same_namespace_collisions_list:
                    new_duplicate_entries.append(entry)

        if len(new_duplicate_entries) > 0:
            new_duplicate_entries_dict[key] = new_duplicate_entries

    return new_duplicate_entries_dict


def filter_blob_related_duplicates(duplicate_entries_dict: Dict[str, List[NVS_Entry]]) -> Dict[str, List[NVS_Entry]]:
    new_duplicate_entries_dict: Dict[str, List[NVS_Entry]] = {}
    for key, duplicate_entries in duplicate_entries_dict.items():
        seen_blob_index: List[NVS_Entry] = []
        seen_blob_data: List[NVS_Entry] = []
        seen_another_type_data: List[NVS_Entry] = []
        blob_index_chunk_index_collisions_list: Set[NVS_Entry] = set()
        blob_data_chunk_index_collisions_list: Set[NVS_Entry] = set()

        # Search through the "duplicates" and see if there are real duplicates
        # E.g. the key can be the same for blob_index and blob_data
        # (and even for more blob_data entries if they have a different chunk_index)
        for entry in duplicate_entries:
            if entry.metadata['type'] == 'blob_index':

                blob_index_chunk_index_collisions = set()
                for other_entry in seen_blob_index:
                    if entry.metadata['namespace'] == other_entry.metadata['namespace']:
                        blob_index_chunk_index_collisions.add(entry)
                        blob_index_chunk_index_collisions.add(other_entry)

                if len(blob_index_chunk_index_collisions) != 0:
                    blob_index_chunk_index_collisions_list.update(blob_index_chunk_index_collisions)
                seen_blob_index.append(entry)

            elif entry.metadata['type'] == 'blob_data':

                blob_data_chunk_index_collisions = set()
                for other_entry in seen_blob_data:
                    if (entry.metadata['namespace'] == other_entry.metadata['namespace']
                            and entry.metadata['chunk_index'] == other_entry.metadata['chunk_index']):
                        blob_data_chunk_index_collisions.add(entry)
                        blob_data_chunk_index_collisions.add(other_entry)

                if len(blob_data_chunk_index_collisions) != 0:
                    blob_data_chunk_index_collisions_list.update(blob_data_chunk_index_collisions)
                seen_blob_data.append(entry)

            else:
                seen_another_type_data.append(entry)

        # Catch real duplicates
        new_duplicate_entries: List[NVS_Entry] = []
        if len(seen_blob_index) > 1:
            for entry in seen_blob_index:
                if entry in blob_index_chunk_index_collisions_list:
                    new_duplicate_entries.append(entry)

        if len(seen_blob_data) > 1:
            for entry in seen_blob_data:
                if entry in blob_data_chunk_index_collisions_list:
                        new_duplicate_entries.append(entry)

        for entry in seen_another_type_data:  # If there are any duplicates of other types
            new_duplicate_entries.append(entry)

        if len(new_duplicate_entries) > 0:
            new_duplicate_entries_dict[key] = new_duplicate_entries

    return new_duplicate_entries_dict


def filter_entry_duplicates(seen_written_entires: Dict[str, List[NVS_Entry]]) -> Dict[str, List[NVS_Entry]]:
    duplicate_entries_list = {key: v for key, v in seen_written_entires.items() if len(v) > 1}
    duplicate_entries_list_1 = filter_namespaces_fake_duplicates(duplicate_entries_list)
    duplicate_entries_list_2 = filter_blob_related_duplicates(duplicate_entries_list_1)
    return duplicate_entries_list_2


def print_entry_duplicates(duplicate_entries_list: Dict[str, List[NVS_Entry]], nvs_log: NVS_Logger) -> None:
    if len(duplicate_entries_list) > 0:
        nvs_log.info(nvs_log.red('Found duplicate entries:'))
        nvs_log.info(nvs_log.red('Entry\tKey\t\t\tType\t\tNamespace idx\tPage\tPage status'))

    for _, duplicate_entries in duplicate_entries_list.items():
        # duplicate_entries: List[NVS_Entry]
        for entry in duplicate_entries:
            # entry: NVS_Entry
            if entry.metadata['namespace'] == 0:
                entry_type = f'namespace ({entry.data["value"]})'
            else:
                entry_type = entry.metadata['type']

            if entry.page is not None:
                page_num = entry.page.header['page_index']
                page_status = entry.page.header['status']
            else:
                page_num = 'Unknown'
                page_status = 'Unknown'

            entry_key_tab_cnt = len(entry.key) // 8
            entry_key_tab = '\t' * (3 - entry_key_tab_cnt)

            namespace_tab_cnt = len(entry_type) // 8
            namepace_tab = '\t' * (2 - namespace_tab_cnt)
            namespace_str = f'{entry.metadata["namespace"]}'

            nvs_log.info(
                nvs_log.red(
                    f'#{entry.index:03d}\t{entry.key}{entry_key_tab}{entry_type}{namepace_tab}{namespace_str}\t\t{page_num}\t{page_status}'
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
        key = found_namespaces.pop(used_ns, None)
        if key is None:
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


def reset_global_variables() -> None:
    global used_namespaces, found_namespaces, blobs, blob_chunks
    used_namespaces = {}
    found_namespaces = {}
    blobs = {}
    blob_chunks = []


def integrity_check(nvs_partition: NVS_Partition, nvs_log: NVS_Logger) -> None:
    # Partition size check
    check_partition_size(nvs_partition, nvs_log)

    # Free/empty page check
    check_empty_page_present(nvs_partition, nvs_log)

    seen_written_entires_all: Dict[str, List[NVS_Entry]] = {}

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

        # Collect all seen written entries
        for key in seen_written_entires:
            if key in seen_written_entires_all:
                seen_written_entires_all[key].extend(seen_written_entires[key])
            else:
                seen_written_entires_all[key] = seen_written_entires[key]

    # Duplicate entry check (2) - same key, different index
    duplicates = filter_entry_duplicates(seen_written_entires_all)
    # Print duplicate entries
    print_entry_duplicates(duplicates, nvs_log)

    nvs_log.info()  # Empty line

    # Blob checks
    check_blobs(nvs_log)

    # Namespace checks
    check_namespaces(nvs_log)

    reset_global_variables()
