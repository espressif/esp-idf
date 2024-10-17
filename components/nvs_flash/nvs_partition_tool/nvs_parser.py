#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from typing import Any
from typing import Dict
from typing import List
from typing import Optional
from zlib import crc32


# Constants
class NVS_Constants:
    class ConstantError(AttributeError):
        pass

    def __init__(self) -> None:
        self.page_size = 4096
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
            0xFFFFFFFF: 'Empty',
            0xFFFFFFFE: 'Active',
            0xFFFFFFFC: 'Full',
            0xFFFFFFF8: 'Erasing',
            0x00000000: 'Corrupted',
        }
        self.entry_status = {
            0b11: 'Empty',
            0b10: 'Written',
            0b00: 'Erased',
        }

    def __setattr__(self, key: str, val: Any) -> None:
        if self.__dict__.get(key, None) is None:
            self.__dict__[key] = val
        else:
            raise NVS_Constants.ConstantError('Cannot change a constant!')


nvs_const = NVS_Constants()


class NotAlignedError(ValueError):
    pass


class NVS_Partition:
    def __init__(self, name: str, raw_data: bytearray):
        if len(raw_data) % nvs_const.page_size != 0:
            raise NotAlignedError(
                f'Given partition data is not aligned to page size ({len(raw_data)} % {nvs_const.page_size} = {len(raw_data)%nvs_const.page_size})'
            )

        self.name = name
        self.raw_data = raw_data
        # Divide partition into pages
        self.pages = []
        for i in range(0, len(raw_data), nvs_const.page_size):
            self.pages.append(NVS_Page(raw_data[i: i + nvs_const.page_size], i))

    def toJSON(self) -> Dict[str, Any]:
        return dict(name=self.name, pages=self.pages)


class NVS_Page:
    def __init__(self, page_data: bytearray, address: int):
        if len(page_data) != nvs_const.page_size:
            raise NotAlignedError(
                f'Size of given page does not match page size ({len(page_data)} != {nvs_const.page_size})'
            )

        # Initialize class
        self.is_empty = (
            page_data[0: nvs_const.entry_size]
            == bytearray({0xFF}) * nvs_const.entry_size
        )
        self.start_address = address
        self.raw_header = page_data[0: nvs_const.entry_size]
        self.raw_entry_state_bitmap = page_data[
            nvs_const.entry_size: 2 * nvs_const.entry_size
        ]
        self.entries = []

        # Load header
        self.header: Dict[str, Any] = {
            'status': nvs_const.page_status.get(
                int.from_bytes(page_data[0:4], byteorder='little'), 'Invalid'
            ),
            'page_index': int.from_bytes(page_data[4:8], byteorder='little'),
            'version': 256 - page_data[8],
            'crc': {
                'original': int.from_bytes(page_data[28:32], byteorder='little'),
                'computed': crc32(page_data[4:28], 0xFFFFFFFF),
            },
        }

        # Load entry state bitmap
        entry_states = []
        for c in self.raw_entry_state_bitmap:
            for index in range(0, 8, 2):
                entry_states.append(
                    nvs_const.entry_status.get((c >> index) & 3, 'Invalid')
                )
        entry_states = entry_states[:-2]

        # Load entries
        i = 2
        while i < int(
            nvs_const.page_size / nvs_const.entry_size
        ):  # Loop through every entry
            span = page_data[(i * nvs_const.entry_size) + 2]
            if span in [0xFF, 0]:  # 'Default' span length to prevent span overflow
                span = 1

            # Load an entry
            entry = NVS_Entry(
                index=(i - 2),
                entry_data=page_data[i * nvs_const.entry_size: (i + 1) * nvs_const.entry_size],
                entry_state=entry_states[i - 2],
            )
            self.entries.append(entry)

            # Load all children entries
            if span != 1:
                for span_idx in range(1, span):
                    page_addr = i + span_idx
                    entry_idx = page_addr - 2
                    if page_addr * nvs_const.entry_size >= nvs_const.page_size:
                        break
                    child_entry = NVS_Entry(
                        index=entry_idx,
                        entry_data=page_data[
                            page_addr
                            * nvs_const.entry_size: (page_addr + 1)
                            * nvs_const.entry_size
                        ],
                        entry_state=entry_states[entry_idx],
                    )
                    entry.child_assign(child_entry)
                entry.compute_crc()
            i += span

    def toJSON(self) -> Dict[str, Any]:
        return dict(
            is_empty=self.is_empty,
            start_address=self.start_address,
            raw_header=self.raw_header,
            raw_entry_state_bitmap=self.raw_entry_state_bitmap,
            header=self.header,
            entries=self.entries,
        )


class NVS_Entry:
    def __init__(self, index: int, entry_data: bytearray, entry_state: str):
        if len(entry_data) != nvs_const.entry_size:
            raise NotAlignedError(
                f'Given entry is not aligned to entry size ({len(entry_data)} % {nvs_const.entry_size} = {len(entry_data)%nvs_const.entry_size})'
            )

        def item_convert(i_type: int, data: bytearray) -> Dict:
            byte_size_mask = 0x0F
            number_sign_mask = 0xF0
            fixed_entry_length_threshold = (
                0x20  # Fixed length entry type number is always smaller than this
            )
            if i_type in nvs_const.item_type:
                # Deal with non variable length entries
                if i_type < fixed_entry_length_threshold:
                    size = i_type & byte_size_mask
                    num = int.from_bytes(
                        data[:size],
                        byteorder='little',
                        signed=bool(i_type & number_sign_mask),
                    )
                    return {'value': num}

                # Deal with variable length entries
                if nvs_const.item_type[i_type] in ['string', 'blob_data', 'blob']:
                    size = int.from_bytes(data[:2], byteorder='little')
                    crc = int.from_bytes(data[4:8], byteorder='little')
                    return {'value': [size, crc], 'size': size, 'crc': crc}
                if nvs_const.item_type[i_type] == 'blob_index':
                    size = int.from_bytes(data[:4], byteorder='little')
                    chunk_count = data[4]
                    chunk_start = data[5]
                    return {
                        'value': [size, chunk_count, chunk_start],
                        'size': size,
                        'chunk_count': chunk_count,
                        'chunk_start': chunk_start,
                    }

            return {'value': None}

        def key_decode(data: bytearray) -> Optional[str]:
            decoded = ''
            for n in data.rstrip(b'\x00'):
                char = chr(n)
                if char.isascii():
                    decoded += char
                else:
                    return None
            return decoded

        self.raw = entry_data
        self.state = entry_state
        self.is_empty = self.raw == bytearray({0xFF}) * nvs_const.entry_size
        self.index = index
        self.page = None

        namespace = self.raw[0]
        entry_type = self.raw[1]
        span = self.raw[2]
        chunk_index = self.raw[3]
        crc = self.raw[4:8]
        key = self.raw[8:24]
        data = self.raw[24:32]
        raw_without_crc = self.raw[:4] + self.raw[8:32]
        self.metadata: Dict[str, Any] = {
            'namespace': namespace,
            'type': nvs_const.item_type.get(entry_type, f'0x{entry_type:02x}'),
            'span': span,
            'chunk_index': chunk_index,
            'crc': {
                'original': int.from_bytes(crc, byteorder='little'),
                'computed': crc32(raw_without_crc, 0xFFFFFFFF),
                'data_original': int.from_bytes(data[-4:], byteorder='little'),
                'data_computed': 0,
            },
        }
        self.children: List['NVS_Entry'] = []
        self.key = key_decode(key)
        if self.key is None:
            self.data = None
        else:
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

    def child_assign(self, entry: 'NVS_Entry') -> None:
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
        if self.data:
            if self.data['value'] is not None:
                if self.data['size']:
                    children_data = children_data[: self.data['size']]  # Discard padding
        self.metadata['crc']['data_computed'] = crc32(children_data, 0xFFFFFFFF)

    def toJSON(self) -> Dict[str, Any]:
        return dict(
            raw=self.raw,
            state=self.state,
            is_empty=self.is_empty,
            index=self.index,
            metadata=self.metadata,
            key=self.key,
            data=self.data,
            children=self.children,
        )
