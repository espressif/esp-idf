# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import struct

from src.backend.internal_decoder import decode_internal_frame
from src.backend.models import InternalSource


def _make_internal_payload(os_ts: int, int_src: int, sub_payload: bytes) -> bytes:
    """Build a full INTERNAL frame payload (os_ts + int_src_code + sub_payload)."""
    return struct.pack('<I', os_ts) + bytes([int_src]) + sub_payload


class TestInitDone:
    def test_decode_init_done(self) -> None:
        payload = _make_internal_payload(os_ts=1234, int_src=0, sub_payload=b'\x03')
        result = decode_internal_frame(payload)
        assert result is not None
        assert result['int_src'] == InternalSource.INIT_DONE
        assert result['version'] == 3
        assert result['os_ts_ms'] == 1234


class TestInfo:
    def test_decode_info(self) -> None:
        payload = _make_internal_payload(os_ts=5678, int_src=3, sub_payload=b'\x03')
        result = decode_internal_frame(payload)
        assert result is not None
        assert result['int_src'] == InternalSource.INFO
        assert result['version'] == 3


class TestEnhStat:
    def test_decode_enh_stat(self) -> None:
        sub = struct.pack('<BIIII', 2, 100, 5, 4096, 256)  # src=2, written=100, lost=5, ...
        payload = _make_internal_payload(os_ts=9999, int_src=2, sub_payload=sub)
        result = decode_internal_frame(payload)
        assert result is not None
        assert result['int_src'] == InternalSource.ENH_STAT
        assert result['src_code'] == 2
        assert result['written_frame_cnt'] == 100
        assert result['lost_frame_cnt'] == 5
        assert result['written_bytes_cnt'] == 4096
        assert result['lost_bytes_cnt'] == 256


class TestFlush:
    def test_decode_flush(self) -> None:
        payload = _make_internal_payload(os_ts=0, int_src=4, sub_payload=b'\x03')
        result = decode_internal_frame(payload)
        assert result is not None
        assert result['int_src'] == InternalSource.FLUSH
        assert result['version'] == 3


class TestTs:
    def test_ts_ignored(self) -> None:
        sub = struct.pack('<BIII', 1, 100, 200, 300)  # io_level, lc_ts, esp_ts, os_ts
        payload = _make_internal_payload(os_ts=0, int_src=1, sub_payload=sub)
        result = decode_internal_frame(payload)
        assert result is None  # TS is ignored


class TestUnknown:
    def test_unknown_int_src(self) -> None:
        payload = _make_internal_payload(os_ts=0, int_src=99, sub_payload=b'\x00')
        result = decode_internal_frame(payload)
        assert result is None


class TestMalformed:
    def test_too_short_payload(self) -> None:
        result = decode_internal_frame(b'\x00\x00\x00')
        assert result is None
