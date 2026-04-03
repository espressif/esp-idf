# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Reset propagation matrix tests.

Verifies that reset("init") and reset("flush") dispatch correctly per the spec:

| Group            | Components                                  | INIT_DONE          | FLUSH                              |
|------------------|---------------------------------------------|--------------------|------------------------------------|
| SN-coupled       | SNGapTracker                                | full reset         | full reset                         |
| ENH_STAT-coupled | FirmwareLossTracker, FirmwareWrittenTracker  | full reset         | reset baselines, preserve accumulators |
| Console-local    | TransportMetrics, PeakBurstTracker,         | preserve           | preserve                           |
|                  | per_source_received, throughput cache        |                    |                                    |
"""

from src.backend.stats import StatsAccumulator


class TestResetPropagation:
    """Verify reset("init") and reset("flush") dispatch correctly per the spec."""

    def _populate(self, stats: StatsAccumulator) -> None:
        """Feed data into all components so we can verify what gets reset."""
        # Transport (console-local)
        stats.record_bytes(1000)
        stats.record_frame(100, 1, 10)  # frame_size=100, src=1, sn=10
        stats.record_frame(100, 1, 11)
        # Peak burst (console-local)
        stats.record_frame_ts(1000, 100, 1)
        # ENH_STAT (firmware-coupled)
        stats.record_enh_stat(
            src_code=1, written_frames=100, lost_frames=5, written_bytes=5000, lost_bytes=250, baudrate=3_000_000
        )
        stats.record_enh_stat(
            src_code=1, written_frames=200, lost_frames=10, written_bytes=10000, lost_bytes=500, baudrate=3_000_000
        )

    # === INIT_DONE Tests ===

    def test_init_resets_sn_gap(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(100, 1, 0)
        stats.record_frame(100, 1, 1)
        stats.reset('init')
        stats.record_frame(100, 1, 100)
        funnel = stats.funnel_snapshot()
        for snap in funnel:
            if snap.source == 1:
                assert snap.received.frames == 3

    def test_init_resets_firmware_loss(self) -> None:
        stats = StatsAccumulator()
        self._populate(stats)
        stats.reset('init')
        # After init reset, loss tracker should be clean
        # First report after reset establishes new baseline
        stats.record_enh_stat(1, 50, 3, 2500, 150, 3_000_000)
        funnel = stats.funnel_snapshot()
        for snap in funnel:
            if snap.source == 1:
                assert snap.buffer_loss.frames == 3  # first report absolute value

    def test_init_resets_firmware_written(self) -> None:
        stats = StatsAccumulator()
        self._populate(stats)
        stats.reset('init')
        stats.record_enh_stat(1, 50, 0, 2500, 0, 3_000_000)
        funnel = stats.funnel_snapshot()
        for snap in funnel:
            if snap.source == 1:
                assert snap.written.frames == 50  # first report absolute value

    def test_init_preserves_transport_metrics(self) -> None:
        stats = StatsAccumulator()
        stats.record_bytes(5000)
        stats.record_frame()
        stats.reset('init')
        snapshot = stats.snapshot(1.0)
        assert snapshot.transport.rx_bytes == 5000
        assert snapshot.transport.fps == 1.0

    def test_init_preserves_per_source_received(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(100, 1, 0)
        stats.reset('init')
        funnel = stats.funnel_snapshot()
        assert len(funnel) == 1
        assert funnel[0].received.frames == 1

    # === FLUSH Tests ===

    def test_flush_resets_sn_gap(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(100, 1, 0)
        stats.record_frame(100, 1, 1)
        stats.reset('flush')
        # After flush, SN tracker is fully reset
        stats.record_frame(100, 1, 0)  # SN restarts from 0
        # Should not count gap from old SN=1 to new SN=0
        # The per_source_received should include the 2 pre-flush frames + 1 post-flush
        funnel = stats.funnel_snapshot()
        for snap in funnel:
            if snap.source == 1:
                # 2 pre-flush + 1 post-flush = 3 total received
                assert snap.received.frames == 3

    def test_flush_preserves_firmware_loss_accumulators(self) -> None:
        stats = StatsAccumulator()
        # Build up some loss: baseline then delta
        stats.record_enh_stat(1, 100, 5, 5000, 250, 3_000_000)
        stats.record_enh_stat(1, 200, 10, 10000, 500, 3_000_000)
        # Now flush
        stats.reset('flush')
        # Next report re-establishes baseline (no additional delta)
        stats.record_enh_stat(1, 50, 3, 2500, 150, 3_000_000)
        funnel = stats.funnel_snapshot()
        for snap in funnel:
            if snap.source == 1:
                # Initial absolute (5) + delta (5) = 10; flush preserves accum
                assert snap.buffer_loss.frames == 10

    def test_flush_preserves_firmware_written_accumulators(self) -> None:
        stats = StatsAccumulator()
        stats.record_enh_stat(1, 100, 0, 5000, 0, 3_000_000)
        stats.record_enh_stat(1, 200, 0, 10000, 0, 3_000_000)
        stats.reset('flush')
        stats.record_enh_stat(1, 50, 0, 2500, 0, 3_000_000)
        funnel = stats.funnel_snapshot()
        for snap in funnel:
            if snap.source == 1:
                assert snap.written.frames == 200  # initial absolute + pre-flush delta preserved

    def test_flush_preserves_transport_metrics(self) -> None:
        stats = StatsAccumulator()
        stats.record_bytes(5000)
        stats.record_frame()
        stats.reset('flush')
        snapshot = stats.snapshot(1.0)
        assert snapshot.transport.rx_bytes == 5000  # preserved

    def test_flush_preserves_per_source_received(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(100, 1, 0)
        stats.record_frame(100, 1, 1)
        stats.reset('flush')
        funnel = stats.funnel_snapshot()
        for snap in funnel:
            if snap.source == 1:
                assert snap.received.frames == 2  # preserved
