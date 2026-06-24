#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Sort yaml file

Exit non-zero if any file is modified
"""

import io
import os
import sys
import tempfile
import unittest

from ruamel.yaml import YAML
from ruamel.yaml import CommentedMap

COMMON_COMPONENTS_ANCHOR: str = '- *common_components'
TEMP_ANCHOR: str = '- __temp_common_components__'


def replace_common_components(text: str) -> str:
    """Temporarily replace the common_components anchor so YAML can be processed."""
    return text.replace(COMMON_COMPONENTS_ANCHOR, TEMP_ANCHOR)


def revert_common_components(text: str) -> str:
    """Restore the original common_components anchor after processing."""
    return text.replace(TEMP_ANCHOR, COMMON_COMPONENTS_ANCHOR)


def sort_yaml(f: str) -> int:
    yaml = YAML()
    yaml.indent(mapping=2, sequence=4, offset=2)
    yaml.width = 4096  # avoid wrap lines``

    exit_code = 0
    with open(f) as fr:
        file_s = replace_common_components(fr.read())

        try:
            file_d: CommentedMap = yaml.load(io.StringIO(file_s))
        except Exception as e:
            print(f'Failed to load yaml file {f}: {e}')
            return 1

    # sort dict keys
    sorted_yaml = CommentedMap(dict(sorted(file_d.items())))
    file_d.copy_attributes(sorted_yaml)

    # sort item
    for k, v in sorted_yaml.items():
        if isinstance(v, list):
            sorted_yaml[k].sort()

    with io.StringIO() as s:
        yaml.dump(sorted_yaml, s)

        string = s.getvalue()
        if string != file_s:
            with open(f, 'w') as fw:
                fw.write(revert_common_components(string))
            print(f'Sorted yaml file {f}. Please take a look. sometimes the format is a bit messy')
            exit_code = 1

    return exit_code


class TestSortYaml(unittest.TestCase):
    def test_sort_yaml(self) -> None:
        _, test_yaml = tempfile.mkstemp()
        with open(test_yaml, 'w') as fw:
            fw.write(
                """no_runner: []
no_env_marker:
  - 1
  - 3 # foo
  - 2 # bar"""
            )

        sort_yaml(fw.name)

        try:
            with open(test_yaml) as fr:
                self.assertEqual(
                    fr.read(),
                    """no_env_marker:
    - 1
    - 2 # bard
    - 3 # foo
    no_runner: []""",
                )
        except AssertionError:
            print(f'Please check the sorted yaml file {test_yaml}')
        else:
            os.remove(test_yaml)


if __name__ == '__main__':
    ret = 0
    for _f in sys.argv[1:]:
        exit_code = sort_yaml(_f)
        if exit_code != 0 and ret == 0:
            ret = exit_code

    sys.exit(ret)
