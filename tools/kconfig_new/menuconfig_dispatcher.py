#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import subprocess
import sys
from pathlib import Path


def _read_entries(entries_file: Path) -> list[tuple[str, str]]:
    entries: list[tuple[str, str]] = []
    for line in entries_file.read_text().splitlines():
        line = line.strip()
        if not line:
            continue
        name, _, target = line.partition('|')
        entries.append((name, target))
    return entries


def _prompt(entries: list[tuple[str, str]]) -> str | None:
    width = max(len(name) for name, _ in entries)
    print('\nMultiple menuconfig configurations available:\n')
    for i, (name, target) in enumerate(entries, 1):
        print(f'  [{i}] {name:<{width}}   (idf.py {target})')
    print()

    while True:
        try:
            choice = input(f'Select [1-{len(entries)}, q to quit]: ').strip().lower()
        except EOFError:
            return None
        if choice in ('q', 'quit', ''):
            return None
        if choice.isdigit():
            idx = int(choice)
            if 1 <= idx <= len(entries):
                return entries[idx - 1][1]
        print('Invalid choice.')


def main() -> int:
    if len(sys.argv) != 3:
        print(f'Usage: {sys.argv[0]} <entries_file> <build_dir>', file=sys.stderr)
        return 2

    entries_file = Path(sys.argv[1])
    build_dir = sys.argv[2]

    entries = _read_entries(entries_file)
    if not entries:
        print(f'No menuconfig entries registered in {entries_file}', file=sys.stderr)
        return 1

    target = _prompt(entries)
    if target is None:
        return 0

    return subprocess.run(['cmake', '--build', build_dir, '--target', target]).returncode


if __name__ == '__main__':
    sys.exit(main())
