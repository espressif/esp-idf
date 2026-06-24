# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Firmware ENH_STAT loss tracking with incremental delta accumulation.

Resilient to firmware counter resets from ``ble_log_bench_reset_stat``.
"""

from src.backend.models import LossSnapshot
from src.backend.models import SourceCode


class FirmwareLossTracker:
    """Tracks per-source firmware-reported loss using incremental deltas."""

    def __init__(self) -> None:
        self._frames_prev: dict[SourceCode, int] = {}
        self._bytes_prev: dict[SourceCode, int] = {}
        self._frames_accum: dict[SourceCode, int] = {}
        self._bytes_accum: dict[SourceCode, int] = {}

    def record(self, src_code: SourceCode, lost_frames: int, lost_bytes: int) -> tuple[int, int]:
        """Record firmware-reported loss.

        Returns (new_frames, new_bytes) delta since last report.
        On first report or counter reset, returns (0, 0) and suppresses alert.
        """
        if src_code not in self._frames_prev:
            self._frames_prev[src_code] = lost_frames
            self._bytes_prev[src_code] = lost_bytes
            if src_code not in self._frames_accum:
                self._frames_accum[src_code] = lost_frames
                self._bytes_accum[src_code] = lost_bytes
            return (0, 0)

        prev_frames = self._frames_prev[src_code]
        prev_bytes = self._bytes_prev[src_code]
        d_frames = lost_frames - prev_frames
        d_bytes = lost_bytes - prev_bytes

        self._frames_prev[src_code] = lost_frames
        self._bytes_prev[src_code] = lost_bytes

        if d_frames < 0 or d_bytes < 0:
            self._frames_accum[src_code] += max(0, lost_frames)
            self._bytes_accum[src_code] += max(0, lost_bytes)
            return (0, 0)

        self._frames_accum[src_code] += d_frames
        self._bytes_accum[src_code] += d_bytes
        return (d_frames, d_bytes)

    def reset(self) -> None:
        self._frames_prev.clear()
        self._bytes_prev.clear()
        self._frames_accum.clear()
        self._bytes_accum.clear()

    def reset_baselines(self) -> None:
        self._frames_prev.clear()
        self._bytes_prev.clear()

    def per_source_totals(self) -> dict[SourceCode, tuple[int, int]]:
        """Return per-source cumulative loss as {src: (frames, bytes)}."""
        return {src: (self._frames_accum[src], self._bytes_accum[src]) for src in self._frames_accum}

    def totals(self) -> LossSnapshot:
        """Return cumulative loss across all sources."""
        return LossSnapshot(
            total_frames=sum(self._frames_accum.values()),
            total_bytes=sum(self._bytes_accum.values()),
        )
