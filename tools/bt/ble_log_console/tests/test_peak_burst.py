# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from src.backend.stats.peak_burst import WRITE_RATE_WINDOW_MS
from src.backend.stats.peak_burst import PeakBurstTracker
from src.backend.stats.peak_burst import _ts_delta_ms

_SRC = 1
_SRC_B = 2


class TestTsDeltaMs:
    def test_normal_forward(self) -> None:
        assert _ts_delta_ms(1100, 1000) == 100

    def test_zero_delta(self) -> None:
        assert _ts_delta_ms(5000, 5000) == 0

    def test_wraparound(self) -> None:
        assert _ts_delta_ms(50, 0xFFFF_FF00) == 306

    def test_backward_jump_returns_negative(self) -> None:
        assert _ts_delta_ms(1000, 0x8000_0100) == -1


class TestPeakBurstTracker:
    def test_single_frame(self) -> None:
        t = PeakBurstTracker()
        t.record(1000, 100, _SRC)
        snap = t.harvest()
        assert snap.per_source is not None
        assert snap.per_source[_SRC].peak_frames == 1

    def test_two_frames_same_ms(self) -> None:
        t = PeakBurstTracker()
        t.record(1000, 50, _SRC)
        t.record(1000, 70, _SRC)
        snap = t.harvest()
        assert snap.per_source is not None
        assert snap.per_source[_SRC].peak_frames == 2
        assert snap.per_source[_SRC].peak_bytes == 120

    def test_far_apart_are_separate_windows(self) -> None:
        t = PeakBurstTracker()
        t.record(100, 60, _SRC)
        t.record(100 + WRITE_RATE_WINDOW_MS, 40, _SRC)
        snap = t.harvest()
        assert snap.per_source is not None
        assert snap.per_source[_SRC].peak_frames == 1

    def test_multi_source_independent_peaks(self) -> None:
        t = PeakBurstTracker()
        for _ in range(5):
            t.record(1000, 30, _SRC)
        t.record(1000, 30, _SRC_B)
        for _ in range(4):
            t.record(2000, 30, _SRC_B)
        snap = t.harvest()
        assert snap.per_source is not None
        assert snap.per_source[_SRC].peak_frames == 5
        assert snap.per_source[_SRC_B].peak_frames == 4

    def test_max_persists_across_harvests(self) -> None:
        t = PeakBurstTracker()
        for _ in range(3):
            t.record(1000, 100, _SRC)
        snap1 = t.harvest()
        assert snap1.max_per_source is not None
        assert snap1.max_per_source[_SRC].peak_frames == 3

        t.record(5000, 200, _SRC)
        snap2 = t.harvest()
        assert snap2.max_per_source is not None
        assert snap2.max_per_source[_SRC].peak_frames == 3

    def test_harvest_resets_current_period(self) -> None:
        t = PeakBurstTracker()
        t.record(1000, 100, _SRC)
        t.harvest()
        snap = t.harvest()
        assert snap.per_source is None

    def test_backward_timestamp_resets_window(self) -> None:
        t = PeakBurstTracker()
        t.record(5000, 80, _SRC)
        t.record(5000, 80, _SRC)
        t.record(100, 80, _SRC)
        snap = t.harvest()
        assert snap.per_source is not None
        assert snap.per_source[_SRC].peak_frames == 2

    def test_wraparound_within_window(self) -> None:
        t = PeakBurstTracker()
        t.record(0xFFFF_FFFF, 50, _SRC)
        t.record(0, 50, _SRC)
        snap = t.harvest()
        assert snap.per_source is not None
        assert snap.per_source[_SRC].peak_frames == 2
