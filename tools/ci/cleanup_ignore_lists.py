#!/usr/bin/env python

# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import glob
import os
import typing as t
from pathlib import Path

from idf_ci_utils import IDF_PATH


def print_list(_list: t.Iterable[t.Any], title: t.Optional[str] = None) -> None:
    if not _list:
        return

    if title:
        print(title)

    for i in _list:
        print('- ', str(i))


if __name__ == '__main__':
    os.chdir(IDF_PATH)
    ignore_lists: t.Set[Path] = set()
    ignore_lists.update(Path('tools', 'ci').glob('**/*.txt'))
    ignore_lists.remove(Path('tools', 'ci', 'ignore_build_warnings.txt'))
    ignore_lists.remove(Path('tools', 'ci', 'check_ldgen_mapping_exceptions.txt'))

    print_list(ignore_lists, 'Ignore lists:')

    updated_files = []
    for f in ignore_lists:
        print('Checking file:', f)

        updated = False
        lines = []
        with open(f) as fr:
            for line in map(str.strip, fr.readlines()):
                if line.startswith('#'):
                    lines.append(line)
                    continue

                if not line:
                    lines.append(line)
                    continue

                glob_pattern = line
                if not list(glob.glob(glob_pattern, recursive=True)):
                    print('  - No match:', glob_pattern)
                    updated = True
                else:
                    lines.append(glob_pattern)
        lines.append('')

        if updated:
            updated_files.append(f)
            with open(f, 'w') as fw:
                fw.write('\n'.join(lines))

    if updated_files:
        print_list(updated_files, 'Updated files:')
        exit(1)
