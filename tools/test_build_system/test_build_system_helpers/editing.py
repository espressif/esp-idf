# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import typing
from pathlib import Path


def append_to_file(filename: typing.Union[str, Path], what: str) -> None:
    with open(filename, 'a', encoding='utf-8') as f:
        f.write(what)


def replace_in_file(filename: typing.Union[str, Path], search: str, replace: str) -> None:
    with open(filename, 'r', encoding='utf-8') as f:
        data = f.read()
    result = data.replace(search, replace)
    with open(filename, 'w', encoding='utf-8') as f:
        f.write(result)
