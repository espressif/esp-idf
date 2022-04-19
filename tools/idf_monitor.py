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
# SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# Contains elements taken from miniterm "Very simple serial terminal" which
# is part of pySerial. https://github.com/pyserial/pyserial
# (C)2002-2015 Chris Liechti <cliechti@gmx.net>
#
# Originally released under BSD-3-Clause license.
#

import codecs
import os
import queue
import re
import shlex
import subprocess
import sys
import threading
import time
from builtins import bytes
from typing import Any, List, Optional, Type, Union

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
from idf_monitor_base.serial_reader import LinuxReader, SerialReader
from idf_monitor_base.web_socket_client import WebSocketClient
from serial.tools import miniterm

key_description = miniterm.key_description


class Monitor:
    """
    Monitor application base class.

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
        websocket_client=None,  # type: Optional[WebSocketClient]
        enable_address_decoding=True,  # type: bool
        timestamps=False,  # type: bool
        timestamp_format=''  # type: str
    ):
        self.event_queue = queue.Queue()  # type: queue.Queue
        self.cmd_queue = queue.Queue()  # type: queue.Queue
        self.console = miniterm.Console()

        sys.stderr = get_converter(sys.stderr, decode_output=True)
        self.console.output = get_converter(self.console.output)
        self.console.byte_output = get_converter(self.console.byte_output)

        self.elf_file = elf_file
        self.logger = Logger(self.elf_file, self.console, timestamps, timestamp_format, b'', enable_address_decoding,
                             toolchain_prefix)
        self.coredump = CoreDump(decode_coredumps, self.event_queue, self.logger, websocket_client, self.elf_file)

        # allow for possibility the "make" arg is a list of arguments (for idf.py)
        self.make = make if os.path.exists(make) else shlex.split(make)  # type: Any[Union[str, List[str]], str]
        self.target = target

        # testing hook - data from serial can make exit the monitor
        if isinstance(self, SerialMonitor):
            socket_mode = serial_instance.port.startswith('socket://')
            self.serial = serial_instance
            self.serial_reader = SerialReader(self.serial, self.event_queue)

            self.gdb_helper = GDBHelper(toolchain_prefix, websocket_client, self.elf_file, self.serial.port,
                                        self.serial.baudrate)
        else:
            socket_mode = False
            self.serial = subprocess.Popen([elf_file], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                           stderr=subprocess.STDOUT)
            self.serial_reader = LinuxReader(self.serial, self.event_queue)

            self.gdb_helper = None

        self.serial_handler = SerialHandler(b'', socket_mode, self.logger, decode_panic, PANIC_IDLE, b'', target,
                                            False, False, self.serial, encrypted)

        self.console_parser = ConsoleParser(eol)
        self.console_reader = ConsoleReader(self.console, self.event_queue, self.cmd_queue, self.console_parser,
                                            socket_mode)

        self._line_matcher = LineMatcher(print_filter)

        # internal state
        self._invoke_processing_last_line_timer = None  # type: Optional[threading.Timer]

    def __enter__(self) -> None:
        """ Use 'with self' to temporarily disable monitoring behaviour """
        self.serial_reader.stop()
        self.console_reader.stop()

    def __exit__(self, exc_type, exc_val, exc_tb) -> None:  # type: ignore
        raise NotImplementedError

    def run_make(self, target: str) -> None:
        with self:
            run_make(target, self.make, self.console, self.console_parser, self.event_queue, self.cmd_queue,
                     self.logger)

    def _pre_start(self) -> None:
        self.console_reader.start()
        self.serial_reader.start()

    def main_loop(self) -> None:
        self._pre_start()

        try:
            while self.console_reader.alive and self.serial_reader.alive:
                try:
                    self._main_loop()
                except KeyboardInterrupt:
                    yellow_print('To exit from IDF monitor please use \"Ctrl+]\"')
                    self.serial_write(codecs.encode(CTRL_C))
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

    def serial_write(self, *args, **kwargs):  # type: ignore
        raise NotImplementedError

    def check_gdb_stub_and_run(self, line: bytes) -> None:
        raise NotImplementedError

    def invoke_processing_last_line(self) -> None:
        self.event_queue.put((TAG_SERIAL_FLUSH, b''), False)

    def _main_loop(self) -> None:
        try:
            item = self.cmd_queue.get_nowait()
        except queue.Empty:
            try:
                item = self.event_queue.get(timeout=EVENT_QUEUE_TIMEOUT)
            except queue.Empty:
                return

        event_tag, data = item
        if event_tag == TAG_CMD:
            self.serial_handler.handle_commands(data, self.target, self.run_make, self.console_reader,
                                                self.serial_reader)
        elif event_tag == TAG_KEY:
            self.serial_write(codecs.encode(data))
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
            # finalizing the line when coredump is in progress causes decoding issues
            # the espcoredump loader uses empty line as a sign for end-of-coredump
            # line is finalized only for non coredump data
        elif event_tag == TAG_SERIAL_FLUSH:
            self.serial_handler.handle_serial_input(data, self.console_parser, self.coredump,
                                                    self.gdb_helper, self._line_matcher,
                                                    self.check_gdb_stub_and_run,
                                                    finalize_line=not self.coredump or not self.coredump.in_progress)
        else:
            raise RuntimeError('Bad event data %r' % ((event_tag, data),))


class SerialMonitor(Monitor):
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:  # type: ignore
        """ Use 'with self' to temporarily disable monitoring behaviour """
        self.console_reader.start()
        self.serial_reader.gdb_exit = self.gdb_helper.gdb_exit  # write gdb_exit flag
        self.serial_reader.start()

    def _pre_start(self) -> None:
        super()._pre_start()
        self.gdb_helper.gdb_exit = False
        self.serial_handler.start_cmd_sent = False

    def serial_write(self, *args, **kwargs):  # type: ignore
        self.serial: serial.Serial
        try:
            self.serial.write(*args, **kwargs)
        except serial.SerialException:
            pass  # this shouldn't happen, but sometimes port has closed in serial thread
        except UnicodeEncodeError:
            pass  # this can happen if a non-ascii character was passed, ignoring

    def check_gdb_stub_and_run(self, line: bytes) -> None:  # type: ignore # The base class one is a None value
        if self.gdb_helper.check_gdb_stub_trigger(line):
            with self:  # disable console control
                self.gdb_helper.run_gdb()

    def _main_loop(self) -> None:
        if self.gdb_helper.gdb_exit:
            self.gdb_helper.gdb_exit = False
            time.sleep(GDB_EXIT_TIMEOUT)
            # Continue the program after exit from the GDB
            self.serial_write(codecs.encode(GDB_UART_CONTINUE_COMMAND))
            self.serial_handler.start_cmd_sent = True

        super()._main_loop()


class LinuxMonitor(Monitor):
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:  # type: ignore
        """ Use 'with self' to temporarily disable monitoring behaviour """
        self.console_reader.start()
        self.serial_reader.start()

    def serial_write(self, *args, **kwargs):  # type: ignore
        self.serial.stdin.write(*args, **kwargs)

    def check_gdb_stub_and_run(self, line: bytes) -> None:
        return  # fake function for linux target


def main() -> None:
    parser = get_parser()
    args = parser.parse_args()

    # The port name is changed in cases described in the following lines. Use a local argument and
    # avoid the modification of args.port.
    port = args.port

    # GDB uses CreateFile to open COM port, which requires the COM name to be r'\\.\COMx' if the COM
    # number is larger than 10
    if os.name == 'nt' and port.startswith('COM'):
        port = port.replace('COM', r'\\.\COM')
        yellow_print('--- WARNING: GDB cannot open serial ports accessed as COMx')
        yellow_print('--- Using %s instead...' % port)
    elif port.startswith('/dev/tty.') and sys.platform == 'darwin':
        port = port.replace('/dev/tty.', '/dev/cu.')
        yellow_print('--- WARNING: Serial ports accessed as /dev/tty.* will hang gdb if launched.')
        yellow_print('--- Using %s instead...' % port)

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

    ws = WebSocketClient(args.ws) if args.ws else None
    try:
        cls: Type[Monitor]
        if args.target == 'linux':
            serial_instance = None
            cls = LinuxMonitor
            yellow_print('--- idf_monitor on linux ---')
        else:
            serial_instance = serial.serial_for_url(port, args.baud, do_not_open=True)
            serial_instance.dtr = False
            serial_instance.rts = False

            # Pass the actual used port to callee of idf_monitor (e.g. idf.py/cmake) through `ESPPORT` environment
            # variable.
            # Note that the port must be original port argument without any replacement done in IDF Monitor (idf.py
            # has a check for this).
            # To make sure the key as well as the value are str type, by the requirements of subprocess
            espport_val = str(args.port)
            os.environ.update({ESPPORT_ENVIRON: espport_val})

            cls = SerialMonitor
            yellow_print('--- idf_monitor on {p.name} {p.baudrate} ---'.format(p=serial_instance))

        monitor = cls(serial_instance,
                      args.elf_file.name,
                      args.print_filter,
                      args.make,
                      args.encrypted,
                      args.toolchain_prefix,
                      args.eol,
                      args.decode_coredumps,
                      args.decode_panic,
                      args.target,
                      ws,
                      not args.disable_address_decoding,
                      args.timestamps,
                      args.timestamp_format)

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
