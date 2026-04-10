# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Stats package -- re-exports for backward-compatible imports."""

from src.backend.stats.accumulator import StatsAccumulator
from src.backend.stats.firmware_loss import FirmwareLossTracker
from src.backend.stats.firmware_written import FirmwareWrittenTracker
from src.backend.stats.peak_burst import WRITE_RATE_WINDOW_MS
from src.backend.stats.peak_burst import PeakBurstTracker
from src.backend.stats.sn_gap import REORDER_WINDOW
from src.backend.stats.sn_gap import SN_MAX
from src.backend.stats.sn_gap import SNGapTracker
from src.backend.stats.traffic_spike import TRAFFIC_ALERT_COOLDOWN_SEC
from src.backend.stats.traffic_spike import TRAFFIC_THRESHOLD_PCT
from src.backend.stats.traffic_spike import TRAFFIC_WINDOW_SEC
from src.backend.stats.traffic_spike import TrafficSpikeDetector
from src.backend.stats.traffic_spike import TrafficSpikeResult
from src.backend.stats.transport import UART_BITS_PER_BYTE
from src.backend.stats.transport import TransportMetrics

__all__ = [
    'FirmwareLossTracker',
    'FirmwareWrittenTracker',
    'PeakBurstTracker',
    'REORDER_WINDOW',
    'SN_MAX',
    'SNGapTracker',
    'StatsAccumulator',
    'TRAFFIC_ALERT_COOLDOWN_SEC',
    'TRAFFIC_THRESHOLD_PCT',
    'TRAFFIC_WINDOW_SEC',
    'TrafficSpikeDetector',
    'TrafficSpikeResult',
    'TransportMetrics',
    'UART_BITS_PER_BYTE',
    'WRITE_RATE_WINDOW_MS',
]
