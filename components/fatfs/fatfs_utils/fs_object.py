# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
from datetime import datetime
from typing import List, Optional, Tuple, Union

from .entry import Entry
from .exceptions import FatalError, WriteDirectoryException
from .fat import FAT, Cluster
from .fatfs_state import FATFSState
from .long_filename_utils import (build_lfn_full_name, build_lfn_unique_entry_name_order,
                                  get_required_lfn_entries_count, split_name_to_lfn_entries,
                                  split_name_to_lfn_entry_blocks)
from .utils import (DATETIME, INVALID_SFN_CHARS_PATTERN, MAX_EXT_SIZE, MAX_NAME_SIZE, FATDefaults,
                    build_lfn_short_entry_name, build_name, lfn_checksum, required_clusters_count,
                    split_content_into_sectors, split_to_name_and_extension)


class File:
    """
    The class File provides API to write into the files. It represents file in the FS.
    """
    ATTR_ARCHIVE: int = 0x20
    ENTITY_TYPE: int = ATTR_ARCHIVE

    def __init__(self, name: str, fat: FAT, fatfs_state: FATFSState, entry: Entry, extension: str = '') -> None:
        self.name: str = name
        self.extension: str = extension
        self.fatfs_state: FATFSState = fatfs_state
        self.fat: FAT = fat
        self.size: int = 0
        self._first_cluster: Optional[Cluster] = None
        self._entry: Entry = entry

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
        equals_: bool = build_name(name, extension) == build_name(self.name, self.extension)
        return equals_

    def write(self, content: bytes) -> None:
        self.entry.update_content_size(len(content))
        # we assume that the correct amount of clusters is allocated
        current_cluster = self._first_cluster
        for content_part in split_content_into_sectors(content, self.fatfs_state.boot_sector_state.sector_size):
            content_as_list = content_part
            if current_cluster is None:
                raise FatalError('No free space left!')

            address: int = current_cluster.cluster_data_address
            self.fatfs_state.binary_image[address: address + len(content_part)] = content_as_list
            current_cluster = current_cluster.next_cluster


class Directory:
    """
    The Directory class provides API to add files and directories into the directory
    and to find the file according to path and write it.
    """
    ATTR_DIRECTORY: int = 0x10
    ATTR_ARCHIVE: int = 0x20
    ENTITY_TYPE: int = ATTR_DIRECTORY

    CURRENT_DIRECTORY = '.'
    PARENT_DIRECTORY = '..'

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
        self.name: str = name
        self.fatfs_state: FATFSState = fatfs_state
        self.extension: str = extension

        self.fat: FAT = fat
        self.size: int = size or self.fatfs_state.boot_sector_state.sector_size

        # if directory is root its parent is itself
        self.parent: Directory = parent or self
        self._first_cluster: Cluster = cluster

        # entries will be initialized after the cluster allocation
        self.entries: List[Entry] = []
        self.entities: List[Union[File, Directory]] = []  # type: ignore
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
        equals_: bool = build_name(name, extension) == build_name(self.name, self.extension)
        return equals_

    @property
    def entries_count(self) -> int:
        entries_count_: int = self.size // FATDefaults.ENTRY_SIZE
        return entries_count_

    def create_entries(self, cluster: Cluster) -> List[Entry]:
        return [Entry(entry_id=i,
                      parent_dir_entries_address=cluster.cluster_data_address,
                      fatfs_state=self.fatfs_state)
                for i in range(self.entries_count)]

    def init_directory(self) -> None:
        self.entries = self.create_entries(self._first_cluster)

        # the root directory doesn't contain link to itself nor the parent
        if self.is_root:
            return
        # if the directory is not root we initialize the reference to itself and to the parent directory
        for dir_id, name_ in ((self, self.CURRENT_DIRECTORY), (self.parent, self.PARENT_DIRECTORY)):
            new_dir_: Entry = self.find_free_entry() or self.chain_directory()
            new_dir_.allocate_entry(first_cluster_id=dir_id.first_cluster.id,
                                    entity_name=name_,
                                    entity_extension='',
                                    entity_type=dir_id.ENTITY_TYPE)

    def lookup_entity(self, object_name: str, extension: str):  # type: ignore
        for entity in self.entities:
            if build_name(entity.name, entity.extension) == build_name(object_name, extension):
                return entity
        return None

    @staticmethod
    def _is_end_of_path(path_as_list: List[str]) -> bool:
        """
        :param path_as_list: path split into the list

        :returns: True if the file is the leaf of the directory tree, False otherwise
        The method is part of the base of recursion,
        determines if the path is target file or directory in the tree folder structure.
        """
        return len(path_as_list) == 1

    def recursive_search(self, path_as_list, current_dir):  # type: ignore
        name, extension = split_to_name_and_extension(path_as_list[0])
        next_obj = current_dir.lookup_entity(name, extension)
        if next_obj is None:
            raise FileNotFoundError('No such file or directory!')
        if self._is_end_of_path(path_as_list) and next_obj.name_equals(name, extension):
            return next_obj
        return self.recursive_search(path_as_list[1:], next_obj)

    def find_free_entry(self) -> Optional[Entry]:
        for entry in self.entries:
            if entry.is_empty:
                return entry
        return None

    def _extend_directory(self) -> None:
        current: Cluster = self.first_cluster
        while current.next_cluster is not None:
            current = current.next_cluster
        new_cluster: Cluster = self.fat.find_free_cluster()
        current.set_in_fat(new_cluster.id)
        assert current is not new_cluster
        current.next_cluster = new_cluster
        self.entries += self.create_entries(new_cluster)

    def chain_directory(self) -> Entry:
        """
        :returns: First free entry

        The method adds new Cluster to the Directory and returns first free entry.
        """
        self._extend_directory()
        free_entry: Entry = self.find_free_entry()
        if free_entry is None:
            raise FatalError('No more space left!')
        return free_entry

    @staticmethod
    def allocate_long_name_object(free_entry,
                                  name,
                                  extension,
                                  target_dir,
                                  free_cluster_id,
                                  entity_type,
                                  date,
                                  time):
        # type: (Entry, str, str, Directory, int, int, DATETIME, DATETIME) -> Entry
        lfn_full_name: str = build_lfn_full_name(name, extension)
        lfn_unique_entry_order: int = build_lfn_unique_entry_name_order(target_dir.entities, name)
        lfn_short_entry_name: str = build_lfn_short_entry_name(name, extension, lfn_unique_entry_order)
        checksum: int = lfn_checksum(lfn_short_entry_name)
        entries_count: int = get_required_lfn_entries_count(lfn_full_name)

        # entries in long file name entries chain starts with the last entry
        split_names_reversed = list(reversed(list(enumerate(split_name_to_lfn_entries(lfn_full_name, entries_count)))))
        for i, name_split_to_entry in split_names_reversed:
            order: int = i + 1
            blocks_: List[bytes] = split_name_to_lfn_entry_blocks(name_split_to_entry)
            lfn_names: List[bytes] = list(map(lambda x: x.lower(), blocks_))
            free_entry.allocate_entry(first_cluster_id=free_cluster_id,
                                      entity_name=name,
                                      entity_extension=extension,
                                      entity_type=entity_type,
                                      lfn_order=order,
                                      lfn_names=lfn_names,
                                      lfn_checksum_=checksum,
                                      lfn_is_last=order == entries_count)
            free_entry = target_dir.find_free_entry() or target_dir.chain_directory()
        free_entry.allocate_entry(first_cluster_id=free_cluster_id,
                                  entity_name=lfn_short_entry_name[:MAX_NAME_SIZE],
                                  entity_extension=lfn_short_entry_name[MAX_NAME_SIZE:],
                                  entity_type=entity_type,
                                  lfn_order=Entry.SHORT_ENTRY_LN,
                                  date=date,
                                  time=time)
        return free_entry

    @staticmethod
    def _is_valid_sfn(name: str, extension: str) -> bool:
        if INVALID_SFN_CHARS_PATTERN.search(name) or INVALID_SFN_CHARS_PATTERN.search(name):
            return False
        ret: bool = len(name) <= MAX_NAME_SIZE and len(extension) <= MAX_EXT_SIZE
        return ret

    def allocate_object(self,
                        name,
                        entity_type,
                        object_timestamp_,
                        path_from_root=None,
                        extension='',
                        is_empty=False):
        # type: (str, int, datetime, Optional[List[str]], str, bool) -> Tuple[Cluster, Entry, Directory]
        """
        Method finds the target directory in the path
        and allocates cluster (both the record in FAT and cluster in the data region)
        and entry in the specified directory
        """

        free_cluster: Optional[Cluster] = None
        free_cluster_id = 0x00
        if not is_empty:
            free_cluster = self.fat.find_free_cluster()
            free_cluster_id = free_cluster.id

        target_dir: Directory = self if not path_from_root else self.recursive_search(path_from_root, self)
        free_entry: Entry = target_dir.find_free_entry() or target_dir.chain_directory()

        fatfs_date_ = (object_timestamp_.year, object_timestamp_.month, object_timestamp_.day)
        fatfs_time_ = (object_timestamp_.hour, object_timestamp_.minute, object_timestamp_.second)

        if not self.fatfs_state.long_names_enabled or self._is_valid_sfn(name, extension):
            free_entry.allocate_entry(first_cluster_id=free_cluster_id,
                                      entity_name=name,
                                      entity_extension=extension,
                                      date=fatfs_date_,
                                      time=fatfs_time_,
                                      fits_short=True,
                                      entity_type=entity_type)
            return free_cluster, free_entry, target_dir
        return free_cluster, self.allocate_long_name_object(free_entry=free_entry,
                                                            name=name,
                                                            extension=extension,
                                                            target_dir=target_dir,
                                                            free_cluster_id=free_cluster_id,
                                                            entity_type=entity_type,
                                                            date=fatfs_date_,
                                                            time=fatfs_time_), target_dir

    def new_file(self,
                 name: str,
                 extension: str,
                 path_from_root: Optional[List[str]],
                 object_timestamp_: datetime,
                 is_empty: bool) -> None:
        free_cluster, free_entry, target_dir = self.allocate_object(name=name,
                                                                    extension=extension,
                                                                    entity_type=Directory.ATTR_ARCHIVE,
                                                                    path_from_root=path_from_root,
                                                                    object_timestamp_=object_timestamp_,
                                                                    is_empty=is_empty)

        file: File = File(name=name,
                          fat=self.fat,
                          extension=extension,
                          fatfs_state=self.fatfs_state,
                          entry=free_entry)
        file.first_cluster = free_cluster
        target_dir.entities.append(file)

    def new_directory(self, name, parent, path_from_root, object_timestamp_):
        # type: (str, Directory, Optional[List[str]], datetime) -> None
        free_cluster, free_entry, target_dir = self.allocate_object(name=name,
                                                                    entity_type=Directory.ATTR_DIRECTORY,
                                                                    path_from_root=path_from_root,
                                                                    object_timestamp_=object_timestamp_)

        directory: Directory = Directory(name=name,
                                         fat=self.fat,
                                         parent=parent,
                                         fatfs_state=self.fatfs_state,
                                         entry=free_entry)
        directory.first_cluster = free_cluster
        directory.init_directory()
        target_dir.entities.append(directory)

    def write_to_file(self, path: List[str], content: bytes) -> None:
        """
        Writes to file existing in the directory structure.

        :param path: path split into the list
        :param content: content as a string to be written into a file
        :returns: None
        :raises WriteDirectoryException: raised is the target object for writing is a directory
        """
        entity_to_write: Entry = self.recursive_search(path, self)
        if isinstance(entity_to_write, File):
            clusters_cnt: int = required_clusters_count(cluster_size=self.fatfs_state.boot_sector_state.sector_size,
                                                        content=content)
            self.fat.allocate_chain(entity_to_write.first_cluster, clusters_cnt)
            entity_to_write.write(content)
        else:
            raise WriteDirectoryException(f'`{os.path.join(*path)}` is a directory!')
