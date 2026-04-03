# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Textual App wiring backend Worker to frontend widgets.

See Spec Section 6.
"""

import struct
import threading
import time
from datetime import datetime
from pathlib import Path
from typing import cast

import serial
from textual.app import App
from textual.app import ComposeResult
from textual.binding import Binding
from textual.message import Message

from src.backend.frame_parser import FrameParser
from src.backend.internal_decoder import decode_internal_frame
from src.backend.models import FRAME_OVERHEAD
from src.backend.models import LL_TS_OFFSET
from src.backend.models import BackendStopped
from src.backend.models import BleLogSource
from src.backend.models import EnhStatResult
from src.backend.models import FrameLossDetected
from src.backend.models import FunnelSnapshot
from src.backend.models import InfoResult
from src.backend.models import InternalFrameDecoded
from src.backend.models import InternalSource
from src.backend.models import LaunchConfig
from src.backend.models import LogLine
from src.backend.models import LossType
from src.backend.models import ParsedFrame
from src.backend.models import SourcePeakWrite
from src.backend.models import StatsUpdated
from src.backend.models import SyncState
from src.backend.models import SyncStateChanged
from src.backend.models import TrafficSpikeDetected
from src.backend.models import has_os_ts
from src.backend.models import is_ll_source
from src.backend.models import resolve_source_name
from src.backend.stats import StatsAccumulator
from src.backend.uart_transport import UART_BLOCK_SIZE
from src.backend.uart_transport import open_serial
from src.frontend.launch_screen import LaunchScreen
from src.frontend.log_view import LogView
from src.frontend.shortcut_screen import ShortcutScreen
from src.frontend.stats_screen import StatsScreen
from src.frontend.status_panel import StatusPanel

STATS_INTERVAL = 0.25  # seconds


class BLELogApp(App):
    CSS = """
    Screen {
        layout: vertical;
    }
    """

    BINDINGS = [
        Binding('q', 'quit', 'Quit'),
        Binding('Q', 'quit', show=False),
        Binding('ctrl+c', 'quit', show=False, priority=True),
        Binding('c', 'clear_log', 'Clear'),
        Binding('C', 'clear_log', show=False),
        Binding('s', 'toggle_scroll', 'Auto-scroll'),
        Binding('S', 'toggle_scroll', show=False),
        Binding('d', 'dump_stats', 'Stats'),
        Binding('D', 'dump_stats', show=False),
        Binding('h', 'show_help', 'Help'),
        Binding('H', 'show_help', show=False),
        Binding('r', 'reset_chip', 'Reset'),
        Binding('R', 'reset_chip', show=False),
    ]

    def __init__(
        self,
        port: str | None = None,
        baudrate: int = 3_000_000,
        log_dir: Path | None = None,
    ) -> None:
        super().__init__()
        self._port = port
        self._baudrate = baudrate
        self._log_dir = log_dir or Path.cwd()
        self._output_path: Path | None = None
        self._serial: serial.Serial | None = None
        # All-time per-source chip write peak (updated from StatsUpdated messages)
        self._max_per_source_peak: dict[int, SourcePeakWrite] | None = None
        self._ll_max_per_source_peak: dict[int, SourcePeakWrite] | None = None
        # Console-side per-source received bytes (from StatsUpdated snapshots)
        self._per_source_rx_bytes: dict[int, int] | None = None
        self._funnel_snapshots: list[FunnelSnapshot] = []
        # Wall-clock capture start (set when backend loop begins)
        self._capture_start_time: float = 0.0
        self._serial_lock = threading.Lock()

    def compose(self) -> ComposeResult:
        yield LogView()
        yield StatusPanel()

    def on_mount(self) -> None:
        if self._port is not None:
            self._resolve_output_path()
            self.run_worker(self._backend_loop, thread=True, exclusive=True)
        else:
            self.push_screen(LaunchScreen(default_log_dir=self._log_dir), callback=self._on_launch_result)

    @property
    def funnel_snapshots(self) -> list[FunnelSnapshot]:
        """Public accessor for funnel snapshots (used by StatsScreen)."""
        return self._funnel_snapshots

    def _on_launch_result(self, config: LaunchConfig | None) -> None:
        """Handle Launch Screen dismissal."""
        if config is None:
            self.exit()
            return
        self._port = config.port
        self._baudrate = config.baudrate
        self._log_dir = config.log_dir
        self._resolve_output_path()
        self.run_worker(self._backend_loop, thread=True, exclusive=True)

    def _resolve_output_path(self) -> None:
        """Generate timestamped output file path in the log directory."""
        self._log_dir.mkdir(parents=True, exist_ok=True)
        ts = datetime.now().strftime('%Y%m%d_%H%M%S')
        self._output_path = self._log_dir / f'ble_log_{ts}.bin'

    def _post(self, msg: Message) -> None:
        """Thread-safe message posting from backend worker."""
        self.call_from_thread(self.post_message, msg)

    def _emit_stats(self, stats: StatsAccumulator, parser: FrameParser, last_time: float) -> float:
        """Emit a stats snapshot if the interval has elapsed. Returns updated timestamp."""
        now = time.perf_counter()
        if now - last_time < STATS_INTERVAL:
            return last_time

        elapsed = now - last_time
        snapshot = stats.snapshot(
            elapsed,
            sync_state=parser.sync_state,
            checksum_mode=parser.checksum_mode,
        )
        funnel = stats.funnel_snapshot(elapsed)
        self._post(StatsUpdated(snapshot, funnel))
        return now

    def _backend_loop(self) -> None:
        """Background worker: UART read -> parse -> stats -> messages."""
        if self._port is None or self._output_path is None:
            self._post(LogLine('Backend started without port/output configuration'))
            self._post(BackendStopped('Configuration missing'))
            return
        parser = FrameParser()
        stats = StatsAccumulator()
        stats.set_wire_max(self._baudrate)
        redir_line_buf = ''
        prev_sync_state = SyncState.SEARCHING
        last_snapshot_time = time.perf_counter()

        try:
            self._serial = open_serial(self._port, self._baudrate)
        except Exception as e:
            self._post(LogLine(f'Failed to open UART: {e}'))
            self._post(BackendStopped(f'Failed to open UART: {e}'))
            return

        self._capture_start_time = time.perf_counter()
        ser = self._serial
        self._post(LogLine(f'Connected to {self._port} at {self._baudrate} baud'))

        # Lazy file handles — created on first data arrival
        output_file = None
        console_log_file = None
        console_log_path = self._output_path.with_name(self._output_path.stem + '_console.log')

        try:
            while True:
                with self._serial_lock:
                    block = ser.read(UART_BLOCK_SIZE)
                if not block:
                    last_snapshot_time = self._emit_stats(stats, parser, last_snapshot_time)
                    continue

                # 1. Save raw binary (lazy-open on first block)
                if output_file is None:
                    output_file = open(self._output_path, 'wb')  # noqa: SIM115
                    self._post(LogLine(f'Saving to {self._output_path}'))
                output_file.write(block)
                output_file.flush()

                # 2. Track bytes
                stats.record_bytes(len(block))

                # 3. Parse frames
                results = parser.feed(block)

                # 4. Check sync state transition
                if parser.sync_state != prev_sync_state:
                    self._post(SyncStateChanged(parser.sync_state))
                    prev_sync_state = parser.sync_state

                # 5. Process results
                for item in results:
                    if isinstance(item, ParsedFrame):
                        frame_size = len(item.payload) + FRAME_OVERHEAD
                        if item.source_code != BleLogSource.INTERNAL:
                            stats.record_frame(frame_size, item.source_code, item.frame_sn)
                            stats.record_frame_traffic(frame_size, item.source_code)
                        else:
                            stats.record_frame()  # count frame for transport metrics, no SN tracking
                        if has_os_ts(item.source_code) and item.source_code != BleLogSource.INTERNAL:
                            stats.record_frame_ts(item.os_ts_ms, frame_size, item.source_code)
                        elif is_ll_source(item.source_code) and len(item.payload) >= 6:
                            (lc_ts_us,) = struct.unpack_from('<I', item.payload, LL_TS_OFFSET)
                            stats.record_ll_frame_ts(lc_ts_us, frame_size, item.source_code)
                        elif item.source_code == BleLogSource.REDIR:
                            wall_ms = int(time.perf_counter() * 1000) & 0xFFFFFFFF
                            stats.record_frame_wall_ts(wall_ms, frame_size, item.source_code)

                        # Decode internal frames
                        if item.source_code == BleLogSource.INTERNAL:
                            decoded = decode_internal_frame(item.payload)
                            if decoded:
                                int_src = decoded['int_src']
                                self._post(InternalFrameDecoded(int_src, decoded))

                                if int_src in (InternalSource.INIT_DONE, InternalSource.INFO):
                                    info = cast(InfoResult, decoded)
                                    stats.set_firmware_version(info['version'])
                                if int_src == InternalSource.INIT_DONE:
                                    stats.reset('init')
                                elif int_src == InternalSource.FLUSH:
                                    stats.reset('flush')
                                elif int_src == InternalSource.ENH_STAT:
                                    enh = cast(EnhStatResult, decoded)
                                    new_frames, new_bytes = stats.record_enh_stat(
                                        src_code=enh['src_code'],
                                        written_frames=enh['written_frame_cnt'],
                                        lost_frames=enh['lost_frame_cnt'],
                                        written_bytes=enh['written_bytes_cnt'],
                                        lost_bytes=enh['lost_bytes_cnt'],
                                        baudrate=self._baudrate,
                                    )
                                    if new_frames > 0:
                                        source_name = resolve_source_name(enh['src_code'])
                                        self._post(
                                            FrameLossDetected(
                                                source_name,
                                                loss_type=LossType.BUFFER,
                                                lost_frames=new_frames,
                                                lost_bytes=new_bytes,
                                            )
                                        )

                        # Decode UART redirect frames (raw ASCII, no os_ts prefix).
                        # A single log line may span multiple frames due to
                        # batch sealing, so buffer partial lines until '\n'.
                        elif item.source_code == BleLogSource.REDIR:
                            payload_text = item.payload.decode('ascii', errors='replace')

                            # Write raw payload to console log (independent of line buffering)
                            if console_log_file is None:
                                console_log_file = open(console_log_path, 'w')  # noqa: SIM115
                            console_log_file.write(payload_text)
                            console_log_file.flush()

                            redir_line_buf += payload_text
                            while '\n' in redir_line_buf:
                                line, redir_line_buf = redir_line_buf.split('\n', 1)
                                if line:
                                    self._post(LogLine(line))

                    elif isinstance(item, str):
                        self._post(LogLine(item))

                # 6. Traffic spike detection
                spike = stats.check_traffic()
                if spike is not None:
                    self._post(
                        TrafficSpikeDetected(
                            throughput_kbs=spike.throughput_kbs,
                            wire_max_kbs=spike.wire_max_kbs,
                            utilization_pct=spike.utilization_pct,
                            duration_ms=spike.duration_ms,
                            per_source=spike.per_source,
                        )
                    )

                # 7. Periodic stats snapshot
                last_snapshot_time = self._emit_stats(stats, parser, last_snapshot_time)

        except Exception as e:
            self._post(LogLine(f'Error: {e}'))
        finally:
            ser.close()
            if output_file is not None:
                output_file.close()
            if console_log_file is not None:
                console_log_file.close()
            self._post(BackendStopped('Serial connection closed'))

    # --- Message handlers ---

    def on_sync_state_changed(self, msg: SyncStateChanged) -> None:
        log_view = self.query_one(LogView)
        log_view.write_sync(f'State: {msg.state.value}')

    def on_stats_updated(self, msg: StatsUpdated) -> None:
        panel = self.query_one(StatusPanel)
        panel.stats = msg.stats
        self._funnel_snapshots = msg.funnel_snapshots
        # Preserve all-time per-source peak for the stats screen
        if msg.stats.os_peak.max_per_source is not None:
            self._max_per_source_peak = msg.stats.os_peak.max_per_source
        if msg.stats.ll_peak.max_per_source is not None:
            self._ll_max_per_source_peak = msg.stats.ll_peak.max_per_source
        if msg.stats.per_source_rx_bytes is not None:
            self._per_source_rx_bytes = msg.stats.per_source_rx_bytes

    def on_internal_frame_decoded(self, msg: InternalFrameDecoded) -> None:
        if msg.int_src == InternalSource.INIT_DONE:
            info = cast(InfoResult, msg.payload)
            log_view = self.query_one(LogView)
            log_view.write_info(f'BLE Log v{info["version"]} initialized - statistics reset')
        elif msg.int_src == InternalSource.FLUSH:
            log_view = self.query_one(LogView)
            log_view.write_info('Firmware flush - SN counters reset')

    def on_log_line(self, msg: LogLine) -> None:
        log_view = self.query_one(LogView)
        log_view.write_ascii(msg.text)

    def on_frame_loss_detected(self, msg: FrameLossDetected) -> None:
        log_view = self.query_one(LogView)
        log_view.write_warning(
            f'Frame loss [{msg.source_name}] ({msg.loss_type.value}): {msg.lost_frames} frames, {msg.lost_bytes} bytes'
        )

    def on_backend_stopped(self, msg: BackendStopped) -> None:
        log_view = self.query_one(LogView)
        log_view.write_warning(f'Backend stopped: {msg.reason}')
        panel = self.query_one(StatusPanel)
        panel.disconnected = True

    def on_traffic_spike_detected(self, msg: TrafficSpikeDetected) -> None:
        top_sources = sorted(msg.per_source.items(), key=lambda x: x[1], reverse=True)
        src_parts = ', '.join(f'{resolve_source_name(s)} {p:.0f}%' for s, p in top_sources if p >= 1.0)
        if msg.utilization_pct >= 100.0:
            util_str = 'saturated'
        else:
            util_str = f'{msg.utilization_pct:.0f}% wire'
        log_view = self.query_one(LogView)
        log_view.write_traffic(f'{msg.throughput_kbs:.0f} KB/s ({util_str}) over {msg.duration_ms:.0f}ms | {src_parts}')

    # --- Actions ---

    def action_clear_log(self) -> None:
        self.query_one(LogView).clear()

    def action_toggle_scroll(self) -> None:
        log_view = self.query_one(LogView)
        log_view.auto_scroll = not log_view.auto_scroll

    def action_dump_stats(self) -> None:
        self.push_screen(StatsScreen(start_time=self._capture_start_time))

    def action_show_help(self) -> None:
        self.push_screen(ShortcutScreen())

    def action_reset_chip(self) -> None:
        """Reset ESP32 via DTR/RTS toggle (same sequence as esptool)."""
        ser = self._serial
        if ser is None or not ser.is_open:
            return
        with self._serial_lock:
            ser.dtr = False
            ser.rts = True
            time.sleep(0.1)
            ser.rts = False
        log_view = self.query_one(LogView)
        log_view.write_info('Chip reset triggered')
