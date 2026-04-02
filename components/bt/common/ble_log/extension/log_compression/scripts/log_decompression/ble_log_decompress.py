# SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# The current project needs to support environments before Python 3.9,
# and UP007 will prohibit the use of Tuple, Union, etc.
# ruff: noqa: UP007
# The current project needs to support environments before Python 3.9,
# Therefore, it is necessary to prohibit UP006 from automatically
# changing the annotation type
# ruff: noqa: UP006
# ruff: noqa: UP035
"""
ESP BLE compressed log decompressor for host (Bluedroid or NimBLE) over BLE Log LBM.

Reads UART lines that contain space-separated hex dumps of 1024‑byte SPI/UART DMA
buffers, walks them strictly frame‑by‑frame using the BLE Log frame header,
decodes host compressed logs (source id 0) using the same layout as the C implementation,
and prints human‑readable log lines.
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any
from typing import Dict
from typing import Iterable
from typing import List
from typing import Literal
from typing import Optional
from typing import Tuple

try:
    import serial  # type: ignore
except ImportError:  # pragma: no cover - dependency notice only
    serial = None  # type: ignore

_current_dir = Path(__file__).resolve().parent
_parent_dir = _current_dir.parent

if str(_parent_dir) not in sys.path:
    sys.path.insert(0, str(_parent_dir))

try:
    # Reuse Espressif's format tokenizer and integer formatting helper.
    from c_format_parse import parse_format_string  # type: ignore

    _HAVE_C_FORMAT_PARSE = True
except Exception:  # pragma: no cover - optional dependency
    _HAVE_C_FORMAT_PARSE = False

# ble_log_frame_head_t.frame_meta low byte
BLE_LOG_SRC_ENCODE = 7
BLE_LOG_SRC_MAX = 7

# log_compression/utils.h
BLE_COMPRESSED_LOG_OUT_SOURCE_HOST = 0
BLE_COMPRESSED_LOG_OUT_SOURCE_MESH = 1
BLE_COMPRESSED_LOG_OUT_SOURCE_MESH_LIB = 2

LOG_TYPE_ZERO_ARGS = 0
LOG_TYPE_HEX_ARGS = 1
LOG_TYPE_HEX_BUF = 2
LOG_TYPE_INFO = 3

LOG_TYPE_INFO_TASK_ID_UPDATE = 0
LOG_TYPE_INFO_NULL_BUF = 1
LOG_TYPE_INFO_TASK_SWITCH = 2

ARG_SIZE_TYPE_U32 = 0
ARG_SIZE_TYPE_STR = 1
ARG_SIZE_TYPE_U64 = 2
ARG_SIZE_TYPE_LZU32 = 3
ARG_SIZE_TYPE_LZU64 = 4
ARG_SIZE_TYPE_AZU32 = 5
ARG_SIZE_TYPE_AZU64 = 6


FRAME_HEAD_LEN = 6  # uint16_t length + uint32_t frame_meta (packed)
FRAME_TAIL_LEN = 4  # uint32_t checksum

ChecksumMode = Literal['off', 'head', 'head_payload', 'auto']

HexBufMode = Literal['skip', 'summary', 'full']

DEFAULT_TRANSPORT_BUF_SIZE = 1024
DEFAULT_MAX_PAYLOAD_LEN = DEFAULT_TRANSPORT_BUF_SIZE - (FRAME_HEAD_LEN + FRAME_TAIL_LEN)


@dataclass
class ChecksumAutoState:
    """
    Tracks which checksum mode matches the live stream.

    We "learn" the mode by counting successful validations. Once one mode has a
    clear lead, we lock to it for full integrity.
    """

    head_ok: int = 0
    head_payload_ok: int = 0
    locked: Optional[Literal['head', 'head_payload']] = None

    def observe(self, head_matches: bool, head_payload_matches: bool) -> None:
        if head_matches:
            self.head_ok += 1
        if head_payload_matches:
            self.head_payload_ok += 1

        if self.locked is not None:
            return

        # Lock once we have enough evidence and a clear lead.
        # Thresholds chosen to be quick but avoid locking on a fluke.
        if self.head_ok >= 8 and (self.head_ok - self.head_payload_ok) >= 4:
            self.locked = 'head'
        elif self.head_payload_ok >= 8 and (self.head_payload_ok - self.head_ok) >= 4:
            self.locked = 'head_payload'


@dataclass
class FrameHeader:
    payload_len: int
    src_code: int
    seq: int


@dataclass
class ArgValue:
    """
    Decoded argument with raw bytes preserved for exact formatting.

    For integer-like arguments, raw is little-endian (as produced by BLE log encoders).
    For strings, raw is UTF-8 bytes (no NUL).
    """

    raw: bytes
    value: Any
    kind: str  # "int" | "str" | "bytes"


def _u16_le(b: bytes) -> int:
    return int.from_bytes(b, 'little', signed=False)


def _u32_le(b: bytes) -> int:
    return int.from_bytes(b, 'little', signed=False)


def ble_log_fast_checksum_sum(data: bytes) -> int:
    """
    BLE Log additive checksum (default non-XOR mode in firmware).

    Equivalent to firmware logic:
      checksum = sum(data) & 0xFFFFFFFF
    """
    return sum(data) & 0xFFFFFFFF


def ble_log_fast_checksum_xor(data: bytes) -> int:
    """
    BLE Log XOR checksum equivalent for CONFIG_BLE_LOG_XOR_CHECKSUM_ENABLED.

    Firmware computes XOR in 32-bit little-endian words (with alignment tricks
    for speed). Effective value over a byte stream is XOR of consecutive
    little-endian u32 chunks, with the final partial chunk zero-padded.
    """
    if not data:
        return 0

    checksum = 0
    i = 0
    n = len(data)

    while i + 4 <= n:
        checksum ^= int.from_bytes(data[i: i + 4], 'little', signed=False)
        i += 4

    if i < n:
        tail = data[i:] + (b'\x00' * (4 - (n - i)))
        checksum ^= int.from_bytes(tail, 'little', signed=False)

    return checksum & 0xFFFFFFFF


# Backward-compatible alias used by existing code paths.
def ble_log_fast_checksum(data: bytes) -> int:
    return ble_log_fast_checksum_sum(data)


def _safe_json_logs_mapping(path: Path) -> Dict[str, str]:
    with path.open('r', encoding='utf-8') as f:
        data = json.load(f)

    logs = data.get('logs')
    if not isinstance(logs, dict):
        raise ValueError(f"JSON file {path} does not contain a top-level 'logs' mapping")

    out: Dict[str, str] = {}
    for key, entry in logs.items():
        # Entry can be nested; for host compressed logs we expect {"format": "..."}
        caller: Optional[str] = None
        if isinstance(entry, dict):
            fmt = entry.get('format')
            caller_val = entry.get('caller')
            if isinstance(caller_val, str) and caller_val:
                caller = caller_val
        else:
            fmt = entry
        if not isinstance(fmt, str):
            continue
        # Normalize key so we can look up by decimal string
        try:
            kid = str(int(key))
        except (ValueError, TypeError):
            kid = str(key)
        # Unescape common builder escaping and strip outer literal quotes if present.
        cleaned = fmt.replace('\\"', '"').replace("\\'", "'")
        if len(cleaned) >= 2 and cleaned[0] == '"' and cleaned[-1] == '"':
            cleaned = cleaned[1:-1]
        # Treat escaped newline sequences as real newlines for printing.
        cleaned = cleaned.replace('\\r\\n', '\n').replace('\\n', '\n').replace('\\r', '\n')

        # Some dictionary entries are fragment-only formats like " rc=%d\n" or
        # ", err=%d\n". Recover a useful function-like prefix from `caller`.
        if caller and not cleaned.startswith(caller):
            normalized = cleaned.lstrip()
            if normalized.startswith(('rc=', 'err=', 'status=', ', rc=', ', err=', ', status=')):
                if cleaned[:1].isspace() or cleaned.startswith(','):
                    cleaned = f'{caller}{cleaned}'
                else:
                    cleaned = f'{caller} {cleaned}'

        out[kid] = cleaned
    return out


def _auto_discover_dict(build_root: Path) -> Optional[Path]:
    """
    Best-effort discovery of a JSON dict file under build artifacts.

    Strategy:
      - search for *.json under build/, prioritize those whose content has 'logs'.
    """
    candidates: List[Path] = []
    build_dir = build_root / 'build'
    if not build_dir.is_dir():
        return None

    for p in build_dir.rglob('*.json'):
        candidates.append(p)

    for p in candidates:
        try:
            with p.open('r', encoding='utf-8') as f:
                # Lightweight scan
                head = f.read(4096)
            if '"logs"' in head:
                return p
        except Exception:
            continue
    return None


def _load_all_dicts_from_dir(db_dir: Path) -> Dict[int, Dict[str, str]]:
    """
    Load all known source dictionaries from a directory.

    Expected filenames are generated by LogDBManager:
      - BLE_HOST_logs.json
      - BLE_MESH_logs.json
    """
    # Host-only (Bluedroid or NimBLE): we intentionally ignore mesh sources here.
    name_to_source_id = {'BLE_HOST_logs.json': BLE_COMPRESSED_LOG_OUT_SOURCE_HOST}

    out: Dict[int, Dict[str, str]] = {}
    for fname, sid in name_to_source_id.items():
        p = db_dir / fname
        if not p.exists():
            continue
        try:
            out[sid] = _safe_json_logs_mapping(p)
        except Exception:
            continue
    return out


def load_format_dicts(dict_path: Optional[str]) -> Dict[int, Dict[str, str]]:
    """
    Load dictionaries per compressed source id.

    If --dict points at one *_logs.json file, we also attempt to load its siblings
    for other sources from the same directory, so a single argument enables all.
    """
    if dict_path:
        p = Path(dict_path)
        # If user points to a specific *_logs.json, load that and try siblings.
        if p.is_file():
            all_from_dir = _load_all_dicts_from_dir(p.parent)
            # Ensure the explicitly provided file is loaded even if it isn't named as expected.
            try:
                explicit = _safe_json_logs_mapping(p)
                # Host-only: map the explicit file to BLE_HOST (source id 0).
                all_from_dir[BLE_COMPRESSED_LOG_OUT_SOURCE_HOST] = explicit
            except Exception:
                pass
            return all_from_dir

        # If user points to a directory, load all known dicts from it.
        if p.is_dir():
            return _load_all_dicts_from_dir(p)

        raise SystemExit(f'--dict path not found: {p}')

    auto = _auto_discover_dict(Path.cwd())
    if not auto:
        raise SystemExit(
            'Could not auto-discover logs JSON dictionary under build/. Please provide --dict /path/to/logs.json'
        )
    # Auto-discovery found one JSON; assume it's inside the db directory.
    out = load_format_dicts(str(auto))
    # Ensure host-only output.
    return {BLE_COMPRESSED_LOG_OUT_SOURCE_HOST: out.get(BLE_COMPRESSED_LOG_OUT_SOURCE_HOST, {})}


def parse_frame_header(buf: bytes, offset: int) -> Optional[Tuple[FrameHeader, int]]:
    """
    Parse BLE Log frame header starting at offset, returning (header, total_size).

    total_size = FRAME_HEAD_LEN + payload_len + FRAME_TAIL_LEN
    """
    if offset + FRAME_HEAD_LEN > len(buf):
        return None

    payload_len = _u16_le(buf[offset: offset + 2])
    frame_meta = _u32_le(buf[offset + 2: offset + 6])

    src_code = frame_meta & 0xFF
    seq = (frame_meta >> 8) & 0xFFFFFF

    total_size = FRAME_HEAD_LEN + payload_len + FRAME_TAIL_LEN
    if offset + total_size > len(buf):
        return None

    return FrameHeader(payload_len=payload_len, src_code=src_code, seq=seq), total_size


def iter_frames(buf: bytes) -> Iterable[Tuple[FrameHeader, bytes]]:
    """
    Walk a DMA buffer strictly frame-by-frame.

    Never scans the entire 1024-byte buffer for tags; uses only the length field.
    """
    idx = 0
    n = len(buf)
    while idx + FRAME_HEAD_LEN <= n:
        parsed = parse_frame_header(buf, idx)
        if not parsed:
            break
        hdr, total_size = parsed

        if hdr.payload_len == 0:
            # Heartbeat; still advance by total_size to avoid desync.
            idx += total_size
            continue

        payload_start = idx + FRAME_HEAD_LEN
        payload_end = payload_start + hdr.payload_len
        payload = buf[payload_start:payload_end]

        yield hdr, payload
        idx += total_size


def _parse_lzu32_arg(data: bytes, pos: int) -> Tuple[int, int]:
    """
    Decode ARG_SIZE_TYPE_LZU32 encoding.

    Layout in encoder:
      - When value <= 0xff:
           push_u8(3); push_u8(value)
      - When value <= 0xffff:
           push_u8(2); push_u16(value)
      - Otherwise it falls back to ARG_SIZE_TYPE_U32 (4-byte value).
    """
    if pos >= len(data):
        raise IndexError('LZU32 missing lz byte')
    lz = data[pos]
    pos += 1

    # Generic rule: 4 - lz non-zero bytes.
    non_zero_bytes = 4 - lz
    if non_zero_bytes < 0:
        non_zero_bytes = 0
    if pos + non_zero_bytes > len(data):
        raise IndexError('LZU32 missing value bytes')

    value_bytes = data[pos: pos + non_zero_bytes]
    pos += non_zero_bytes
    value = int.from_bytes(value_bytes, 'little', signed=False)
    return value, pos


def _parse_lzu64_arg(data: bytes, pos: int) -> Tuple[int, int]:
    """
    Decode ARG_SIZE_TYPE_LZU64 encoding.

    Encoder writes:
      - lz: number of leading zero bytes (0-7)
      - then (8 - lz) bytes built from a mix of u32/u16/u8 writes.

    We treat the (8 - lz) bytes as the low-order bytes of the integer.
    """
    if pos >= len(data):
        raise IndexError('LZU64 missing lz byte')
    lz = data[pos]
    pos += 1
    body_len = 8 - lz
    if body_len < 0:
        body_len = 0
    if pos + body_len > len(data):
        raise IndexError('LZU64 missing value bytes')
    value_bytes = data[pos: pos + body_len]
    pos += body_len
    value = int.from_bytes(value_bytes, 'little', signed=False)
    return value, pos


def decode_arg_sequence(size_bytes: bytes, args_blob: bytes, arg_count: int) -> Tuple[List[ArgValue], int]:
    """
    Decode compressed arguments according to ARG_SIZE_TYPE_* nibble list.

    Returns (values, bytes_consumed_from_args_blob).
    """
    values: List[ArgValue] = []
    pos = 0

    for i in range(arg_count):
        if i // 2 >= len(size_bytes):
            raise ValueError('Insufficient size/type bytes for arguments')
        sb = size_bytes[i // 2]
        if i % 2 == 0:
            arg_type = (sb >> 4) & 0x0F
        else:
            arg_type = sb & 0x0F

        if arg_type == ARG_SIZE_TYPE_U32:
            if pos + 4 > len(args_blob):
                raise IndexError('U32 argument truncated')
            raw = args_blob[pos: pos + 4]
            value = _u32_le(raw)
            pos += 4
            values.append(ArgValue(raw=raw, value=value, kind='int'))
        elif arg_type == ARG_SIZE_TYPE_AZU32:
            values.append(ArgValue(raw=b'\x00\x00\x00\x00', value=0, kind='int'))
        elif arg_type == ARG_SIZE_TYPE_LZU32:
            value, pos = _parse_lzu32_arg(args_blob, pos)
            # FIX: Reconstruct a pure 4-byte raw representation, discarding the lz metadata byte
            raw = value.to_bytes(4, byteorder='little', signed=False)
            values.append(ArgValue(raw=raw, value=value, kind='int'))
        elif arg_type == ARG_SIZE_TYPE_U64:
            if pos + 8 > len(args_blob):
                raise IndexError('U64 argument truncated')
            raw = args_blob[pos: pos + 8]
            value = int.from_bytes(raw, 'little', signed=False)
            pos += 8
            values.append(ArgValue(raw=raw, value=value, kind='int'))
        elif arg_type == ARG_SIZE_TYPE_AZU64:
            values.append(ArgValue(raw=b'\x00' * 8, value=0, kind='int'))
        elif arg_type == ARG_SIZE_TYPE_LZU64:
            value, pos = _parse_lzu64_arg(args_blob, pos)
            # FIX: Reconstruct a pure 8-byte raw representation, discarding the lz metadata byte
            raw = value.to_bytes(8, byteorder='little', signed=False)
            values.append(ArgValue(raw=raw, value=value, kind='int'))
        elif arg_type == ARG_SIZE_TYPE_STR:
            start = pos
            while pos < len(args_blob) and args_blob[pos] != 0:
                pos += 1
            if pos >= len(args_blob):
                # Unterminated string: recover whatever we have and mark it.
                raw = args_blob[start:]
                s = raw.decode('utf-8', errors='replace') + ' [UNTERMINATED]'
                pos = len(args_blob)
                values.append(ArgValue(raw=raw, value=s, kind='str'))
            else:
                raw = args_blob[start:pos]
                s = raw.decode('utf-8', errors='replace')
                pos += 1  # skip NUL
                values.append(ArgValue(raw=raw, value=s, kind='str'))
        else:
            # Unknown type; treat as raw byte and hex-print
            if pos >= len(args_blob):
                raise IndexError('Unknown argument type with no data')
            raw = bytes([args_blob[pos]])
            values.append(ArgValue(raw=raw, value=f'0x{args_blob[pos]:02x}', kind='bytes'))
            pos += 1

    return values, pos


def _length_mod_to_expected_size_bytes(length_mod: str) -> int:
    """
    Expected argument byte width for integer conversions on 32-bit targets.

    Notes:
      - ESP32 is 32-bit, but printf length modifiers can request wider values.
      - We follow the user's requested mapping.
    """
    if length_mod in ('hh',):
        return 1
    if length_mod in ('h',):
        return 2
    if length_mod in ('l',):
        return 4
    if length_mod in ('ll', 'j'):
        return 8
    if length_mod in ('z', 't'):
        return 4
    return 4


def _coerce_int_from_arg(av: ArgValue) -> Optional[int]:
    try:
        if av.kind == 'int':
            if isinstance(av.value, int):
                return av.value
            return int(av.value)
        if isinstance(av.value, int):
            return av.value
        return int(av.value)  # type: ignore[arg-type]
    except Exception:
        return None


def _decode_int_from_av(av: ArgValue, *, signed: bool, expected_size: int) -> int:
    """
    Decode integer from little-endian raw bytes, with explicit two's-complement sign extension.
    """
    expected_size = max(1, expected_size)
    raw = av.raw[:expected_size]
    if len(raw) < expected_size:
        if signed and raw:
            pad_byte = 0xFF if (raw[-1] & 0x80) else 0x00
        else:
            pad_byte = 0x00
        raw = raw + bytes([pad_byte]) * (expected_size - len(raw))

    if signed:
        return int.from_bytes(raw, 'little', signed=True)
    return int.from_bytes(raw, 'little', signed=False)


def _format_integer_printf(
    av: ArgValue,
    conv_char: str,
    flags: str,
    width_val: Optional[int],
    precision_val: Optional[int],
    length_mod: str,
) -> str:
    """
    Robust printf-style integer formatting for a limited subset.

    Supports:
      - flags: '-', '+', ' ', '#', '0'
      - conversions: d, i, u, o, x, X
      - length modifiers: hh, h, l, ll, j, z, t
      - width and precision (precision applies to digit counts)
    """
    expected_size = _length_mod_to_expected_size_bytes(length_mod)

    signed_conv = conv_char in ('d', 'i')
    unsigned_conv = conv_char in ('u', 'o', 'x', 'X')

    precision_for_digits = precision_val
    if precision_for_digits is not None and precision_for_digits < 0:
        precision_for_digits = None

    if signed_conv:
        signed_value = _decode_int_from_av(av, signed=True, expected_size=expected_size)
        sign_prefix = ''
        if signed_value < 0:
            sign_prefix = '-'
            abs_u = -signed_value
        else:
            abs_u = signed_value
            if '+' in flags:
                sign_prefix = '+'
            elif ' ' in flags:
                sign_prefix = ' '
        digits = str(abs_u)
        unsigned_value = abs_u
    elif unsigned_conv:
        unsigned_value = _decode_int_from_av(av, signed=False, expected_size=expected_size)
        sign_prefix = ''
        if conv_char == 'u':
            digits = str(unsigned_value)
        elif conv_char == 'o':
            digits = format(unsigned_value, 'o')
        elif conv_char in ('x', 'X'):
            digits = format(unsigned_value, 'x' if conv_char == 'x' else 'X')
        else:
            raise ValueError(f'Unsupported integer conv: {conv_char}')
    else:
        raise ValueError(f'Unsupported integer conversion: {conv_char}')

    prefix = ''
    if '#' in flags:
        if conv_char in ('x', 'X'):
            if unsigned_value != 0:
                prefix = '0X' if conv_char == 'X' else '0x'
        elif conv_char == 'o':
            prefix = '0'

    if precision_for_digits is not None:
        if precision_for_digits == 0 and unsigned_value == 0:
            digits = ''
        else:
            digits = digits.rjust(precision_for_digits, '0')

    total_len = len(sign_prefix) + len(prefix) + len(digits)
    if width_val is None:
        return f'{sign_prefix}{prefix}{digits}'

    if width_val < 0:
        width_val = abs(width_val)
        flags = flags.replace('0', '')
        if '-' not in flags:
            flags = flags + '-'

    if '-' in flags:
        pad_len = max(0, width_val - total_len)
        return f'{sign_prefix}{prefix}{digits}' + (' ' * pad_len)

    zero_pad = '0' in flags and precision_for_digits is None
    pad_len = max(0, width_val - total_len)
    if pad_len == 0:
        return f'{sign_prefix}{prefix}{digits}'

    if zero_pad:
        return f'{sign_prefix}{prefix}' + ('0' * pad_len) + digits

    return (' ' * pad_len) + f'{sign_prefix}{prefix}{digits}'


def _format_hex_float_printf(
    val: float,
    conv_char: str,
    flags: str,
    width_val: Optional[int],
    precision_val: Optional[int],
) -> str:
    """
    Best-effort formatting for C %a/%A (hex floats).

    Python doesn't support format code 'a'/'A', so we take `float.hex()` and
    adjust radix-point precision by truncation/padding.
    """
    if not (val == val) or val in (float('inf'), float('-inf')):
        s = str(val)
    else:
        s = val.hex()  # "0x<hex>.<hex>p<exp>"
        if 'p' in s:
            mantissa, exp = s.split('p', 1)
            exp = 'p' + exp
        else:
            mantissa, exp = s, 'p+0'

        if mantissa.startswith('0x'):
            mantissa_body = mantissa[2:]
            prefix_0x = '0x'
        else:
            mantissa_body = mantissa
            prefix_0x = ''

        if '.' in mantissa_body:
            int_part, frac_part = mantissa_body.split('.', 1)
        else:
            int_part, frac_part = mantissa_body, ''

        if precision_val is not None and precision_val >= 0:
            if precision_val == 0:
                frac_part2 = ''
            else:
                frac_part2 = (frac_part + ('0' * precision_val))[:precision_val]
            s = prefix_0x + int_part + ('.' + frac_part2 if precision_val >= 0 else '') + exp
        else:
            s = prefix_0x + int_part + ('.' + frac_part if frac_part else '') + exp

        if conv_char == 'A':
            s = s.replace('0x', '0X')
            s = s.replace('p', 'P')

    # Apply sign flags (+/space) for positive values.
    if s and s[0] != '-' and conv_char in ('a', 'A'):
        if '+' in flags:
            s = '+' + s
        elif ' ' in flags:
            s = ' ' + s

    if width_val is None or width_val <= len(s):
        return s
    if '-' in flags:
        return s + (' ' * (width_val - len(s)))
    if '0' in flags:
        pad_len = width_val - len(s)
        sign = ''
        rest = s
        if rest.startswith(('+', '-', ' ')):
            sign = rest[0]
            rest = rest[1:]
        if rest.startswith(('0x', '0X')):
            prefix = rest[:2]
            remainder = rest[2:]
            return sign + prefix + ('0' * pad_len) + remainder
        return sign + ('0' * pad_len) + rest
    return (' ' * (width_val - len(s))) + s


def _format_float_printf(
    av: ArgValue,
    conv_char: str,
    flags: str,
    width_val: Optional[int],
    precision_val: Optional[int],
) -> str:
    """
    Robust printf-style floating formatting for f/F/e/E/g/G and hex a/A.
    """
    try:
        if len(av.raw) >= 8:
            float_value = struct.unpack('<d', av.raw[:8])[0]
        elif len(av.raw) >= 4:
            float_value = struct.unpack('<f', av.raw[:4])[0]
        else:
            float_value = float(av.value)
    except Exception:
        return str(av.value)

    if conv_char in ('a', 'A'):
        return _format_hex_float_printf(float_value, conv_char, flags, width_val, precision_val)

    type_char = conv_char
    precision_part = '' if precision_val is None else f'.{precision_val}'

    sign_part = '+' if '+' in flags else (' ' if ' ' in flags else '')
    alt_part = '#' if '#' in flags else ''

    if '-' in flags:
        align_part = '<'
        fill_part = ' '
    else:
        if '0' in flags:
            align_part = '='
            fill_part = '0'
        else:
            align_part = '>'
            fill_part = ' '

    width_part = '' if width_val is None else str(width_val)
    fmt_spec = f'{fill_part}{align_part}{sign_part}{alt_part}{width_part}{precision_part}{type_char}'
    try:
        return format(float_value, fmt_spec)
    except Exception:
        return str(float_value)


def _render_with_c_format_parse(fmt: str, args: List[ArgValue]) -> Optional[str]:
    """
    Render with Espressif's tokenizer, supporting a broader printf subset.

    We use `parse_format_string()` from `c_format_parse.py` and implement rendering
    ourselves to match our decoded argument representation.
    """
    if not _HAVE_C_FORMAT_PARSE:
        return None

    try:
        tokens = parse_format_string(fmt)
    except Exception:
        return None

    out: List[str] = []
    arg_i = 0

    for tk in tokens:
        if not isinstance(tk, tuple):
            out.append(tk)
            continue

        # Version-1: (start, end, full_spec, flags, width, length_mod, conv_char) -> 7 elements
        # Version-2: (start, end, full_spec, flags, width, precision, length_mod, conv_char) -> 8 elements
        if len(tk) == 7:
            _start, _end, _full_spec, flags, width, length_mod, conv = tk
            precision = ''
        elif len(tk) == 8:
            _start, _end, _full_spec, flags, width, precision, length_mod, conv = tk
        else:
            out.append(str(tk))
            continue

        if conv == '%':
            out.append('%')
            continue

        def consume_width_precision(spec: str) -> Optional[int]:
            nonlocal arg_i
            if spec == '' or spec is None:
                return None
            if spec == '*':
                if arg_i >= len(args):
                    raise IndexError("Missing '*' width/precision argument")
                width_av = args[arg_i]
                arg_i += 1
                return _coerce_int_from_arg(width_av)
            try:
                return int(spec)
            except Exception:
                return None

        av_for_fallback: Optional[ArgValue] = None
        try:
            width_val = consume_width_precision(width)
            precision_val = consume_width_precision(precision)

            if width_val is not None and width_val < 0:
                width_val = abs(width_val)
                flags = flags.replace('0', '')
                if '-' not in flags:
                    flags = flags + '-'

            if precision_val is not None and precision_val < 0:
                precision_val = None

            if conv == 'n':
                if arg_i >= len(args):
                    return None
                arg_i += 1
                continue

            if arg_i >= len(args):
                return None
            av = args[arg_i]
            arg_i += 1
            av_for_fallback = av

            if conv == 's':
                s = str(av.value)
                if precision_val is not None:
                    s = s[: max(0, precision_val)]
                if width_val is not None and width_val > len(s) and '-' not in flags:
                    s = (' ' * (width_val - len(s))) + s
                elif width_val is not None and width_val > len(s) and '-' in flags:
                    s = s + (' ' * (width_val - len(s)))
                out.append(s)
                continue

            if conv == 'c':
                try:
                    if av.kind == 'str':
                        ch = '' if not str(av.value) else str(av.value)[0]
                    else:
                        ch_int = _coerce_int_from_arg(av)
                        ch = chr((ch_int or 0) & 0xFF)
                except Exception:
                    ch = str(av.value)[:1]

                # For %c, C ignores '0' padding; it is always space padding.
                if width_val is None or width_val <= len(ch):
                    out.append(ch)
                elif '-' in flags:
                    out.append(ch + (' ' * (width_val - len(ch))))
                else:
                    out.append((' ' * (width_val - len(ch))) + ch)
                continue

            if conv == 'p':
                try:
                    p_val = _decode_int_from_av(av, signed=False, expected_size=4)
                    out.append('0x' + format(p_val, 'x'))
                except Exception:
                    out.append(str(av.value))
                continue

            if conv in 'fFeEgGaA':
                out.append(_format_float_printf(av, conv, flags, width_val, precision_val))
                continue

            if conv in 'diuoxX':
                out.append(_format_integer_printf(av, conv, flags, width_val, precision_val, length_mod))
                continue

            out.append(str(av.value))
        except Exception:
            try:
                if av_for_fallback is not None:
                    out.append(str(av_for_fallback.value))
                elif arg_i < len(args):
                    out.append(str(args[arg_i].value))
                else:
                    out.append('')
            except Exception:
                out.append('')

    return ''.join(out)


def format_c_like(fmt: str, args: List[ArgValue]) -> str:
    """
    Very small subset of C printf formatting to Python.

    We deliberately keep this simple; for complex cases we fall back to
    showing the raw format and argument list.
    """
    rendered = _render_with_c_format_parse(fmt, args)
    if rendered is not None:
        return rendered

    # Minimal fallback: just substitute %s/%d-ish via Python %-formatting.
    # We only pass plain python values to this layer.
    try:
        import re

        def repl(m: re.Match[str]) -> str:
            spec = m.group(0)
            if spec.endswith(('d', 'i', 'u', 'x', 'X', 'o')):
                return '%' + spec[-1]
            if spec.endswith(('s', 'c')):
                return '%s'
            if spec.endswith(('p',)):
                return '%s'
            if spec.endswith(('f', 'F', 'e', 'E', 'g', 'G', 'a', 'A')):
                return '%s'
            return spec

        cleaned = re.sub(r'%[-+ #0]*\d*(?:\.\d+)?[hlzjtL]*[diuoxXfFeEgGaApcs]', repl, fmt)
        py_args = [a.value for a in args]
        return cleaned % tuple(py_args)
    except Exception:
        return f'{fmt} | args={[a.value for a in args]!r}'


def decode_compressed_payload(
    payload: bytes, dicts_by_source: Dict[int, Dict[str, str]]
) -> Iterable[Tuple[float, int, Optional[int], Optional[str], Optional[int], Optional[List[ArgValue]]]]:
    """
    Decode BLE_LOG_SRC_ENCODE payload into high-level log records.

    Yields tuples:
      (timestamp_seconds, seq, log_id, message, arg_count, raw_args_or_none)
    """
    if len(payload) < 4:
        return

    os_ts_ms = _u32_le(payload[0:4])
    timestamp_s = os_ts_ms / 1000.0

    data = payload[4:]
    if not data:
        return

    # First byte is the compressed-log output source.
    target_source = data[0]
    if target_source != BLE_COMPRESSED_LOG_OUT_SOURCE_HOST:
        return

    formats = dicts_by_source.get(target_source)
    if not formats:
        yield (
            timestamp_s,
            target_source,
            None,
            '[!] FATAL: Missing host dictionary file (BLE_HOST_logs.json)',
            0,
            None,
        )
        return
    pos = 1

    while pos < len(data):
        # At minimum we need a type header
        if pos >= len(data):
            break
        type_hdr = data[pos]
        pos += 1

        log_type = (type_hdr >> 6) & 0x03
        info = type_hdr & 0x3F

        if log_type == LOG_TYPE_INFO:
            # For now, INFO records are skipped. Known ones are single-byte or have
            # fixed small layouts and are not user-facing logs.
            if info in (LOG_TYPE_INFO_TASK_ID_UPDATE, LOG_TYPE_INFO_TASK_SWITCH):
                # No additional payload defined in our patches.
                continue
            if info == LOG_TYPE_INFO_NULL_BUF:
                # Followed by u16 log_index; skip it.
                if pos + 2 > len(data):
                    break
                pos += 2
                continue
            continue

        if log_type == LOG_TYPE_HEX_BUF:
            # Buffer logs. Newer format is self-delimiting:
            #   [type_hdr][log_id(u16)][buf_len(u16)][buf_bytes...]
            if pos + 2 > len(data):
                break
            log_index = _u16_le(data[pos: pos + 2])
            pos += 2
            if pos + 2 > len(data):
                break
            buf_len = _u16_le(data[pos: pos + 2])
            pos += 2
            if pos + buf_len > len(data):
                # Malformed HEX_BUF record; stop parsing payload to avoid desync.
                break
            buf_bytes = data[pos: pos + buf_len]
            pos += buf_len
            # Yield as a record with message=None but args containing one ArgValue
            yield (
                timestamp_s,
                target_source,
                log_index,
                None,
                -1,
                [ArgValue(raw=buf_bytes, value=buf_bytes, kind='bytes')],
            )
            continue

        if log_type == LOG_TYPE_ZERO_ARGS and info == 0:
            # No-args compressed record without the full HEX_ARGS metadata; not
            # expected for host logs here.
            continue

        if log_type != LOG_TYPE_HEX_ARGS:
            # Unknown type – stop to avoid desync within payload.
            break

        arg_count = info
        if pos + 2 > len(data):
            break
        log_id = _u16_le(data[pos: pos + 2])
        pos += 2

        if arg_count == 0:
            fmt = formats.get(str(log_id))
            message = format_c_like(fmt, []) if fmt else None
            yield (timestamp_s, target_source, log_id, message, arg_count, None)
            continue

        size_bytes_len = (arg_count + 1) // 2
        if pos + size_bytes_len > len(data):
            break
        size_bytes = data[pos: pos + size_bytes_len]
        pos += size_bytes_len

        args_blob = data[pos:]
        try:
            values, consumed = decode_arg_sequence(size_bytes, args_blob, arg_count)
        except (IndexError, ValueError):
            # Stop at first error to avoid running off the end.
            break
        pos += consumed

        fmt = formats.get(str(log_id))
        message = format_c_like(fmt, values) if fmt else None
        yield (timestamp_s, target_source, log_id, message, arg_count, values)


def parse_caught_line(line: str) -> Optional[bytes]:
    """
    Parse a single UART line into bytes.

    Accepts either:
      - plain hex dumps: "08 00 07 4F 01 00 ..."
      - optionally-prefixed dumps: "<label>: 08 00 07 ..."

    Returns None if the line doesn't look like a hex dump.

        Integrity:
            - Prefer partial recovery: malformed tokens are skipped.
            - Caller-side frame parsing and checksum validation handle final resync.
    """
    parts = line.split()
    if not parts:
        return None

    # If the first token looks like a label (e.g. "CAUGHT:"), drop it.
    start_idx = 0
    if parts[0].endswith(':') and len(parts) > 1:
        start_idx = 1

    hex_bytes = parts[start_idx:]
    buf = bytearray()

    for token in hex_bytes:
        if not token:
            continue
        # If a token is malformed, skip just that byte, don't kill the line.
        if len(token) != 2:
            continue
        try:
            buf.append(int(token, 16))
        except ValueError:
            continue

    return bytes(buf) if buf else None


def process_buffer(
    buf: bytes,
    dicts_by_source: Dict[int, Dict[str, str]],
) -> None:
    for hdr, payload in iter_frames(buf):
        if hdr.src_code != BLE_LOG_SRC_ENCODE:
            continue

        for ts_s, target_source, log_id, msg, _arg_cnt, raw_args in decode_compressed_payload(payload, dicts_by_source):
            prefix = f'[{ts_s:.3f}s] [Seq {hdr.seq}]'
            if log_id is None:
                print(f'{prefix} <unknown-log>')
                continue

            if msg is not None:
                # If the log's entire purpose is to print a newline, emit a blank line.
                if msg.replace('\n', '').strip() == '' and '\n' in msg:
                    print('')
                else:
                    print(f'{prefix} [Src {target_source}] [Log {log_id}] {msg}')
            else:
                args_list = None if raw_args is None else [a.value for a in raw_args]
                print(f'{prefix} [Src {target_source}] [Log {log_id}] (no format) args={args_list!r}')


def process_stream_bytes(
    rolling: bytearray,
    dicts_by_source: Dict[int, Dict[str, str]],
    max_payload_len: int,
) -> None:
    """
    Process a persistent rolling RX buffer in-place.

    Fixes the DMA boundary trap: a BLE Log frame may span multiple UART prints.
    We only delete bytes once complete frames have been successfully consumed.
    """
    idx = 0
    n = len(rolling)
    consumed = 0

    # Note: this function assumes the stream is mostly well-aligned and does not
    # scan for headers in the normal case. Resync scanning is only used when
    # checksum validation is enabled and a mismatch is detected.
    while idx + FRAME_HEAD_LEN <= n:
        # Sanity check to avoid "ghost frame" stalls caused by FF/idle padding.
        # payload_len is uint16_le at [idx:idx+2]. If it's physically impossible
        # for this transport, discard bytes until a plausible header appears.
        claimed_len = rolling[idx] | (rolling[idx + 1] << 8)
        src_code = rolling[idx + 2]
        if claimed_len > max_payload_len or src_code > BLE_LOG_SRC_MAX:
            idx += 1
            consumed = idx
            continue
        parsed = parse_frame_header(rolling, idx)
        if not parsed:
            break
        hdr, total_size = parsed
        if idx + total_size > n:
            break

        payload_start = idx + FRAME_HEAD_LEN
        payload_end = payload_start + hdr.payload_len
        payload = bytes(rolling[payload_start:payload_end])

        if hdr.payload_len != 0 and hdr.src_code == BLE_LOG_SRC_ENCODE:
            try:
                for ts_s, target_source, log_id, msg, _arg_cnt, raw_args in decode_compressed_payload(
                    payload, dicts_by_source
                ):
                    prefix = f'[{ts_s:.3f}s] [Seq {hdr.seq}]'
                    if log_id is None:
                        print(f'{prefix} <unknown-log>')
                        continue
                    if msg is not None:
                        if msg.replace('\n', '').strip() == '' and '\n' in msg:
                            print('')
                        else:
                            print(f'{prefix} [Src {target_source}] [Log {log_id}] {msg}')
                    else:
                        args_list = None if raw_args is None else [a.value for a in raw_args]
                        print(f'{prefix} [Src {target_source}] [Log {log_id}] (no format) args={args_list!r}')
            except Exception as e:
                # Keep stream processing alive even if one payload is malformed.
                print(f'[!] Decoder exception on Seq {hdr.seq}: {e}')

        idx += total_size
        consumed = idx

    if consumed:
        del rolling[:consumed]


def _format_hex_bytes(b: bytes) -> str:
    return ' '.join(f'{x:02X}' for x in b)


def process_stream_bytes_with_checksum(
    rolling: bytearray,
    dicts_by_source: Dict[int, Dict[str, str]],
    checksum_mode: ChecksumMode,
    auto_state: Optional[ChecksumAutoState] = None,
    hex_buf_mode: HexBufMode = 'summary',
    max_payload_len: int = DEFAULT_MAX_PAYLOAD_LEN,
) -> None:
    """
    Rolling parser with optional checksum validation.

    - checksum_mode='off': identical behavior to process_stream_bytes()
    - checksum_mode='head': validate checksum over frame header only
    - checksum_mode='head_payload': validate checksum over header+payload

    On checksum mismatch we attempt a safe resync by shifting forward one byte
    and trying again (bounded by available data). This avoids permanently losing
    the stream when a single byte is corrupted.
    """
    if checksum_mode == 'off':
        process_stream_bytes(rolling, dicts_by_source, max_payload_len=max_payload_len)
        return

    if checksum_mode == 'auto' and auto_state is None:
        auto_state = ChecksumAutoState()

    idx = 0
    n = len(rolling)
    consumed = 0

    while idx + FRAME_HEAD_LEN + FRAME_TAIL_LEN <= n:
        # Same ghost-frame guard as the non-checksum path.
        claimed_len = rolling[idx] | (rolling[idx + 1] << 8)
        src_code = rolling[idx + 2]
        if claimed_len > max_payload_len or src_code > BLE_LOG_SRC_MAX:
            idx += 1
            consumed = idx
            continue
        parsed = parse_frame_header(rolling, idx)
        if not parsed:
            break
        hdr, total_size = parsed
        if idx + total_size > n:
            break

        head = bytes(rolling[idx: idx + FRAME_HEAD_LEN])
        payload_start = idx + FRAME_HEAD_LEN
        payload_end = payload_start + hdr.payload_len
        payload = bytes(rolling[payload_start:payload_end])
        tail = bytes(rolling[payload_end: payload_end + FRAME_TAIL_LEN])
        expected = _u32_le(tail)

        # Firmware can use either additive checksum or XOR checksum
        # (CONFIG_BLE_LOG_XOR_CHECKSUM_ENABLED). Validate against both.
        head_sum_actual = ble_log_fast_checksum_sum(head)
        head_payload_sum_actual = ble_log_fast_checksum_sum(head + payload)
        head_xor_actual = ble_log_fast_checksum_xor(head)
        head_payload_xor_actual = ble_log_fast_checksum_xor(head + payload)

        head_matches = (head_sum_actual == expected) or (head_xor_actual == expected)
        head_payload_matches = (head_payload_sum_actual == expected) or (head_payload_xor_actual == expected)

        if checksum_mode == 'head':
            ok = head_matches
        elif checksum_mode == 'head_payload':
            ok = head_payload_matches
        else:  # auto
            auto_state.observe(head_matches, head_payload_matches)  # type: ignore[union-attr]
            locked = auto_state.locked  # type: ignore[union-attr]
            if locked == 'head':
                ok = head_matches
            elif locked == 'head_payload':
                ok = head_payload_matches
            else:
                # Not locked yet: accept frames that match either mode.
                ok = head_matches or head_payload_matches

        if not ok:
            # Resync attempt: shift by 1 byte and try again.
            idx += 1
            continue

        # Valid frame; consume and process.
        if hdr.payload_len != 0 and hdr.src_code == BLE_LOG_SRC_ENCODE:
            try:
                for ts_s, target_source, log_id, msg, _arg_cnt, raw_args in decode_compressed_payload(
                    payload, dicts_by_source
                ):
                    prefix = f'[{ts_s:.3f}s] [Seq {hdr.seq}]'
                    if log_id is None:
                        print(f'{prefix} <unknown-log>')
                        continue
                    if msg is not None:
                        if msg.replace('\n', '').strip() == '' and '\n' in msg:
                            print('')
                        else:
                            print(f'{prefix} [Src {target_source}] [Log {log_id}] {msg}')
                    else:
                        # This path is used for unknown formats and HEX_BUF records.
                        if raw_args and len(raw_args) == 1 and isinstance(raw_args[0].value, (bytes, bytearray)):
                            buf = bytes(raw_args[0].value)
                            if hex_buf_mode == 'skip':
                                continue
                            if hex_buf_mode == 'summary':
                                print(f'{prefix} [Src {target_source}] [Log {log_id}] HEX_BUF len={len(buf)}')
                            else:
                                print(
                                    f'{prefix} [Src {target_source}] [Log {log_id}]'
                                    f'HEX_BUF len={len(buf)} data={_format_hex_bytes(buf)}'
                                )
                        else:
                            args_list = None if raw_args is None else [a.value for a in raw_args]
                            print(f'{prefix} [Src {target_source}] [Log {log_id}] (no format) args={args_list!r}')
            except Exception as e:
                # Keep stream processing alive even if one payload is malformed.
                print(f'[!] Decoder exception on Seq {hdr.seq}: {e}')

        idx += total_size
        consumed = idx

    if consumed:
        del rolling[:consumed]

    # Prevent unbounded growth if stream is mostly garbage and we keep resyncing.
    if len(rolling) > 64 * 1024:
        del rolling[: -FRAME_HEAD_LEN - FRAME_TAIL_LEN]


def run_serial(
    port: str,
    baud: int,
    dicts_by_source: Dict[int, Dict[str, str]],
) -> None:
    if serial is None:
        raise SystemExit('pyserial is required: pip install pyserial')

    ser = serial.Serial(port=port, baudrate=baud, timeout=1.0)
    rolling = bytearray()
    try:
        while True:
            line_bytes = ser.readline()
            if not line_bytes:
                continue
            try:
                line = line_bytes.decode('utf-8', errors='replace').strip()
            except Exception:
                continue

            buf = parse_caught_line(line)
            if buf is None:
                # Corruption/noise: ignore this line and keep rolling buffer state.
                continue
            rolling.extend(buf)
            process_stream_bytes(rolling, dicts_by_source, DEFAULT_MAX_PAYLOAD_LEN)
    finally:
        ser.close()


def main(argv: Optional[List[str]] = None) -> int:
    parser = argparse.ArgumentParser(description='ESP BLE host compressed log decompressor')
    parser.add_argument('--port', default='/dev/ttyUSB0', help='Serial port path')
    parser.add_argument('--baud', type=int, default=115200, help='Serial baud rate')
    parser.add_argument('--dict', dest='dict_path', help='Path to logs JSON dictionary')
    parser.add_argument(
        '--checksum',
        choices=['off', 'head', 'head_payload', 'auto'],
        default='auto',
        help=(
            'Validate checksum: off, head-only, head+payload, or auto-detect (recommended). '
            'If you force a mode, it must match firmware config.'
        ),
    )
    parser.add_argument(
        '--hex-buf',
        choices=['skip', 'summary', 'full'],
        default='summary',
        help='How to print LOG_TYPE_HEX_BUF records (skip/summary/full hexdump)',
    )
    parser.add_argument(
        '--max-payload-len',
        type=int,
        default=DEFAULT_MAX_PAYLOAD_LEN,
        help=(
            'Maximum physically plausible BLE Log payload_len for ghost-frame guard. '
            'Default assumes transport buffer size 1024.'
        ),
    )
    parser.add_argument(
        '--stdin',
        action='store_true',
        help='Read hex dump lines from stdin instead of a serial port (for testing)',
    )
    args = parser.parse_args(argv)
    dicts_by_source = load_format_dicts(args.dict_path)

    if args.stdin:
        rolling = bytearray()
        auto_state = ChecksumAutoState() if args.checksum == 'auto' else None
        for line in sys.stdin:
            buf = parse_caught_line(line.strip())
            if buf is None:
                continue
            rolling.extend(buf)
            process_stream_bytes_with_checksum(
                rolling,
                dicts_by_source,
                args.checksum,
                auto_state=auto_state,  # type: ignore[arg-type]
                hex_buf_mode=args.hex_buf,  # type: ignore[arg-type]
                max_payload_len=args.max_payload_len,
            )
        return 0

    if serial is None:
        raise SystemExit('pyserial is required: pip install pyserial')

    # Inline serial loop so we can pass checksum_mode without changing external API.
    ser = serial.Serial(port=args.port, baudrate=args.baud, timeout=1.0)
    rolling = bytearray()
    auto_state = ChecksumAutoState() if args.checksum == 'auto' else None
    try:
        while True:
            line_bytes = ser.readline()
            if not line_bytes:
                continue
            try:
                line = line_bytes.decode('utf-8', errors='replace').strip()
            except Exception:
                continue
            buf = parse_caught_line(line)
            if buf is None:
                continue
            rolling.extend(buf)
            process_stream_bytes_with_checksum(
                rolling,
                dicts_by_source,
                args.checksum,
                auto_state=auto_state,  # type: ignore[arg-type]
                hex_buf_mode=args.hex_buf,  # type: ignore[arg-type]
                max_payload_len=args.max_payload_len,
            )

    except KeyboardInterrupt:
        print('\nKeyboard interrupt detected. Exiting...')

    finally:
        ser.close()
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
