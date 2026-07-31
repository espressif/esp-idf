#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import re
import sys
import time
from pathlib import Path

BEGIN_RE = re.compile(r'BEGIN_PPM:(.+)')
END_RE = re.compile(r'END_PPM:(.+)')


def _safe_name(name: str) -> str:
    return Path(name.strip()).name


def _save_ppm(name: str, lines: list[str], output_dir: Path) -> Path:
    output_dir.mkdir(parents=True, exist_ok=True)
    path = output_dir / _safe_name(name)
    path.write_text(''.join(lines), encoding='ascii')
    return path


def _consume_line(line: str, state: dict, output_dir: Path) -> Path | None:
    begin = BEGIN_RE.search(line)
    if begin:
        state['name'] = begin.group(1)
        state['lines'] = []
        return None

    name = state.get('name')
    if not name:
        return None

    end = END_RE.search(line)
    if end:
        path = _save_ppm(name, state['lines'], output_dir)
        state.clear()
        return path

    state['lines'].append(line)
    return None


def _extract_from_log(log_path: Path, output_dir: Path) -> list[Path]:
    saved = []
    state: dict = {}
    with log_path.open('r', encoding='utf-8', errors='ignore') as fp:
        for line in fp:
            path = _consume_line(line, state, output_dir)
            if path:
                saved.append(path)
    return saved


def _extract_from_serial(port: str, baud: int, output_dir: Path, test_id: str, timeout: float) -> list[Path]:
    try:
        import serial
    except ImportError as e:
        raise SystemExit('pyserial is required. Please run this script in the ESP-IDF Python environment.') from e

    saved = []
    state: dict = {}
    deadline = time.monotonic() + timeout

    with serial.Serial(port, baudrate=baud, timeout=0.2) as ser:
        ser.reset_input_buffer()
        ser.write(b'\n')
        time.sleep(0.2)
        ser.write(test_id.encode('ascii') + b'\n')

        while time.monotonic() < deadline:
            raw = ser.readline()
            if not raw:
                continue
            line = raw.decode('utf-8', errors='ignore')
            sys.stdout.write(line)
            sys.stdout.flush()

            path = _consume_line(line, state, output_dir)
            if path:
                saved.append(path)
                print(f'\nSaved {path}')
                if len(saved) >= 2:
                    break

    return saved


def main() -> None:
    default_output = Path(__file__).resolve().parent / 'dpc_images'
    parser = argparse.ArgumentParser(description='Save ISP DPC PPM dumps from serial output or a monitor log.')
    parser.add_argument('--output-dir', type=Path, default=default_output, help='Directory to write .ppm files')
    parser.add_argument('--log', type=Path, help='Extract PPM files from an existing monitor log')
    parser.add_argument('--port', help='Serial port, for example /dev/ttyACM0')
    parser.add_argument('--baud', type=int, default=115200, help='Serial baud rate')
    parser.add_argument('--test-id', default='11', help='Unity test menu id to run')
    parser.add_argument('--timeout', type=float, default=30.0, help='Serial capture timeout in seconds')
    args = parser.parse_args()

    if args.log:
        saved = _extract_from_log(args.log, args.output_dir)
    elif args.port:
        saved = _extract_from_serial(args.port, args.baud, args.output_dir, args.test_id, args.timeout)
    else:
        parser.error('Either --log or --port is required')

    if not saved:
        raise SystemExit('No PPM images were found.')

    print('Saved images:')
    for path in saved:
        print(path)


if __name__ == '__main__':
    main()
