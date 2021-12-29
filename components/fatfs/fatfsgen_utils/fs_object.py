# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
from typing import List, Optional, Tuple

from .entry import Entry
from .exceptions import FatalError, WriteDirectoryException
from .fat import FAT, Cluster
from .fatfs_state import FATFSState
from .utils import required_clusters_count, split_content_into_sectors, split_to_name_and_extension


class File:
    """
    The class File provides API to write into the files. It represents file in the FS.
    """
    ATTR_ARCHIVE = 0x20
    ENTITY_TYPE = ATTR_ARCHIVE

    def __init__(self, name: str, fat: FAT, fatfs_state: FATFSState, entry: Entry, extension: str = '') -> None:
        self.name = name
        self.extension = extension
        self.fatfs_state = fatfs_state
        self.fat = fat
        self.size = 0
        self._first_cluster = None
        self._entry = entry

    @property
    def entry(self) -> Entry:
        return self._entry

    @property
    def first_cluster(self) -> Optional[Cluster]:
        return self._first_cluster

    @first_cluster.setter
    def first_cluster(self, value: Cluster) -> None:
        self._first_cluster = value

    def name_equals(self, name: str, extension: str) -> bool:
        return self.name == name and self.extension == extension

    def write(self, content: str) -> None:
        self.entry.update_content_size(len(content))
        # we assume that the correct amount of clusters is allocated
        current_cluster = self._first_cluster
        for content_part in split_content_into_sectors(content, self.fatfs_state.sector_size):
            content_as_list = content_part
            if current_cluster is None:
                raise FatalError('No free space left!')

            address = current_cluster.cluster_data_address
            self.fatfs_state.binary_image[address: address + len(content_part)] = content_as_list
            current_cluster = current_cluster.next_cluster


class Directory:
    """
    The Directory class provides API to add files and directories into the directory
    and to find the file according to path and write it.
    """
    ATTR_DIRECTORY = 0x10
    ATTR_ARCHIVE = 0x20
    ENTITY_TYPE = ATTR_DIRECTORY

    def __init__(self,
                 name,
                 fat,
                 fatfs_state,
                 entry=None,
                 cluster=None,
                 size=None,
                 extension='',
                 parent=None):
        # type: (str, FAT, FATFSState, Optional[Entry], Cluster, Optional[int], str, Directory) -> None
        self.name = name
        self.fatfs_state = fatfs_state
        self.extension = extension

        self.fat = fat
        self.size = size or self.fatfs_state.sector_size

        # if directory is root its parent is itself
        self.parent: Directory = parent or self
        self._first_cluster = cluster

        # entries will be initialized after the cluster allocation
        self.entries: List[Entry] = []
        self.entities = []  # type: ignore
        self._entry = entry  # currently not in use (will use later for e.g. modification time, etc.)

    @property
    def is_root(self) -> bool:
        return self.parent is self

    @property
    def first_cluster(self) -> Cluster:
        return self._first_cluster

    @first_cluster.setter
    def first_cluster(self, value: Cluster) -> None:
        self._first_cluster = value

    def name_equals(self, name: str, extension: str) -> bool:
        return self.name == name and self.extension == extension

    def create_entries(self, cluster: Cluster) -> list:
        return [Entry(entry_id=i,
                      parent_dir_entries_address=cluster.cluster_data_address,
                      fatfs_state=self.fatfs_state)
                for i in range(self.size // self.fatfs_state.entry_size)]

    def init_directory(self) -> None:
        self.entries = self.create_entries(self._first_cluster)
        if not self.is_root:
            # the root directory doesn't contain link to itself nor the parent
            free_entry1 = self.find_free_entry() or self.chain_directory()
            free_entry1.allocate_entry(first_cluster_id=self.first_cluster.id,
                                       entity_name='.',
                                       entity_extension='',
                                       entity_type=self.ENTITY_TYPE)
            self.first_cluster = self._first_cluster
            free_entry2 = self.find_free_entry() or self.chain_directory()
            free_entry2.allocate_entry(first_cluster_id=self.parent.first_cluster.id,
                                       entity_name='..',
                                       entity_extension='',
                                       entity_type=self.parent.ENTITY_TYPE)
            self.parent.first_cluster = self.parent.first_cluster

    def lookup_entity(self, object_name: str, extension: str):  # type: ignore
        for entity in self.entities:
            if entity.name == object_name and entity.extension == extension:
                return entity
        return None

    def recursive_search(self, path_as_list, current_dir):  # type: ignore
        name, extension = split_to_name_and_extension(path_as_list[0])
        next_obj = current_dir.lookup_entity(name, extension)
        if next_obj is None:
            raise FileNotFoundError('No such file or directory!')
        if len(path_as_list) == 1 and next_obj.name_equals(name, extension):
            return next_obj
        return self.recursive_search(path_as_list[1:], next_obj)

    def find_free_entry(self) -> Optional[Entry]:
        for entry in self.entries:
            if entry.is_empty:
                return entry
        return None

    def _extend_directory(self) -> None:
        current = self.first_cluster
        while current.next_cluster is not None:
            current = current.next_cluster
        new_cluster = self.fat.find_free_cluster()
        current.set_in_fat(new_cluster.id)
        current.next_cluster = new_cluster
        self.entries += self.create_entries(new_cluster)

    def chain_directory(self) -> Entry:
        self._extend_directory()
        free_entry = self.find_free_entry()
        if free_entry is None:
            raise FatalError('No more space left!')
        return free_entry

    def allocate_object(self,
                        name,
                        entity_type,
                        path_from_root=None,
                        extension=''):
        # type: (str, int, Optional[List[str]], str) -> Tuple[Cluster, Entry, Directory]
        """
        Method finds the target directory in the path
        and allocates cluster (both the record in FAT and cluster in the data region)
        and entry in the specified directory
        """
        free_cluster = self.fat.find_free_cluster()
        target_dir = self if not path_from_root else self.recursive_search(path_from_root, self)
        free_entry = target_dir.find_free_entry() or target_dir.chain_directory()
        free_entry.allocate_entry(first_cluster_id=free_cluster.id,
                                  entity_name=name,
                                  entity_extension=extension,
                                  entity_type=entity_type)
        return free_cluster, free_entry, target_dir

    def new_file(self, name: str, extension: str, path_from_root: Optional[List[str]]) -> None:
        free_cluster, free_entry, target_dir = self.allocate_object(name=name,
                                                                    extension=extension,
                                                                    entity_type=Directory.ATTR_ARCHIVE,
                                                                    path_from_root=path_from_root)

        file = File(name, fat=self.fat, extension=extension, fatfs_state=self.fatfs_state, entry=free_entry)
        file.first_cluster = free_cluster
        target_dir.entities.append(file)

    def new_directory(self, name, parent, path_from_root):
        # type: (str, Directory, Optional[List[str]]) -> None
        free_cluster, free_entry, target_dir = self.allocate_object(name=name,
                                                                    entity_type=Directory.ATTR_DIRECTORY,
                                                                    path_from_root=path_from_root)

        directory = Directory(name=name, fat=self.fat, parent=parent, fatfs_state=self.fatfs_state, entry=free_entry)
        directory.first_cluster = free_cluster
        directory.init_directory()
        target_dir.entities.append(directory)

    def write_to_file(self, path: List[str], content: str) -> None:
        """
        Writes to file existing in the directory structure.

        :param path: path split into the list
        :param content: content as a string to be written into a file
        :returns: None
        :raises WriteDirectoryException: raised is the target object for writing is a directory
        """
        entity_to_write = self.recursive_search(path, self)
        if isinstance(entity_to_write, File):
            clusters_cnt = required_clusters_count(cluster_size=self.fatfs_state.sector_size, content=content)
            self.fat.allocate_chain(entity_to_write.first_cluster, clusters_cnt)
            entity_to_write.write(content)
        else:
            raise WriteDirectoryException(f'`{os.path.join(*path)}` is a directory!')
