# SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import re

# Control-key characters
CTRL_A = '\x01'
CTRL_B = '\x02'
CTRL_C = '\x03'
CTRL_F = '\x06'
CTRL_H = '\x08'
CTRL_I = '\x09'
CTRL_R = '\x12'
CTRL_T = '\x14'
CTRL_Y = '\x19'
CTRL_P = '\x10'
CTRL_X = '\x18'
CTRL_L = '\x0c'
CTRL_RBRACKET = '\x1d'  # Ctrl+]

# Command parsed from console inputs
CMD_STOP = 1
CMD_RESET = 2
CMD_MAKE = 3
CMD_APP_FLASH = 4
CMD_OUTPUT_TOGGLE = 5
CMD_TOGGLE_LOGGING = 6
CMD_ENTER_BOOT = 7
CMD_TOGGLE_TIMESTAMPS = 8

# Tags for tuples in queues
TAG_KEY = 0
TAG_SERIAL = 1
TAG_SERIAL_FLUSH = 2
TAG_CMD = 3

__version__ = '1.1'

# paths to scripts
PANIC_OUTPUT_DECODE_SCRIPT = os.path.join(os.path.dirname(__file__), '..', 'gdb_panic_server.py')
COREDUMP_SCRIPT = os.path.join(os.path.dirname(__file__), '..', '..', 'components', 'espcoredump', 'espcoredump.py')

# regex matches an potential PC value (0x4xxxxxxx)
MATCH_PCADDR = re.compile(r'0x4[0-9a-f]{7}', re.IGNORECASE)

DEFAULT_TOOLCHAIN_PREFIX = 'xtensa-esp32-elf-'

DEFAULT_PRINT_FILTER = ''

# panic handler related messages
PANIC_START = r'Core \s*\d+ register dump:'
PANIC_END = b'ELF file SHA256:'
PANIC_STACK_DUMP = b'Stack memory:'

# panic handler decoding states
PANIC_IDLE = 0
PANIC_READING = 1

# panic handler decoding options
PANIC_DECODE_DISABLE = 'disable'
PANIC_DECODE_BACKTRACE = 'backtrace'

EVENT_QUEUE_TIMEOUT = 0.03  # timeout before raising queue.Empty exception in case of empty event queue

ESPPORT_ENVIRON = str('ESPPORT')
MAKEFLAGS_ENVIRON = 'MAKEFLAGS'

GDB_UART_CONTINUE_COMMAND = '+$c#63'
GDB_EXIT_TIMEOUT = 0.3  # time delay between exit and writing GDB_UART_CONTINUE_COMMAND

# workaround for data sent without EOL
# if no data received during the time, last line is considered finished
LAST_LINE_THREAD_INTERVAL = 0.1

MINIMAL_EN_LOW_DELAY = 0.005
RECONNECT_DELAY = 0.5  # timeout between reconnect tries
CHECK_ALIVE_FLAG_TIMEOUT = 0.25  # timeout for checking alive flags (currently used by serial reader)
