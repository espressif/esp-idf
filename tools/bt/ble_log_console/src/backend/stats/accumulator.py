# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Thin composition of stats sub-modules into a single accumulator."""

from __future__ import annotations

from src.backend.models import BleLogSource
from src.backend.models import BufUtilEntry
from src.backend.models import ChecksumMode
from src.backend.models import FrameByteCount
from src.backend.models import FrameStats
from src.backend.models import FunnelSnapshot
from src.backend.models import SourceCode
from src.backend.models import SyncState
from src.backend.models import ThroughputInfo
from src.backend.stats.buf_util import BufUtilTracker
from src.backend.stats.firmware_loss import FirmwareLossTracker
from src.backend.stats.firmware_written import FirmwareWrittenTracker
from src.backend.stats.peak_burst import WRITE_RATE_WINDOW_MS
from src.backend.stats.peak_burst import PeakBurstTracker
from src.backend.stats.sn_gap import SNGapTracker
from src.backend.stats.traffic_spike import TrafficSpikeDetector
from src.backend.stats.traffic_spike import TrafficSpikeResult
from src.backend.stats.transport import UART_BITS_PER_BYTE
from src.backend.stats.transport import TransportMetrics

_ZERO = FrameByteCount(frames=0, bytes=0)

_SN_PRODUCED_MIN_VERSION = 4


class StatsAccumulator:
    def __init__(self) -> None:
        self._transport = TransportMetrics()
        self._os_burst = PeakBurstTracker()
        self._ll_burst = PeakBurstTracker()
        self._wall_burst = PeakBurstTracker()
        self._fw_loss = FirmwareLossTracker()
        self._fw_written = FirmwareWrittenTracker()
        self._sn_gap = SNGapTracker()
        self._traffic = TrafficSpikeDetector()
        self._buf_util = BufUtilTracker()
        self._per_source_received_frames: dict[SourceCode, int] = {}
        self._per_source_received_bytes: dict[SourceCode, int] = {}
        self._enh_stat_prev: dict[SourceCode, tuple[int, int, int, int]] = {}
        self._total_elapsed: float = 0.0
        self._prev_written: dict[SourceCode, tuple[int, int]] = {}
        self._sn_gap_enabled = False  # disabled until firmware version >= 4 confirmed

    def set_firmware_version(self, version: int) -> None:
        self._sn_gap_enabled = version >= _SN_PRODUCED_MIN_VERSION

    def record_bytes(self, count: int) -> None:
        self._transport.record_bytes(count)

    def record_frame(self, frame_size: int = 0, src_code: int = 0, frame_sn: int = -1) -> int:
        """Record a received frame. Returns confirmed SN gap count (0 if SN tracking disabled)."""
        self._transport.record_frame()
        gap = 0
        if frame_sn >= 0 and src_code > 0:
            if self._sn_gap_enabled:
                gap = self._sn_gap.record(src_code, frame_sn)
            self._per_source_received_frames[src_code] = self._per_source_received_frames.get(src_code, 0) + 1
            self._per_source_received_bytes[src_code] = self._per_source_received_bytes.get(src_code, 0) + frame_size
        return gap

    # -- Timestamp-based burst tracking ------------------------------------------

    def record_frame_ts(self, os_ts_ms: int, frame_size: int, src_code: SourceCode) -> None:
        self._os_burst.record(os_ts_ms, frame_size, src_code)

    def record_ll_frame_ts(self, lc_ts_us: int, frame_size: int, src_code: SourceCode) -> None:
        self._ll_burst.record(lc_ts_us // 1000, frame_size, src_code)

    def record_frame_wall_ts(self, wall_ms: int, frame_size: int, src_code: SourceCode) -> None:
        """Record frame with wall-clock timestamp for sources without chip-side timestamps."""
        self._wall_burst.record(wall_ms, frame_size, src_code)

    # -- Traffic spike -----------------------------------------------------------

    def set_wire_max(self, baudrate: int) -> None:
        self._traffic.set_wire_max_bps(baudrate / UART_BITS_PER_BYTE)

    def record_frame_traffic(self, frame_size: int, src_code: SourceCode) -> None:
        self._traffic.record(frame_size, src_code)

    def check_traffic(self) -> TrafficSpikeResult | None:
        return self._traffic.check()

    # -- Buffer utilization ------------------------------------------------------

    def record_buf_util(self, lbm_id: int, trans_cnt: int, inflight_peak: int) -> None:
        self._buf_util.record(lbm_id, trans_cnt, inflight_peak)

    def buf_util_snapshot(self) -> list[BufUtilEntry]:
        return self._buf_util.snapshot()  # type: ignore[no-any-return]

    # -- Firmware ENH_STAT -------------------------------------------------------

    def record_enh_stat(
        self,
        src_code: SourceCode,
        written_frames: int,
        lost_frames: int,
        written_bytes: int,
        lost_bytes: int,
        baudrate: int,
    ) -> tuple[int, int]:
        """Record firmware ENH_STAT report. Returns (loss_delta_frames, loss_delta_bytes).

        Torn-read guard: discards reports where byte deltas exceed 2s of wire
        capacity (non-atomic enh_stat_t reads under concurrent ISR/task updates).
        """
        prev = self._enh_stat_prev.get(src_code)
        if prev is not None:
            max_bytes_delta = baudrate * 2 // UART_BITS_PER_BYTE
            d_written_bytes = written_bytes - prev[2]
            d_lost_bytes = lost_bytes - prev[3]
            if d_written_bytes > max_bytes_delta or d_lost_bytes > max_bytes_delta:
                # Update prev to avoid cascading discards on next report
                self._enh_stat_prev[src_code] = (written_frames, lost_frames, written_bytes, lost_bytes)
                return (0, 0)

        self._enh_stat_prev[src_code] = (written_frames, lost_frames, written_bytes, lost_bytes)
        self._fw_written.record(src_code, written_frames, written_bytes)
        return self._fw_loss.record(src_code, lost_frames, lost_bytes)  # type: ignore[no-any-return]

    # -- Reset -------------------------------------------------------------------

    def reset(self, reason: str) -> None:
        """Reset components by group.

        reason: "init" (INIT_DONE) or "flush" (FLUSH)
        """
        # SN-coupled: always full reset
        self._sn_gap.reset()

        if reason == 'init':
            # ENH_STAT-coupled: full reset
            self._fw_loss.reset()
            self._fw_written.reset()
            self._enh_stat_prev.clear()
            self._prev_written.clear()
            self._buf_util.reset()
        elif reason == 'flush':
            # ENH_STAT-coupled: reset baselines only
            self._fw_loss.reset_baselines()
            self._fw_written.reset_baselines()
            self._enh_stat_prev.clear()
            # Console-local: preserve (no action)

    # -- Snapshots ---------------------------------------------------------------

    def snapshot(
        self,
        elapsed_sec: float,
        sync_state: SyncState = SyncState.SEARCHING,
        checksum_mode: ChecksumMode | None = None,
    ) -> FrameStats:
        self._wall_burst.harvest()
        return FrameStats(
            transport=self._transport.harvest(elapsed_sec),
            loss=self._fw_loss.totals(),
            os_peak=self._os_burst.harvest(),
            ll_peak=self._ll_burst.harvest(),
            per_source_rx_bytes=(dict(self._per_source_received_bytes) if self._per_source_received_bytes else None),
            sync_state=sync_state,
            checksum_algorithm=checksum_mode.algorithm if checksum_mode else None,
            checksum_scope=checksum_mode.scope if checksum_mode else None,
        )

    def funnel_snapshot(self, elapsed_sec: float = 0.0) -> list[FunnelSnapshot]:
        """Build per-source funnel snapshots from all component data."""
        written_totals = self._fw_written.totals()
        loss_totals = self._fw_loss.per_source_totals()
        os_max_peaks = self._os_burst.max_peaks()
        ll_max_peaks = self._ll_burst.max_peaks()
        wall_max_peaks = self._wall_burst.max_peaks()

        sources: set[int] = set()
        sources.update(written_totals)
        sources.update(loss_totals)
        sources.update(self._per_source_received_frames)

        # Exclude INTERNAL (src_code=0): its transport_loss is inherently
        # unknowable — if INTERNAL frames are lost, the ENH_STAT data inside
        # them never arrives, making the written-vs-received comparison circular.
        sources.discard(BleLogSource.INTERNAL)

        self._total_elapsed += elapsed_sec

        result: list[FunnelSnapshot] = []
        for src in sorted(sources):
            w_frames, w_bytes = written_totals.get(src, (0, 0))
            l_frames, l_bytes = loss_totals.get(src, (0, 0))
            r_frames = self._per_source_received_frames.get(src, 0)
            r_bytes = self._per_source_received_bytes.get(src, 0)

            produced = FrameByteCount(frames=w_frames + l_frames, bytes=w_bytes + l_bytes)
            written = FrameByteCount(frames=w_frames, bytes=w_bytes)
            received = FrameByteCount(frames=r_frames, bytes=r_bytes)
            buffer_loss = FrameByteCount(frames=l_frames, bytes=l_bytes)
            pw_frames, pw_bytes = self._prev_written.get(src, (0, 0))
            transport_loss = FrameByteCount(
                frames=max(0, pw_frames - r_frames),
                bytes=max(0, pw_bytes - r_bytes),
            )

            if self._total_elapsed > 0:
                tp_fps = r_frames / self._total_elapsed
                tp_bps = r_bytes / self._total_elapsed
            else:
                tp_fps = 0.0
                tp_bps = 0.0

            peak = os_max_peaks.get(src) or ll_max_peaks.get(src) or wall_max_peaks.get(src)
            if peak:
                peak_frames = peak.peak_frames
                peak_bytes = peak.peak_bytes
            else:
                peak_frames = 0
                peak_bytes = 0

            result.append(
                FunnelSnapshot(
                    source=src,
                    produced=produced,
                    written=written,
                    received=received,
                    buffer_loss=buffer_loss,
                    transport_loss=transport_loss,
                    throughput=ThroughputInfo(
                        throughput_fps=tp_fps,
                        throughput_bps=tp_bps,
                        peak_write_frames=peak_frames,
                        peak_write_bytes=peak_bytes,
                        peak_window_ms=WRITE_RATE_WINDOW_MS,
                    ),
                )
            )

        self._prev_written = dict(written_totals)

        return result
