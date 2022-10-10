# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from typing import List, Optional

from .cluster import Cluster
from .exceptions import NoFreeClusterException
from .fatfs_state import BootSectorState


class FAT:
    """
    The FAT represents the FAT region in file system. It is responsible for storing clusters
    and chaining them in case we need to extend file or directory to more clusters.
    """

    def allocate_root_dir(self) -> None:
        """
        The root directory is implicitly created with the FatFS,
        its block is on the index 1 (second index) and is allocated implicitly.
        """
        self.clusters[Cluster.ROOT_BLOCK_ID].allocate_cluster()

    def __init__(self, boot_sector_state: BootSectorState, init_: bool) -> None:
        self._first_free_cluster_id = 1
        self.boot_sector_state = boot_sector_state
        self.clusters: List[Cluster] = [Cluster(cluster_id=i,
                                                boot_sector_state=self.boot_sector_state,
                                                init_=init_) for i in range(self.boot_sector_state.clusters)]
        if init_:
            self.allocate_root_dir()

    def get_cluster_value(self, cluster_id_: int) -> int:
        """
        The method retrieves the values of the FAT memory block.
        E.g. in case of FAT12:
        00000000: F8 FF FF 55 05 00 00 00 00 00 00 00 00 00 00 00

        The reserved value is 0xFF8, the value of first cluster if 0xFFF, thus is last in chain,
        and the value of the second cluster is 0x555, so refers to the cluster number 0x555.
        """
        fat_cluster_value_: int = self.clusters[cluster_id_].get_from_fat()
        return fat_cluster_value_

    def is_cluster_last(self, cluster_id_: int) -> bool:
        """
        Checks if the cluster is last in its cluster chain. If the value of the cluster is
        0xFFF for FAT12, 0xFFFF for FAT16 or 0xFFFFFFFF for FAT32, the cluster is the last.
        """
        value_ = self.get_cluster_value(cluster_id_)
        is_cluster_last_: bool = value_ == (1 << self.boot_sector_state.fatfs_type) - 1
        return is_cluster_last_

    def get_chained_content(self, cluster_id_: int, size: Optional[int] = None) -> bytearray:
        """
        The purpose of the method is retrieving the content from chain of clusters when the FAT FS partition
        is analyzed. The file entry provides the reference to the first cluster, this method
        traverses linked list of clusters and append partial results to the content.
        """
        binary_image: bytearray = self.boot_sector_state.binary_image

        data_address_ = Cluster.compute_cluster_data_address(self.boot_sector_state, cluster_id_)
        content_ = binary_image[data_address_: data_address_ + self.boot_sector_state.sector_size]

        while not self.is_cluster_last(cluster_id_):
            cluster_id_ = self.get_cluster_value(cluster_id_)
            data_address_ = Cluster.compute_cluster_data_address(self.boot_sector_state, cluster_id_)
            content_ += binary_image[data_address_: data_address_ + self.boot_sector_state.sector_size]
        # the size is None if the object is directory
        if size is None:
            return content_
        return content_[:size]

    def find_free_cluster(self) -> Cluster:
        """
        Returns the first free cluster and increments value of `self._first_free_cluster_id`.
        The method works only in context of creating a partition from scratch.
        In situations where the clusters are allocated and freed during the run of the program,
        might the method cause `Out of space` error despite there would be free clusters.
        """

        if self._first_free_cluster_id + 1 >= len(self.clusters):
            raise NoFreeClusterException('No free cluster available!')
        cluster = self.clusters[self._first_free_cluster_id + 1]
        if not cluster.is_empty:
            raise NoFreeClusterException('No free cluster available!')
        cluster.allocate_cluster()
        self._first_free_cluster_id += 1
        return cluster

    def allocate_chain(self, first_cluster: Cluster, size: int) -> None:
        """
        Allocates the linked list of clusters needed for the given file or directory.
        """
        current = first_cluster
        for _ in range(size - 1):
            free_cluster = self.find_free_cluster()
            current.next_cluster = free_cluster
            current.set_in_fat(free_cluster.id)
            current = free_cluster
