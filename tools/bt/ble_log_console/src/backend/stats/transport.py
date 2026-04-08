# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Transport-layer metrics: RX bytes, throughput (bps), frame rate (fps)."""

from src.backend.models import TransportSnapshot

UART_BITS_PER_BYTE = 10  # 8 data + 1 start + 1 stop


class TransportMetrics:
    """Tracks cumulative RX bytes and frame count with delta-based rate snapshots."""

    def __init__(self) -> None:
        self._rx_bytes = 0
        self._rx_bytes_snapshot = 0
        self._frame_count = 0
        self._frame_count_snapshot = 0
        self._max_bps = 0.0

    def record_bytes(self, count: int) -> None:
        self._rx_bytes += count

    def record_frame(self) -> None:
        self._frame_count += 1

    def harvest(self, elapsed_sec: float) -> TransportSnapshot:
        """Compute rates from deltas since last harvest, update max, and reset deltas."""
        rx_delta = self._rx_bytes - self._rx_bytes_snapshot
        frame_delta = self._frame_count - self._frame_count_snapshot

        bps = rx_delta * UART_BITS_PER_BYTE / elapsed_sec if elapsed_sec > 0 else 0.0
        fps = frame_delta / elapsed_sec if elapsed_sec > 0 else 0.0

        if bps > self._max_bps:
            self._max_bps = bps

        self._rx_bytes_snapshot = self._rx_bytes
        self._frame_count_snapshot = self._frame_count

        return TransportSnapshot(
            rx_bytes=self._rx_bytes,
            bps=bps,
            max_bps=self._max_bps,
            fps=fps,
        )
