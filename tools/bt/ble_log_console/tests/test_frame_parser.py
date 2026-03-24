# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from src.backend.checksum import sum_checksum
from src.backend.checksum import xor_checksum
from src.backend.frame_parser import FrameParser
from src.backend.models import ChecksumAlgorithm
from src.backend.models import ChecksumScope
from src.backend.models import SyncState

from tests.helpers import build_frame


def _make_sum_frame(payload: bytes, src: int, sn: int) -> bytes:
    return build_frame(payload, src, sn, sum_checksum, checksum_scope_full=True)  # type: ignore[no-any-return]


def _make_xor_frame(payload: bytes, src: int, sn: int) -> bytes:
    return build_frame(payload, src, sn, xor_checksum, checksum_scope_full=True)  # type: ignore[no-any-return]


class TestFrameParserStateTransitions:
    def test_initial_state_is_searching(self) -> None:
        parser = FrameParser()
        assert parser.sync_state == SyncState.SEARCHING

    def test_three_valid_frames_reach_synced(self) -> None:
        """N=3 consecutive valid frames should transition SEARCHING -> CONFIRMING -> SYNCED."""
        parser = FrameParser()
        payload = b'\x00' * 8  # 4B os_ts + 4B data
        frames_data = b''
        for sn in range(3):
            frames_data += _make_sum_frame(payload, src=1, sn=sn)

        parser.feed(frames_data)
        assert parser.sync_state == SyncState.SYNCED

    def test_garbage_stays_searching(self) -> None:
        parser = FrameParser()
        garbage = b'\xde\xad\xbe\xef' * 100
        parser.feed(garbage)
        assert parser.sync_state == SyncState.SEARCHING

    def test_mixed_garbage_then_valid_frames(self) -> None:
        parser = FrameParser()
        payload = b'\x00' * 8
        garbage = b'\xff' * 50
        frames = b''
        for sn in range(3):
            frames += _make_sum_frame(payload, src=1, sn=sn)

        parser.feed(garbage + frames)
        assert parser.sync_state == SyncState.SYNCED

    def test_checksum_failure_in_synced_triggers_confirming_loss(self) -> None:
        parser = FrameParser()
        payload = bytes(range(0xA0, 0xA8))
        good_frames = b''
        for sn in range(3):
            good_frames += _make_sum_frame(payload, src=1, sn=sn)
        parser.feed(good_frames)
        assert parser.sync_state == SyncState.SYNCED

        bad_frame = _make_sum_frame(payload, src=1, sn=99)
        corrupt = bytearray(bad_frame)
        corrupt[-1] ^= 0xFF
        parser.feed(bytes(corrupt))
        assert parser.sync_state == SyncState.CONFIRMING_LOSS

    def test_confirming_loss_recovers_to_synced(self) -> None:
        """After corrupt bytes, enough valid frames should re-establish SYNCED."""
        parser = FrameParser()
        payload = bytes(range(0xA0, 0xA8))
        good_frames = b''
        for sn in range(3):
            good_frames += _make_sum_frame(payload, src=1, sn=sn)
        parser.feed(good_frames)
        assert parser.sync_state == SyncState.SYNCED

        corrupt = b'\xfe' * 20
        recovery_frames = b''
        for sn in range(3, 6):
            recovery_frames += _make_sum_frame(payload, src=1, sn=sn)
        parser.feed(corrupt + recovery_frames)
        assert parser.sync_state == SyncState.SYNCED

    def test_confirming_loss_to_searching_after_m_plus_1_failures(self) -> None:
        from src.backend.frame_parser import LOSS_TOLERANCE

        parser = FrameParser()
        payload = bytes(range(0xA0, 0xA8))
        good_frames = b''
        for sn in range(3):
            good_frames += _make_sum_frame(payload, src=1, sn=sn)
        parser.feed(good_frames)
        assert parser.sync_state == SyncState.SYNCED

        garbage = b'\xfe' * (LOSS_TOLERANCE + 20)
        parser.feed(garbage)
        assert parser.sync_state == SyncState.SEARCHING

    def test_confirming_sync_rejects_mismatched_mode(self) -> None:
        """Review Correction #3: CONFIRMING_SYNC must verify same checksum mode."""
        parser = FrameParser()
        payload = b'\x00' * 8
        # Feed one SUM frame to enter CONFIRMING_SYNC
        sum_frame = _make_sum_frame(payload, src=1, sn=0)
        parser.feed(sum_frame)
        assert parser.sync_state == SyncState.CONFIRMING_SYNC

        # Feed an XOR frame — mode mismatch should restart confirmation
        xor_frame = _make_xor_frame(payload, src=1, sn=1)
        parser.feed(xor_frame)
        # Should still be in CONFIRMING_SYNC (restarted with new mode), not SYNCED
        assert parser.sync_state == SyncState.CONFIRMING_SYNC


class TestFrameParserOutput:
    def test_parsed_frames_returned(self) -> None:
        parser = FrameParser()
        payload = b'\x00\x00\x00\x00\xaa\xbb'  # 4B os_ts + 2B data
        frames_data = b''
        for sn in range(3):
            frames_data += _make_sum_frame(payload, src=2, sn=sn)

        results = parser.feed(frames_data)
        parsed = [r for r in results if hasattr(r, 'source_code')]
        assert len(parsed) == 3
        assert all(f.source_code == 2 for f in parsed)

    def test_ascii_lines_extracted_from_non_frame_data(self) -> None:
        parser = FrameParser()
        # In SEARCHING state, non-frame data should be collected as ASCII
        ascii_data = b'Hello world\n'
        results = parser.feed(ascii_data)
        lines = [r for r in results if isinstance(r, str)]
        assert any('Hello world' in line for line in lines)


def _make_sum_header_only_frame(payload: bytes, src: int, sn: int) -> bytes:
    return build_frame(payload, src, sn, sum_checksum, checksum_scope_full=False)  # type: ignore[no-any-return]


def _make_xor_header_only_frame(payload: bytes, src: int, sn: int) -> bytes:
    return build_frame(payload, src, sn, xor_checksum, checksum_scope_full=False)  # type: ignore[no-any-return]


class TestChecksumAutoDetection:
    def test_detects_sum_full(self) -> None:
        parser = FrameParser()
        payload = b'\x00' * 8
        frames = b''
        for sn in range(3):
            frames += _make_sum_frame(payload, src=1, sn=sn)

        parser.feed(frames)
        assert parser.sync_state == SyncState.SYNCED
        assert parser.checksum_mode is not None
        assert parser.checksum_mode.algorithm == ChecksumAlgorithm.SUM
        assert parser.checksum_mode.scope == ChecksumScope.FULL

    def test_detects_xor_full(self) -> None:
        parser = FrameParser()
        payload = b'\x00' * 8
        frames = b''
        for sn in range(3):
            frames += _make_xor_frame(payload, src=1, sn=sn)

        parser.feed(frames)
        assert parser.sync_state == SyncState.SYNCED
        assert parser.checksum_mode is not None
        assert parser.checksum_mode.algorithm == ChecksumAlgorithm.XOR

    def test_detects_sum_header_only(self) -> None:
        parser = FrameParser()
        payload = b'\x01\x02\x03\x04\xaa\xbb\xcc\xdd'
        frames = b''
        for sn in range(3):
            frames += _make_sum_header_only_frame(payload, src=1, sn=sn)

        parser.feed(frames)
        assert parser.sync_state == SyncState.SYNCED
        assert parser.checksum_mode is not None
        assert parser.checksum_mode.algorithm == ChecksumAlgorithm.SUM
        assert parser.checksum_mode.scope == ChecksumScope.HEADER_ONLY

    def test_detects_xor_header_only(self) -> None:
        parser = FrameParser()
        payload = b'\x01\x02\x03\x04\xaa\xbb\xcc\xdd'
        frames = b''
        for sn in range(3):
            frames += _make_xor_header_only_frame(payload, src=1, sn=sn)

        parser.feed(frames)
        assert parser.sync_state == SyncState.SYNCED
        assert parser.checksum_mode is not None
        assert parser.checksum_mode.algorithm == ChecksumAlgorithm.XOR
        assert parser.checksum_mode.scope == ChecksumScope.HEADER_ONLY


class TestBoundedBuffer:
    def test_remainder_buffer_bounded(self) -> None:
        parser = FrameParser()
        # Feed more than MAX_REMAINDER_SIZE of garbage
        huge_garbage = b'\xfe' * (131072 + 1)
        parser.feed(huge_garbage)
        # Buffer should have been reset, state should be SEARCHING
        assert parser.sync_state == SyncState.SEARCHING
        # Verify parser can still sync after overflow (buffer was cleared)
        payload = b'\x00' * 8
        frames = b''
        for sn in range(3):
            frames += _make_sum_frame(payload, src=1, sn=sn)
        parser.feed(frames)
        assert parser.sync_state == SyncState.SYNCED

    def test_buffer_overflow_emits_warning(self) -> None:
        """Review Correction #2: buffer overflow must log warning."""
        parser = FrameParser()
        huge_garbage = b'\xfe' * (131072 + 1)
        results = parser.feed(huge_garbage)
        warnings = [r for r in results if isinstance(r, str) and 'WARN' in r]
        assert len(warnings) >= 1


class TestFrameSplitAcrossChunks:
    def test_frame_split_across_chunks(self) -> None:
        """Review Correction #7: partial frames split across feed() calls."""
        parser = FrameParser()
        payload = b'\x00' * 8
        frames = b''
        for sn in range(3):
            frames += _make_sum_frame(payload, src=1, sn=sn)

        # Split in the middle of the second frame
        mid = len(frames) // 2
        parser.feed(frames[:mid])
        parser.feed(frames[mid:])
        assert parser.sync_state == SyncState.SYNCED
