# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from src.backend.models import FrameByteCount
from src.backend.models import FunnelSnapshot
from src.backend.models import ThroughputInfo
from src.backend.models import format_throughput
from src.backend.stats import StatsAccumulator
from src.frontend.stats_screen import _build_console_table
from src.frontend.stats_screen import _build_firmware_table

_SRC_HOST = 5
_SRC_LL_TASK = 2

_ZERO = FrameByteCount(frames=0, bytes=0)
_ZERO_TP = ThroughputInfo(
    throughput_fps=0.0, throughput_bps=0.0, peak_write_frames=0, peak_write_bytes=0, peak_window_ms=10
)


def _snap(
    src,
    produced=(0, 0),
    written=(0, 0),
    received=(0, 0),
    buf_loss=(0, 0),
    tx_loss=(0, 0),
    tp_fps=0.0,
    peak_frames=0,
):
    return FunnelSnapshot(
        source=src,
        produced=FrameByteCount(*produced),
        written=FrameByteCount(*written),
        received=FrameByteCount(*received),
        buffer_loss=FrameByteCount(*buf_loss),
        transport_loss=FrameByteCount(*tx_loss),
        throughput=ThroughputInfo(
            throughput_fps=tp_fps,
            throughput_bps=0.0,
            peak_write_frames=peak_frames,
            peak_write_bytes=0,
            peak_window_ms=10,
        ),
    )


class TestFormatThroughput:
    def test_zero(self) -> None:
        assert format_throughput(0.0) == '0.0 KB/s'

    def test_small_kb(self) -> None:
        assert format_throughput(512.0) == '0.5 KB/s'

    def test_one_kb(self) -> None:
        assert format_throughput(1024.0) == '1.0 KB/s'

    def test_large_kb(self) -> None:
        assert format_throughput(500 * 1024) == '500.0 KB/s'

    def test_boundary_just_below_mb(self) -> None:
        bps = 1023.9 * 1024
        result = format_throughput(bps)
        assert 'KB/s' in result

    def test_boundary_at_mb(self) -> None:
        bps = 1024 * 1024
        assert format_throughput(bps) == '1.00 MB/s'

    def test_large_mb(self) -> None:
        bps = 2.5 * 1024 * 1024
        assert format_throughput(bps) == '2.50 MB/s'

    def test_peak_extrapolation_typical(self) -> None:
        peak_bytes_1ms = 300
        bps = peak_bytes_1ms * 1000
        result = format_throughput(bps)
        assert 'KB/s' in result

    def test_peak_extrapolation_high(self) -> None:
        peak_bytes_1ms = 1500
        bps = peak_bytes_1ms * 1000
        result = format_throughput(bps)
        assert 'MB/s' in result


class TestBuildFirmwareTable:
    def test_empty_returns_no_rows(self):
        table = _build_firmware_table([])
        assert table.row_count == 0

    def test_column_headers(self):
        table = _build_firmware_table([])
        headers = [str(col.header) for col in table.columns]
        assert 'Source' in headers
        assert any('Written' in h for h in headers)
        assert any('Loss' in h for h in headers)

    def test_single_source(self):
        snap = _snap(_SRC_HOST, written=(120, 6000))
        table = _build_firmware_table([snap])
        assert table.row_count == 1
        assert len(table.columns) == 5

    def test_with_loss_shows_red(self):
        snap = _snap(_SRC_HOST, written=(110, 5500), buf_loss=(10, 500))
        table = _build_firmware_table([snap])
        assert table.row_count == 1

    def test_multiple_sources(self):
        snaps = [
            _snap(_SRC_HOST, written=(100, 5000)),
            _snap(_SRC_LL_TASK, written=(200, 10000)),
        ]
        table = _build_firmware_table(snaps)
        assert table.row_count == 2


class TestBuildConsoleTable:
    def test_empty_returns_no_rows(self):
        table = _build_console_table([])
        assert table.row_count == 0

    def test_column_headers(self):
        table = _build_console_table([])
        headers = [str(col.header) for col in table.columns]
        assert 'Source' in headers
        assert any('Received' in h for h in headers)
        assert any('Average' in h for h in headers)
        assert any('Peak' in h for h in headers)

    def test_single_source(self):
        snap = _snap(_SRC_HOST, tp_fps=850.0, peak_frames=12)
        table = _build_console_table([snap])
        assert table.row_count == 1
        assert len(table.columns) == 7

    def test_zero_throughput_shows_dash(self):
        snap = _snap(_SRC_HOST, tp_fps=0.0, peak_frames=0)
        table = _build_console_table([snap])
        assert table.row_count == 1


class TestPerSourceRxBytes:
    def test_single_frame(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(frame_size=100, src_code=_SRC_HOST, frame_sn=0)
        snapshot = stats.snapshot(1.0)
        assert snapshot.per_source_rx_bytes == {_SRC_HOST: 100}

    def test_multiple_frames_same_source(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(frame_size=100, src_code=_SRC_HOST, frame_sn=0)
        stats.record_frame(frame_size=200, src_code=_SRC_HOST, frame_sn=1)
        snapshot = stats.snapshot(1.0)
        assert snapshot.per_source_rx_bytes == {_SRC_HOST: 300}

    def test_multiple_sources(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(frame_size=100, src_code=_SRC_HOST, frame_sn=0)
        stats.record_frame(frame_size=200, src_code=_SRC_LL_TASK, frame_sn=0)
        snapshot = stats.snapshot(1.0)
        assert snapshot.per_source_rx_bytes == {_SRC_HOST: 100, _SRC_LL_TASK: 200}

    def test_cumulative_across_snapshots(self) -> None:
        stats = StatsAccumulator()
        stats.record_frame(frame_size=100, src_code=_SRC_HOST, frame_sn=0)
        stats.snapshot(1.0)
        stats.record_frame(frame_size=200, src_code=_SRC_HOST, frame_sn=1)
        snapshot = stats.snapshot(1.0)
        assert snapshot.per_source_rx_bytes == {_SRC_HOST: 300}

    def test_none_when_no_data(self) -> None:
        stats = StatsAccumulator()
        snapshot = stats.snapshot(1.0)
        assert snapshot.per_source_rx_bytes is None
