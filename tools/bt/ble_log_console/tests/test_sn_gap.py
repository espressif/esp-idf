# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from src.backend.stats.sn_gap import SNGapTracker


class TestSNGapTracker:
    def setup_method(self) -> None:
        self.tracker = SNGapTracker()

    # --- Baseline ---
    def test_first_frame_establishes_baseline(self) -> None:
        assert self.tracker.record(src_code=1, frame_sn=42) == 0

    # --- In-order ---
    def test_sequential_no_gap(self) -> None:
        self.tracker.record(1, 0)
        assert self.tracker.record(1, 1) == 0
        assert self.tracker.record(1, 2) == 0

    # --- Simple reorder (within window) ---
    def test_reorder_no_false_gap(self) -> None:
        """SN=8 arrives before SN=5,6,7 — no gaps should be counted."""
        self.tracker.record(1, 5)  # baseline → window_base=6
        assert self.tracker.record(1, 8) == 0  # within window, NOT a gap
        assert self.tracker.record(1, 6) == 0  # late fill
        assert self.tracker.record(1, 7) == 0  # late fill
        assert self.tracker.totals().get(1, 0) == 0

    # --- Confirmed loss ---
    def test_loss_confirmed_when_window_expires(self) -> None:
        """Frame beyond window forces expiry of unreceived SNs."""
        self.tracker.record(1, 0)  # baseline → base=1
        # SN=1 never arrives; jump to SN=257 (beyond window of 256)
        gaps = self.tracker.record(1, 257)
        assert gaps > 0  # SN=1 expired as confirmed loss
        assert self.tracker.totals()[1] > 0

    # --- Late arrival behind window ---
    def test_late_arrival_ignored(self) -> None:
        self.tracker.record(1, 0)
        self.tracker.record(1, 257)  # force window advance past 0
        assert self.tracker.record(1, 1) == 0  # too late, ignored

    # --- Reset detection ---
    def test_large_backward_jump_resets_baseline(self) -> None:
        self.tracker.record(1, 1000)
        # SN jumps back to 5 (far beyond REORDER_WINDOW backward)
        assert self.tracker.record(1, 5) == 0
        # After re-baseline, SN=6 should be normal
        assert self.tracker.record(1, 6) == 0

    # --- Multi-source independence ---
    def test_sources_independent(self) -> None:
        self.tracker.record(1, 10)
        self.tracker.record(2, 20)
        assert self.tracker.record(1, 11) == 0
        assert self.tracker.record(2, 21) == 0

    # --- 24-bit wraparound ---
    def test_wraparound(self) -> None:
        SN_MAX = 1 << 24
        self.tracker.record(1, SN_MAX - 2)  # base = SN_MAX-1
        assert self.tracker.record(1, SN_MAX - 1) == 0
        assert self.tracker.record(1, 0) == 0  # wraps to 0
        assert self.tracker.record(1, 1) == 0

    # --- Reset method ---
    def test_reset_clears_all(self) -> None:
        self.tracker.record(1, 10)
        self.tracker.reset()
        # After reset, next frame establishes new baseline
        assert self.tracker.record(1, 0) == 0

    def test_reset_single_source(self) -> None:
        self.tracker.record(1, 10)
        self.tracker.record(2, 20)
        self.tracker.reset(src_code=1)
        assert self.tracker.record(1, 0) == 0  # re-baselined
        assert self.tracker.record(2, 21) == 0  # unaffected
