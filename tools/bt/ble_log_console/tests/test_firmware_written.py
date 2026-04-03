# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from src.backend.stats.firmware_written import FirmwareWrittenTracker


class TestFirmwareWrittenTracker:
    def test_first_report_zero_delta(self) -> None:
        t = FirmwareWrittenTracker()
        new_f, new_b = t.record(src_code=1, written_frames=1000, written_bytes=5000)
        assert new_f == 0
        assert new_b == 0
        totals = t.totals()
        assert totals[1] == (1000, 5000)

    def test_incremental_delta(self) -> None:
        t = FirmwareWrittenTracker()
        t.record(1, 0, 0)
        new_f, new_b = t.record(1, 5, 200)
        assert new_f == 5
        assert new_b == 200
        new_f, new_b = t.record(1, 8, 320)
        assert new_f == 3
        assert new_b == 120

    def test_multi_source(self) -> None:
        t = FirmwareWrittenTracker()
        t.record(1, 100, 1000)
        t.record(2, 50, 500)
        t.record(1, 105, 1200)
        t.record(2, 52, 580)
        totals = t.totals()
        assert totals[1] == (105, 1200)
        assert totals[2] == (52, 580)

    def test_counter_reset(self) -> None:
        t = FirmwareWrittenTracker()
        t.record(1, 0, 0)
        t.record(1, 100, 4000)
        new_f, new_b = t.record(1, 30, 1200)
        assert new_f == 0
        assert new_b == 0
        totals = t.totals()
        assert totals[1] == (130, 5200)

    def test_normal_after_reset(self) -> None:
        t = FirmwareWrittenTracker()
        t.record(1, 0, 0)
        t.record(1, 100, 4000)
        t.record(1, 30, 1200)
        new_f, new_b = t.record(1, 50, 2000)
        assert new_f == 20
        assert new_b == 800

    def test_reset_clears_all(self) -> None:
        t = FirmwareWrittenTracker()
        t.record(1, 10, 100)
        t.record(1, 20, 200)
        t.reset()
        assert t.totals() == {}
        new_f, new_b = t.record(1, 50, 500)
        assert new_f == 0
        assert new_b == 0

    def test_reset_baselines_preserves_accum(self) -> None:
        t = FirmwareWrittenTracker()
        t.record(1, 0, 0)
        t.record(1, 100, 4000)
        t.reset_baselines()
        new_f, new_b = t.record(1, 30, 1200)
        assert new_f == 0
        assert new_b == 0
        totals = t.totals()
        assert totals[1] == (100, 4000)

    def test_reset_baselines_then_incremental(self) -> None:
        t = FirmwareWrittenTracker()
        t.record(1, 0, 0)
        t.record(1, 50, 2000)
        t.reset_baselines()
        t.record(1, 10, 400)
        new_f, new_b = t.record(1, 25, 1000)
        assert new_f == 15
        assert new_b == 600
        totals = t.totals()
        assert totals[1] == (65, 2600)
