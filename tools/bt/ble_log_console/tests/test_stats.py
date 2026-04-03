# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from unittest.mock import patch

from src.backend.models import BleLogSource
from src.backend.models import has_os_ts
from src.backend.stats import TRAFFIC_ALERT_COOLDOWN_SEC
from src.backend.stats import TRAFFIC_THRESHOLD_PCT
from src.backend.stats import TRAFFIC_WINDOW_SEC
from src.backend.stats import WRITE_RATE_WINDOW_MS
from src.backend.stats import StatsAccumulator
from src.backend.stats import TrafficSpikeResult
from src.backend.stats.peak_burst import _ts_delta_ms

# Convenience: default frame size used in peak write tests (arbitrary but consistent)
_FRAME_SZ = 100
_SRC = 1  # default source code for single-source tests


class TestStatsAccumulator:
    def test_initial_state(self) -> None:
        stats = StatsAccumulator()
        snapshot = stats.snapshot(0.25)
        assert snapshot.transport.rx_bytes == 0
        assert snapshot.loss.total_frames == 0
        assert snapshot.loss.total_bytes == 0
        assert snapshot.os_peak.per_source is None
        assert snapshot.os_peak.max_per_source is None

    def test_record_bytes(self) -> None:
        stats = StatsAccumulator()
        stats.record_bytes(1024)
        snapshot = stats.snapshot(1.0)
        assert snapshot.transport.rx_bytes == 1024
        # bps = 1024 * 10 / 1.0 = 10240
        assert snapshot.transport.bps == 10240.0

    def test_record_frame(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame()
        stats.record_frame()
        snapshot = stats.snapshot(1.0)
        assert snapshot.transport.fps == 2.0

    def test_max_bps_tracked(self) -> None:
        stats = StatsAccumulator()
        stats.record_bytes(10000)
        stats.snapshot(1.0)  # bps = 100000
        stats.record_bytes(100)
        snap2 = stats.snapshot(1.0)  # bps = 1000
        assert snap2.transport.max_bps == 100000.0

    def _enh_stat_loss(
        self, stats: StatsAccumulator, src_code: int, lost_frames: int, lost_bytes: int
    ) -> tuple[int, int]:
        """Helper: call record_enh_stat with dummy written/baudrate, return loss delta."""
        return stats.record_enh_stat(  # type: ignore[no-any-return]
            src_code=src_code,
            written_frames=0,
            lost_frames=lost_frames,
            written_bytes=0,
            lost_bytes=lost_bytes,
            baudrate=3_000_000,
        )

    def test_firmware_loss_first_report_zero_delta(self) -> None:
        """First ENH_STAT initializes prev (delta=0); subsequent reports show delta."""
        stats = StatsAccumulator()
        new_frames, new_bytes = self._enh_stat_loss(stats, src_code=1, lost_frames=1000, lost_bytes=5000)
        assert new_frames == 0
        assert new_bytes == 0
        snapshot = stats.snapshot(0.25)
        assert snapshot.loss.total_frames == 1000
        assert snapshot.loss.total_bytes == 5000

        new_frames, new_bytes = self._enh_stat_loss(stats, src_code=1, lost_frames=1003, lost_bytes=5128)
        assert new_frames == 3
        assert new_bytes == 128
        snapshot = stats.snapshot(0.25)
        assert snapshot.loss.total_frames == 1003
        assert snapshot.loss.total_bytes == 5128

    def test_firmware_loss_incremental_returns(self) -> None:
        """Incremental return reflects per-report delta, not cumulative."""
        stats = StatsAccumulator()
        self._enh_stat_loss(stats, src_code=1, lost_frames=0, lost_bytes=0)

        new_frames, new_bytes = self._enh_stat_loss(stats, src_code=1, lost_frames=5, lost_bytes=200)
        assert new_frames == 5
        assert new_bytes == 200

        new_frames, new_bytes = self._enh_stat_loss(stats, src_code=1, lost_frames=8, lost_bytes=320)
        assert new_frames == 3
        assert new_bytes == 120

    def test_multi_source_firmware_loss(self) -> None:
        """Firmware loss tracked independently per source code."""
        stats = StatsAccumulator()
        self._enh_stat_loss(stats, src_code=1, lost_frames=100, lost_bytes=1000)
        self._enh_stat_loss(stats, src_code=2, lost_frames=50, lost_bytes=500)
        self._enh_stat_loss(stats, src_code=1, lost_frames=105, lost_bytes=1200)
        self._enh_stat_loss(stats, src_code=2, lost_frames=52, lost_bytes=580)
        snapshot = stats.snapshot(0.25)
        assert snapshot.loss.total_frames == 157  # 100 + 50 + 5 + 2
        assert snapshot.loss.total_bytes == 1780  # 1000 + 500 + 200 + 80

    def test_firmware_loss_counter_reset(self) -> None:
        """Counter reset (bench_reset_stat) detected and handled correctly."""
        stats = StatsAccumulator()
        self._enh_stat_loss(stats, src_code=1, lost_frames=0, lost_bytes=0)
        self._enh_stat_loss(stats, src_code=1, lost_frames=100, lost_bytes=4000)

        new_frames, new_bytes = self._enh_stat_loss(stats, src_code=1, lost_frames=30, lost_bytes=1200)
        assert new_frames == 0
        assert new_bytes == 0

        snapshot = stats.snapshot(0.25)
        assert snapshot.loss.total_frames == 130
        assert snapshot.loss.total_bytes == 5200

        new_frames, new_bytes = self._enh_stat_loss(stats, src_code=1, lost_frames=50, lost_bytes=2000)
        assert new_frames == 20
        assert new_bytes == 800
        snapshot = stats.snapshot(0.25)
        assert snapshot.loss.total_frames == 150
        assert snapshot.loss.total_bytes == 6000

    def test_firmware_loss_multiple_resets(self) -> None:
        """Multiple resets accumulate correctly across all cycles."""
        stats = StatsAccumulator()
        self._enh_stat_loss(stats, src_code=1, lost_frames=0, lost_bytes=0)
        self._enh_stat_loss(stats, src_code=1, lost_frames=50, lost_bytes=2000)

        self._enh_stat_loss(stats, src_code=1, lost_frames=10, lost_bytes=400)
        self._enh_stat_loss(stats, src_code=1, lost_frames=30, lost_bytes=1200)

        self._enh_stat_loss(stats, src_code=1, lost_frames=5, lost_bytes=200)

        snapshot = stats.snapshot(0.25)
        assert snapshot.loss.total_frames == 85
        assert snapshot.loss.total_bytes == 3400

    def test_firmware_loss_uint32_overflow_treated_as_reset(self) -> None:
        """uint32 counter overflow is indistinguishable from reset -- handled same way."""
        stats = StatsAccumulator()
        self._enh_stat_loss(stats, src_code=1, lost_frames=0xFFFF_FF00, lost_bytes=0)

        new_frames, _ = self._enh_stat_loss(stats, src_code=1, lost_frames=50, lost_bytes=0)
        assert new_frames == 0

        snapshot = stats.snapshot(0.25)
        assert snapshot.loss.total_frames == 0xFFFF_FF00 + 50


class TestRecordFrameWithSN:
    def test_backward_compatible_no_args(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame()
        snapshot = stats.snapshot(1.0)
        assert snapshot.transport.fps == 1.0

    def test_tracks_per_source_received(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(frame_size=100, src_code=1, frame_sn=0)
        stats.record_frame(frame_size=200, src_code=1, frame_sn=1)
        stats.record_frame(frame_size=50, src_code=2, frame_sn=0)
        assert stats._per_source_received_frames[1] == 2
        assert stats._per_source_received_bytes[1] == 300
        assert stats._per_source_received_frames[2] == 1
        assert stats._per_source_received_bytes[2] == 50

    def test_sn_gap_tracked(self) -> None:
        stats = StatsAccumulator()
        stats.set_firmware_version(4)  # enable SN gap tracking (requires version >= 4)
        stats.record_frame(frame_size=100, src_code=1, frame_sn=0)
        # SN=257 is beyond the reorder window (256), forcing SN=1 to be confirmed lost
        stats.record_frame(frame_size=100, src_code=1, frame_sn=257)
        assert stats._sn_gap.totals() == {1: 1}

    def test_no_sn_tracking_when_sn_negative(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(frame_size=100, src_code=1, frame_sn=-1)
        assert 1 not in stats._per_source_received_frames
        snapshot = stats.snapshot(1.0)
        assert snapshot.transport.fps == 1.0

    def test_no_sn_tracking_when_src_zero(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(frame_size=100, src_code=0, frame_sn=5)
        assert 0 not in stats._per_source_received_frames


class TestRecordEnhStat:
    def test_feeds_both_trackers(self) -> None:
        stats = StatsAccumulator()
        stats.record_enh_stat(
            src_code=1, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=1, written_frames=100, lost_frames=10, written_bytes=5000, lost_bytes=500, baudrate=3_000_000
        )
        written = stats._fw_written.totals()
        assert written[1] == (100, 5000)
        loss = stats._fw_loss.per_source_totals()
        assert loss[1] == (10, 500)

    def test_returns_loss_delta(self) -> None:
        stats = StatsAccumulator()
        d_f, d_b = stats.record_enh_stat(
            src_code=1, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=3_000_000
        )
        assert (d_f, d_b) == (0, 0)

        d_f, d_b = stats.record_enh_stat(
            src_code=1, written_frames=50, lost_frames=5, written_bytes=2500, lost_bytes=250, baudrate=3_000_000
        )
        assert (d_f, d_b) == (5, 250)

    def test_torn_read_guard_rejects_implausible_written_bytes(self) -> None:
        stats = StatsAccumulator()
        baudrate = 3_000_000
        max_delta = baudrate * 2 // 10
        stats.record_enh_stat(
            src_code=1, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=baudrate
        )
        d_f, d_b = stats.record_enh_stat(
            src_code=1, written_frames=10, lost_frames=0, written_bytes=max_delta + 1, lost_bytes=0, baudrate=baudrate
        )
        assert (d_f, d_b) == (0, 0)
        assert stats._fw_written.totals()[1] == (0, 0)

    def test_torn_read_guard_rejects_implausible_lost_bytes(self) -> None:
        stats = StatsAccumulator()
        baudrate = 3_000_000
        max_delta = baudrate * 2 // 10
        stats.record_enh_stat(
            src_code=1, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=baudrate
        )
        d_f, d_b = stats.record_enh_stat(
            src_code=1, written_frames=10, lost_frames=5, written_bytes=500, lost_bytes=max_delta + 1, baudrate=baudrate
        )
        assert (d_f, d_b) == (0, 0)
        assert stats._fw_loss.per_source_totals()[1] == (0, 0)

    def test_torn_read_guard_accepts_plausible_delta(self) -> None:
        stats = StatsAccumulator()
        baudrate = 3_000_000
        max_delta = baudrate * 2 // 10
        stats.record_enh_stat(
            src_code=1, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=baudrate
        )
        d_f, d_b = stats.record_enh_stat(
            src_code=1, written_frames=10, lost_frames=2, written_bytes=max_delta, lost_bytes=100, baudrate=baudrate
        )
        assert d_f == 2
        assert d_b == 100

    def test_torn_read_recovery_uses_last_good_prev(self) -> None:
        stats = StatsAccumulator()
        baudrate = 3_000_000
        max_delta = baudrate * 2 // 10
        stats.record_enh_stat(
            src_code=1, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=baudrate
        )
        stats.record_enh_stat(
            src_code=1, written_frames=10, lost_frames=0, written_bytes=max_delta + 1, lost_bytes=0, baudrate=baudrate
        )
        d_f, d_b = stats.record_enh_stat(
            src_code=1, written_frames=20, lost_frames=3, written_bytes=1000, lost_bytes=150, baudrate=baudrate
        )
        assert d_f == 3
        assert d_b == 150


class TestRecordFrameReturnsGap:
    def test_returns_zero_for_sequential_frames(self) -> None:
        stats = StatsAccumulator()
        stats.set_firmware_version(4)
        assert stats.record_frame(frame_size=100, src_code=1, frame_sn=0) == 0
        assert stats.record_frame(frame_size=100, src_code=1, frame_sn=1) == 0

    def test_returns_gap_count_for_large_jump(self) -> None:
        stats = StatsAccumulator()
        stats.set_firmware_version(4)
        stats.record_frame(frame_size=100, src_code=1, frame_sn=0)
        gap = stats.record_frame(frame_size=100, src_code=1, frame_sn=300)
        assert gap > 0

    def test_returns_zero_when_no_sn_tracking(self) -> None:
        stats = StatsAccumulator()
        assert stats.record_frame(frame_size=100, src_code=1, frame_sn=-1) == 0
        assert stats.record_frame(frame_size=100, src_code=0, frame_sn=5) == 0

    def test_sn_gap_disabled_for_old_firmware(self) -> None:
        stats = StatsAccumulator()
        stats.set_firmware_version(3)
        stats.record_frame(frame_size=100, src_code=1, frame_sn=0)
        gap = stats.record_frame(frame_size=100, src_code=1, frame_sn=300)
        assert gap == 0

    def test_sn_gap_disabled_by_default(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(frame_size=100, src_code=1, frame_sn=0)
        gap = stats.record_frame(frame_size=100, src_code=1, frame_sn=300)
        assert gap == 0


class TestReset:
    def test_init_clears_firmware_preserves_console(self) -> None:
        stats = StatsAccumulator()
        stats.record_bytes(1000)
        stats.record_frame(frame_size=100, src_code=1, frame_sn=0)
        stats.record_enh_stat(
            src_code=1, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=1, written_frames=50, lost_frames=5, written_bytes=2500, lost_bytes=250, baudrate=3_000_000
        )
        stats.reset('init')

        snapshot = stats.snapshot(1.0)
        assert snapshot.transport.rx_bytes == 1000
        assert snapshot.loss.total_frames == 0
        assert stats._per_source_received_frames == {1: 1}
        assert stats._per_source_received_bytes == {1: 100}
        assert stats._fw_written.totals() == {}

    def test_flush_resets_baselines_only(self) -> None:
        stats = StatsAccumulator()
        stats.record_bytes(1000)
        stats.record_enh_stat(
            src_code=1, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=1, written_frames=50, lost_frames=5, written_bytes=2500, lost_bytes=250, baudrate=3_000_000
        )
        stats.record_frame(frame_size=100, src_code=1, frame_sn=0)

        stats.reset('flush')

        # Console-local data preserved
        snapshot = stats.snapshot(1.0)
        assert snapshot.transport.rx_bytes == 1000
        assert stats._per_source_received_bytes == {1: 100}

        # Loss accumulators preserved but baselines reset
        assert snapshot.loss.total_frames == 5

        # Next ENH_STAT re-baselines (first report = 0 delta)
        d_f, d_b = stats.record_enh_stat(
            src_code=1, written_frames=100, lost_frames=10, written_bytes=5000, lost_bytes=500, baudrate=3_000_000
        )
        assert (d_f, d_b) == (0, 0)


class TestFunnelSnapshot:
    def test_empty(self) -> None:
        stats = StatsAccumulator()
        assert stats.funnel_snapshot() == []

    def test_single_source_full_data(self) -> None:
        stats = StatsAccumulator()
        stats.record_enh_stat(
            src_code=1, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=1, written_frames=100, lost_frames=10, written_bytes=5000, lost_bytes=500, baudrate=3_000_000
        )
        stats.record_frame(frame_size=80, src_code=1, frame_sn=0)
        stats.record_frame(frame_size=80, src_code=1, frame_sn=1)

        stats.funnel_snapshot()  # establishes prev_written baseline
        funnels = stats.funnel_snapshot()
        assert len(funnels) == 1
        f = funnels[0]
        assert f.source == 1
        assert f.written.frames == 100
        assert f.written.bytes == 5000
        assert f.buffer_loss.frames == 10
        assert f.buffer_loss.bytes == 500
        assert f.produced.frames == 110
        assert f.produced.bytes == 5500
        assert f.received.frames == 2
        assert f.received.bytes == 160
        assert f.transport_loss.frames == 98
        assert f.transport_loss.bytes == 4840

    def test_transport_loss_zero_on_first_snapshot(self) -> None:
        stats = StatsAccumulator()
        stats.record_enh_stat(
            src_code=1, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=1, written_frames=100, lost_frames=0, written_bytes=5000, lost_bytes=0, baudrate=3_000_000
        )
        stats.record_frame(frame_size=80, src_code=1, frame_sn=0)
        funnels = stats.funnel_snapshot()
        assert funnels[0].transport_loss.frames == 0

    def test_transport_loss_stable_after_written_jump(self) -> None:
        stats = StatsAccumulator()
        stats.record_enh_stat(
            src_code=1, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=1, written_frames=50, lost_frames=0, written_bytes=2500, lost_bytes=0, baudrate=3_000_000
        )
        for i in range(50):
            stats.record_frame(frame_size=50, src_code=1, frame_sn=i)
        stats.funnel_snapshot()  # prev_written = {1: (50, 2500)}
        stats.record_enh_stat(
            src_code=1, written_frames=100, lost_frames=0, written_bytes=5000, lost_bytes=0, baudrate=3_000_000
        )
        for i in range(49):
            stats.record_frame(frame_size=50, src_code=1, frame_sn=50 + i)
        funnels = stats.funnel_snapshot()
        assert funnels[0].transport_loss.frames == 0

    def test_multi_source(self) -> None:
        stats = StatsAccumulator()
        stats.record_enh_stat(
            src_code=1, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=2, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=1, written_frames=50, lost_frames=5, written_bytes=2500, lost_bytes=250, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=2, written_frames=30, lost_frames=2, written_bytes=1500, lost_bytes=100, baudrate=3_000_000
        )
        funnels = stats.funnel_snapshot()
        assert len(funnels) == 2
        assert funnels[0].source == 1
        assert funnels[1].source == 2
        assert funnels[0].written.frames == 50
        assert funnels[1].written.frames == 30

    def test_throughput_lifetime_average(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(frame_size=100, src_code=1, frame_sn=0)
        stats.record_frame(frame_size=100, src_code=1, frame_sn=1)
        stats.record_frame(frame_size=100, src_code=1, frame_sn=2)
        funnels = stats.funnel_snapshot(elapsed_sec=1.0)
        assert funnels[0].throughput.throughput_fps == 3.0
        assert funnels[0].throughput.throughput_bps == 300.0

    def test_throughput_accumulates_across_snapshots(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(frame_size=100, src_code=1, frame_sn=0)
        stats.record_frame(frame_size=100, src_code=1, frame_sn=1)
        stats.funnel_snapshot(elapsed_sec=1.0)
        stats.record_frame(frame_size=200, src_code=1, frame_sn=2)
        funnels = stats.funnel_snapshot(elapsed_sec=1.0)
        assert funnels[0].throughput.throughput_fps == 1.5
        assert funnels[0].throughput.throughput_bps == 200.0

    def test_peak_write_from_burst_tracker(self) -> None:
        stats = StatsAccumulator()
        for i in range(5):
            stats.record_frame_ts(1000, 80, 1)
        stats.snapshot(0.25)
        stats.record_frame(frame_size=80, src_code=1, frame_sn=0)
        funnels = stats.funnel_snapshot()
        assert len(funnels) == 1
        assert funnels[0].throughput.peak_write_frames == 5
        assert funnels[0].throughput.peak_write_bytes == 5 * 80
        assert funnels[0].throughput.peak_window_ms == WRITE_RATE_WINDOW_MS

    def test_throughput_zero_without_elapsed(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(frame_size=100, src_code=1, frame_sn=0)
        funnels = stats.funnel_snapshot()
        assert funnels[0].throughput.throughput_fps == 0.0
        assert funnels[0].throughput.throughput_bps == 0.0


class TestFunnelExcludesInternal:
    def test_internal_only_returns_empty(self) -> None:
        stats = StatsAccumulator()
        stats.record_enh_stat(
            src_code=0, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=0, written_frames=50, lost_frames=0, written_bytes=2500, lost_bytes=0, baudrate=3_000_000
        )
        funnels = stats.funnel_snapshot()
        assert funnels == []

    def test_internal_excluded_alongside_others(self) -> None:
        stats = StatsAccumulator()
        stats.record_enh_stat(
            src_code=0, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=0, written_frames=50, lost_frames=0, written_bytes=2500, lost_bytes=0, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=1, written_frames=0, lost_frames=0, written_bytes=0, lost_bytes=0, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=1, written_frames=100, lost_frames=0, written_bytes=5000, lost_bytes=0, baudrate=3_000_000
        )
        funnels = stats.funnel_snapshot()
        assert len(funnels) == 1
        assert funnels[0].source == 1


class TestHasOsTs:
    def test_sources_with_os_ts(self) -> None:
        assert has_os_ts(BleLogSource.INTERNAL) is True
        assert has_os_ts(BleLogSource.CUSTOM) is True
        assert has_os_ts(BleLogSource.HOST) is True
        assert has_os_ts(BleLogSource.HCI) is True
        assert has_os_ts(BleLogSource.ENCODE) is True

    def test_sources_without_os_ts(self) -> None:
        assert has_os_ts(BleLogSource.LL_TASK) is False
        assert has_os_ts(BleLogSource.LL_HCI) is False
        assert has_os_ts(BleLogSource.LL_ISR) is False
        assert has_os_ts(BleLogSource.REDIR) is False


class TestTsDeltaMs:
    def test_normal_forward(self) -> None:
        assert _ts_delta_ms(1100, 1000) == 100

    def test_zero_delta(self) -> None:
        assert _ts_delta_ms(5000, 5000) == 0

    def test_wraparound(self) -> None:
        # uint32 wraps: newer=50, older=0xFFFFFF00 -> delta=0x100+50=306
        assert _ts_delta_ms(50, 0xFFFF_FF00) == 306

    def test_backward_jump_returns_negative(self) -> None:
        # older > newer by a large amount -> detected as backward
        assert _ts_delta_ms(1000, 0x8000_0100) == -1


class TestPeakWriteBurst:
    """Tests for sliding window peak write burst (count + bytes)."""

    def test_single_frame_counts_as_peak(self) -> None:
        """A single frame in window -> peak_write_count=1."""
        stats = StatsAccumulator()
        stats.record_frame_ts(1000, _FRAME_SZ, _SRC)
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source[_SRC].peak_frames == 1
        assert snapshot.os_peak.per_source[_SRC].peak_bytes == _FRAME_SZ

    def test_two_frames_same_ms(self) -> None:
        """Two frames at same timestamp -> both in window -> count=2."""
        stats = StatsAccumulator()
        stats.record_frame_ts(1000, 50, _SRC)
        stats.record_frame_ts(1000, 70, _SRC)
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source[_SRC].peak_frames == 2
        assert snapshot.os_peak.per_source[_SRC].peak_bytes == 120

    def test_adjacent_ms_within_window(self) -> None:
        """Frames at ts=100 and ts=101 (delta=1 < WRITE_RATE_WINDOW_MS) are in the same window."""
        stats = StatsAccumulator()
        stats.record_frame_ts(100, 60, _SRC)
        stats.record_frame_ts(101, 40, _SRC)
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source[_SRC].peak_frames == 2
        assert snapshot.os_peak.per_source[_SRC].peak_bytes == 100

    def test_far_apart_ms_are_separate_windows(self) -> None:
        """Frames with delta >= WRITE_RATE_WINDOW_MS are in separate windows."""
        stats = StatsAccumulator()
        stats.record_frame_ts(100, 60, _SRC)
        stats.record_frame_ts(100 + WRITE_RATE_WINDOW_MS, 40, _SRC)
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source[_SRC].peak_frames == 1
        assert snapshot.os_peak.per_source[_SRC].peak_bytes == 60

    def test_burst_same_timestamp(self) -> None:
        """Many frames at the same ms -> all in window."""
        stats = StatsAccumulator()
        for _ in range(10):
            stats.record_frame_ts(5000, 32, _SRC)
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source[_SRC].peak_frames == 10
        assert snapshot.os_peak.per_source[_SRC].peak_bytes == 320

    def test_peak_captures_densest_burst(self) -> None:
        """Sparse phase (far apart) then dense phase (same ms) -> peak from dense."""
        stats = StatsAccumulator()
        # Sparse: 3 frames at 0, 10, 20 ms -- each alone in its 1ms window
        for i in range(3):
            stats.record_frame_ts(1000 + i * 10, 50, _SRC)
        # Dense: 5 frames all at 2000 ms
        for _ in range(5):
            stats.record_frame_ts(2000, 80, _SRC)
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source[_SRC].peak_frames == 5
        assert snapshot.os_peak.per_source[_SRC].peak_bytes == 400

    def test_max_peak_persists_across_snapshots(self) -> None:
        stats = StatsAccumulator()
        # First: 3 frames same ms
        for _ in range(3):
            stats.record_frame_ts(1000, 100, _SRC)
        snap1 = stats.snapshot(0.25)
        assert snap1.os_peak.per_source[_SRC].peak_frames == 3
        assert snap1.os_peak.per_source[_SRC].peak_bytes == 300
        assert snap1.os_peak.max_per_source[_SRC].peak_frames == 3
        assert snap1.os_peak.max_per_source[_SRC].peak_bytes == 300

        # Second: only 1 frame
        stats.record_frame_ts(5000, 200, _SRC)
        snap2 = stats.snapshot(0.25)
        assert snap2.os_peak.per_source[_SRC].peak_frames == 1
        assert snap2.os_peak.per_source[_SRC].peak_bytes == 200
        # All-time max preserved from first snapshot
        assert snap2.os_peak.max_per_source[_SRC].peak_frames == 3
        assert snap2.os_peak.max_per_source[_SRC].peak_bytes == 300

    def test_peak_resets_per_snapshot(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame_ts(1000, _FRAME_SZ, _SRC)
        stats.record_frame_ts(1000, _FRAME_SZ, _SRC)
        stats.snapshot(0.25)

        # No new frames -> peak should be None
        snap2 = stats.snapshot(0.25)
        assert snap2.os_peak.per_source is None

    def test_window_evicts_old_entries(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame_ts(0, _FRAME_SZ, _SRC)
        # Frame far beyond window -- old entry evicted, only 1 frame remains
        stats.record_frame_ts(WRITE_RATE_WINDOW_MS + 5, _FRAME_SZ, _SRC)
        snapshot = stats.snapshot(0.25)
        # Peak is still 1 (each frame alone in its window), but the best was
        # recorded when each individual frame entered.
        assert snapshot.os_peak.per_source[_SRC].peak_frames == 1

    def test_backward_timestamp_resets_window(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame_ts(5000, 80, _SRC)
        stats.record_frame_ts(5000, 80, _SRC)
        # Chip rebooted -- timestamp jumps back to near-zero
        stats.record_frame_ts(100, 80, _SRC)
        # After reset, window contains only [100]. Peak from before reset was 2.
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source[_SRC].peak_frames == 2
        assert snapshot.os_peak.per_source[_SRC].peak_bytes == 160

    def test_wraparound_same_ms_bucket(self) -> None:
        """Timestamps that wrap around uint32 but have delta=0 stay in window."""
        stats = StatsAccumulator()
        stats.record_frame_ts(0xFFFF_FFFF, 50, _SRC)
        stats.record_frame_ts(0xFFFF_FFFF, 50, _SRC)
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source[_SRC].peak_frames == 2
        assert snapshot.os_peak.per_source[_SRC].peak_bytes == 100

    def test_wraparound_within_window(self) -> None:
        """Wrap from 0xFFFFFFFF to 0 (delta=1 < WRITE_RATE_WINDOW_MS) stays in window."""
        stats = StatsAccumulator()
        stats.record_frame_ts(0xFFFF_FFFF, 50, _SRC)
        stats.record_frame_ts(0, 50, _SRC)
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source[_SRC].peak_frames == 2

    def test_wraparound_far_evicts(self) -> None:
        """Wrap with delta >= WRITE_RATE_WINDOW_MS evicts old entry."""
        stats = StatsAccumulator()
        stats.record_frame_ts(0xFFFF_FFFF, 50, _SRC)
        stats.record_frame_ts(WRITE_RATE_WINDOW_MS, 50, _SRC)
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source[_SRC].peak_frames == 1


class TestPerSourcePeak:
    """Tests for per-source peak write burst tracking."""

    def test_single_source_peak(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame_ts(1000, 50, 1)
        stats.record_frame_ts(1000, 70, 1)
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source is not None
        assert 1 in snapshot.os_peak.per_source
        assert snapshot.os_peak.per_source[1].peak_frames == 2
        assert snapshot.os_peak.per_source[1].peak_bytes == 120

    def test_multi_source_peak(self) -> None:
        """Two sources writing at same ms -- per-source counts are independent."""
        stats = StatsAccumulator()
        stats.record_frame_ts(1000, 50, 1)
        stats.record_frame_ts(1000, 30, 2)
        stats.record_frame_ts(1000, 60, 1)
        stats.record_frame_ts(1000, 40, 2)
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source is not None
        assert snapshot.os_peak.per_source[1].peak_frames == 2
        assert snapshot.os_peak.per_source[1].peak_bytes == 110
        assert snapshot.os_peak.per_source[2].peak_frames == 2
        assert snapshot.os_peak.per_source[2].peak_bytes == 70

    def test_per_source_all_time_max(self) -> None:
        stats = StatsAccumulator()
        # First burst: src 1 has 3 frames
        for _ in range(3):
            stats.record_frame_ts(1000, 40, 1)
        snap1 = stats.snapshot(0.25)
        assert snap1.os_peak.max_per_source is not None
        assert snap1.os_peak.max_per_source[1].peak_frames == 3

        # Second burst: src 1 has only 1 frame -- all-time max preserved
        stats.record_frame_ts(5000, 40, 1)
        snap2 = stats.snapshot(0.25)
        assert snap2.os_peak.max_per_source is not None
        assert snap2.os_peak.max_per_source[1].peak_frames == 3

    def test_per_source_peak_none_when_no_data(self) -> None:
        stats = StatsAccumulator()
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source is None
        assert snapshot.os_peak.max_per_source is None

    def test_per_source_independent_peak_moments(self) -> None:
        """Each source's peak is tracked even if it occurs at a different moment than global peak."""
        stats = StatsAccumulator()
        # At ts=1000: src 1 has 5 frames, src 2 has 1
        for _ in range(5):
            stats.record_frame_ts(1000, 30, 1)
        stats.record_frame_ts(1000, 30, 2)
        # At ts=2000: src 2 has 4 frames, src 1 has 0
        for _ in range(4):
            stats.record_frame_ts(2000, 30, 2)
        snapshot = stats.snapshot(0.25)
        # Global peak is 6 (at ts=1000), but per-source:
        assert snapshot.os_peak.per_source is not None
        assert snapshot.os_peak.per_source[1].peak_frames == 5  # from ts=1000
        assert snapshot.os_peak.per_source[2].peak_frames == 4  # from ts=2000


class TestLLPeakWriteBurst:
    """Tests for LL peak write burst tracking (lc_ts clock domain)."""

    def test_ll_single_source_peak(self) -> None:
        """LL frames with same lc_ts_ms are counted in one window."""
        stats = StatsAccumulator()
        for _ in range(5):
            stats.record_ll_frame_ts(1000000, 30, BleLogSource.LL_TASK)
        snapshot = stats.snapshot(0.25)
        assert snapshot.ll_peak.per_source is not None
        assert snapshot.ll_peak.per_source[BleLogSource.LL_TASK].peak_frames == 5

    def test_ll_multi_source_peak(self) -> None:
        """LL per-source peaks are tracked independently."""
        stats = StatsAccumulator()
        for _ in range(3):
            stats.record_ll_frame_ts(2000000, 20, BleLogSource.LL_TASK)
        for _ in range(7):
            stats.record_ll_frame_ts(2000000, 20, BleLogSource.LL_ISR)
        snapshot = stats.snapshot(0.25)
        assert snapshot.ll_peak.per_source is not None
        assert snapshot.ll_peak.per_source[BleLogSource.LL_TASK].peak_frames == 3
        assert snapshot.ll_peak.per_source[BleLogSource.LL_ISR].peak_frames == 7

    def test_ll_all_time_max_persists(self) -> None:
        """LL all-time peak persists across snapshots."""
        stats = StatsAccumulator()
        for _ in range(10):
            stats.record_ll_frame_ts(1000000, 30, BleLogSource.LL_HCI)
        stats.snapshot(0.25)
        for _ in range(3):
            stats.record_ll_frame_ts(2000000, 30, BleLogSource.LL_HCI)
        snapshot = stats.snapshot(0.25)
        assert snapshot.ll_peak.max_per_source is not None
        assert snapshot.ll_peak.max_per_source[BleLogSource.LL_HCI].peak_frames == 10

    def test_ll_window_separate_from_os_ts(self) -> None:
        """LL window does not interfere with os_ts window."""
        stats = StatsAccumulator()
        stats.record_frame_ts(1000, 30, BleLogSource.CUSTOM)
        stats.record_ll_frame_ts(1000000, 30, BleLogSource.LL_TASK)
        snapshot = stats.snapshot(0.25)
        assert snapshot.os_peak.per_source is not None
        assert BleLogSource.CUSTOM in snapshot.os_peak.per_source
        assert BleLogSource.LL_TASK not in snapshot.os_peak.per_source
        assert snapshot.ll_peak.per_source is not None
        assert BleLogSource.LL_TASK in snapshot.ll_peak.per_source
        assert BleLogSource.CUSTOM not in snapshot.ll_peak.per_source


class TestTrafficSpikeDetection:
    """Tests for real-time traffic spike detection."""

    def _make_stats(self, baudrate: int = 3_000_000) -> StatsAccumulator:
        stats = StatsAccumulator()
        stats.set_wire_max(baudrate)
        return stats

    def test_no_spike_below_threshold(self) -> None:
        """Traffic below 80% of wire max does not trigger spike."""
        stats = self._make_stats(3_000_000)
        wire_max_bps = 3_000_000 / 10  # 300,000 bytes/sec
        safe_bps = wire_max_bps * 0.5
        bytes_in_window = int(safe_bps * TRAFFIC_WINDOW_SEC)

        t = 1000.0
        with patch('src.backend.stats.traffic_spike.time') as mock_time:
            mock_time.perf_counter.return_value = t
            for _ in range(10):
                stats.record_frame_traffic(bytes_in_window // 10, 1)
            mock_time.perf_counter.return_value = t + TRAFFIC_WINDOW_SEC + 0.001
            assert stats.check_traffic() is None

    def test_spike_detected_on_exit(self) -> None:
        """Spike alert fires when traffic drops below threshold after exceeding it."""
        stats = self._make_stats(3_000_000)
        wire_max_bps = 3_000_000 / 10
        hot_bps = wire_max_bps * 0.9
        bytes_in_window = int(hot_bps * TRAFFIC_WINDOW_SEC)

        t = 1000.0
        with patch('src.backend.stats.traffic_spike.time') as mock_time:
            mock_time.perf_counter.return_value = t
            stats.record_frame_traffic(bytes_in_window, 1)

            mock_time.perf_counter.return_value = t + 0.05
            result = stats.check_traffic()
            assert result is None  # still in spike, no alert yet

            mock_time.perf_counter.return_value = t + TRAFFIC_WINDOW_SEC + 0.01
            result = stats.check_traffic()
            assert result is not None
            assert result.utilization_pct > TRAFFIC_THRESHOLD_PCT * 100
            assert result.duration_ms > 0
            assert result.throughput_kbs > 0

    def _trigger_spike(
        self, stats: StatsAccumulator, mock_time: object, t: float, hot_bytes: int, src: int = 1
    ) -> TrafficSpikeResult | None:
        """Helper: inject traffic, enter spike, then exit and return result."""
        mock_time.perf_counter.return_value = t  # type: ignore[attr-defined]
        stats.record_frame_traffic(hot_bytes, src)
        mock_time.perf_counter.return_value = t + 0.05  # type: ignore[attr-defined]
        stats.check_traffic()  # enter spike
        mock_time.perf_counter.return_value = t + TRAFFIC_WINDOW_SEC + 0.01  # type: ignore[attr-defined]
        return stats.check_traffic()  # exit spike -> alert

    def test_cooldown_suppresses_rapid_alerts(self) -> None:
        """Second spike within cooldown is suppressed."""
        stats = self._make_stats(3_000_000)
        wire_max_bps = 3_000_000 / 10
        hot_bytes = int(wire_max_bps * 0.9 * TRAFFIC_WINDOW_SEC)

        t = 1000.0
        with patch('src.backend.stats.traffic_spike.time') as mock_time:
            first = self._trigger_spike(stats, mock_time, t, hot_bytes)
            assert first is not None

            t2 = t + 0.5
            second = self._trigger_spike(stats, mock_time, t2, hot_bytes)
            assert second is None

    def test_alert_after_cooldown_expires(self) -> None:
        """Alert fires again after cooldown period."""
        stats = self._make_stats(3_000_000)
        wire_max_bps = 3_000_000 / 10
        hot_bytes = int(wire_max_bps * 0.9 * TRAFFIC_WINDOW_SEC)

        t = 1000.0
        with patch('src.backend.stats.traffic_spike.time') as mock_time:
            first = self._trigger_spike(stats, mock_time, t, hot_bytes)
            assert first is not None

            t2 = t + TRAFFIC_ALERT_COOLDOWN_SEC + 1.0
            second = self._trigger_spike(stats, mock_time, t2, hot_bytes)
            assert second is not None

    def test_per_source_breakdown(self) -> None:
        """Spike result includes per-source percentage breakdown."""
        stats = self._make_stats(3_000_000)
        wire_max_bps = 3_000_000 / 10
        hot_bytes = int(wire_max_bps * 0.9 * TRAFFIC_WINDOW_SEC)

        t = 1000.0
        with patch('src.backend.stats.traffic_spike.time') as mock_time:
            mock_time.perf_counter.return_value = t
            stats.record_frame_traffic(int(hot_bytes * 0.7), 1)
            stats.record_frame_traffic(int(hot_bytes * 0.3), 2)
            mock_time.perf_counter.return_value = t + 0.05
            stats.check_traffic()  # enter spike
            mock_time.perf_counter.return_value = t + TRAFFIC_WINDOW_SEC + 0.01
            result = stats.check_traffic()  # exit spike
            assert result is not None
            assert 1 in result.per_source
            assert 2 in result.per_source
            assert result.per_source[1] > result.per_source[2]

    def test_no_wire_max_disables_detection(self) -> None:
        """Traffic detection is disabled when wire max is not set."""
        stats = StatsAccumulator()
        t = 1000.0
        with patch('src.backend.stats.traffic_spike.time') as mock_time:
            mock_time.perf_counter.return_value = t
            stats.record_frame_traffic(999999, 1)
            mock_time.perf_counter.return_value = t + TRAFFIC_WINDOW_SEC + 0.01
            assert stats.check_traffic() is None
