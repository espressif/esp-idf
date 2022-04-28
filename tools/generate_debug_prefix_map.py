# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# General Workflow:
# 1. read all components dirs, a semicolon-separated string (cmake list)
# 2. map the component dir with a unique prefix /COMPONENT_<NAME>_DIR
# 2. write the prefix mapping file to $BUILD_DIR/prefix_map_gdbinit
# 3. print the unique prefix out, a space-separated string, will be used by the build system to add compile options.

import argparse
import os
from typing import List


def component_name(component_dir: str) -> str:
    return '/COMPONENT_{}_DIR'.format(os.path.basename(component_dir).upper())


GDB_SUBSTITUTE_PATH_FMT = 'set substitute-path {} {}\n'


def write_gdbinit(build_dir: str, folders: List[str]) -> None:
    gdb_init_filepath = os.path.join(build_dir, 'prefix_map_gdbinit')

    with open(gdb_init_filepath, 'w') as fw:
        for folder in folders:
            fw.write(f'{GDB_SUBSTITUTE_PATH_FMT.format(component_name(folder), folder)}')


def main(build_dir: str, folders: List[str]) -> None:
    write_gdbinit(build_dir, folders)
    print(' '.join([component_name(folder) for folder in folders]), end='')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='print the debug-prefix-map and write to '
                                                 '$BUILD_DIR/prefix_map_gdbinit file')

    parser.add_argument('build_dir',
                        help='build dir')
    parser.add_argument('folders',
                        help='component folders, semicolon separated string')
    args = parser.parse_args()

    main(args.build_dir, args.folders.split(';'))
