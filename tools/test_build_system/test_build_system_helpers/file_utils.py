# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import re
import typing as t
from pathlib import Path
from pathlib import WindowsPath


def append_to_file(filename: t.Union[str, Path], what: str) -> None:
    with open(filename, 'a', encoding='utf-8') as f:
        f.write(what)


def replace_in_file(filename: t.Union[str, Path], search: str, replace: str) -> None:
    with open(filename, 'r', encoding='utf-8') as f:
        data = f.read()
    result = data.replace(search, replace)
    with open(filename, 'w', encoding='utf-8') as f:
        f.write(result)


def file_contains(filename: t.Union[str, Path], what: t.Union[t.Union[str, Path], t.Pattern]) -> bool:
    """
    Returns true if file contains required object
    :param filename: path to file where lookup is executed
    :param what: searched substring or regex object
    """
    with open(filename, 'r', encoding='utf-8') as f:
        data = f.read()
        if isinstance(what, t.Pattern):
            return re.search(what, data) is not None
        else:
            what_str = str(what)
            # In case of windows path, try both single-slash `\` and double-slash '\\' paths
            if isinstance(what, WindowsPath):
                what_double_slash = what_str.replace('\\', '\\\\')
                return what_str in data or what_double_slash in data

            return what_str in data


def bin_file_contains(filename: t.Union[str, Path], what: bytearray) -> bool:
    """
    Returns true if the binary file contains the given string
    :param filename: path to file where lookup is executed
    :param what: searched bytes
    """
    with open(filename, 'rb') as f:
        data = f.read()
        return data.find(what) != -1


def bin_files_differ(filename1: t.Union[str, Path], filename2: t.Union[str, Path]) -> bool:
    """
    Checks if two binary files are different
    :param filename1: path to first file
    :param filename2: path to second file
    :return: True if files have different content, False if the content is the same
    """
    with open(filename1, 'rb') as f1:
        data1 = f1.read()
    with open(filename2, 'rb') as f2:
        data2 = f2.read()
    return data1 != data2
