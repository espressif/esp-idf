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

    def find_free_cluster(self) -> Cluster:
        # finds first empty cluster and allocates it
        for cluster in self.clusters:
            if cluster.is_empty:
                cluster.allocate_cluster()
                return cluster
        raise NoFreeClusterException('No free cluster available!')

    def allocate_chain(self, first_cluster: Cluster, size: int) -> None:
        current = first_cluster
        for _ in range(size - 1):
            free_cluster = self.find_free_cluster()
            current.next_cluster = free_cluster
            current.set_in_fat(free_cluster.id)
            current = free_cluster
