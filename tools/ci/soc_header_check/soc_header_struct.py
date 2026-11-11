# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Bit-field width checker for regtool-generated ``*_struct.h`` headers.

Register descriptions are ``typedef union { struct { uint32_t f:n; ... };
uint32_t val; } name;``. This checker only verifies that each struct whose
members include bit-fields sums to exactly 32 bits — the layout error CSV
generation actually produces. Overlay unions, device structs, and other
hand-written shapes are ignored.
"""

import re
from dataclasses import dataclass
from pathlib import Path

_BLOCK_COMMENT = re.compile(r'/\*.*?\*/', re.S)
_LINE_COMMENT = re.compile(r'//[^\n]*')
_IDENT = r'[A-Za-z_]\w*'

_BEGIN = re.compile(rf'^\s*(?:typedef\b\s*)?(?:volatile\b\s*)?(?P<kind>struct|union)\b(?:\s+{_IDENT})?\s*\{{?')
_END = re.compile(r'^\s*\}\s*[\w\[\]*]*\s*;')
_MEMBER = re.compile(
    rf'^\s*(?:volatile\b\s*)?{_IDENT}\s*(?:\*\s*)*'
    rf'{_IDENT}\s*(?:\[[^\]]*\]\s*)*(?::\s*(?P<bits>\d+))?\s*;\s*$'
)


@dataclass(frozen=True)
class SourceLine:
    number: int
    text: str


def logical_lines(source: str) -> list[SourceLine]:
    """Strip comments, join continuations, and keep original line numbers."""

    def blank(match: re.Match) -> str:
        return re.sub(r'[^\n]', ' ', match.group(0))

    source = _BLOCK_COMMENT.sub(blank, source)
    source = _LINE_COMMENT.sub('', source)
    lines: list[SourceLine] = []
    pending = ''
    start = 1
    for number, raw in enumerate(source.splitlines(), start=1):
        if not pending:
            start = number
        stripped = raw.rstrip()
        if stripped.endswith('\\'):
            pending += stripped[:-1] + ' '
            continue
        text = (pending + raw).rstrip()
        pending = ''
        if text.strip():
            lines.append(SourceLine(start, text))
    if pending.strip():
        lines.append(SourceLine(start, pending.rstrip()))
    return lines


@dataclass
class Block:
    kind: str
    bitfield_bits: int = 0


def _bitfield_bits(text: str) -> int:
    """Total bit-field width of semicolon-separated members on one line."""
    bits = 0
    for chunk in text.split(';'):
        member = _MEMBER.match(chunk.strip() + ';')
        if member and member.group('bits') is not None:
            bits += int(member.group('bits'))
    return bits


def check_struct_content(source: str) -> list[tuple[str, int | None, str]]:
    """Return errors for structs whose bit-fields do not sum to 32."""
    diagnostics: list[tuple[str, int | None, str]] = []
    blocks: list[Block] = []

    for line in logical_lines(source):
        text = line.text

        begin = _BEGIN.match(text)
        if begin:
            rest = text[begin.end() :]
            if '}' in rest:
                # Single-line struct/union: it opens and closes on this line,
                # so there is no separate _END line to pop the block.
                bits = _bitfield_bits(rest.replace('}', ''))
                if begin.group('kind') == 'struct' and bits not in (0, 32):
                    diagnostics.append(('ERROR', line.number, f'bit-field sum is {bits}, expected 32'))
            else:
                blocks.append(Block(kind=begin.group('kind')))
            continue

        if _END.match(text):
            if not blocks:
                continue
            block = blocks.pop()
            if block.kind == 'struct' and block.bitfield_bits not in (0, 32):
                diagnostics.append(('ERROR', line.number, f'bit-field sum is {block.bitfield_bits}, expected 32'))
            continue

        if not blocks or text.strip() == '{':
            continue

        member = _MEMBER.match(text)
        if member and member.group('bits') is not None:
            blocks[-1].bitfield_bits += int(member.group('bits'))

    return diagnostics


def check_file(path: Path) -> list[tuple[str, int | None, str]]:
    return check_struct_content(path.read_text(encoding='utf-8', errors='replace'))
