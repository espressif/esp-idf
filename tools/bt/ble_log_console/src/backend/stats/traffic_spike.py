# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Real-time traffic spike detection using a sliding window over wall-clock time."""

import time
from collections import deque
from dataclasses import dataclass

from src.backend.models import SourceCode

TRAFFIC_WINDOW_SEC = 0.1  # 100ms detection window
TRAFFIC_THRESHOLD_PCT = 0.8  # 80% of wire max
TRAFFIC_ALERT_COOLDOWN_SEC = 2.0  # minimum interval between alerts


@dataclass(slots=True)
class TrafficSpikeResult:
    throughput_kbs: float
    wire_max_kbs: float
    utilization_pct: float
    duration_ms: float
    per_source: dict[SourceCode, float]


class TrafficSpikeDetector:
    """Detects traffic spikes exceeding a percentage of theoretical wire capacity."""

    def __init__(self) -> None:
        self._wire_max_bps: float = 0.0
        self._window: deque[tuple[float, int, SourceCode]] = deque()
        self._spike_active = False
        self._spike_start: float = 0.0
        self._spike_peak_bps: float = 0.0
        self._spike_per_source: dict[SourceCode, int] = {}
        self._last_alert_time: float = 0.0

    def set_wire_max_bps(self, wire_max_bps: float) -> None:
        self._wire_max_bps = wire_max_bps

    def record(self, frame_size: int, src_code: SourceCode) -> None:
        self._window.append((time.perf_counter(), frame_size, src_code))
        if self._spike_active:
            self._spike_per_source[src_code] = self._spike_per_source.get(src_code, 0) + frame_size

    def check(self) -> TrafficSpikeResult | None:
        now = time.perf_counter()
        window = self._window
        cutoff = now - TRAFFIC_WINDOW_SEC

        while window and window[0][0] < cutoff:
            window.popleft()

        if self._wire_max_bps <= 0:
            return None

        window_bytes = sum(b for _, b, _ in window)
        throughput_bps = window_bytes / TRAFFIC_WINDOW_SEC
        utilization = throughput_bps / self._wire_max_bps

        if utilization >= TRAFFIC_THRESHOLD_PCT:
            if not self._spike_active:
                self._spike_active = True
                self._spike_start = now
                self._spike_peak_bps = 0.0
                self._spike_per_source = {}
                for _, b, src in window:
                    self._spike_per_source[src] = self._spike_per_source.get(src, 0) + b
            if throughput_bps > self._spike_peak_bps:
                self._spike_peak_bps = throughput_bps
            return None

        if not self._spike_active:
            return None

        self._spike_active = False
        duration_ms = (now - self._spike_start) * 1000.0

        if now - self._last_alert_time < TRAFFIC_ALERT_COOLDOWN_SEC:
            return None

        self._last_alert_time = now

        spike_bps = self._spike_peak_bps
        src_total = max(sum(self._spike_per_source.values()), 1)
        src_pcts = {src: v / src_total * 100.0 for src, v in self._spike_per_source.items()}

        return TrafficSpikeResult(
            throughput_kbs=spike_bps / 1024.0,
            wire_max_kbs=self._wire_max_bps / 1024.0,
            utilization_pct=spike_bps / self._wire_max_bps * 100.0,
            duration_ms=duration_ms,
            per_source=src_pcts,
        )
