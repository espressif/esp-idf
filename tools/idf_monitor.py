#!/usr/bin/env python
#
# esp-idf serial output monitor tool. Does some helpful things:
# - Looks up hex addresses in ELF file with addr2line
# - Reset ESP32 via serial RTS line (Ctrl-T Ctrl-R)
# - Run flash build target to rebuild and flash entire project (Ctrl-T Ctrl-F)
# - Run app-flash build target to rebuild and flash app only (Ctrl-T Ctrl-A)
# - If gdbstub output is detected, gdb is automatically loaded
# - If core dump output is detected, it is converted to a human-readable report
#   by espcoredump.py.
#
# Copyright 2015-2021 Espressif Systems (Shanghai) CO LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Contains elements taken from miniterm "Very simple serial terminal" which
# is part of pySerial. https://github.com/pyserial/pyserial
# (C)2002-2015 Chris Liechti <cliechti@gmx.net>
#
# Originally released under BSD-3-Clause license.
#

from __future__ import division, print_function, unicode_literals

import codecs
import os
import re
import threading
import time
from builtins import bytes, object

try:
    from typing import Any, List, Optional, Union  # noqa
except ImportError:
    pass

import queue
import shlex
import sys

import serial
import serial.tools.list_ports
# Windows console stuff
from idf_monitor_base.ansi_color_converter import get_converter
from idf_monitor_base.argument_parser import get_parser
from idf_monitor_base.console_parser import ConsoleParser
from idf_monitor_base.console_reader import ConsoleReader
from idf_monitor_base.constants import (CTRL_C, CTRL_H, DEFAULT_PRINT_FILTER, DEFAULT_TOOLCHAIN_PREFIX,
                                        ESPPORT_ENVIRON, EVENT_QUEUE_TIMEOUT, GDB_EXIT_TIMEOUT,
                                        GDB_UART_CONTINUE_COMMAND, LAST_LINE_THREAD_INTERVAL, MAKEFLAGS_ENVIRON,
                                        PANIC_DECODE_DISABLE, PANIC_IDLE, TAG_CMD, TAG_KEY, TAG_SERIAL,
                                        TAG_SERIAL_FLUSH)
from idf_monitor_base.coredump import COREDUMP_DECODE_INFO, CoreDump
from idf_monitor_base.exceptions import SerialStopException
from idf_monitor_base.gdbhelper import GDBHelper
from idf_monitor_base.line_matcher import LineMatcher
from idf_monitor_base.logger import Logger
from idf_monitor_base.output_helpers import normal_print, yellow_print
from idf_monitor_base.serial_handler import SerialHandler, run_make
from idf_monitor_base.serial_reader import SerialReader
from idf_monitor_base.web_socket_client import WebSocketClient
from serial.tools import miniterm

key_description = miniterm.key_description


class Monitor(object):
    """
    Monitor application main class.

    This was originally derived from miniterm.Miniterm, but it turned out to be easier to write from scratch for this
    purpose.

    Main difference is that all event processing happens in the main thread, not the worker threads.
    """

    def __init__(
        self,
        serial_instance,  # type: serial.Serial
        elf_file,  # type: str
        print_filter,  # type: str
        make='make',  # type: str
        encrypted=False,  # type: bool
        toolchain_prefix=DEFAULT_TOOLCHAIN_PREFIX,  # type: str
        eol='CRLF',  # type: str
        decode_coredumps=COREDUMP_DECODE_INFO,  # type: str
        decode_panic=PANIC_DECODE_DISABLE,  # type: str
        target='esp32',  # type: str
        websocket_client=None,  # type: WebSocketClient
        enable_address_decoding=True,  # type: bool
        timestamps=False,  # type: bool
        timestamp_format=''  # type: str
    ):
        super(Monitor, self).__init__()
        self.event_queue = queue.Queue()  # type: queue.Queue
        self.cmd_queue = queue.Queue()  # type: queue.Queue
        self.console = miniterm.Console()

        sys.stderr = get_converter(sys.stderr, decode_output=True)
        self.console.output = get_converter(self.console.output)
        self.console.byte_output = get_converter(self.console.byte_output)

        # testing hook - data from serial can make exit the monitor
        socket_mode = serial_instance.port.startswith('socket://')
        self.serial = serial_instance

        self.console_parser = ConsoleParser(eol)
        self.console_reader = ConsoleReader(self.console, self.event_queue, self.cmd_queue, self.console_parser,
                                            socket_mode)
        self.serial_reader = SerialReader(self.serial, self.event_queue)
        self.elf_file = elf_file

        # allow for possibility the "make" arg is a list of arguments (for idf.py)
        self.make = make if os.path.exists(make) else shlex.split(make)  # type: Any[Union[str, List[str]], str]
        self.target = target

        self._line_matcher = LineMatcher(print_filter)
        self.gdb_helper = GDBHelper(toolchain_prefix, websocket_client, self.elf_file, self.serial.port,
                                    self.serial.baudrate)
        self.logger = Logger(self.elf_file, self.console, timestamps, timestamp_format, b'', enable_address_decoding,
                             toolchain_prefix)
        self.coredump = CoreDump(decode_coredumps, self.event_queue, self.logger, websocket_client, self.elf_file)
        self.serial_handler = SerialHandler(b'', socket_mode, self.logger, decode_panic, PANIC_IDLE, b'', target,
                                            False, False, self.serial, encrypted)

        # internal state
        self._invoke_processing_last_line_timer = None  # type: Optional[threading.Timer]

    def invoke_processing_last_line(self):
        # type: () -> None
        self.event_queue.put((TAG_SERIAL_FLUSH, b''), False)

    def main_loop(self):
        # type: () -> None
        self.console_reader.start()
        self.serial_reader.start()
        self.gdb_helper.gdb_exit = False
        self.serial_handler.start_cmd_sent = False
        try:
            while self.console_reader.alive and self.serial_reader.alive:
                try:
                    if self.gdb_helper.gdb_exit:
                        self.gdb_helper.gdb_exit = False
                        time.sleep(GDB_EXIT_TIMEOUT)
                        try:
                            # Continue the program after exit from the GDB
                            self.serial.write(codecs.encode(GDB_UART_CONTINUE_COMMAND))
                            self.serial_handler.start_cmd_sent = True
                        except serial.SerialException:
                            pass  # this shouldn't happen, but sometimes port has closed in serial thread
                        except UnicodeEncodeError:
                            pass  # this can happen if a non-ascii character was passed, ignoring

                    try:
                        item = self.cmd_queue.get_nowait()
                    except queue.Empty:
                        try:
                            item = self.event_queue.get(timeout=EVENT_QUEUE_TIMEOUT)
                        except queue.Empty:
                            continue

                    event_tag, data = item
                    if event_tag == TAG_CMD:
                        self.serial_handler.handle_commands(data, self.target, self.run_make, self.console_reader,
                                                            self.serial_reader)
                    elif event_tag == TAG_KEY:
                        try:
                            self.serial.write(codecs.encode(data))
                        except serial.SerialException:
                            pass  # this shouldn't happen, but sometimes port has closed in serial thread
                        except UnicodeEncodeError:
                            pass  # this can happen if a non-ascii character was passed, ignoring
                    elif event_tag == TAG_SERIAL:
                        self.serial_handler.handle_serial_input(data, self.console_parser, self.coredump,
                                                                self.gdb_helper, self._line_matcher,
                                                                self.check_gdb_stub_and_run)
                        if self._invoke_processing_last_line_timer is not None:
                            self._invoke_processing_last_line_timer.cancel()
                        self._invoke_processing_last_line_timer = threading.Timer(LAST_LINE_THREAD_INTERVAL,
                                                                                  self.invoke_processing_last_line)
                        self._invoke_processing_last_line_timer.start()
                        # If no further data is received in the next short period
                        # of time then the _invoke_processing_last_line_timer
                        # generates an event which will result in the finishing of
                        # the last line. This is fix for handling lines sent
                        # without EOL.
                    elif event_tag == TAG_SERIAL_FLUSH:
                        self.serial_handler.handle_serial_input(data, self.console_parser, self.coredump,
                                                                self.gdb_helper, self._line_matcher,
                                                                self.check_gdb_stub_and_run, finalize_line=True)
                    else:
                        raise RuntimeError('Bad event data %r' % ((event_tag, data),))
                except KeyboardInterrupt:
                    try:
                        yellow_print('To exit from IDF monitor please use \"Ctrl+]\"')
                        self.serial.write(codecs.encode(CTRL_C))
                    except serial.SerialException:
                        pass  # this shouldn't happen, but sometimes port has closed in serial thread
                    except UnicodeEncodeError:
                        pass  # this can happen if a non-ascii character was passed, ignoring
        except SerialStopException:
            normal_print('Stopping condition has been received\n')
        except KeyboardInterrupt:
            pass
        finally:
            try:
                self.console_reader.stop()
                self.serial_reader.stop()
                self.logger.stop_logging()
                # Cancelling _invoke_processing_last_line_timer is not
                # important here because receiving empty data doesn't matter.
                self._invoke_processing_last_line_timer = None
            except Exception:  # noqa
                pass
            normal_print('\n')

    def __enter__(self):
        # type: () -> None
        """ Use 'with self' to temporarily disable monitoring behaviour """
        self.serial_reader.stop()
        self.console_reader.stop()

    def __exit__(self, *args, **kwargs):  # type: ignore
        """ Use 'with self' to temporarily disable monitoring behaviour """
        self.console_reader.start()
        self.serial_reader.gdb_exit = self.gdb_helper.gdb_exit  # write gdb_exit flag
        self.serial_reader.start()

    def check_gdb_stub_and_run(self, line):  # type: (bytes) -> None
        if self.gdb_helper.check_gdb_stub_trigger(line):
            with self:  # disable console control
                self.gdb_helper.run_gdb()

    def run_make(self, target):  # type: (str) -> None
        with self:
            run_make(target, self.make, self.console, self.console_parser, self.event_queue, self.cmd_queue,
                     self.logger)


def main():  # type: () -> None

    parser = get_parser()
    args = parser.parse_args()

    # GDB uses CreateFile to open COM port, which requires the COM name to be r'\\.\COMx' if the COM
    # number is larger than 10
    if os.name == 'nt' and args.port.startswith('COM'):
        args.port = args.port.replace('COM', r'\\.\COM')
        yellow_print('--- WARNING: GDB cannot open serial ports accessed as COMx')
        yellow_print('--- Using %s instead...' % args.port)
    elif args.port.startswith('/dev/tty.') and sys.platform == 'darwin':
        args.port = args.port.replace('/dev/tty.', '/dev/cu.')
        yellow_print('--- WARNING: Serial ports accessed as /dev/tty.* will hang gdb if launched.')
        yellow_print('--- Using %s instead...' % args.port)

    serial_instance = serial.serial_for_url(args.port, args.baud, do_not_open=True)
    serial_instance.dtr = False
    serial_instance.rts = False
    args.elf_file.close()  # don't need this as a file

    # remove the parallel jobserver arguments from MAKEFLAGS, as any
    # parent make is only running 1 job (monitor), so we can re-spawn
    # all of the child makes we need (the -j argument remains part of
    # MAKEFLAGS)
    try:
        makeflags = os.environ[MAKEFLAGS_ENVIRON]
        makeflags = re.sub(r'--jobserver[^ =]*=[0-9,]+ ?', '', makeflags)
        os.environ[MAKEFLAGS_ENVIRON] = makeflags
    except KeyError:
        pass  # not running a make jobserver

    # Pass the actual used port to callee of idf_monitor (e.g. make) through `ESPPORT` environment
    # variable
    # To make sure the key as well as the value are str type, by the requirements of subprocess
    espport_val = str(args.port)
    os.environ.update({ESPPORT_ENVIRON: espport_val})

    ws = WebSocketClient(args.ws) if args.ws else None
    try:
        monitor = Monitor(serial_instance, args.elf_file.name, args.print_filter, args.make, args.encrypted,
                          args.toolchain_prefix, args.eol,
                          args.decode_coredumps, args.decode_panic, args.target,
                          ws, enable_address_decoding=not args.disable_address_decoding,
                          timestamps=args.timestamps, timestamp_format=args.timestamp_format)

        yellow_print('--- idf_monitor on {p.name} {p.baudrate} ---'.format(p=serial_instance))
        yellow_print('--- Quit: {} | Menu: {} | Help: {} followed by {} ---'.format(
            key_description(monitor.console_parser.exit_key),
            key_description(monitor.console_parser.menu_key),
            key_description(monitor.console_parser.menu_key),
            key_description(CTRL_H)))
        if args.print_filter != DEFAULT_PRINT_FILTER:
            yellow_print('--- Print filter: {} ---'.format(args.print_filter))
        monitor.main_loop()
    except KeyboardInterrupt:
        pass
    finally:
        if ws:
            ws.close()


if __name__ == '__main__':
    main()
