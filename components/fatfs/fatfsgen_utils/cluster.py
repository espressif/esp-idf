# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from typing import Dict, Optional

from construct import Int16ul

from .fatfs_state import FATFSState
from .utils import (FAT12, FAT16, build_byte, clean_first_half_byte, clean_second_half_byte,
                    split_by_half_byte_12_bit_little_endian)


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
                 fatfs_state: FATFSState,
                 is_empty: bool = True) -> None:

        self.id: int = cluster_id
        self.fatfs_state: FATFSState = fatfs_state

        self._next_cluster = None  # type: Optional[Cluster]
        if self.id == Cluster.RESERVED_BLOCK_ID:
            self.is_empty = False
            self.set_in_fat(self.INITIAL_BLOCK_SWITCH[self.fatfs_state.fatfs_type])
            return

        self.cluster_data_address: int = self._compute_cluster_data_address()
        self.is_empty = is_empty

        assert self.cluster_data_address or self.is_empty

    @property
    def next_cluster(self):  # type: () -> Optional[Cluster]
        return self._next_cluster

    @next_cluster.setter
    def next_cluster(self, value):  # type: (Optional[Cluster]) -> None
        self._next_cluster = value

    def _cluster_id_to_logical_position_in_bits(self, _id: int) -> int:
        # computes address of the cluster in fat table
        return self.fatfs_state.fatfs_type * _id  # type: ignore

    def _compute_cluster_data_address(self) -> int:
        if self.id == Cluster.ROOT_BLOCK_ID:
            return self.fatfs_state.root_directory_start  # type: ignore
        # the first data cluster id is 2 (we have to subtract reserved cluster and cluster for root)
        return self.fatfs_state.sector_size * (self.id - 2) + self.fatfs_state.data_region_start  # type: ignore

    def _set_first_half_byte(self, address: int, value: int) -> None:
        clean_second_half_byte(self.fatfs_state.binary_image, address)
        self.fatfs_state.binary_image[address] |= value << 4

    def _set_second_half_byte(self, address: int, value: int) -> None:
        clean_first_half_byte(self.fatfs_state.binary_image, address)
        self.fatfs_state.binary_image[address] |= value

    @property
    def fat_cluster_address(self) -> int:
        """Determines how many bits precede the first bit of the cluster in FAT"""
        return self._cluster_id_to_logical_position_in_bits(self.id)

    @property
    def real_cluster_address(self) -> int:
        return self.fatfs_state.start_address + self.fat_cluster_address // 8  # type: ignore

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

        # value must fit into number of bits of the fat (12, 16 or 32)
        assert value <= (1 << self.fatfs_state.fatfs_type) - 1
        half_bytes = split_by_half_byte_12_bit_little_endian(value)

        if self.fatfs_state.fatfs_type == FAT12:
            if self.fat_cluster_address % 8 == 0:
                self.fatfs_state.binary_image[self.real_cluster_address] = build_byte(half_bytes[1], half_bytes[0])
                self._set_second_half_byte(self.real_cluster_address + 1, half_bytes[2])
            elif self.fat_cluster_address % 8 != 0:
                self._set_first_half_byte(self.real_cluster_address, half_bytes[0])
                self.fatfs_state.binary_image[self.real_cluster_address + 1] = build_byte(half_bytes[2], half_bytes[1])
        elif self.fatfs_state.fatfs_type == FAT16:
            self.fatfs_state.binary_image[self.real_cluster_address:self.real_cluster_address + 2] = Int16ul.build(
                value)

    @property
    def is_root(self) -> bool:
        return self.id == Cluster.ROOT_BLOCK_ID

    def allocate_cluster(self) -> None:
        """
        This method sets bits in FAT table to `allocated` and clean the corresponding sector(s)
        """
        self.is_empty = False
        self.set_in_fat(self.ALLOCATED_BLOCK_SWITCH[self.fatfs_state.fatfs_type])

        cluster_start = self.cluster_data_address
        dir_size = self.fatfs_state.get_dir_size(self.is_root)
        cluster_end = cluster_start + dir_size
        self.fatfs_state.binary_image[cluster_start:cluster_end] = dir_size * b'\x00'
