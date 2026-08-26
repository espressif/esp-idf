# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import fnmatch
import glob
import os
from typing import List, Optional, Tuple, Union


class Snapshot:
    """
    Helper class for working with build snapshots.
    A snapshot is a set of files along with their modification timestamps.
    """
    def __init__(self, files: List[str]) -> None:
        """Create a snapshot of the given list of files, recording their mtimes"""
        self.info: List[Tuple[str, int]] = []
        for file in sorted(files):
            st = os.stat(file)
            self.info.append((file, st.st_mtime_ns))

    def assert_same(self, other: 'Snapshot') -> None:
        """Assert that this snapshot has the same files and mtimes as the other snapshot"""
        for my_info, other_info in zip(self.info, other.info):
            assert my_info[0] == other_info[0], f'Snapshots mismatch, expected file {my_info[0]}, found {other_info[0]}'
            assert my_info[1] == other_info[1], f'File {my_info[0]} timestamp has changed! diff={(other_info[1]-my_info[1])/10**9}'

    def assert_different(self, other: 'Snapshot') -> None:
        """Assert that this snapshot has the same files as the other snapshot, but all mtimes are different"""
        for my_info, other_info in zip(self.info, other.info):
            assert my_info[0] == other_info[0], f'Snapshots mismatch, expected file {my_info[0]}, found {other_info[0]}'
            assert my_info[1] != other_info[1], f'File {my_info[0]} timestamp has not changed! ({my_info[1]})'


def get_snapshot(glob_patterns: Union[str, List[str]],
                 exclude_patterns: Optional[Union[str, List[str]]] = None) -> Snapshot:
    """Return a snapshot including the files matched by glob_patterns, and excluding those matched by exclude_patterns"""
    if isinstance(glob_patterns, str):
        glob_patterns = [glob_patterns]

    if isinstance(exclude_patterns, str):
        exclude_patterns_list = [exclude_patterns]
    elif exclude_patterns is None:
        exclude_patterns_list = []
    elif isinstance(exclude_patterns, list):
        exclude_patterns_list = exclude_patterns
    else:
        raise ValueError(f'Unexpcted type of exclude_patterns: ({type(exclude_patterns)}')

    # whether the path found by glob.glob should be excluded?
    def should_exclude(filename: str) -> bool:
        return os.path.isdir(filename) or \
            any((fnmatch.fnmatch(filename, pattern) for pattern in exclude_patterns_list))

    files_to_snapshot: List[str] = []
    for pattern in glob_patterns:
        found = filter(lambda f: not should_exclude(f),
                       glob.glob(pattern, recursive=True))
        if not found:
            raise RuntimeError(f'failed to match any files with pattern {pattern}')
        files_to_snapshot.extend(found)
    return Snapshot(files_to_snapshot)
