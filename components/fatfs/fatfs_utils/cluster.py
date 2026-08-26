# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from typing import Dict, Optional

from construct import Int16ul

from .fatfs_state import BootSectorState
from .utils import (EMPTY_BYTE, FAT12, FAT16, build_byte, merge_by_half_byte_12_bit_little_endian,
                    split_by_half_byte_12_bit_little_endian)


def get_dir_size(is_root: bool, boot_sector: BootSectorState) -> int:
    dir_size_: int = boot_sector.root_dir_sectors_cnt * boot_sector.sector_size if is_root else boot_sector.sector_size
    return dir_size_


class Cluster:
    """
    class Cluster handles values in FAT table and allocates sectors in data region.
    """
    RESERVED_BLOCK_ID: int = 0
    ROOT_BLOCK_ID: int = 1
    ALLOCATED_BLOCK_FAT12: int = 0xFFF
    ALLOCATED_BLOCK_FAT16: int = 0xFFFF
    ALLOCATED_BLOCK_SWITCH = {FAT12: ALLOCATED_BLOCK_FAT12, FAT16: ALLOCATED_BLOCK_FAT16}
    INITIAL_BLOCK_SWITCH: Dict[int, int] = {FAT12: 0xFF8, FAT16: 0xFFF8}

    def __init__(self,
                 cluster_id: int,
                 boot_sector_state: BootSectorState,
                 init_: bool) -> None:
        """
        Initially, if init_ is False, the cluster is virtual and is not allocated (doesn't do changes in the FAT).
        :param cluster_id: the cluster ID - a key value linking the file's cluster,
          the corresponding physical cluster (data region) and the FAT table cluster.
        :param boot_sector_state: auxiliary structure holding the file-system's metadata
        :param init_: True for allocation the cluster on instantiation, otherwise False.
        :returns: None
        """
        self.id: int = cluster_id
        self.boot_sector_state: BootSectorState = boot_sector_state

        self._next_cluster = None  # type: Optional[Cluster]
        # First cluster in FAT is reserved, low 8 bits contains BPB_Media and the rest is filled with 1
        # e.g. the esp32 media type is 0xF8 thus the FAT[0] = 0xFF8 for FAT12, 0xFFF8 for FAT16
        if self.id == Cluster.RESERVED_BLOCK_ID and init_:
            self.set_in_fat(self.INITIAL_BLOCK_SWITCH[self.boot_sector_state.fatfs_type])
            return
        self.cluster_data_address: int = self._compute_cluster_data_address()
        assert self.cluster_data_address

    @property
    def next_cluster(self):  # type: () -> Optional[Cluster]
        return self._next_cluster

    @next_cluster.setter
    def next_cluster(self, value):  # type: (Optional[Cluster]) -> None
        self._next_cluster = value

    def _cluster_id_to_fat_position_in_bits(self, _id: int) -> int:
        """
        This private method calculates the position of the memory block (cluster) in the FAT table.

        :param _id: the cluster ID - a key value linking the file's cluster,
          the corresponding physical cluster (data region) and the FAT table cluster.
        :returns: bit offset of the cluster in FAT
          e.g.:
          00003000: 42 65 00 2E 00 74 00 78 00 74 00 0F 00 43 FF FF

          For FAT12 the third cluster has value = 0x02E and ID = 2.
          Its bit-address is 24 (24 bits preceding, 0-indexed), because 0x2E starts at the bit-offset 24.
        """
        logical_position_: int = self.boot_sector_state.fatfs_type * _id
        return logical_position_

    @staticmethod
    def compute_cluster_data_address(boot_sector_state: BootSectorState, id_: int) -> int:
        """
        This method translates the id of the cluster to the address in data region.

        :param boot_sector_state: the class with FS shared data
        :param id_: id of the cluster
        :returns: integer denoting the address of the cluster in the data region
        """
        data_address_: int = boot_sector_state.root_directory_start
        if not id_ == Cluster.ROOT_BLOCK_ID:
            # the first data cluster id is 2 (we have to subtract reserved cluster and cluster for root)
            data_address_ = boot_sector_state.sector_size * (id_ - 2) + boot_sector_state.data_region_start
        return data_address_

    def _compute_cluster_data_address(self) -> int:
        return self.compute_cluster_data_address(self.boot_sector_state, self.id)

    @property
    def fat_cluster_address(self) -> int:
        """Determines how many bits precede the first bit of the cluster in FAT"""
        return self._cluster_id_to_fat_position_in_bits(self.id)

    @property
    def real_cluster_address(self) -> int:
        """
        The property method computes the real address of the cluster in the FAT region. Result is simply
        address of the cluster in fat + fat table address.
        """
        cluster_address: int = self.boot_sector_state.fat_table_start_address + self.fat_cluster_address // 8
        return cluster_address

    def get_from_fat(self) -> int:
        """
        Calculating the value in the FAT block, that denotes if the block is full, empty, or chained to other block.

        For FAT12 is the block stored in one and half byte. If the order of the block is even the first byte and second
        half of the second byte belongs to the block. First half of the second byte and the third byte belongs to
        the second block.

        e.g. b'\xff\x0f\x00' stores two blocks. First of them is evenly ordered (index 0) and is set to 0xfff,
        that means full block that is final in chain of blocks
        and second block is set to 0x000 that means empty block.

        three bytes - AB XC YZ - stores two blocks - CAB YZX
        """
        address_: int = self.real_cluster_address
        bin_img_: bytearray = self.boot_sector_state.binary_image
        if self.boot_sector_state.fatfs_type == FAT12:
            if self.fat_cluster_address % 8 == 0:
                # even block
                return bin_img_[self.real_cluster_address] | ((bin_img_[self.real_cluster_address + 1] & 0x0F) << 8)
            # odd block
            return ((bin_img_[self.real_cluster_address] & 0xF0) >> 4) | (bin_img_[self.real_cluster_address + 1] << 4)
        if self.boot_sector_state.fatfs_type == FAT16:
            return int.from_bytes(bin_img_[address_:address_ + 2], byteorder='little')
        raise NotImplementedError('Only valid fatfs types are FAT12 and FAT16.')

    @property
    def is_empty(self) -> bool:
        """
        The property method takes a look into the binary array and checks if the bytes ordered by little endian
        and relates to the current cluster are all zeros (which denotes they are empty).
        """
        return self.get_from_fat() == 0x00

    def set_in_fat(self, value: int) -> None:
        """
        Sets cluster in FAT to certain value.
        Firstly, we split the target value into 3 half bytes (max value is 0xfff).
        Then we could encounter two situations:
        1. if the cluster index (indexed from zero) is even, we set the full byte computed by
        self.cluster_id_to_logical_position_in_bits and the second half of the consequent byte.
        Order of half bytes is 2, 1, 3.

        2. if the cluster index is odd, we set the first half of the computed byte and the full consequent byte.
        Order of half bytes is 1, 3, 2.
        """

        def _set_msb_half_byte(address: int, value_: int) -> None:
            """
            Sets 4 most significant bits (msb half-byte) of 'boot_sector_state.binary_image' at given
            'address' to 'value_' (size of variable 'value_' is half byte)

            If a byte contents is 0b11110000, the msb half-byte would be 0b1111
            """
            self.boot_sector_state.binary_image[address] &= 0x0f
            self.boot_sector_state.binary_image[address] |= value_ << 4

        def _set_lsb_half_byte(address: int, value_: int) -> None:
            """
            Sets 4 least significant bits (lsb half-byte) of 'boot_sector_state.binary_image' at given
            'address' to 'value_' (size of variable 'value_' is half byte)

            If a byte contents is 0b11110000, the lsb half-byte would be 0b0000
            """
            self.boot_sector_state.binary_image[address] &= 0xf0
            self.boot_sector_state.binary_image[address] |= value_

        # value must fit into number of bits of the fat (12, 16 or 32)
        assert value <= (1 << self.boot_sector_state.fatfs_type) - 1
        half_bytes = split_by_half_byte_12_bit_little_endian(value)
        bin_img_: bytearray = self.boot_sector_state.binary_image

        if self.boot_sector_state.fatfs_type == FAT12:
            assert merge_by_half_byte_12_bit_little_endian(*half_bytes) == value
            if self.fat_cluster_address % 8 == 0:
                # even block
                bin_img_[self.real_cluster_address] = build_byte(half_bytes[1], half_bytes[0])
                _set_lsb_half_byte(self.real_cluster_address + 1, half_bytes[2])
            elif self.fat_cluster_address % 8 != 0:
                # odd block
                _set_msb_half_byte(self.real_cluster_address, half_bytes[0])
                bin_img_[self.real_cluster_address + 1] = build_byte(half_bytes[2], half_bytes[1])
        elif self.boot_sector_state.fatfs_type == FAT16:
            bin_img_[self.real_cluster_address:self.real_cluster_address + 2] = Int16ul.build(value)
        assert self.get_from_fat() == value

    @property
    def is_root(self) -> bool:
        """
        The FAT12/FAT16 contains only one root directory,
        the root directory allocates the first cluster with the ID `ROOT_BLOCK_ID`.
        The method checks if the cluster belongs to the root directory.
        """
        return self.id == Cluster.ROOT_BLOCK_ID

    def allocate_cluster(self) -> None:
        """
        This method sets bits in FAT table to `allocated` and clean the corresponding sector(s)
        """
        self.set_in_fat(self.ALLOCATED_BLOCK_SWITCH[self.boot_sector_state.fatfs_type])

        cluster_start = self.cluster_data_address
        dir_size = get_dir_size(self.is_root, self.boot_sector_state)
        cluster_end = cluster_start + dir_size
        self.boot_sector_state.binary_image[cluster_start:cluster_end] = dir_size * EMPTY_BYTE
