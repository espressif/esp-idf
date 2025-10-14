# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import struct
import sys
import time

import serial
import serial.tools.list_ports
from rich.align import Align
from rich.console import Console
from rich.live import Live
from rich.panel import Panel
from rich.table import Table

# Constants
UART_READ_TIMEOUT = 0.1
UART_BLOCK_SIZE = 1024 * 1024
UART_BITS_PER_BYTE = 10

FRAME_HEADER_SIZE = 6
FRAME_TAIL_SIZE = 4
FRAME_OVERHEAD_SIZE = FRAME_HEADER_SIZE + FRAME_TAIL_SIZE

LOSS_FRAME_SOURCE_CODE = 0xFF
LOSS_FRAME_LEN = 7


def format_speed(bps: float) -> str:
    if bps < 1_000:
        return f'{bps:.1f} bps'
    elif bps < 1_000_000:
        return f'{bps / 1_000:.1f} kbps'
    else:
        return f'{bps / 1_000_000:.2f} Mbps'


def format_byte_cnt(cnt: int) -> str:
    if cnt < 1024:
        return f'{cnt} B'
    elif cnt < 1024 * 1024:
        return f'{cnt / 1024:.1f} KB'
    else:
        return f'{cnt / 1024 / 1024:.2f} MB'


def list_serial_ports() -> list[str]:
    """List all available serial ports"""
    ports = serial.tools.list_ports.comports()
    return [port.device for port in ports]


class BLELogConsole:
    def __init__(self, port: str):
        if not self.validate_uart_port(port):
            sys.exit(1)

        self.port = port
        self.remained = b''
        self.last_time = time.time()
        self.console = Console()
        self.str_tmp = ''
        self.rx_bytes = 0
        self.lost_frames = 0
        self.lost_bytes = 0
        self.max_bps = 0.0

    def validate_uart_port(self, port: str) -> bool:
        """Validate if UART port exists and is accessible"""
        # First check if port exists in the system
        available_ports = list_serial_ports()
        if port not in available_ports:
            self.console.print(f"Error: UART port '{port}' does not exist or is not accessible")
            self.console.print(f'Available ports: {available_ports}')
            return False

        # Try to open port to confirm accessibility
        try:
            ser = serial.Serial(port, baudrate=115200, timeout=UART_READ_TIMEOUT)
            ser.close()
            return True
        except Exception as e:
            self.console.print(f"Error: Cannot access UART port '{port}': {str(e)}")
            return False

    def loss_frame_to_console(self, payload: bytes) -> None:
        if len(payload) != LOSS_FRAME_LEN:
            return

        _, lost_frame_cnt, lost_bytes_cnt = struct.unpack('<BHI', payload)
        self.lost_frames += lost_frame_cnt
        self.lost_bytes += lost_bytes_cnt
        self.console.print(f'[yellow][WARNING][/yellow] BLE Log lost {lost_frame_cnt} frames, {lost_bytes_cnt} bytes')

    def ascii_to_console(self, data: bytes) -> None:
        try:
            decoded = data.decode('ascii')
        except UnicodeDecodeError:
            return

        if decoded == '\n':
            self.console.print(self.str_tmp, emoji=False)
            self.str_tmp = ''
        else:
            self.str_tmp += decoded

    def handle_data_block(self, block: bytes) -> None:
        # Concatenate remained data with input block
        full_data = self.remained + block
        offset = 0
        while offset + FRAME_OVERHEAD_SIZE <= len(full_data):
            payload_offset = offset + FRAME_HEADER_SIZE

            # Check if we have enough data for header
            if payload_offset > len(full_data):
                break

            header = full_data[offset:payload_offset]
            payload_len, source_code, _, _ = struct.unpack_from('<HBBH', header)
            if payload_len > len(full_data):
                self.ascii_to_console(bytes([full_data[offset]]))
                offset += 1
                continue

            frame_len = payload_len + FRAME_OVERHEAD_SIZE
            if offset + frame_len > len(full_data):
                break

            # Verify checksum
            checksum_offset = payload_offset + payload_len
            checksum_val = struct.unpack_from('<I', full_data, checksum_offset)[0]

            payload = full_data[payload_offset:checksum_offset]
            checksum_cal = sum(header + payload)
            if checksum_cal != checksum_val:
                self.ascii_to_console(bytes([full_data[offset]]))
                offset += 1
                continue

            # Warn frame loss
            if source_code == LOSS_FRAME_SOURCE_CODE:
                self.loss_frame_to_console(payload)

            # Update offset
            offset += frame_len

        # Update remained
        self.remained = full_data[offset:] if offset < len(full_data) else b''

    def build_status_panel(self, curr_rx_bytes: int) -> Panel:
        # Update time info
        now = time.perf_counter()
        elapsed = now - self.last_time
        self.last_time = now

        # Update rx bytes
        self.rx_bytes += curr_rx_bytes
        rx_bytes_str = format_byte_cnt(self.rx_bytes)

        # Calculate transport speed
        bps = curr_rx_bytes * UART_BITS_PER_BYTE / elapsed if elapsed > 0.0 else 0.0
        self.max_bps = bps if bps > self.max_bps else self.max_bps
        speed_str = format_speed(bps)
        max_speed_str = format_speed(self.max_bps)

        # Prepare render table
        line = Table.grid(padding=(0, 2), expand=True)
        for _ in range(4):
            line.add_column()

        line.add_row(
            f'[green]Received:[/green] {rx_bytes_str}',
            f'[cyan]Speed:[/cyan] {speed_str}',
            f'[cyan]Max Speed:[/cyan] {max_speed_str}',
            f'[yellow]Lost:[/yellow] {self.lost_frames} frames, {self.lost_bytes} bytes',
        )

        return Panel(
            Align(line, align='left'),
            padding=(0, 1),
            style='dim',
            border_style='bright_blue',
            title='BLE Log Status',
            title_align='left',
        )

    def start_uart_transport(self, baudrate: int, output_filename: str) -> None:
        try:
            with (
                serial.Serial(self.port, baudrate=baudrate, timeout=UART_READ_TIMEOUT) as ser,
                open(output_filename, 'wb') as f,
                Live(self.build_status_panel(0), console=self.console, refresh_per_second=4) as live,
            ):
                self.console.print(f'Starting UART transport from {self.port} to {output_filename}')
                while True:
                    # Transport
                    block = ser.read(UART_BLOCK_SIZE)
                    if block:
                        self.handle_data_block(block)
                        live.update(self.build_status_panel(len(block)))
                        f.write(block)
                        f.flush()
        except KeyboardInterrupt:
            self.console.print('\nUART transport stopped by user')
        except Exception as e:
            self.console.print('Error: Exception occurred while transporting UART stream')
            self.console.print(e)

        self.console.print(f'[green]UART stream is saved to {output_filename}[/green]')


def main() -> None:
    parser = argparse.ArgumentParser(description='BLE Log Console Helper')
    parser.add_argument('--port', required=True, help='UART port to connect (e.g., /dev/ttyUSB0)')
    parser.add_argument('--output', default='uart_data.bin', help='Output filename (default: uart_data.bin)')
    parser.add_argument('--baudrate', type=int, default=115200, help='Baud rate (default: 115200)')

    args = parser.parse_args()

    ble_log_console = BLELogConsole(args.port)
    ble_log_console.start_uart_transport(args.baudrate, args.output)


if __name__ == '__main__':
    main()
