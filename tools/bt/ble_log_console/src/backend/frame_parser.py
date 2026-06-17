# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Frame sync state machine with checksum auto-detection.

Parses BLE Log module frames from a raw byte stream.
See Spec Sections 7, 8.
"""

from collections.abc import Callable

from src.backend.checksum import sum_checksum
from src.backend.checksum import xor_checksum
from src.backend.models import CHECKSUM_STRUCT
from src.backend.models import FRAME_HEADER_SIZE
from src.backend.models import FRAME_OVERHEAD
from src.backend.models import HEADER_STRUCT
from src.backend.models import MAX_FRAME_SIZE
from src.backend.models import MAX_REMAINDER_SIZE
from src.backend.models import ChecksumAlgorithm
from src.backend.models import ChecksumMode
from src.backend.models import ChecksumScope
from src.backend.models import ParsedFrame
from src.backend.models import SyncState

ChecksumProbe = tuple[ChecksumAlgorithm, ChecksumScope, Callable[[bytes], int]]

_CHECKSUM_PROBES: list[ChecksumProbe] = [
    (ChecksumAlgorithm.XOR, ChecksumScope.FULL, xor_checksum),
    (ChecksumAlgorithm.XOR, ChecksumScope.HEADER_ONLY, xor_checksum),
    (ChecksumAlgorithm.SUM, ChecksumScope.FULL, sum_checksum),
    (ChecksumAlgorithm.SUM, ChecksumScope.HEADER_ONLY, sum_checksum),
]

SYNC_CONFIRM_THRESHOLD = 3  # N consecutive valid frames to confirm sync
LOSS_TOLERANCE = 3  # M consecutive failures before resync


class FrameParser:
    """Stateful frame parser with sync state machine and checksum auto-detection."""

    def __init__(self) -> None:
        self._remained = b''
        self._sync_state = SyncState.SEARCHING
        self._checksum_mode: ChecksumMode | None = None
        self._confirm_count = 0
        self._loss_count = 0
        self._ascii_buffer = ''
        self._ever_synced = False

    @property
    def sync_state(self) -> SyncState:
        return self._sync_state

    @property
    def checksum_mode(self) -> ChecksumMode | None:
        return self._checksum_mode

    def feed(self, data: bytes) -> list[ParsedFrame | str]:
        """Feed raw bytes into the parser.

        Returns a list of:
        - ParsedFrame for successfully parsed frames
        - str for ASCII log lines or warning messages
        """
        self._remained += data
        results: list[ParsedFrame | str] = []

        # Bounded buffer check (Review Correction #2)
        if len(self._remained) > MAX_REMAINDER_SIZE:
            self._remained = b''
            self._transition_to(SyncState.SEARCHING)
            results.append('[WARN] Buffer overflow — discarded remainder, resync')
            return results

        offset = 0
        buf = self._remained

        while offset < len(buf):
            if self._sync_state in (SyncState.SEARCHING, SyncState.CONFIRMING_SYNC):
                result = self._try_parse_with_probe(buf, offset)
                if result is not None:
                    frame, next_offset, mode = result
                    self._flush_ascii(results)
                    results.append(frame)
                    offset = next_offset
                    self._on_frame_found(mode)
                elif self._sync_state == SyncState.CONFIRMING_SYNC and self._might_be_incomplete_frame(buf, offset):
                    break
                elif (
                    self._sync_state == SyncState.SEARCHING
                    and self._ever_synced
                    and self._might_be_incomplete_frame(buf, offset)
                ):
                    break
                else:
                    if not self._ever_synced:
                        self._collect_ascii(buf[offset : offset + 1], results)
                    offset += 1
            else:
                # SYNCED or CONFIRMING_LOSS: use locked checksum mode
                result_locked = self._try_parse_locked(buf, offset)
                if result_locked is not None:
                    frame, next_offset = result_locked
                    self._flush_ascii(results)
                    results.append(frame)
                    offset = next_offset
                    self._on_frame_valid()
                else:
                    # Check if we might have incomplete data at the end
                    if self._might_be_incomplete_frame(buf, offset):
                        break

                    self._on_frame_invalid()
                    if self._sync_state == SyncState.SEARCHING:
                        # Full resync — reprocess from current offset
                        continue
                    # Silently discard — do NOT collect ASCII here.
                    # In CONFIRMING_LOSS, failed bytes are corrupt frame data,
                    # not readable text.  Collecting them would leak binary
                    # payload bytes that happen to be printable (0x20-0x7E).
                    offset += 1

        # Save remainder
        self._remained = buf[offset:] if offset < len(buf) else b''
        self._flush_ascii(results)
        return results

    def _try_parse_at(
        self,
        buf: bytes,
        offset: int,
        checksum_fn: Callable[[bytes], int],
        scope: ChecksumScope,
    ) -> tuple[ParsedFrame, int] | None:
        """Try to parse a frame at the given offset with specific checksum params."""
        if offset + FRAME_HEADER_SIZE > len(buf):
            return None

        payload_len, frame_meta = HEADER_STRUCT.unpack_from(buf, offset)

        # Sanity checks
        if payload_len > MAX_FRAME_SIZE:
            return None
        if offset + FRAME_OVERHEAD + payload_len > len(buf):
            return None

        header = buf[offset : offset + FRAME_HEADER_SIZE]
        payload = buf[offset + FRAME_HEADER_SIZE : offset + FRAME_HEADER_SIZE + payload_len]
        checksum_offset = offset + FRAME_HEADER_SIZE + payload_len
        stored_checksum = CHECKSUM_STRUCT.unpack_from(buf, checksum_offset)[0]

        # Compute checksum
        if scope == ChecksumScope.FULL:
            checksum_data = header + payload
        else:
            checksum_data = header

        computed = checksum_fn(checksum_data)
        if computed != stored_checksum:
            return None

        source_code = frame_meta & 0xFF
        frame_sn = frame_meta >> 8

        # Extract os_ts from first 4 bytes of payload
        os_ts_ms = 0
        if payload_len >= 4:
            os_ts_ms = int.from_bytes(payload[:4], 'little')

        frame = ParsedFrame(
            source_code=source_code,
            frame_sn=frame_sn,
            payload=payload,
            os_ts_ms=os_ts_ms,
        )
        next_offset = offset + FRAME_OVERHEAD + payload_len
        return frame, next_offset

    def _try_parse_with_probe(self, buf: bytes, offset: int) -> tuple[ParsedFrame, int, ChecksumMode] | None:
        """Try all checksum combinations at the given offset (SEARCHING mode)."""
        for algo, scope, fn in _CHECKSUM_PROBES:
            result = self._try_parse_at(buf, offset, fn, scope)
            if result is not None:
                frame, next_offset = result
                mode = ChecksumMode(algo, scope)
                return frame, next_offset, mode
        return None

    def _try_parse_locked(self, buf: bytes, offset: int) -> tuple[ParsedFrame, int] | None:
        """Try to parse with the locked checksum mode."""
        if self._checksum_mode is None:
            return None
        fn = xor_checksum if self._checksum_mode.algorithm == ChecksumAlgorithm.XOR else sum_checksum
        return self._try_parse_at(buf, offset, fn, self._checksum_mode.scope)

    def _on_frame_found(self, mode: ChecksumMode) -> None:
        """Called when a frame is found during SEARCHING/CONFIRMING_SYNC."""
        if self._sync_state == SyncState.SEARCHING:
            self._checksum_mode = mode
            self._confirm_count = 1
            self._transition_to(SyncState.CONFIRMING_SYNC)
        elif self._sync_state == SyncState.CONFIRMING_SYNC:
            # Review Correction #3: verify same checksum mode
            if (
                self._checksum_mode is not None
                and mode.algorithm == self._checksum_mode.algorithm
                and mode.scope == self._checksum_mode.scope
            ):
                self._confirm_count += 1
                if self._confirm_count >= SYNC_CONFIRM_THRESHOLD:
                    self._transition_to(SyncState.SYNCED)
            else:
                # Mode mismatch — restart confirmation with new mode
                self._checksum_mode = mode
                self._confirm_count = 1

    def _on_frame_valid(self) -> None:
        """Called when a frame passes checksum in SYNCED/CONFIRMING_LOSS."""
        self._loss_count = 0
        if self._sync_state == SyncState.CONFIRMING_LOSS:
            self._transition_to(SyncState.SYNCED)

    def _on_frame_invalid(self) -> None:
        """Called when checksum fails in SYNCED/CONFIRMING_LOSS."""
        if self._sync_state == SyncState.SYNCED:
            self._loss_count = 1
            self._transition_to(SyncState.CONFIRMING_LOSS)
        elif self._sync_state == SyncState.CONFIRMING_LOSS:
            self._loss_count += 1
            if self._loss_count > LOSS_TOLERANCE:
                self._transition_to(SyncState.SEARCHING)
                self._checksum_mode = None
                self._confirm_count = 0
                self._loss_count = 0

    def _might_be_incomplete_frame(self, buf: bytes, offset: int) -> bool:
        """Check if remaining data could be a partial frame waiting for more data."""
        remaining = len(buf) - offset
        if remaining < FRAME_OVERHEAD:
            return True
        if remaining >= FRAME_HEADER_SIZE:
            payload_len, _ = HEADER_STRUCT.unpack_from(buf, offset)
            if payload_len <= MAX_FRAME_SIZE and remaining < FRAME_OVERHEAD + payload_len:
                return True
        return False

    def _transition_to(self, new_state: SyncState) -> None:
        if new_state == SyncState.SYNCED:
            self._ever_synced = True
        self._sync_state = new_state

    def _collect_ascii(self, byte_data: bytes, results: list[ParsedFrame | str]) -> None:
        """Collect bytes for ASCII line assembly.

        Only printable ASCII (0x20-0x7E) and newline (0x0A) are collected.
        Carriage return (0x0D) and other control characters are silently
        dropped, which normalises \\r\\n line endings to \\n for display.
        """
        for b in byte_data:
            if 0x20 <= b <= 0x7E:
                self._ascii_buffer += chr(b)
            elif b == 0x0A:  # newline
                if self._ascii_buffer:
                    results.append(self._ascii_buffer)
                    self._ascii_buffer = ''

    def _flush_ascii(self, results: list[ParsedFrame | str]) -> None:
        """Flush any pending ASCII buffer."""
        if self._ascii_buffer:
            results.append(self._ascii_buffer)
            self._ascii_buffer = ''
