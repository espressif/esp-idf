# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Source extraction and C execution shared by lifecycle regressions."""

import pathlib
import re
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else pathlib.Path(__file__).resolve().parents[7]
BT = ROOT / 'components/bt'


def extract(path: pathlib.Path, names: list[tuple[str, str]]) -> str:
    source = path.read_text()
    result = []
    for name, result_type in names:
        match = re.search(r'\b' + name + r'\([^;]*?\n\{.*?^\}', source, re.M | re.S)
        assert match, f'Missing production function: {name}'
        result.append(result_type + ' ' + match[0])
    return '\n'.join(result)


def run_c(source: str) -> None:
    with tempfile.TemporaryDirectory(prefix='ble-log-stop-') as directory:
        c = pathlib.Path(directory) / 'test.c'
        c.write_text(source)
        exe = pathlib.Path(directory) / 'test'
        subprocess.run(
            [
                'cc',
                '-std=gnu11',
                '-Wall',
                '-Wextra',
                '-Werror',
                '-Wno-unused-function',
                '-fsanitize=undefined',
                str(c),
                '-o',
                str(exe),
            ],
            check=True,
        )
        subprocess.run([str(exe)], check=True, timeout=10)
