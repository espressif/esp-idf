# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from src.backend.stats.firmware_loss import FirmwareLossTracker


class TestFirmwareLossTracker:
    def test_first_report_zero_delta(self) -> None:
        t = FirmwareLossTracker()
        new_f, new_b = t.record(src_code=1, lost_frames=1000, lost_bytes=5000)
        assert new_f == 0
        assert new_b == 0
        totals = t.totals()
        assert totals.total_frames == 1000
        assert totals.total_bytes == 5000
        per_src = t.per_source_totals()
        assert per_src[1] == (1000, 5000)

    def test_incremental_delta(self) -> None:
        t = FirmwareLossTracker()
        t.record(1, 0, 0)
        new_f, new_b = t.record(1, 5, 200)
        assert new_f == 5
        assert new_b == 200
        new_f, new_b = t.record(1, 8, 320)
        assert new_f == 3
        assert new_b == 120

    def test_multi_source(self) -> None:
        t = FirmwareLossTracker()
        t.record(1, 100, 1000)
        t.record(2, 50, 500)
        t.record(1, 105, 1200)
        t.record(2, 52, 580)
        totals = t.totals()
        assert totals.total_frames == 157
        assert totals.total_bytes == 1780

    def test_counter_reset(self) -> None:
        t = FirmwareLossTracker()
        t.record(1, 0, 0)
        t.record(1, 100, 4000)
        new_f, new_b = t.record(1, 30, 1200)
        assert new_f == 0
        totals = t.totals()
        assert totals.total_frames == 130
        assert totals.total_bytes == 5200

    def test_normal_after_reset(self) -> None:
        t = FirmwareLossTracker()
        t.record(1, 0, 0)
        t.record(1, 100, 4000)
        t.record(1, 30, 1200)
        new_f, new_b = t.record(1, 50, 2000)
        assert new_f == 20
        assert new_b == 800

    def test_reset_clears_everything(self) -> None:
        t = FirmwareLossTracker()
        t.record(1, 10, 100)
        t.reset()
        assert t.totals().total_frames == 0
        assert t.totals().total_bytes == 0

    def test_reset_baselines_preserves_accumulators(self) -> None:
        t = FirmwareLossTracker()
        t.record(1, 10, 100)
        d_frames, d_bytes = t.record(1, 15, 150)
        assert d_frames == 5
        t.reset_baselines()
        # Next report is treated as new baseline (no delta)
        d_frames, d_bytes = t.record(1, 20, 200)
        assert d_frames == 0  # baseline re-established
        # Accumulators preserved from before
        totals = t.totals()
        assert totals.total_frames == 15  # initial absolute + pre-reset delta
