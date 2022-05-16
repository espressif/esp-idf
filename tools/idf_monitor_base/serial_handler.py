# SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import queue  # noqa: F401
import re
import subprocess
import time
from typing import Callable, Optional

import serial  # noqa: F401
from serial.tools import miniterm  # noqa: F401

from .chip_specific_config import get_chip_config
from .console_parser import ConsoleParser, prompt_next_action  # noqa: F401
from .console_reader import ConsoleReader  # noqa: F401
from .constants import (CMD_APP_FLASH, CMD_ENTER_BOOT, CMD_MAKE, CMD_OUTPUT_TOGGLE, CMD_RESET, CMD_STOP,
                        CMD_TOGGLE_LOGGING, CMD_TOGGLE_TIMESTAMPS, PANIC_DECODE_DISABLE, PANIC_END, PANIC_IDLE,
                        PANIC_READING, PANIC_STACK_DUMP, PANIC_START)
from .coredump import CoreDump
from .exceptions import SerialStopException
from .gdbhelper import GDBHelper
from .line_matcher import LineMatcher
from .logger import Logger
from .output_helpers import yellow_print
from .serial_reader import Reader


def run_make(target, make, console, console_parser, event_queue, cmd_queue, logger):
    # type: (str, str, miniterm.Console, ConsoleParser, queue.Queue, queue.Queue, Logger) -> None
    if isinstance(make, list):
        popen_args = make + [target]
    else:
        popen_args = [make, target]
    yellow_print('Running %s...' % ' '.join(popen_args))
    p = subprocess.Popen(popen_args, env=os.environ)
    try:
        p.wait()
    except KeyboardInterrupt:
        p.wait()
    if p.returncode != 0:
        prompt_next_action('Build failed', console, console_parser, event_queue, cmd_queue)
    else:
        logger.output_enabled = True


class SerialHandler:
    """
    The class is responsible for buffering serial input and performing corresponding commands.
    """
    def __init__(self, last_line_part, serial_check_exit, logger, decode_panic, reading_panic, panic_buffer, target,
                 force_line_print, start_cmd_sent, serial_instance, encrypted):
        # type: (bytes, bool, Logger, str, int, bytes,str, bool, bool, serial.Serial, bool) -> None
        self._last_line_part = last_line_part
        self._serial_check_exit = serial_check_exit
        self.logger = logger
        self._decode_panic = decode_panic
        self._reading_panic = reading_panic
        self._panic_buffer = panic_buffer
        self.target = target
        self._force_line_print = force_line_print
        self.start_cmd_sent = start_cmd_sent
        self.serial_instance = serial_instance
        self.encrypted = encrypted

    def handle_serial_input(self, data, console_parser, coredump, gdb_helper, line_matcher,
                            check_gdb_stub_and_run, finalize_line=False):
        #  type: (bytes, ConsoleParser, CoreDump, Optional[GDBHelper], LineMatcher, Callable, bool) -> None
        # Remove "+" after Continue command
        if self.start_cmd_sent:
            self.start_cmd_sent = False
            pos = data.find(b'+')
            if pos != -1:
                data = data[(pos + 1):]

        sp = data.split(b'\n')
        if self._last_line_part != b'':
            # add unprocessed part from previous "data" to the first line
            sp[0] = self._last_line_part + sp[0]
            self._last_line_part = b''
        if sp[-1] != b'':
            # last part is not a full line
            self._last_line_part = sp.pop()
        for line in sp:
            if line == b'':
                continue
            if self._serial_check_exit and line == console_parser.exit_key.encode('latin-1'):
                raise SerialStopException()
            if gdb_helper:
                self.check_panic_decode_trigger(line, gdb_helper)
            with coredump.check(line):
                if self._force_line_print or line_matcher.match(line.decode(errors='ignore')):
                    self.logger.print(line + b'\n')
                    self.logger.handle_possible_pc_address_in_line(line)
            check_gdb_stub_and_run(line)
            self._force_line_print = False
        # Now we have the last part (incomplete line) in _last_line_part. By
        # default we don't touch it and just wait for the arrival of the rest
        # of the line. But after some time when we didn't received it we need
        # to make a decision.
        force_print_or_matched = any((
            self._force_line_print,
            (finalize_line and line_matcher.match(self._last_line_part.decode(errors='ignore')))
        ))
        if self._last_line_part != b'' and force_print_or_matched:
            self._force_line_print = True
            self.logger.print(self._last_line_part)
            self.logger.handle_possible_pc_address_in_line(self._last_line_part)
            check_gdb_stub_and_run(self._last_line_part)
            # It is possible that the incomplete line cuts in half the PC
            # address. A small buffer is kept and will be used the next time
            # handle_possible_pc_address_in_line is invoked to avoid this problem.
            # MATCH_PCADDR matches 10 character long addresses. Therefore, we
            # keep the last 9 characters.
            self.logger.pc_address_buffer = self._last_line_part[-9:]
            # GDB sequence can be cut in half also. GDB sequence is 7
            # characters long, therefore, we save the last 6 characters.
            if gdb_helper:
                gdb_helper.gdb_buffer = self._last_line_part[-6:]
            self._last_line_part = b''
        # else: keeping _last_line_part and it will be processed the next time
        # handle_serial_input is invoked

    def check_panic_decode_trigger(self, line, gdb_helper):  # type: (bytes, GDBHelper) -> None
        if self._decode_panic == PANIC_DECODE_DISABLE:
            return

        if self._reading_panic == PANIC_IDLE and re.search(PANIC_START, line.decode('ascii', errors='ignore')):
            self._reading_panic = PANIC_READING
            yellow_print('Stack dump detected')

        if self._reading_panic == PANIC_READING and PANIC_STACK_DUMP in line:
            self.logger.output_enabled = False

        if self._reading_panic == PANIC_READING:
            self._panic_buffer += line.replace(b'\r', b'') + b'\n'

        if self._reading_panic == PANIC_READING and PANIC_END in line:
            self._reading_panic = PANIC_IDLE
            self.logger.output_enabled = True
            gdb_helper.process_panic_output(self._panic_buffer, self.logger, self.target)
            self._panic_buffer = b''

    def handle_commands(self, cmd, chip, run_make_func, console_reader, serial_reader):
        # type: (int, str, Callable, ConsoleReader, Reader) -> None
        config = get_chip_config(chip)
        reset_delay = config['reset']
        enter_boot_set = config['enter_boot_set']
        enter_boot_unset = config['enter_boot_unset']

        high = False
        low = True

        if chip == 'linux':
            if cmd in [CMD_RESET,
                       CMD_MAKE,
                       CMD_APP_FLASH,
                       CMD_ENTER_BOOT]:
                yellow_print('linux target does not support this command')
                return

        if cmd == CMD_STOP:
            console_reader.stop()
            serial_reader.stop()
        elif cmd == CMD_RESET:
            self.serial_instance.setRTS(low)
            self.serial_instance.setDTR(self.serial_instance.dtr)  # usbser.sys workaround
            time.sleep(reset_delay)
            self.serial_instance.setRTS(high)
            self.serial_instance.setDTR(self.serial_instance.dtr)  # usbser.sys workaround
            self.logger.output_enabled = True
        elif cmd == CMD_MAKE:
            run_make_func('encrypted-flash' if self.encrypted else 'flash')
        elif cmd == CMD_APP_FLASH:
            run_make_func('encrypted-app-flash' if self.encrypted else 'app-flash')
        elif cmd == CMD_OUTPUT_TOGGLE:
            self.logger.output_toggle()
        elif cmd == CMD_TOGGLE_LOGGING:
            self.logger.toggle_logging()
        elif cmd == CMD_TOGGLE_TIMESTAMPS:
            self.logger.toggle_timestamps()
        elif cmd == CMD_ENTER_BOOT:
            yellow_print('Pause app (enter bootloader mode), press Ctrl-T Ctrl-R to restart')
            self.serial_instance.setDTR(high)  # IO0=HIGH
            self.serial_instance.setRTS(low)  # EN=LOW, chip in reset
            self.serial_instance.setDTR(self.serial_instance.dtr)  # usbser.sys workaround
            time.sleep(enter_boot_set)  # timeouts taken from esptool.py, includes esp32r0 workaround. defaults: 0.1
            self.serial_instance.setDTR(low)  # IO0=LOW
            self.serial_instance.setRTS(high)  # EN=HIGH, chip out of reset
            self.serial_instance.setDTR(self.serial_instance.dtr)  # usbser.sys workaround
            time.sleep(enter_boot_unset)  # timeouts taken from esptool.py, includes esp32r0 workaround. defaults: 0.05
            self.serial_instance.setDTR(high)  # IO0=HIGH, done
        else:
            raise RuntimeError('Bad command data %d' % cmd)  # type: ignore
