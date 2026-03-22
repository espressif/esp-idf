# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import struct
from dataclasses import dataclass
from dataclasses import field
from enum import Enum
from pathlib import Path
from typing import TypedDict

from textual.message import Message

# --- Frame constants (Spec Section 7) ---

FRAME_HEADER_SIZE = 6  # 2B payload_len + 4B frame_meta
FRAME_TAIL_SIZE = 4  # 4B checksum
FRAME_OVERHEAD = FRAME_HEADER_SIZE + FRAME_TAIL_SIZE  # 10
MAX_FRAME_SIZE = 2048  # Max payload_len sanity check
MAX_REMAINDER_SIZE = 131072  # 128KB bounded buffer
HEADER_FMT = '<HI'  # payload_len (uint16), frame_meta (uint32)
CHECKSUM_FMT = '<I'  # checksum (uint32)
HEADER_STRUCT = struct.Struct(HEADER_FMT)
CHECKSUM_STRUCT = struct.Struct(CHECKSUM_FMT)


# --- Formatting helpers ---


def format_bytes(cnt: int) -> str:
    """Format byte count as human-readable string (B / KB / MB)."""
    if cnt < 1024:
        return f'{cnt} B'
    elif cnt < 1024 * 1024:
        return f'{cnt / 1024:.1f} KB'
    else:
        return f'{cnt / 1024 / 1024:.2f} MB'


def format_throughput(bytes_per_sec: float) -> str:
    """Format throughput as human-readable string with auto KB/s ↔ MB/s switching."""
    kb_per_sec = bytes_per_sec / 1024
    if kb_per_sec < 1024:
        return f'{kb_per_sec:.1f} KB/s'
    else:
        return f'{kb_per_sec / 1024:.2f} MB/s'


# --- Enums ---


class SyncState(str, Enum):
    SEARCHING = 'SEARCHING'
    CONFIRMING_SYNC = 'CONFIRMING'
    SYNCED = 'SYNCED'
    CONFIRMING_LOSS = 'CONFIRMING_LOSS'


class ChecksumAlgorithm(str, Enum):
    XOR = 'XOR'
    SUM = 'Sum'


class ChecksumScope(str, Enum):
    FULL = 'Header+Payload'
    HEADER_ONLY = 'Header'


class BleLogSource(int, Enum):
    INTERNAL = 0
    CUSTOM = 1
    LL_TASK = 2
    LL_HCI = 3
    LL_ISR = 4
    HOST = 5
    HCI = 6
    ENCODE = 7
    REDIR = 8  # BLE_LOG_SRC_REDIR in firmware ble_log.h (UART PORT 0 only)


# Type alias for source code values (BleLogSource member or unknown firmware code).
SourceCode = int


# Sources written via ble_log_write_hex_ll() or stream_write -- no 4-byte os_ts prefix.
_NO_OS_TS_SOURCES: frozenset[int] = frozenset(
    {BleLogSource.LL_TASK, BleLogSource.LL_HCI, BleLogSource.LL_ISR, BleLogSource.REDIR}
)


_LL_SOURCES: frozenset[int] = frozenset({BleLogSource.LL_TASK, BleLogSource.LL_HCI, BleLogSource.LL_ISR})

LL_TS_OFFSET = 2  # lc_ts starts at payload[2:6]
LL_TS_SIZE = 4


def has_os_ts(source_code: int) -> bool:
    """Return True if frames from this source carry a valid os_ts prefix."""
    return source_code not in _NO_OS_TS_SOURCES


def is_ll_source(source_code: int) -> bool:
    """Return True if this is a Link Layer source with lc_ts timestamp."""
    return source_code in _LL_SOURCES


def resolve_source_name(src_code: int) -> str:
    """Resolve source code to BleLogSource name, with fallback for unknown codes."""
    try:
        return str(BleLogSource(src_code).name)
    except ValueError:
        return f'SRC_{src_code}'


class InternalSource(int, Enum):
    INIT_DONE = 0
    TS = 1
    ENH_STAT = 2
    INFO = 3
    FLUSH = 4


# --- Data classes ---


@dataclass(slots=True)
class ChecksumMode:
    algorithm: ChecksumAlgorithm
    scope: ChecksumScope


@dataclass(slots=True)
class ParsedFrame:
    source_code: int
    frame_sn: int
    payload: bytes  # includes os_ts prefix for ble_log_write_hex() frames
    os_ts_ms: int  # extracted from first 4 bytes of payload; only valid when has_os_ts(source_code) is True


@dataclass(slots=True)
class SourcePeakWrite:
    """Peak write burst for a single source within a 1ms window."""

    peak_frames: int = 0  # max frame count in any 1ms window
    peak_bytes: int = 0  # total bytes in that same window


@dataclass(slots=True)
class SourceStats:
    """Console-side accumulated per-source statistics (resilient to firmware counter resets)."""

    written_frames: int = 0
    written_bytes: int = 0
    lost_frames: int = 0
    lost_bytes: int = 0


@dataclass(slots=True)
class TransportSnapshot:
    """Snapshot of transport-layer metrics for the current stats interval."""

    rx_bytes: int = 0
    bps: float = 0.0
    max_bps: float = 0.0
    fps: float = 0.0


@dataclass(slots=True)
class LossSnapshot:
    """Snapshot of firmware-reported cumulative loss."""

    total_frames: int = 0
    total_bytes: int = 0


@dataclass(slots=True)
class PeakBurstSnapshot:
    """Peak write burst metrics for a single clock domain (os_ts or lc_ts)."""

    per_source: dict[SourceCode, SourcePeakWrite] | None = None
    max_per_source: dict[SourceCode, SourcePeakWrite] | None = None


class LossType(str, Enum):
    BUFFER = 'buffer'  # firmware buffer full, frame dropped
    TRANSPORT = 'transport'  # UART/link loss


@dataclass(frozen=True)
class FrameByteCount:
    """A (frames, bytes) pair."""

    frames: int
    bytes: int


@dataclass(frozen=True)
class ThroughputInfo:
    """Rate metrics (frames/s and bytes/s)."""

    throughput_fps: float  # current console receive rate (rolling 1s window)
    throughput_bps: float  # current console receive byte rate
    peak_write_frames: int  # raw frame count in densest burst window
    peak_write_bytes: int  # raw byte count in that burst window
    peak_window_ms: int  # burst window size in ms


@dataclass(frozen=True)
class FunnelSnapshot:
    """Per-source three-layer funnel snapshot."""

    source: int  # SourceCode

    # Three-layer funnel
    produced: FrameByteCount  # Layer 0: written + buffer_loss
    written: FrameByteCount  # Layer 1: from ENH_STAT
    received: FrameByteCount  # Layer 2: console-side counting

    # Loss breakdown
    buffer_loss: FrameByteCount  # from ENH_STAT lost counts
    transport_loss: FrameByteCount  # max(0, written - received)

    # Rate
    throughput: ThroughputInfo


@dataclass(slots=True)
class LaunchConfig:
    """Configuration returned by the Launch Screen."""

    port: str
    baudrate: int
    log_dir: Path


@dataclass(slots=True)
class FrameStats:
    """Periodic stats snapshot with metrics grouped by dimension."""

    transport: TransportSnapshot = field(default_factory=TransportSnapshot)
    loss: LossSnapshot = field(default_factory=LossSnapshot)
    os_peak: PeakBurstSnapshot = field(default_factory=PeakBurstSnapshot)
    ll_peak: PeakBurstSnapshot = field(default_factory=PeakBurstSnapshot)
    per_source_rx_bytes: dict[SourceCode, int] | None = None
    sync_state: SyncState = SyncState.SEARCHING
    checksum_algorithm: ChecksumAlgorithm | None = None
    checksum_scope: ChecksumScope | None = None


# --- TypedDicts for internal decoder results ---


class InfoResult(TypedDict):
    int_src: InternalSource
    version: int
    os_ts_ms: int


class EnhStatResult(TypedDict):
    int_src: InternalSource
    src_code: int
    written_frame_cnt: int
    lost_frame_cnt: int
    written_bytes_cnt: int
    lost_bytes_cnt: int
    os_ts_ms: int


InternalDecoderResult = InfoResult | EnhStatResult


# --- Textual Messages (backend -> frontend) ---


class SyncStateChanged(Message):
    def __init__(self, state: SyncState) -> None:
        super().__init__()
        self.state = state


class StatsUpdated(Message):
    def __init__(self, stats: FrameStats, funnel_snapshots: list[FunnelSnapshot] | None = None) -> None:
        super().__init__()
        self.stats = stats
        self.funnel_snapshots = funnel_snapshots or []


class InternalFrameDecoded(Message):
    def __init__(self, int_src: InternalSource, payload: InternalDecoderResult) -> None:
        super().__init__()
        self.int_src = int_src
        self.payload = payload


class LogLine(Message):
    def __init__(self, text: str) -> None:
        super().__init__()
        self.text = text


class FrameLossDetected(Message):
    def __init__(
        self,
        source_name: str,
        loss_type: LossType,
        lost_frames: int,
        lost_bytes: int,
        sn_range: tuple[int, int] | None = None,
    ) -> None:
        super().__init__()
        self.source_name = source_name
        self.loss_type = loss_type
        self.lost_frames = lost_frames
        self.lost_bytes = lost_bytes
        self.sn_range = sn_range


class BackendStopped(Message):
    def __init__(self, reason: str = '') -> None:
        super().__init__()
        self.reason = reason


class TrafficSpikeDetected(Message):
    def __init__(
        self,
        throughput_kbs: float,
        wire_max_kbs: float,
        utilization_pct: float,
        duration_ms: float,
        per_source: dict[int, float],
    ) -> None:
        super().__init__()
        self.throughput_kbs = throughput_kbs
        self.wire_max_kbs = wire_max_kbs
        self.utilization_pct = utilization_pct
        self.duration_ms = duration_ms
        self.per_source = per_source
