# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from typing import List

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
        self._first_free_cluster_id = 0
        self.boot_sector_state = boot_sector_state
        self.clusters: List[Cluster] = [Cluster(cluster_id=i,
                                                boot_sector_state=self.boot_sector_state,
                                                init_=init_) for i in range(self.boot_sector_state.clusters)]
        if init_:
            self.allocate_root_dir()

    def get_cluster_value(self, cluster_id_: int) -> int:
        fat_cluster_value_: int = self.clusters[cluster_id_].get_from_fat()
        return fat_cluster_value_

    def is_cluster_last(self, cluster_id_: int) -> bool:
        value_ = self.get_cluster_value(cluster_id_)
        is_cluster_last_: bool = value_ == (1 << self.boot_sector_state.fatfs_type) - 1
        return is_cluster_last_

    def chain_content(self, cluster_id_: int) -> bytearray:
        bin_im: bytearray = self.boot_sector_state.binary_image
        if self.is_cluster_last(cluster_id_):
            data_address_ = Cluster.compute_cluster_data_address(self.boot_sector_state, cluster_id_)
            content_: bytearray = bin_im[data_address_: data_address_ + self.boot_sector_state.sector_size]
            return content_
        fat_value_: int = self.get_cluster_value(cluster_id_)
        data_address_ = Cluster.compute_cluster_data_address(self.boot_sector_state, cluster_id_)
        content_ = bin_im[data_address_: data_address_ + self.boot_sector_state.sector_size]

        while not self.is_cluster_last(cluster_id_):
            cluster_id_ = fat_value_
            fat_value_ = self.get_cluster_value(cluster_id_)
            data_address_ = Cluster.compute_cluster_data_address(self.boot_sector_state, cluster_id_)
            content_ += bin_im[data_address_: data_address_ + self.boot_sector_state.sector_size]
        return content_

    def find_free_cluster(self) -> Cluster:
        # finds first empty cluster and allocates it
        for cluster_id, cluster in enumerate(self.clusters[self._first_free_cluster_id:],
                                             start=self._first_free_cluster_id):
            if cluster.is_empty:
                cluster.allocate_cluster()
                self._first_free_cluster_id = cluster_id
                return cluster
        raise NoFreeClusterException('No free cluster available!')

    def allocate_chain(self, first_cluster: Cluster, size: int) -> None:
        current = first_cluster
        for _ in range(size - 1):
            free_cluster = self.find_free_cluster()
            current.next_cluster = free_cluster
            current.set_in_fat(free_cluster.id)
            current = free_cluster
