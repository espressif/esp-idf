# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from src.backend.stats.buf_util import BufUtilTracker


class TestRecordAndSnapshot:
    def test_single_entry(self) -> None:
        tracker = BufUtilTracker()
        tracker.record(lbm_id=0x00, trans_cnt=4, inflight_peak=3)
        entries = tracker.snapshot()
        assert len(entries) == 1
        assert entries[0].lbm_id == 0x00
        assert entries[0].pool == 0
        assert entries[0].index == 0
        assert entries[0].trans_cnt == 4
        assert entries[0].inflight_peak == 3


class TestUpdateOverwrites:
    def test_same_lbm_id_overwrites(self) -> None:
        tracker = BufUtilTracker()
        tracker.record(lbm_id=0x01, trans_cnt=4, inflight_peak=1)
        tracker.record(lbm_id=0x01, trans_cnt=4, inflight_peak=3)
        entries = tracker.snapshot()
        assert len(entries) == 1
        assert entries[0].inflight_peak == 3


class TestResetClears:
    def test_reset_empties_tracker(self) -> None:
        tracker = BufUtilTracker()
        tracker.record(lbm_id=0x00, trans_cnt=4, inflight_peak=2)
        tracker.record(lbm_id=0x10, trans_cnt=4, inflight_peak=1)
        tracker.reset()
        assert tracker.snapshot() == []


class TestMultipleLbms:
    def test_multiple_lbm_ids_coexist(self) -> None:
        tracker = BufUtilTracker()
        tracker.record(lbm_id=0x00, trans_cnt=4, inflight_peak=1)
        tracker.record(lbm_id=0x01, trans_cnt=4, inflight_peak=2)
        tracker.record(lbm_id=0x10, trans_cnt=4, inflight_peak=3)
        tracker.record(lbm_id=0x20, trans_cnt=4, inflight_peak=4)
        entries = tracker.snapshot()
        assert len(entries) == 4

    def test_snapshot_sorted_by_pool_then_index(self) -> None:
        tracker = BufUtilTracker()
        tracker.record(lbm_id=0x21, trans_cnt=4, inflight_peak=2)
        tracker.record(lbm_id=0x00, trans_cnt=4, inflight_peak=1)
        tracker.record(lbm_id=0x20, trans_cnt=4, inflight_peak=4)
        tracker.record(lbm_id=0x10, trans_cnt=4, inflight_peak=3)
        entries = tracker.snapshot()
        pools_and_indices = [(e.pool, e.index) for e in entries]
        assert pools_and_indices == [(0, 0), (1, 0), (2, 0), (2, 1)]
