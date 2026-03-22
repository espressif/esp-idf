# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Sliding receive window gap tracker for per-source frame sequence numbers.

Frames are only declared lost when the receive window advances past their SN
without them being received, tolerating out-of-order delivery up to
REORDER_WINDOW frames.
"""

from src.backend.models import SourceCode

SN_MAX = 1 << 24  # 24-bit SN space
REORDER_WINDOW = 256  # receive window size


class SNGapTracker:
    """Tracks per-source frame gaps using a sliding receive window."""

    def __init__(self) -> None:
        self._window_base: dict[SourceCode, int] = {}
        self._received: dict[SourceCode, set[int]] = {}
        self._gap_accum: dict[SourceCode, int] = {}

    def record(self, src_code: SourceCode, frame_sn: int) -> int:
        """Record a received frame SN and return newly confirmed gap count.

        Returns the number of SNs confirmed lost by this call (0 for in-order
        or reordered frames within the window).
        """
        if src_code not in self._window_base:
            # First frame: establish baseline
            self._window_base[src_code] = (frame_sn + 1) % SN_MAX
            self._received[src_code] = set()
            self._gap_accum[src_code] = 0
            return 0

        dist = self._distance(frame_sn, self._window_base[src_code])

        if 0 <= dist < REORDER_WINDOW:
            # Within receive window: mark received, advance base
            self._received[src_code].add(frame_sn)
            return self._advance(src_code)

        if dist >= REORDER_WINDOW:
            # Beyond window: expire old slots as confirmed gaps
            new_base = (frame_sn - REORDER_WINDOW + 1) % SN_MAX
            gaps = self._expire_to(src_code, new_base)
            self._received[src_code].add(frame_sn)
            self._advance(src_code)
            return gaps

        if dist >= -REORDER_WINDOW:
            # Behind window within tolerance: late arrival, already handled
            return 0

        # Far behind window: likely reset (FLUSH/INIT_DONE)
        self._window_base[src_code] = (frame_sn + 1) % SN_MAX
        self._received[src_code] = set()
        return 0

    def totals(self) -> dict[SourceCode, int]:
        """Return cumulative confirmed gap count per source."""
        return dict(self._gap_accum)

    def reset(self, src_code: SourceCode | None = None) -> None:
        """Reset tracker state.

        If src_code is None, resets all sources.
        Otherwise resets only the specified source.
        """
        if src_code is None:
            self._window_base.clear()
            self._received.clear()
            self._gap_accum.clear()
        else:
            self._window_base.pop(src_code, None)
            self._received.pop(src_code, None)
            self._gap_accum.pop(src_code, None)

    def _distance(self, sn: int, base: int) -> int:
        """Signed distance from base to sn in 24-bit SN space."""
        d = (sn - base) % SN_MAX
        return d if d < SN_MAX // 2 else d - SN_MAX

    def _advance(self, src_code: SourceCode) -> int:
        """Advance base past continuous received SNs."""
        while self._window_base[src_code] in self._received[src_code]:
            self._received[src_code].discard(self._window_base[src_code])
            self._window_base[src_code] = (self._window_base[src_code] + 1) % SN_MAX
        return 0

    def _expire_to(self, src_code: SourceCode, new_base: int) -> int:
        """Advance base to new_base, counting unreceived SNs as confirmed gaps."""
        gaps = 0
        while self._window_base[src_code] != new_base:
            if self._window_base[src_code] not in self._received[src_code]:
                gaps += 1
            self._received[src_code].discard(self._window_base[src_code])
            self._window_base[src_code] = (self._window_base[src_code] + 1) % SN_MAX
        self._gap_accum[src_code] += gaps
        return gaps
