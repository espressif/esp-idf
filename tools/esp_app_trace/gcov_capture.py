#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# GCOV Data Capture Tool
# Captures data from UART serial port, logs raw bytes in hex format, and parses/executes
# file I/O commands (gcov host file protocol) on the fly.
#
# Usage:
#   CLI mode:
#     python gcov_capture.py -p /dev/tty.usbserial-101 -b 115200 -o gcov.log -l1
#
#   Python API for pytest (single dump):
#     from gcov_capture import gcov_capture_from_uart
#
#     def test_my_gcov(dut):
#         dut.serial.close()  # Free up the port if you use the same port with the dut.serial.port
#         assert gcov_capture_from_uart(
#             port=dut.serial.port,
#             baudrate=dut.app.sdkconfig.get('APPTRACE_UART_BAUDRATE'),
#             log_file='gcov.log'
#         ), "GCOV dump timeout"
#     # Create report with idf.py gcovr-report
#     # idf.py gcovr-report # Uses default build dir
#     # idf.py -B build_esp32c5_gcov_uart gcovr-report
#
#   Python API for pytest (multiple dumps):
#     from gcov_capture import UartGcovCapture
#
#     def test_multiple_gcov_dumps(dut):
#         dut.serial.close() # Free up the port if you use the same port with the dut.serial.port
#         with UartGcovCapture(
#             port=dut.serial.port,
#             baudrate=dut.app.sdkconfig.get('APPTRACE_UART_BAUDRATE'),
#             log_file='gcov.log',
#             log_level=1
#         ) as capture:
#             capture.run(background=True)
#             time.sleep(0.5)
#             # Wait for multiple dumps
#             for i in range(3):
#                 assert capture.wait_for_fstop(timeout=30.0), f"Dump {i+1} timeout"
#                 print(f"Dump {i+1} complete, total: {capture.get_fstop_count()}")
#     # Create report with idf.py gcovr-report
#     # idf.py gcovr-report # Uses default build dir
#     # idf.py -B build_esp32c5_gcov_uart gcovr-report
#
#   Coverage verification helper (works for both JTAG and UART):
#     from gcov_capture import get_coverage_data
#     expected_gcda_paths = [...]  # List of .gcda files under build dir
#     coverage = get_coverage_data(dut.app.binary_path, expected_gcda_paths)
#     print(coverage)

import argparse
import gzip
import json
import logging
import os
import subprocess
import sys
import threading
import time
from collections.abc import Callable
from dataclasses import dataclass
from typing import Any
from typing import BinaryIO
from typing import TextIO
from typing import cast

import serial


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description='Capture GCOV UART data and process file I/O commands in real-time',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        allow_abbrev=False,
    )

    parser.add_argument('-p', '--port', required=True, help='Serial port (e.g., /dev/ttyUSB0, COM3)')
    parser.add_argument('-b', '--baudrate', type=int, required=True, help='Serial baudrate (e.g., 115200)')
    parser.add_argument('-o', '--output', help='Optional log file path (only used if log-level > 0)')
    parser.add_argument(
        '-l',
        '--log-level',
        type=int,
        default=0,
        choices=[0, 1, 2],
        help='Log level: 0=no logging, 1=REQ+RES+SUMMARY, 2=1+CONSOLE',
    )

    return parser.parse_args()


def _le_u32(data: bytes, offset: int) -> int:
    return int.from_bytes(data[offset : offset + 4], byteorder='little', signed=False)


def _le_i32(data: bytes, offset: int) -> int:
    return int.from_bytes(data[offset : offset + 4], byteorder='little', signed=True)


def _normalize_mode(mode: str) -> str | None:
    s = set(mode)
    base = s & {'r', 'w', 'a', 'x'}

    if len(base) != 1 or s - {'r', 'w', 'a', 'x', '+', 'b', 't'}:
        return None

    return next(iter(base)) + 'b' + ('+' if '+' in s else '')


@dataclass
class CommandInfo:
    """Metadata for a file I/O command."""

    code: int
    name: str
    min_payload_len: int
    needs_fd_validation: bool
    handler: Callable | None = None
    arg_extractor: Callable | None = None


class FileOpProcessor:
    """Emulates the gcov file protocol operations like esp_gcov_process_data."""

    # Common response codes (little-endian)
    RESP_ZERO = (0).to_bytes(4, 'little')  # Zero (success for most ops, error for fd/size)
    RESP_ERROR_NEG1 = (-1).to_bytes(4, 'little', signed=True)  # -1 (error for signed ops)
    RESP_FWRITE_OK = (1).to_bytes(4, 'little')  # 1 (FWRITE success)

    def __init__(self) -> None:
        self.files: dict[int, BinaryIO] = {}
        self.max_files = 1024
        self.next_fd = 1
        self.running = True

        # Unified command table with all metadata
        self._commands: dict[int, CommandInfo] = {
            0x0: CommandInfo(0x0, 'FOPEN', 0, False, None, None),  # Special handling
            0x1: CommandInfo(0x1, 'FCLOSE', 4, True, self._close, lambda p: (_le_u32(p, 0),)),
            0x2: CommandInfo(0x2, 'FWRITE', 4, True, self._write, lambda p: (_le_u32(p, 0), p[4:])),
            0x3: CommandInfo(0x3, 'FREAD', 8, True, self._read, lambda p: (_le_u32(p, 0), _le_u32(p, 4))),
            0x4: CommandInfo(
                0x4, 'FSEEK', 12, True, self._seek, lambda p: (_le_u32(p, 0), _le_i32(p, 4), _le_i32(p, 8))
            ),
            0x5: CommandInfo(0x5, 'FTELL', 4, True, self._tell, lambda p: (_le_u32(p, 0),)),
            0x6: CommandInfo(0x6, 'FSTOP', 0, False, None, None),  # Special handling
            0x7: CommandInfo(0x7, 'FEOF', 4, True, self._feof, lambda p: (_le_u32(p, 0),)),
        }

    def _alloc_fd(self) -> int:
        if self.next_fd > self.max_files:
            return 0
        fd = self.next_fd
        self.next_fd += 1
        return fd

    def _get_file(self, fd: int) -> BinaryIO | None:
        return self.files.get(fd)  # type: ignore[return-value]

    def _validate_fd(self, fd: int) -> bool:
        """Validate that FD is in valid range and exists."""
        if fd < 1 or fd > self.max_files:
            return False
        return fd in self.files

    def _close_all_files(self) -> None:
        """Close all open files and reset the files dictionary."""
        for fd, handle in list(self.files.items()):
            try:
                handle.close()
            except OSError:
                pass  # Ignore errors when closing
        self.files.clear()
        self.next_fd = 1  # Reset FD counter

    def _open(self, path: str, mode: str) -> tuple[str, bytes]:
        if len(self.files) >= self.max_files:
            return 'FOPEN failed: max files reached', self.RESP_ZERO

        fd = self._alloc_fd()
        if fd == 0:
            return 'FOPEN failed: fd alloc error', self.RESP_ZERO

        normalized = _normalize_mode(mode)
        if not normalized:
            return f"FOPEN path='{path}' invalid mode='{mode}'", self.RESP_ZERO

        try:
            # Create parent dir for write/append/create modes
            if normalized[0] in ('w', 'a', 'x'):
                dir_path = os.path.dirname(path)
                if dir_path:
                    try:
                        os.makedirs(dir_path, exist_ok=True)
                    except OSError:
                        pass  # let open() raise the real error if it's still a problem

            # Open in binary mode (gcov files are always binary)
            # Always use binary mode since gcov protocol works with binary data
            if 'b' in normalized:
                handle = cast(BinaryIO, open(path, normalized, buffering=0))
            else:
                # Force binary mode even if not specified
                handle = cast(BinaryIO, open(path, normalized[0] + 'b' + normalized[2:], buffering=0))
            self.files[fd] = handle
            return f"FOPEN path='{path}' mode='{mode}' normalized='{normalized}' -> fd={fd}", fd.to_bytes(4, 'little')
        except (OSError, ValueError) as e:
            return f"FOPEN path='{path}' mode='{mode}' failed: {e}", self.RESP_ZERO

    def _close(self, fd: int) -> tuple[str, bytes]:
        handle = self._get_file(fd)
        if not handle:
            return f'FCLOSE fd={fd} failed: not open', self.RESP_ERROR_NEG1
        try:
            handle.close()
            del self.files[fd]
            return f'FCLOSE fd={fd} -> 0', self.RESP_ZERO
        except OSError as e:
            return f'FCLOSE fd={fd} failed: {e}', self.RESP_ERROR_NEG1

    def _write(self, fd: int, payload: bytes) -> tuple[str, bytes]:
        handle = self._get_file(fd)
        if not handle:
            return f'FWRITE fd={fd} failed: not open', self.RESP_ZERO
        try:
            written = handle.write(payload)
            handle.flush()
            fret = self.RESP_FWRITE_OK if written else self.RESP_ZERO
            return (
                f'FWRITE fd={fd} len={len(payload)} -> {1 if written else 0}',
                fret,
            )
        except OSError as e:
            return f'FWRITE fd={fd} failed: {e}', self.RESP_ZERO

    def _read(self, fd: int, length: int) -> tuple[str, bytes]:
        handle = self._get_file(fd)
        if not handle:
            return f'FREAD fd={fd} failed: not open', self.RESP_ZERO
        try:
            data_bytes = handle.read(length)
            if not isinstance(data_bytes, bytes):
                # Should not happen with BinaryIO, but be defensive
                data_bytes = bytes(data_bytes)
            fret = len(data_bytes)
            resp = fret.to_bytes(4, 'little') + data_bytes
            preview = data_bytes[:16].hex(' ')
            suffix = '' if len(data_bytes) <= 16 else ' ...'
            return f'FREAD fd={fd} len={length} -> {fret} bytes "{preview}{suffix}"', resp
        except OSError as e:
            return f'FREAD fd={fd} failed: {e}', self.RESP_ZERO

    def _seek(self, fd: int, offset: int, whence: int) -> tuple[str, bytes]:
        handle = self._get_file(fd)
        if not handle:
            return f'FSEEK fd={fd} failed: not open', self.RESP_ERROR_NEG1
        try:
            fret = handle.seek(offset, whence)
            return f'FSEEK fd={fd} off={offset} whence={whence} -> {fret}', fret.to_bytes(4, 'little', signed=True)
        except OSError as e:
            return f'FSEEK fd={fd} off={offset} whence={whence} failed: {e}', self.RESP_ERROR_NEG1

    def _tell(self, fd: int) -> tuple[str, bytes]:
        handle = self._get_file(fd)
        if not handle:
            return f'FTELL fd={fd} failed: not open', self.RESP_ERROR_NEG1
        try:
            pos = handle.tell()
            return f'FTELL fd={fd} -> {pos}', pos.to_bytes(4, 'little', signed=True)
        except OSError as e:
            return f'FTELL fd={fd} failed: {e}', self.RESP_ERROR_NEG1

    def _feof(self, fd: int) -> tuple[str, bytes]:
        handle = self._get_file(fd)
        if not handle:
            return f'FEOF fd={fd} failed: not open', self.RESP_ERROR_NEG1
        try:
            cur = handle.tell()
            handle.seek(0, 2)  # end
            end = handle.tell()
            handle.seek(cur, 0)
            eof = int(cur >= end)
            return f'FEOF fd={fd} -> {eof}', eof.to_bytes(4, 'little', signed=True)
        except OSError as e:
            return f'FEOF fd={fd} failed: {e}', self.RESP_ERROR_NEG1

    def process(self, data: bytes) -> tuple[str, bytes | None]:
        """Process a single command buffer. Returns summary and optional response bytes."""
        if not data:
            return 'Empty data', None

        cmd = data[0]
        payload = data[1:]

        # Look up command info
        cmd_info = self._commands.get(cmd)
        if cmd_info is None:
            return f'Unknown command 0x{cmd:02X} (len={len(data)})', None

        # Handle FOPEN specially. Extract path and mode from payload.
        if cmd_info.name == 'FOPEN':
            null_pos = payload.find(0)
            if null_pos <= 0:
                return 'FOPEN invalid (empty or missing path)', self.RESP_ZERO
            path = payload[:null_pos].decode('utf-8', errors='replace')
            mode_start = payload[null_pos + 1 :]
            null_pos = mode_start.find(0)
            mode = mode_start[:null_pos].decode('utf-8', errors='replace') if null_pos != -1 else ''
            return self._open(path, mode)

        # Handle FSTOP specially (close all files, no response)
        if cmd_info.name == 'FSTOP':
            self._close_all_files()
            return 'FSTOP (all files closed, continuing capture)', None

        # Handle other commands via unified command table
        if len(payload) < cmd_info.min_payload_len:
            return f'{cmd_info.name} invalid (too short)', None

        # Extract arguments using the extractor function
        if cmd_info.arg_extractor is None:
            return f'{cmd_info.name} has no arg extractor', None
        args = cmd_info.arg_extractor(payload)

        if cmd_info.needs_fd_validation:
            fd: int = args[0]  # Type annotation for mypy
            if not self._validate_fd(fd):
                return f'{cmd_info.name} invalid fd={fd} (not open or out of range)', None

        # Call handler function
        if cmd_info.handler is None:
            return f'{cmd_info.name} has no handler', None
        result: tuple[str, bytes | None] = cmd_info.handler(*args)
        return result

    def _get_cmd_name(self, cmd: int) -> str:
        """Get command name for error messages."""
        cmd_info = self._commands.get(cmd)
        return cmd_info.name if cmd_info else f'0x{cmd:02X}'


def _parse_command(data: bytes, proc: FileOpProcessor) -> tuple[str, bytes | None]:
    """Process a command buffer and return summary plus optional response bytes."""
    return proc.process(data)


class FrameParser:
    """State machine parser for STX|LEN(2)|payload|ETX frames."""

    STX = 0x02
    ETX = 0x03

    # states
    WAIT_STX = 0
    WAIT_LENL = 1
    WAIT_LENH = 2
    WAIT_PAYLOAD = 3
    WAIT_ETX = 4

    # Timeout in seconds for incomplete frames (100ms)
    FRAME_TIMEOUT = 0.1

    def __init__(self) -> None:
        self.state = self.WAIT_STX
        self.length = 0
        self.payload = bytearray()
        self.frame = bytearray()
        self.state_timestamp = time.time()  # Track when we entered current state

    def reset(self) -> None:
        """Reset parser to initial state."""
        self.state = self.WAIT_STX
        self.length = 0
        self.payload.clear()
        self.frame.clear()
        self.state_timestamp = time.time()

    def check_timeout(self) -> bytes | None:
        """Check if parser has been stuck in a non-STX state for too long.

        Returns the incomplete frame bytes if timeout occurred (and resets parser),
        or None if no timeout.
        """
        # Only check timeout if we're not in WAIT_STX state
        if self.state != self.WAIT_STX:
            elapsed = time.time() - self.state_timestamp
            if elapsed > self.FRAME_TIMEOUT:
                # Timeout - save incomplete frame for caller to log
                incomplete = bytes(self.frame) if self.frame else None
                self.reset()
                return incomplete
        return None

    def feed(self, data: bytes) -> list[tuple[bytes, bytes]]:
        """Feed bytes to the parser.

        Returns list of (frame, payload) for each complete frame found.
        """
        out: list[tuple[bytes, bytes]] = []
        for byte in data:
            result = self._process_byte(byte)
            if result is not None:
                out.append(result)
        return out

    def _process_byte(self, byte: int) -> tuple[bytes, bytes] | None:
        """Process a single byte. Returns (frame, payload) when frame is complete."""
        self.frame.append(byte)

        if self.state == self.WAIT_STX:
            if byte == self.STX:
                self.state = self.WAIT_LENL
                self.payload.clear()
                self.state_timestamp = time.time()  # Update timestamp on state change
            else:
                # Not STX, reset and wait for next STX
                self.reset()

        elif self.state == self.WAIT_LENL:
            self.length = byte
            self.state = self.WAIT_LENH
            self.state_timestamp = time.time()

        elif self.state == self.WAIT_LENH:
            self.length |= byte << 8
            self.payload.clear()
            if self.length == 0:
                # There is no payload, go directly to ETX
                self.state = self.WAIT_ETX
            else:
                self.state = self.WAIT_PAYLOAD
            self.state_timestamp = time.time()

        elif self.state == self.WAIT_PAYLOAD:
            self.payload.append(byte)
            if len(self.payload) >= self.length:
                self.state = self.WAIT_ETX
                self.state_timestamp = time.time()

        elif self.state == self.WAIT_ETX:
            if byte == self.ETX:
                # Frame complete! Save the result and reset for next frame
                result = (bytes(self.frame), bytes(self.payload))
                self.reset()
                return result
            else:
                # Expected ETX but got something else, reset
                self.reset()

        return None


def _log_frame_to_file(file_desc: TextIO | None, log_level: int, frame: bytes, resp: bytes | None, summary: str) -> str:
    """Write frame data to log file based on log level and return console response string.

    Args:
        file_desc: Open file descriptor (or None to skip logging)
        log_level: Log level (1=REQ+RES+summary, 2=1+CONSOLE)
        frame: Complete frame bytes including STX/LEN/payload/ETX
        resp: Response bytes (or None)
        summary: Parse summary string

    Returns:
        Console response string to append to print output
    """
    if not file_desc or log_level <= 0:
        return ''

    hex_chunk = ' '.join(f'{byte:02X}' for byte in frame)
    resp_hex = resp.hex(' ').upper() if resp else ''
    console_resp = ''

    if log_level >= 1:
        file_desc.write(f'REQ: {hex_chunk}\r\n')
        if resp:
            file_desc.write(f'RES: {resp_hex}\n')
        if summary:
            file_desc.write(f'   : {summary}\r\n')

    if log_level >= 2 and resp:
        console_resp = f' | RES: {resp_hex}'

    file_desc.flush()
    return console_resp


class UartGcovCapture:
    """UART-based GCOV capture handler for both CLI and pytest-embedded-idf integration.

    Can run in either blocking mode (CLI) or background thread mode (pytest).
    Processes gcov file I/O commands in real-time, sending responses immediately
    to avoid blocking gcov rtio operations.

    Usage in CLI:
        capture = UartGcovCapture(port='/dev/ttyUSB0', baudrate=115200, log_file='gcov.log', log_level=2)
        capture.run(background=False)  # Blocks until Ctrl+C

    Usage in pytest:
        with UartGcovCapture(port='/dev/ttyUSB0', baudrate=115200, log_file='gcov.log', log_level=2) as capture:
            capture.run(background=True)
            # ... wait for FSTOP or perform test actions ...
            if capture.wait_for_fstop(timeout=30):
                print(f'Dump completed, count: {capture.get_fstop_count()}')
    """

    def __init__(self, port: str, baudrate: int = 115200, log_file: str | None = None, log_level: int = 2) -> None:
        """Initialize UART GCOV capture.

        Args:
            port: Serial port path (e.g., '/dev/ttyUSB0')
            baudrate: Serial baud rate (default: 115200)
            log_file: Optional path to log file for REQ/RES/SUMMARY output (default: None)
            log_level: Log level: 0=nothing, 1=REQ+RES+SUMMARY, 2=1+CONSOLE (default: 0)
        """
        self.port = port
        self.baudrate = baudrate
        self.log_file = log_file
        self.log_level = log_level
        self.ser: serial.Serial | None = None
        self.capture_thread: threading.Thread | None = None
        self.stop_event = threading.Event()
        self.proc = FileOpProcessor()
        self.parser = FrameParser()
        self.logger = logging.getLogger(__name__)
        self.file_desc: TextIO | None = None
        self.fstop_count = 0  # Track number of FSTOP commands received
        self.fstop_event = threading.Event()  # Event to signal FSTOP received
        self.total_bytes = 0  # Track total bytes captured

    def __enter__(self) -> 'UartGcovCapture':
        """Context manager entry."""
        return self

    def __exit__(self, exception_type: Any, exception_value: Any, exception_traceback: Any) -> None:
        """Context manager exit - stop capture and cleanup."""
        self.stop()

    def run(self, background: bool = True) -> 'UartGcovCapture':
        """Start UART capture.

        Args:
            background: If True, run in background thread (pytest mode).
                       If False, run in foreground blocking mode (CLI mode).

        Returns:
            self for chaining
        """
        if background:
            return self._run_background()
        else:
            return self._run_foreground()

    def _run_background(self) -> 'UartGcovCapture':
        if self.capture_thread is not None and self.capture_thread.is_alive():
            self.logger.warning('Capture thread already running')
            return self

        self._open_serial_and_log()

        self.stop_event.clear()
        self.capture_thread = threading.Thread(target=self._capture_loop, daemon=True, name='UartGcovCapture')
        self.capture_thread.start()
        self.logger.info('Started UART capture thread')
        return self

    def _run_foreground(self) -> 'UartGcovCapture':
        self._open_serial_and_log()

        self.logger.info('Started gcov data capture and processing... Press Ctrl+C to exit')

        try:
            self._capture_loop()
        except KeyboardInterrupt:
            self.logger.info(f'Data capture interrupted. Captured {self.total_bytes} bytes.')
            if self.log_file:
                self.logger.info(f'Data saved to: {self.log_file}')
        finally:
            self.stop()

        return self

    def _open_serial_and_log(self) -> None:
        """Open serial port and log file (common for both modes)."""
        try:
            self.ser = serial.Serial(
                self.port,
                self.baudrate,
                timeout=0.1,  # Short timeout for non-blocking reads
                write_timeout=1,
                inter_byte_timeout=None,  # No timeout between bytes
            )
            self.logger.info(f'Opened serial port: {self.port} at {self.baudrate} baud')
        except serial.SerialException as e:
            self.logger.error(f'Failed to open serial port {self.port}: {e}')
            raise

        if self.log_file and self.log_level > 0:
            try:
                self.file_desc = open(self.log_file, 'w', encoding='ascii', buffering=8192)
                self.logger.info(f'Opened log file: {self.log_file} (log level: {self.log_level})')
            except OSError as e:
                self.logger.warning(f'Failed to open log file {self.log_file}: {e}')

    def wait_for_fstop(self, timeout: float = 10.0) -> bool:
        """Wait for an FSTOP command to be received.

        Args:
            timeout: Maximum time to wait in seconds

        Returns:
            True if FSTOP was received, False if timeout
        """
        result = self.fstop_event.wait(timeout=timeout)
        if result:
            self.fstop_event.clear()  # Clear after successful wait
        return result

    def get_fstop_count(self) -> int:
        """Get the number of FSTOP commands received so far."""
        return self.fstop_count

    def stop(self) -> None:
        """Stop capture thread and close serial port."""
        if self.stop_event.is_set():
            return

        self.stop_event.set()

        # Close serial port first to unblock any blocking read in the capture thread
        if self.ser is not None and self.ser.is_open:
            try:
                self.ser.close()
                self.logger.info('Closed serial port')
            except OSError:
                pass
            self.ser = None

        # Now wait for the thread to finish (should exit quickly after serial close)
        if self.capture_thread is not None and self.capture_thread.is_alive():
            self.logger.info('Waiting for capture thread to stop...')
            self.capture_thread.join(timeout=2.0)
            if self.capture_thread.is_alive():
                self.logger.warning('Capture thread did not stop within timeout')

        # Close log file
        if self.file_desc is not None:
            try:
                self.file_desc.flush()
                self.file_desc.close()
                self.logger.info('Closed log file')
            except OSError:
                pass
            self.file_desc = None

    def _capture_loop(self) -> None:
        """Main capture loop (runs in either foreground or background)."""
        if self.ser is None:
            return

        self.logger.info('Capture loop started')
        try:
            while not self.stop_event.is_set():
                # Check if serial port was closed (e.g., by stop())
                if self.ser is None or not self.ser.is_open:
                    break

                # Check for parser timeout (in case of incomplete frames)
                incomplete_frame = self.parser.check_timeout()
                if incomplete_frame is not None:
                    hex_bytes = incomplete_frame.hex(' ').upper() if incomplete_frame else '(empty)'
                    self.logger.warning(f'Parser timeout - incomplete frame discarded: {hex_bytes}')

                if self.ser.in_waiting > 0:
                    chunk = self.ser.read(self.ser.in_waiting)
                    frames = self.parser.feed(chunk)  # Returns List[Tuple[bytes, bytes]]

                    for frame, payload in frames:
                        summary, resp = _parse_command(payload, self.proc)

                        # Check if this is an FSTOP command
                        if payload and payload[0] == 0x6:
                            self.fstop_count += 1
                            self.fstop_event.set()  # Signal that FSTOP was received
                            self.fstop_event.clear()  # Clear for next FSTOP

                        if resp and self.ser is not None and self.ser.is_open:
                            self.ser.write(resp)
                            self.ser.flush()

                        self.total_bytes += len(frame)

                        console_resp = _log_frame_to_file(self.file_desc, self.log_level, frame, resp, summary)

                        if self.log_level > 1:
                            self.logger.info(f'Captured {self.total_bytes} bytes | {summary}{console_resp}')
                else:
                    time.sleep(0.001)

        except serial.SerialException:
            # Expected when serial port is closed during stop()
            pass
        except Exception as e:
            self.logger.error(f'Unexpected error in capture loop: {e}')
        finally:
            self.logger.info('Capture loop ended')


def main() -> None:
    logging.basicConfig(
        level=logging.INFO, format='%(asctime)s [%(levelname)s] %(message)s', datefmt='%Y-%m-%d %H:%M:%S'
    )

    args = parse_args()

    if args.log_level > 0 and not args.output:
        print('Error: -o/--output is required when log-level > 0')
        sys.exit(1)

    print(f'Opening serial port: {args.port} at {args.baudrate} baud')
    if args.output:
        print(f'Log file: {args.output} (log level: {args.log_level})')
    else:
        print('Log level 0: No file logging')

    try:
        capture = UartGcovCapture(
            port=args.port, baudrate=args.baudrate, log_file=args.output, log_level=args.log_level
        )
        capture.run(background=False)
    except Exception as e:
        print(f'Unexpected error: {e}')
        sys.exit(1)


def get_coverage_data(binary_path: str, expected_gcda_paths: list) -> dict:
    """Extract coverage data from .gcda files using gcov tool.

    Returns a dictionary with function execution counts for verification.
    If gcov tool is not available, returns empty dict after verifying files exist.
    """
    desc_path = os.path.join(binary_path, 'project_description.json')
    with open(desc_path, encoding='utf-8') as f:
        project_desc = json.load(f)

    toolchain_prefix = project_desc.get('monitor_toolprefix', '')
    gcov_tool = toolchain_prefix + 'gcov'

    try:
        subprocess.run([gcov_tool, '--version'], capture_output=True, check=True, timeout=3)
    except (subprocess.SubprocessError, FileNotFoundError):
        print(f'gcov tool is not available ({gcov_tool}), skipping detailed coverage data processing')
        for gcda_path in expected_gcda_paths:
            if os.path.isfile(gcda_path):
                print(f'✓ .gcda file exists: {os.path.basename(gcda_path)}')
        return {}

    coverage_data = {}

    for gcda_path in expected_gcda_paths:
        if not os.path.isfile(gcda_path):
            continue

        gcda_dir = os.path.dirname(gcda_path)

        try:
            subprocess.run(
                [gcov_tool, '-j', os.path.basename(gcda_path)],
                cwd=gcda_dir,
                capture_output=True,
                text=True,
                check=True,
                timeout=3,
            )

            basename = os.path.basename(gcda_path).replace('.gcda', '')
            json_gz_file = os.path.join(gcda_dir, f'{basename}.gcov.json.gz')

            if os.path.isfile(json_gz_file):
                with gzip.open(json_gz_file, 'rt', encoding='utf-8') as f:
                    gcov_data = json.load(f)

                for file_data in gcov_data.get('files', []):
                    source_file = os.path.basename(file_data.get('file', ''))
                    for function in file_data.get('functions', []):
                        func_name = function.get('name', '')
                        exec_count = function.get('execution_count', 0)
                        coverage_data[f'{source_file}:{func_name}'] = exec_count

                os.remove(json_gz_file)

        except (subprocess.SubprocessError, FileNotFoundError, json.JSONDecodeError) as e:
            print(f'Warning: Could not extract coverage from {gcda_path}: {e}')

    return coverage_data


def gcov_capture_from_uart(
    port: str, baudrate: int = 115200, log_file: str | None = None, log_level: int = 1, timeout: float = 10.0
) -> bool:
    """Convenience function to capture a single GCOV dump from UART and wait for FSTOP.

    This is a simple helper for pytest scripts that need to capture GCOV data via UART.
    It starts capture, waits for one FSTOP command, then stops.

    Args:
        port: Serial port path (e.g., '/dev/ttyUSB0')
        baudrate: Serial baud rate (default: 115200)
        log_file: Optional path to log file for REQ/RES/SUMMARY output
        log_level: Log level: 0=nothing, 1=REQ+RES+SUMMARY, 2=1+CONSOLE (default: 1)
        timeout: Maximum time to wait for FSTOP in seconds (default: 10.0)

    Returns:
        True if FSTOP was received, False if timeout

    Example usage in pytest:
        >>> from gcov_capture import gcov_capture_from_uart
        >>> # Close DUT console
        >>> dut.serial.close()
        >>> # Trigger gcov dump on target via esp_gcov_dump()
        >>> # Wait for dump to complete
        >>> assert gcov_capture_from_uart(
        ...     port=dut.serial.port, baudrate=115200, log_file='gcov_dump.log', timeout=30.0
        ... ), 'GCOV dump timeout'
        >>> # Check .gcda files were created
    """
    with UartGcovCapture(port=port, baudrate=baudrate, log_file=log_file, log_level=log_level) as capture:
        capture.run(background=True)
        time.sleep(0.1)  # Give capture a moment to start
        return capture.wait_for_fstop(timeout=timeout)


if __name__ == '__main__':
    main()
