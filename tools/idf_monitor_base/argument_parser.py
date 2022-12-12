# SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import os

from .constants import DEFAULT_PRINT_FILTER, DEFAULT_TOOLCHAIN_PREFIX, PANIC_DECODE_BACKTRACE, PANIC_DECODE_DISABLE
from .coredump import COREDUMP_DECODE_DISABLE, COREDUMP_DECODE_INFO


def get_parser():  # type: () -> argparse.ArgumentParser
    parser = argparse.ArgumentParser('idf_monitor - a serial output monitor for esp-idf')

    parser.add_argument(
        '--port', '-p',
        help='Serial port device',
        default=os.environ.get('ESPTOOL_PORT', '/dev/ttyUSB0')
    )

    parser.add_argument(
        '--no-reset',
        help='Do not reset the chip on monitor startup',
        action='store_true'
    )

    parser.add_argument(
        '--disable-address-decoding', '-d',
        help="Don't print lines about decoded addresses from the application ELF file",
        action='store_true',
        default=os.getenv('ESP_MONITOR_DECODE') == '0'
    )

    parser.add_argument(
        '--baud', '-b',
        help='Serial port baud rate',
        type=int,
        default=os.getenv('IDF_MONITOR_BAUD', os.getenv('MONITORBAUD', 115200)))

    parser.add_argument(
        '--make', '-m',
        help='Command to run make',
        type=str, default='make')

    parser.add_argument(
        '--encrypted',
        help='Use encrypted targets while running make',
        action='store_true')

    parser.add_argument(
        '--toolchain-prefix',
        help='Triplet prefix to add before cross-toolchain names',
        default=DEFAULT_TOOLCHAIN_PREFIX)

    parser.add_argument(
        '--eol',
        choices=['CR', 'LF', 'CRLF'],
        type=lambda c: c.upper(),
        help='End of line to use when sending to the serial port',
        default='CR')

    parser.add_argument(
        'elf_file', help='ELF file of application',
        type=lambda f: open(f, 'rb') if os.path.exists(f) else f'{f}',
        nargs='?'
    )

    parser.add_argument(
        '--print_filter',
        help='Filtering string',
        default=DEFAULT_PRINT_FILTER)

    parser.add_argument(
        '--decode-coredumps',
        choices=[COREDUMP_DECODE_INFO, COREDUMP_DECODE_DISABLE],
        default=COREDUMP_DECODE_INFO,
        help='Handling of core dumps found in serial output'
    )

    parser.add_argument(
        '--decode-panic',
        choices=[PANIC_DECODE_BACKTRACE, PANIC_DECODE_DISABLE],
        default=PANIC_DECODE_DISABLE,
        help='Handling of panic handler info found in serial output'
    )

    parser.add_argument(
        '--target',
        help='Target name (used when stack dump decoding is enabled)',
        default=os.environ.get('IDF_TARGET', 'esp32')
    )

    parser.add_argument(
        '--revision',
        help='Revision of the target',
        type=int,
        default=0
    )

    parser.add_argument(
        '--ws',
        default=os.environ.get('ESP_IDF_MONITOR_WS', None),
        help='WebSocket URL for communicating with IDE tools for debugging purposes'
    )

    parser.add_argument(
        '--timestamps',
        help='Add timestamp for each line',
        default=False,
        action='store_true')

    parser.add_argument(
        '--timestamp-format',
        default=os.environ.get('ESP_IDF_MONITOR_TIMESTAMP_FORMAT', '%Y-%m-%d %H:%M:%S'),
        help='Set a strftime()-compatible timestamp format'
    )

    parser.add_argument(
        '--force-color',
        help='Always colored monitor output, even if output is redirected.',
        default=False,
        action='store_true')

    return parser
