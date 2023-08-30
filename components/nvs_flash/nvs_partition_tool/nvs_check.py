#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from typing import Dict, List

from nvs_logger import NVS_Logger
from nvs_parser import NVS_Entry, NVS_Partition, nvs_const


def integrity_check(nvs_partition: NVS_Partition, nvs_log: NVS_Logger) -> None:
    used_namespaces: Dict[int, None] = {}
    found_namespaces: Dict[int, str] = {}
    blobs: Dict = {}
    blob_chunks: List[NVS_Entry] = []
    empty_entry = NVS_Entry(-1, bytearray(32), 'Erased')

    # Partition size check
    if len(nvs_partition.pages) < 3:
        nvs_log.info(
            nvs_log.yellow(
                'Partition has to have at least 3 pages to function properly!'
            )
        )

    # Free/empty page check
    if not any(page.header['status'] == 'Empty' for page in nvs_partition.pages):
        nvs_log.info(
            nvs_log.red(
                'There are no free (empty) pages in the partition, there needs to be at least one free page!'
            )
        )
        nvs_log.info(nvs_log.red('Has the NVS partition been truncated?\n'))

    for page in nvs_partition.pages:
        # page: NVS_Page

        # Print page header
        if page.header['status'] == 'Empty':
            nvs_log.info(nvs_log.cyan('Page Empty'))

            # Check if page is truly empty
            if page.raw_entry_state_bitmap != bytearray({0xFF}) * nvs_const.entry_size:
                nvs_log.info(
                    nvs_log.red(
                        'Page is reported as empty but entry state bitmap is not empty!'
                    )
                )
            if any([not e.is_empty for e in page.entries]):
                nvs_log.info(
                    nvs_log.red('Page is reported as empty but there are data written!')
                )
        else:
            # Check page header CRC32
            if page.header['crc']['original'] == page.header['crc']['computed']:
                nvs_log.info(
                    nvs_log.cyan(f'Page no. {page.header["page_index"]}'), '\tCRC32: OK'
                )
            else:
                nvs_log.info(
                    nvs_log.cyan(f'Page no. {page.header["page_index"]}'),
                    f'Original CRC32:',
                    nvs_log.red(f'{page.header["crc"]["original"]:x}'),
                    f'Generated CRC32:',
                    nvs_log.green(f'{page.header["crc"]["computed"]:x}'),
                )

        # Check all entries
        for entry in page.entries:
            # entry: NVS_Entry

            # Entry state check
            if entry.is_empty:
                if entry.state == 'Written':
                    nvs_log.info(
                        nvs_log.red(
                            f' Entry #{entry.index:03d} is reported as written but is empty!'
                        )
                    )
                    continue
                elif entry.state == 'Erased':
                    nvs_log.info(
                        nvs_log.yellow(
                            f' Entry #{entry.index:03d} is reported as erased but is empty!'
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
                    and entry.metadata['crc']['data_original']
                    != entry.metadata['crc']['data_computed']
                ):
                    nvs_log.info(
                        nvs_log.red(
                            f' Entry #{entry.index:03d} {entry.key} data has wrong CRC32!'
                        ),
                        f'Written:',
                        nvs_log.red(f'{entry.metadata["crc"]["original"]:x}'),
                        f'Generated:',
                        nvs_log.green(f'{entry.metadata["crc"]["computed"]:x}'),
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
                        empty_entry
                    ] * entry.data['chunk_count']
                elif entry.metadata['type'] == 'blob_data':
                    blob_chunks.append(entry)

                if entry.metadata['namespace'] == 0:
                    found_namespaces[entry.data['value']] = entry.key
                else:
                    used_namespaces[entry.metadata['namespace']] = None

    nvs_log.info()

    # Blob checks
    # Assemble blobs
    for chunk in blob_chunks:
        parent = blobs.get(
            f'{chunk.metadata["namespace"]:03d}{chunk.key}', [empty_entry]
        )[0]
        # Blob chunk without blob index check
        if parent is empty_entry:
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

    # Blob data check
    for blob_key in blobs:
        blob_index = blobs[blob_key][0]
        blob_chunks = blobs[blob_key][1:]
        blob_size = blob_index.data['size']

        for i, chunk in enumerate(blob_chunks):
            # Blob missing chunk check
            if chunk is empty_entry:
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

    # Namespace checks
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
