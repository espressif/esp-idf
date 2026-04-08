# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from src.backend.models import FrameByteCount
from src.backend.models import FunnelSnapshot
from src.backend.models import LossType
from src.backend.models import ThroughputInfo


def test_frame_byte_count() -> None:
    fbc = FrameByteCount(frames=100, bytes=5000)
    assert fbc.frames == 100
    assert fbc.bytes == 5000


def test_loss_type_enum() -> None:
    assert LossType.BUFFER == 'buffer'
    assert LossType.TRANSPORT == 'transport'


def test_funnel_snapshot_structure() -> None:
    zero = FrameByteCount(frames=0, bytes=0)
    tp = ThroughputInfo(
        throughput_fps=0.0, throughput_bps=0.0, peak_write_frames=0, peak_write_bytes=0, peak_window_ms=10
    )
    snap = FunnelSnapshot(
        source=0,
        produced=zero,
        written=zero,
        received=zero,
        buffer_loss=zero,
        transport_loss=zero,
        throughput=tp,
    )
    assert snap.produced.frames == 0
