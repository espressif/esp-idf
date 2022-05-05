# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from .cluster import Cluster
from .exceptions import NoFreeClusterException
from .fatfs_state import FATFSState


class FAT:
    """
    The FAT represents the FAT region in file system. It is responsible for storing clusters
    and chaining them in case we need to extend file or directory to more clusters.
    """
    def __init__(self,
                 fatfs_state: FATFSState,
                 reserved_sectors_cnt: int) -> None:
        self.fatfs_state = fatfs_state
        self.reserved_sectors_cnt = reserved_sectors_cnt

        self.clusters = [Cluster(cluster_id=i, fatfs_state=self.fatfs_state) for i in
                         range(1, self.fatfs_state.clusters)]

        # update root directory record
        self.clusters[0].allocate_cluster()
        # add first reserved cluster
        self.clusters = [Cluster(cluster_id=Cluster.RESERVED_BLOCK_ID, fatfs_state=self.fatfs_state)] + self.clusters

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
