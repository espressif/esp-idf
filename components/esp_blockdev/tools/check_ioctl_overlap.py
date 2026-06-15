#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Check ESP-Blockdev ioctl reservation overlaps.

Each registered file is preprocessed with ESP_BLOCKDEV_CHECK_CMD_OVERLAP defined.
The preprocessor expands reservation macros into sentinel marker calls, which are
then parsed and validated here.
"""

from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
from collections.abc import Iterable
from collections.abc import Iterator
from dataclasses import dataclass
from pathlib import Path

MARKER_TOKEN = 'ESP_BLOCKDEV_RESERVE_MARKER'


@dataclass(frozen=True)
class Reservation:
    component: str
    start: int
    end: int
    source_file: str


class ReservationError(Exception):
    pass


class ExpressionError(ReservationError):
    pass


class MarkerParseError(ReservationError):
    pass


def format_hex(value: int) -> str:
    return f'0x{value:02X}'


def preprocess_file(path: str | Path, compiler: str, include_dirs: Iterable[str]) -> str:
    cmd = [compiler, '-E', '-P', '-DESP_BLOCKDEV_CHECK_CMD_OVERLAP']
    for include_dir in include_dirs:
        cmd.extend(['-I', include_dir])
    cmd.append(str(path))
    proc = subprocess.run(cmd, capture_output=True, text=True)
    if proc.returncode != 0:
        raise ReservationError(f'preprocessor failed for {path}:\n{proc.stderr.strip() or proc.stdout.strip()}')
    return proc.stdout


def _iter_marker_payloads(text: str) -> Iterator[str]:
    idx = 0
    while True:
        hit = text.find(MARKER_TOKEN, idx)
        if hit < 0:
            return
        pos = hit + len(MARKER_TOKEN)
        while pos < len(text) and text[pos].isspace():
            pos += 1
        if pos >= len(text) or text[pos] != '(':
            raise MarkerParseError(f'found {MARKER_TOKEN} without argument list')

        depth = 0
        start = pos + 1
        pos = start
        while pos < len(text):
            ch = text[pos]
            if ch == '(':
                depth += 1
            elif ch == ')':
                if depth == 0:
                    yield text[start:pos]
                    idx = pos + 1
                    break
                depth -= 1
            pos += 1
        else:
            raise MarkerParseError(f'unterminated {MARKER_TOKEN} call')


def _split_top_level_commas(payload: str) -> list[str]:
    parts: list[str] = []
    depth = 0
    start = 0
    for idx, ch in enumerate(payload):
        if ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
            if depth < 0:
                raise MarkerParseError(f'unbalanced parentheses in {payload!r}')
        elif ch == ',' and depth == 0:
            parts.append(payload[start:idx].strip())
            start = idx + 1
    parts.append(payload[start:].strip())
    if depth != 0:
        raise MarkerParseError(f'unbalanced parentheses in {payload!r}')
    return parts


_TOKEN_RE = re.compile(r'\s*(0[xX][0-9a-fA-F]+|\d+|[()+-])')


class _ExprParser:
    def __init__(self, text: str) -> None:
        self.text = text
        self.pos = 0

    def parse(self) -> int:
        value = self._parse_expr()
        self._skip_ws()
        if self.pos != len(self.text):
            raise ExpressionError(f'unexpected trailing input: {self.text[self.pos :]!r}')
        return value

    def _skip_ws(self) -> None:
        while self.pos < len(self.text) and self.text[self.pos].isspace():
            self.pos += 1

    def _peek(self) -> str | None:
        self._skip_ws()
        if self.pos >= len(self.text):
            return None
        return self.text[self.pos]

    def _consume(self, ch: str) -> bool:
        self._skip_ws()
        if self.pos < len(self.text) and self.text[self.pos] == ch:
            self.pos += 1
            return True
        return False

    def _parse_expr(self) -> int:
        value = self._parse_factor()
        while True:
            if self._consume('+'):
                value += self._parse_factor()
            elif self._consume('-'):
                value -= self._parse_factor()
            else:
                return value

    def _parse_factor(self) -> int:
        if self._consume('+'):
            return self._parse_factor()
        if self._consume('-'):
            return -self._parse_factor()
        if self._consume('('):
            value = self._parse_expr()
            if not self._consume(')'):
                raise ExpressionError(f"missing ')' in {self.text!r}")
            return value
        return self._parse_int()

    def _parse_int(self) -> int:
        self._skip_ws()
        match = _TOKEN_RE.match(self.text, self.pos)
        if not match:
            raise ExpressionError(f'expected integer expression near: {self.text[self.pos :]!r}')
        token = match.group(1)
        self.pos = match.end()
        return int(token, 0)


def eval_int_expr(expr: str) -> int:
    return _ExprParser(expr).parse()


def extract_reservations(text: str, source_file: str) -> list[Reservation]:
    reservations: list[Reservation] = []
    for payload in _iter_marker_payloads(text):
        parts = _split_top_level_commas(payload)
        if len(parts) != 3:
            raise MarkerParseError(f'expected 3 arguments in {MARKER_TOKEN}, got {len(parts)}: {payload!r}')
        component, start_expr, end_expr = parts
        start = eval_int_expr(start_expr)
        end = eval_int_expr(end_expr)
        reservations.append(Reservation(component=component, start=start, end=end, source_file=source_file))
    return reservations


def validate_reservations(reservations: Iterable[Reservation]) -> list[str]:
    errors: list[str] = []
    items = list(reservations)

    # Deduplicate identical reservations (same component and range from different source files)
    seen: set[tuple[str, int, int]] = set()
    unique_items: list[Reservation] = []
    for item in items:
        key = (item.component, item.start, item.end)
        if key not in seen:
            seen.add(key)
            unique_items.append(item)
    items = unique_items

    for item in items:
        if item.start > item.end:
            errors.append(
                f'  {item.component}: start {format_hex(item.start)} > end'
                f' {format_hex(item.end)} from {item.source_file}'
            )
        if not 0 <= item.start <= 0xFF:
            errors.append(
                f'  {item.component}: start {format_hex(item.start)} out of range (0x00..0xFF) from {item.source_file}'
            )
        if not 0 <= item.end <= 0xFF:
            errors.append(
                f'  {item.component}: end {format_hex(item.end)} out of range (0x00..0xFF) from {item.source_file}'
            )

    sorted_items = sorted(items, key=lambda r: (r.start, r.end, r.component, r.source_file))
    if not sorted_items:
        return errors

    active = sorted_items[0]
    for current in sorted_items[1:]:
        if current.start <= active.end:
            errors.append('ioctl reservation overlap:')
            width = max(len(active.component), len(current.component))
            pad_a = ' ' * (width - len(active.component) + 1)
            pad_c = ' ' * (width - len(current.component) + 1)
            errors.append(
                f'  {active.component}:{pad_a}'
                f'{format_hex(active.start)}..{format_hex(active.end)}'
                f' from {active.source_file}'
            )
            errors.append(
                f'  {current.component}:{pad_c}'
                f'{format_hex(current.start)}..{format_hex(current.end)}'
                f' from {current.source_file}'
            )
            if current.end > active.end:
                active = current
        elif current.end > active.end:
            active = current
    return errors


def check_files(files: Iterable[str | Path], compiler: str, include_dirs: Iterable[str]) -> list[Reservation]:
    reservations: list[Reservation] = []
    for file_path in files:
        text = preprocess_file(file_path, compiler=compiler, include_dirs=include_dirs)
        reservations.extend(extract_reservations(text, source_file=str(file_path)))
    errors = validate_reservations(reservations)
    if errors:
        raise ReservationError('\n'.join(errors))
    return reservations


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--compiler',
        default=os.environ.get('CC', 'gcc'),
        help='C compiler executable used for preprocessing (default: $CC or gcc)',
    )
    parser.add_argument(
        '--include-dir', action='append', default=[], help='Additional include directory for preprocessing (repeatable)'
    )
    parser.add_argument('--files', nargs='+', required=True, help='Reservation definition files to check for overlaps')
    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_arg_parser()
    args = parser.parse_args(argv)
    try:
        check_files(args.files, compiler=args.compiler, include_dirs=args.include_dir)
    except ReservationError as exc:
        print(str(exc), file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
