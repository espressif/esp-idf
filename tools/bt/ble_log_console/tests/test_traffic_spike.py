# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from unittest.mock import patch

from src.backend.stats.traffic_spike import TRAFFIC_ALERT_COOLDOWN_SEC
from src.backend.stats.traffic_spike import TRAFFIC_WINDOW_SEC
from src.backend.stats.traffic_spike import TrafficSpikeDetector
from src.backend.stats.traffic_spike import TrafficSpikeResult


def _make_detector(baudrate: int = 3_000_000) -> TrafficSpikeDetector:
    d = TrafficSpikeDetector()
    d.set_wire_max_bps(baudrate / 10)
    return d


def _trigger_spike(
    d: TrafficSpikeDetector, mock_time: object, t: float, hot_bytes: int, src: int = 1
) -> TrafficSpikeResult | None:
    mock_time.perf_counter.return_value = t  # type: ignore[attr-defined]
    d.record(hot_bytes, src)
    mock_time.perf_counter.return_value = t + 0.05  # type: ignore[attr-defined]
    d.check()
    mock_time.perf_counter.return_value = t + TRAFFIC_WINDOW_SEC + 0.01  # type: ignore[attr-defined]
    return d.check()


class TestTrafficSpikeDetector:
    def test_no_spike_below_threshold(self) -> None:
        d = _make_detector()
        wire_max_bps = 300_000
        safe_bytes = int(wire_max_bps * 0.5 * TRAFFIC_WINDOW_SEC)
        t = 1000.0
        with patch('src.backend.stats.traffic_spike.time') as mock_time:
            mock_time.perf_counter.return_value = t
            d.record(safe_bytes, 1)
            mock_time.perf_counter.return_value = t + TRAFFIC_WINDOW_SEC + 0.001
            assert d.check() is None

    def test_spike_on_exit(self) -> None:
        d = _make_detector()
        hot_bytes = int(300_000 * 0.9 * TRAFFIC_WINDOW_SEC)
        t = 1000.0
        with patch('src.backend.stats.traffic_spike.time') as mock_time:
            result = _trigger_spike(d, mock_time, t, hot_bytes)
            assert result is not None
            assert result.duration_ms > 0

    def test_cooldown(self) -> None:
        d = _make_detector()
        hot_bytes = int(300_000 * 0.9 * TRAFFIC_WINDOW_SEC)
        t = 1000.0
        with patch('src.backend.stats.traffic_spike.time') as mock_time:
            first = _trigger_spike(d, mock_time, t, hot_bytes)
            assert first is not None
            second = _trigger_spike(d, mock_time, t + 0.5, hot_bytes)
            assert second is None

    def test_alert_after_cooldown(self) -> None:
        d = _make_detector()
        hot_bytes = int(300_000 * 0.9 * TRAFFIC_WINDOW_SEC)
        t = 1000.0
        with patch('src.backend.stats.traffic_spike.time') as mock_time:
            first = _trigger_spike(d, mock_time, t, hot_bytes)
            assert first is not None
            t2 = t + TRAFFIC_ALERT_COOLDOWN_SEC + 1.0
            second = _trigger_spike(d, mock_time, t2, hot_bytes)
            assert second is not None

    def test_no_wire_max_disables(self) -> None:
        d = TrafficSpikeDetector()
        t = 1000.0
        with patch('src.backend.stats.traffic_spike.time') as mock_time:
            mock_time.perf_counter.return_value = t
            d.record(999999, 1)
            mock_time.perf_counter.return_value = t + TRAFFIC_WINDOW_SEC + 0.01
            assert d.check() is None
