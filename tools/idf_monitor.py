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
# Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

import argparse
import codecs
import datetime
import os
import re
import subprocess
from builtins import bytes, chr, object

try:
    import queue
except ImportError:
    import Queue as queue

import ctypes
import json
import shlex
import sys
import tempfile
import textwrap
import threading
import time
import types
from distutils.version import StrictVersion
from io import open

import serial
import serial.tools.list_ports
import serial.tools.miniterm as miniterm

try:
    import websocket
except ImportError:
    # This is needed for IDE integration only.
    pass

key_description = miniterm.key_description

# Control-key characters
CTRL_A = '\x01'
CTRL_B = '\x02'
CTRL_F = '\x06'
CTRL_H = '\x08'
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

# ANSI terminal codes (if changed, regular expressions in LineMatcher need to be udpated)
ANSI_RED = '\033[1;31m'
ANSI_YELLOW = '\033[0;33m'
ANSI_NORMAL = '\033[0m'


def color_print(message, color, newline='\n'):
    """ Print a message to stderr with colored highlighting """
    sys.stderr.write('%s%s%s%s' % (color, message,  ANSI_NORMAL, newline))


def yellow_print(message, newline='\n'):
    color_print(message, ANSI_YELLOW, newline)


def red_print(message, newline='\n'):
    color_print(message, ANSI_RED, newline)


__version__ = '1.1'

# Tags for tuples in queues
TAG_KEY = 0
TAG_SERIAL = 1
TAG_SERIAL_FLUSH = 2
TAG_CMD = 3

# regex matches an potential PC value (0x4xxxxxxx)
MATCH_PCADDR = re.compile(r'0x4[0-9a-f]{7}', re.IGNORECASE)

DEFAULT_TOOLCHAIN_PREFIX = 'xtensa-esp32-elf-'

DEFAULT_PRINT_FILTER = ''

# coredump related messages
COREDUMP_UART_START = b'================= CORE DUMP START ================='
COREDUMP_UART_END = b'================= CORE DUMP END ================='
COREDUMP_UART_PROMPT = b'Press Enter to print core dump to UART...'

# coredump states
COREDUMP_IDLE = 0
COREDUMP_READING = 1
COREDUMP_DONE = 2

# coredump decoding options
COREDUMP_DECODE_DISABLE = 'disable'
COREDUMP_DECODE_INFO = 'info'

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


class StoppableThread(object):
    """
    Provide a Thread-like class which can be 'cancelled' via a subclass-provided
    cancellation method.

    Can be started and stopped multiple times.

    Isn't an instance of type Thread because Python Thread objects can only be run once
    """
    def __init__(self):
        self._thread = None

    @property
    def alive(self):
        """
        Is 'alive' whenever the internal thread object exists
        """
        return self._thread is not None

    def start(self):
        if self._thread is None:
            self._thread = threading.Thread(target=self._run_outer)
            self._thread.start()

    def _cancel(self):
        pass  # override to provide cancellation functionality

    def run(self):
        pass  # override for the main thread behaviour

    def _run_outer(self):
        try:
            self.run()
        finally:
            self._thread = None

    def stop(self):
        if self._thread is not None:
            old_thread = self._thread
            self._thread = None
            self._cancel()
            old_thread.join()


class ConsoleReader(StoppableThread):
    """ Read input keys from the console and push them to the queue,
    until stopped.
    """
    def __init__(self, console, event_queue, cmd_queue, parser, test_mode):
        super(ConsoleReader, self).__init__()
        self.console = console
        self.event_queue = event_queue
        self.cmd_queue = cmd_queue
        self.parser = parser
        self.test_mode = test_mode

    def run(self):
        self.console.setup()
        try:
            while self.alive:
                try:
                    if os.name == 'nt':
                        # Windows kludge: because the console.cancel() method doesn't
                        # seem to work to unblock getkey() on the Windows implementation.
                        #
                        # So we only call getkey() if we know there's a key waiting for us.
                        import msvcrt
                        while not msvcrt.kbhit() and self.alive:
                            time.sleep(0.1)
                        if not self.alive:
                            break
                    elif self.test_mode:
                        # In testing mode the stdin is connected to PTY but is not used for input anything. For PTY
                        # the canceling by fcntl.ioctl isn't working and would hang in self.console.getkey().
                        # Therefore, we avoid calling it.
                        while self.alive:
                            time.sleep(0.1)
                        break
                    c = self.console.getkey()
                except KeyboardInterrupt:
                    c = '\x03'
                if c is not None:
                    ret = self.parser.parse(c)
                    if ret is not None:
                        (tag, cmd) = ret
                        # stop command should be executed last
                        if tag == TAG_CMD and cmd != CMD_STOP:
                            self.cmd_queue.put(ret)
                        else:
                            self.event_queue.put(ret)

        finally:
            self.console.cleanup()

    def _cancel(self):
        if os.name == 'posix' and not self.test_mode:
            # this is the way cancel() is implemented in pyserial 3.3 or newer,
            # older pyserial (3.1+) has cancellation implemented via 'select',
            # which does not work when console sends an escape sequence response
            #
            # even older pyserial (<3.1) does not have this method
            #
            # on Windows there is a different (also hacky) fix, applied above.
            #
            # note that TIOCSTI is not implemented in WSL / bash-on-Windows.
            # TODO: introduce some workaround to make it work there.
            #
            # Note: This would throw exception in testing mode when the stdin is connected to PTY.
            import fcntl
            import termios
            fcntl.ioctl(self.console.fd, termios.TIOCSTI, b'\0')


class ConsoleParser(object):

    def __init__(self, eol='CRLF'):
        self.translate_eol = {
            'CRLF': lambda c: c.replace('\n', '\r\n'),
            'CR': lambda c: c.replace('\n', '\r'),
            'LF': lambda c: c.replace('\r', '\n'),
        }[eol]
        self.menu_key = CTRL_T
        self.exit_key = CTRL_RBRACKET
        self._pressed_menu_key = False

    def parse(self, key):
        ret = None
        if self._pressed_menu_key:
            ret = self._handle_menu_key(key)
        elif key == self.menu_key:
            self._pressed_menu_key = True
        elif key == self.exit_key:
            ret = (TAG_CMD, CMD_STOP)
        else:
            key = self.translate_eol(key)
            ret = (TAG_KEY, key)
        return ret

    def _handle_menu_key(self, c):
        ret = None
        if c == self.exit_key or c == self.menu_key:  # send verbatim
            ret = (TAG_KEY, c)
        elif c in [CTRL_H, 'h', 'H', '?']:
            red_print(self.get_help_text())
        elif c == CTRL_R:  # Reset device via RTS
            ret = (TAG_CMD, CMD_RESET)
        elif c == CTRL_F:  # Recompile & upload
            ret = (TAG_CMD, CMD_MAKE)
        elif c in [CTRL_A, 'a', 'A']:  # Recompile & upload app only
            # "CTRL-A" cannot be captured with the default settings of the Windows command line, therefore, "A" can be used
            # instead
            ret = (TAG_CMD, CMD_APP_FLASH)
        elif c == CTRL_Y:  # Toggle output display
            ret = (TAG_CMD, CMD_OUTPUT_TOGGLE)
        elif c == CTRL_L:  # Toggle saving output into file
            ret = (TAG_CMD, CMD_TOGGLE_LOGGING)
        elif c == CTRL_P:
            yellow_print('Pause app (enter bootloader mode), press Ctrl-T Ctrl-R to restart')
            # to fast trigger pause without press menu key
            ret = (TAG_CMD, CMD_ENTER_BOOT)
        elif c in [CTRL_X, 'x', 'X']:  # Exiting from within the menu
            ret = (TAG_CMD, CMD_STOP)
        else:
            red_print('--- unknown menu character {} --'.format(key_description(c)))

        self._pressed_menu_key = False
        return ret

    def get_help_text(self):
        text = """\
            --- idf_monitor ({version}) - ESP-IDF monitor tool
            --- based on miniterm from pySerial
            ---
            --- {exit:8} Exit program
            --- {menu:8} Menu escape key, followed by:
            --- Menu keys:
            ---    {menu:14} Send the menu character itself to remote
            ---    {exit:14} Send the exit character itself to remote
            ---    {reset:14} Reset target board via RTS line
            ---    {makecmd:14} Build & flash project
            ---    {appmake:14} Build & flash app only
            ---    {output:14} Toggle output display
            ---    {log:14} Toggle saving output into file
            ---    {pause:14} Reset target into bootloader to pause app via RTS line
            ---    {menuexit:14} Exit program
        """.format(version=__version__,
                   exit=key_description(self.exit_key),
                   menu=key_description(self.menu_key),
                   reset=key_description(CTRL_R),
                   makecmd=key_description(CTRL_F),
                   appmake=key_description(CTRL_A) + ' (or A)',
                   output=key_description(CTRL_Y),
                   log=key_description(CTRL_L),
                   pause=key_description(CTRL_P),
                   menuexit=key_description(CTRL_X) + ' (or X)')
        return textwrap.dedent(text)

    def get_next_action_text(self):
        text = """\
            --- Press {} to exit monitor.
            --- Press {} to build & flash project.
            --- Press {} to build & flash app.
            --- Press any other key to resume monitor (resets target).
        """.format(key_description(self.exit_key),
                   key_description(CTRL_F),
                   key_description(CTRL_A))
        return textwrap.dedent(text)

    def parse_next_action_key(self, c):
        ret = None
        if c == self.exit_key:
            ret = (TAG_CMD, CMD_STOP)
        elif c == CTRL_F:  # Recompile & upload
            ret = (TAG_CMD, CMD_MAKE)
        elif c in [CTRL_A, 'a', 'A']:  # Recompile & upload app only
            # "CTRL-A" cannot be captured with the default settings of the Windows command line, therefore, "A" can be used
            # instead
            ret = (TAG_CMD, CMD_APP_FLASH)
        return ret


class SerialReader(StoppableThread):
    """ Read serial data from the serial port and push to the
    event queue, until stopped.
    """
    def __init__(self, serial, event_queue):
        super(SerialReader, self).__init__()
        self.baud = serial.baudrate
        self.serial = serial
        self.event_queue = event_queue
        if not hasattr(self.serial, 'cancel_read'):
            # enable timeout for checking alive flag,
            # if cancel_read not available
            self.serial.timeout = 0.25

    def run(self):
        if not self.serial.is_open:
            self.serial.baudrate = self.baud
            self.serial.rts = True  # Force an RTS reset on open
            self.serial.open()
            time.sleep(0.005)  # Add a delay to meet the requirements of minimal EN low time (2ms for ESP32-C3)
            self.serial.rts = False
            self.serial.dtr = self.serial.dtr   # usbser.sys workaround
        try:
            while self.alive:
                try:
                    data = self.serial.read(self.serial.in_waiting or 1)
                except (serial.serialutil.SerialException, IOError) as e:
                    data = b''
                    # self.serial.open() was successful before, therefore, this is an issue related to
                    # the disappearance of the device
                    red_print(e)
                    yellow_print('Waiting for the device to reconnect', newline='')
                    self.serial.close()
                    while self.alive:  # so that exiting monitor works while waiting
                        try:
                            time.sleep(0.5)
                            self.serial.open()
                            break  # device connected
                        except serial.serialutil.SerialException:
                            yellow_print('.', newline='')
                            sys.stderr.flush()
                    yellow_print('')  # go to new line
                if len(data):
                    self.event_queue.put((TAG_SERIAL, data), False)
        finally:
            self.serial.close()

    def _cancel(self):
        if hasattr(self.serial, 'cancel_read'):
            try:
                self.serial.cancel_read()
            except Exception:
                pass


class LineMatcher(object):
    """
    Assembles a dictionary of filtering rules based on the --print_filter
    argument of idf_monitor. Then later it is used to match lines and
    determine whether they should be shown on screen or not.
    """
    LEVEL_N = 0
    LEVEL_E = 1
    LEVEL_W = 2
    LEVEL_I = 3
    LEVEL_D = 4
    LEVEL_V = 5

    level = {'N': LEVEL_N, 'E': LEVEL_E, 'W': LEVEL_W, 'I': LEVEL_I, 'D': LEVEL_D,
             'V': LEVEL_V, '*': LEVEL_V, '': LEVEL_V}

    def __init__(self, print_filter):
        self._dict = dict()
        self._re = re.compile(r'^(?:\033\[[01];?[0-9]+m?)?([EWIDV]) \([0-9]+\) ([^:]+): ')
        items = print_filter.split()
        if len(items) == 0:
            self._dict['*'] = self.LEVEL_V  # default is to print everything
        for f in items:
            s = f.split(r':')
            if len(s) == 1:
                # specifying no warning level defaults to verbose level
                lev = self.LEVEL_V
            elif len(s) == 2:
                if len(s[0]) == 0:
                    raise ValueError('No tag specified in filter ' + f)
                try:
                    lev = self.level[s[1].upper()]
                except KeyError:
                    raise ValueError('Unknown warning level in filter ' + f)
            else:
                raise ValueError('Missing ":" in filter ' + f)
            self._dict[s[0]] = lev

    def match(self, line):
        try:
            m = self._re.search(line)
            if m:
                lev = self.level[m.group(1)]
                if m.group(2) in self._dict:
                    return self._dict[m.group(2)] >= lev
                return self._dict.get('*', self.LEVEL_N) >= lev
        except (KeyError, IndexError):
            # Regular line written with something else than ESP_LOG*
            # or an empty line.
            pass
        # We need something more than "*.N" for printing.
        return self._dict.get('*', self.LEVEL_N) > self.LEVEL_N


class SerialStopException(Exception):
    """
    This exception is used for stopping the IDF monitor in testing mode.
    """
    pass


class Monitor(object):
    """
    Monitor application main class.

    This was originally derived from miniterm.Miniterm, but it turned out to be easier to write from scratch for this
    purpose.

    Main difference is that all event processing happens in the main thread, not the worker threads.
    """
    def __init__(self, serial_instance, elf_file, print_filter, make='make', encrypted=False,
                 toolchain_prefix=DEFAULT_TOOLCHAIN_PREFIX, eol='CRLF',
                 decode_coredumps=COREDUMP_DECODE_INFO,
                 decode_panic=PANIC_DECODE_DISABLE,
                 target=None,
                 websocket_client=None,
                 enable_address_decoding=True):
        super(Monitor, self).__init__()
        self.event_queue = queue.Queue()
        self.cmd_queue = queue.Queue()
        self.console = miniterm.Console()
        self.enable_address_decoding = enable_address_decoding
        if os.name == 'nt':
            sys.stderr = ANSIColorConverter(sys.stderr, decode_output=True)
            self.console.output = ANSIColorConverter(self.console.output)
            self.console.byte_output = ANSIColorConverter(self.console.byte_output)

        if StrictVersion(serial.VERSION) < StrictVersion('3.3.0'):
            # Use Console.getkey implementation from 3.3.0 (to be in sync with the ConsoleReader._cancel patch above)
            def getkey_patched(self):
                c = self.enc_stdin.read(1)
                if c == chr(0x7f):
                    c = chr(8)    # map the BS key (which yields DEL) to backspace
                return c

            self.console.getkey = types.MethodType(getkey_patched, self.console)

        socket_mode = serial_instance.port.startswith('socket://')  # testing hook - data from serial can make exit the monitor
        self.serial = serial_instance
        self.console_parser = ConsoleParser(eol)
        self.console_reader = ConsoleReader(self.console, self.event_queue, self.cmd_queue, self.console_parser, socket_mode)
        self.serial_reader = SerialReader(self.serial, self.event_queue)
        self.elf_file = elf_file
        if not os.path.exists(make):
            self.make = shlex.split(make)  # allow for possibility the "make" arg is a list of arguments (for idf.py)
        else:
            self.make = make
        self.encrypted = encrypted
        self.toolchain_prefix = toolchain_prefix
        self.websocket_client = websocket_client
        self.target = target

        # internal state
        self._last_line_part = b''
        self._gdb_buffer = b''
        self._pc_address_buffer = b''
        self._line_matcher = LineMatcher(print_filter)
        self._invoke_processing_last_line_timer = None
        self._force_line_print = False
        self._output_enabled = True
        self._serial_check_exit = socket_mode
        self._log_file = None
        self._decode_coredumps = decode_coredumps
        self._reading_coredump = COREDUMP_IDLE
        self._coredump_buffer = b''
        self._decode_panic = decode_panic
        self._reading_panic = PANIC_IDLE
        self._panic_buffer = b''

    def invoke_processing_last_line(self):
        self.event_queue.put((TAG_SERIAL_FLUSH, b''), False)

    def main_loop(self):
        self.console_reader.start()
        self.serial_reader.start()
        try:
            while self.console_reader.alive and self.serial_reader.alive:
                try:
                    item = self.cmd_queue.get_nowait()
                except queue.Empty:
                    try:
                        item = self.event_queue.get(True, 0.03)
                    except queue.Empty:
                        continue

                (event_tag, data) = item
                if event_tag == TAG_CMD:
                    self.handle_commands(data)
                elif event_tag == TAG_KEY:
                    try:
                        self.serial.write(codecs.encode(data))
                    except serial.SerialException:
                        pass  # this shouldn't happen, but sometimes port has closed in serial thread
                    except UnicodeEncodeError:
                        pass  # this can happen if a non-ascii character was passed, ignoring
                elif event_tag == TAG_SERIAL:
                    self.handle_serial_input(data)
                    if self._invoke_processing_last_line_timer is not None:
                        self._invoke_processing_last_line_timer.cancel()
                    self._invoke_processing_last_line_timer = threading.Timer(0.1, self.invoke_processing_last_line)
                    self._invoke_processing_last_line_timer.start()
                    # If no further data is received in the next short period
                    # of time then the _invoke_processing_last_line_timer
                    # generates an event which will result in the finishing of
                    # the last line. This is fix for handling lines sent
                    # without EOL.
                elif event_tag == TAG_SERIAL_FLUSH:
                    self.handle_serial_input(data, finalize_line=True)
                else:
                    raise RuntimeError('Bad event data %r' % ((event_tag,data),))
        except SerialStopException:
            sys.stderr.write(ANSI_NORMAL + 'Stopping condition has been received\n')
        finally:
            try:
                self.console_reader.stop()
                self.serial_reader.stop()
                self.stop_logging()
                # Cancelling _invoke_processing_last_line_timer is not
                # important here because receiving empty data doesn't matter.
                self._invoke_processing_last_line_timer = None
            except Exception:
                pass
            sys.stderr.write(ANSI_NORMAL + '\n')

    def handle_serial_input(self, data, finalize_line=False):
        sp = data.split(b'\n')
        if self._last_line_part != b'':
            # add unprocessed part from previous "data" to the first line
            sp[0] = self._last_line_part + sp[0]
            self._last_line_part = b''
        if sp[-1] != b'':
            # last part is not a full line
            self._last_line_part = sp.pop()
        for line in sp:
            if line != b'':
                if self._serial_check_exit and line == self.console_parser.exit_key.encode('latin-1'):
                    raise SerialStopException()
                self.check_panic_decode_trigger(line)
                self.check_coredump_trigger_before_print(line)
                if self._force_line_print or self._line_matcher.match(line.decode(errors='ignore')):
                    self._print(line + b'\n')
                    self.handle_possible_pc_address_in_line(line)
                self.check_coredump_trigger_after_print(line)
                self.check_gdbstub_trigger(line)
                self._force_line_print = False
        # Now we have the last part (incomplete line) in _last_line_part. By
        # default we don't touch it and just wait for the arrival of the rest
        # of the line. But after some time when we didn't received it we need
        # to make a decision.
        if self._last_line_part != b'':
            if self._force_line_print or (finalize_line and self._line_matcher.match(self._last_line_part.decode(errors='ignore'))):
                self._force_line_print = True
                self._print(self._last_line_part)
                self.handle_possible_pc_address_in_line(self._last_line_part)
                self.check_gdbstub_trigger(self._last_line_part)
                # It is possible that the incomplete line cuts in half the PC
                # address. A small buffer is kept and will be used the next time
                # handle_possible_pc_address_in_line is invoked to avoid this problem.
                # MATCH_PCADDR matches 10 character long addresses. Therefore, we
                # keep the last 9 characters.
                self._pc_address_buffer = self._last_line_part[-9:]
                # GDB sequence can be cut in half also. GDB sequence is 7
                # characters long, therefore, we save the last 6 characters.
                self._gdb_buffer = self._last_line_part[-6:]
                self._last_line_part = b''
        # else: keeping _last_line_part and it will be processed the next time
        # handle_serial_input is invoked

    def handle_possible_pc_address_in_line(self, line):
        line = self._pc_address_buffer + line
        self._pc_address_buffer = b''
        if self.enable_address_decoding:
            for m in re.finditer(MATCH_PCADDR, line.decode(errors='ignore')):
                self.lookup_pc_address(m.group())

    def __enter__(self):
        """ Use 'with self' to temporarily disable monitoring behaviour """
        self.serial_reader.stop()
        self.console_reader.stop()

    def __exit__(self, *args, **kwargs):
        """ Use 'with self' to temporarily disable monitoring behaviour """
        self.console_reader.start()
        self.serial_reader.start()

    def prompt_next_action(self, reason):
        self.console.setup()  # set up console to trap input characters
        try:
            red_print('--- {}'.format(reason))
            red_print(self.console_parser.get_next_action_text())

            k = CTRL_T  # ignore CTRL-T here, so people can muscle-memory Ctrl-T Ctrl-F, etc.
            while k == CTRL_T:
                k = self.console.getkey()
        finally:
            self.console.cleanup()
        ret = self.console_parser.parse_next_action_key(k)
        if ret is not None:
            cmd = ret[1]
            if cmd == CMD_STOP:
                # the stop command should be handled last
                self.event_queue.put(ret)
            else:
                self.cmd_queue.put(ret)

    def run_make(self, target):
        with self:
            if isinstance(self.make, list):
                popen_args = self.make + [target]
            else:
                popen_args = [self.make, target]
            yellow_print('Running %s...' % ' '.join(popen_args))
            p = subprocess.Popen(popen_args, env=os.environ)
            try:
                p.wait()
            except KeyboardInterrupt:
                p.wait()
            if p.returncode != 0:
                self.prompt_next_action('Build failed')
            else:
                self.output_enable(True)

    def lookup_pc_address(self, pc_addr):
        cmd = ['%saddr2line' % self.toolchain_prefix,
               '-pfiaC', '-e', self.elf_file, pc_addr]
        try:
            translation = subprocess.check_output(cmd, cwd='.')
            if b'?? ??:0' not in translation:
                self._print(translation.decode(), console_printer=yellow_print)
        except OSError as e:
            red_print('%s: %s' % (' '.join(cmd), e))

    def check_gdbstub_trigger(self, line):
        line = self._gdb_buffer + line
        self._gdb_buffer = b''
        m = re.search(b'\\$(T..)#(..)', line)  # look for a gdb "reason" for a break
        if m is not None:
            try:
                chsum = sum(ord(bytes([p])) for p in m.group(1)) & 0xFF
                calc_chsum = int(m.group(2), 16)
            except ValueError:
                return  # payload wasn't valid hex digits
            if chsum == calc_chsum:
                if self.websocket_client:
                    yellow_print('Communicating through WebSocket')
                    self.websocket_client.send({'event': 'gdb_stub',
                                                'port': self.serial.port,
                                                'prog': self.elf_file})
                    yellow_print('Waiting for debug finished event')
                    self.websocket_client.wait([('event', 'debug_finished')])
                    yellow_print('Communications through WebSocket is finished')
                else:
                    self.run_gdb()
            else:
                red_print('Malformed gdb message... calculated checksum %02x received %02x' % (chsum, calc_chsum))

    def check_coredump_trigger_before_print(self, line):
        if self._decode_coredumps == COREDUMP_DECODE_DISABLE:
            return

        if COREDUMP_UART_PROMPT in line:
            yellow_print('Initiating core dump!')
            self.event_queue.put((TAG_KEY, '\n'))
            return

        if COREDUMP_UART_START in line:
            yellow_print('Core dump started (further output muted)')
            self._reading_coredump = COREDUMP_READING
            self._coredump_buffer = b''
            self._output_enabled = False
            return

        if COREDUMP_UART_END in line:
            self._reading_coredump = COREDUMP_DONE
            yellow_print('\nCore dump finished!')
            self.process_coredump()
            return

        if self._reading_coredump == COREDUMP_READING:
            kb = 1024
            buffer_len_kb = len(self._coredump_buffer) // kb
            self._coredump_buffer += line.replace(b'\r', b'') + b'\n'
            new_buffer_len_kb = len(self._coredump_buffer) // kb
            if new_buffer_len_kb > buffer_len_kb:
                yellow_print('Received %3d kB...' % (new_buffer_len_kb), newline='\r')

    def check_coredump_trigger_after_print(self, line):
        if self._decode_coredumps == COREDUMP_DECODE_DISABLE:
            return

        # Re-enable output after the last line of core dump has been consumed
        if not self._output_enabled and self._reading_coredump == COREDUMP_DONE:
            self._reading_coredump = COREDUMP_IDLE
            self._output_enabled = True
            self._coredump_buffer = b''

    def process_coredump(self):
        if self._decode_coredumps != COREDUMP_DECODE_INFO:
            raise NotImplementedError('process_coredump: %s not implemented' % self._decode_coredumps)

        coredump_script = os.path.join(os.path.dirname(__file__), '..', 'components', 'espcoredump', 'espcoredump.py')
        coredump_file = None
        try:
            # On Windows, the temporary file can't be read unless it is closed.
            # Set delete=False and delete the file manually later.
            with tempfile.NamedTemporaryFile(mode='wb', delete=False) as coredump_file:
                coredump_file.write(self._coredump_buffer)
                coredump_file.flush()

            if self.websocket_client:
                self._output_enabled = True
                yellow_print('Communicating through WebSocket')
                self.websocket_client.send({'event': 'coredump',
                                            'file': coredump_file.name,
                                            'prog': self.elf_file})
                yellow_print('Waiting for debug finished event')
                self.websocket_client.wait([('event', 'debug_finished')])
                yellow_print('Communications through WebSocket is finished')
            else:
                cmd = [sys.executable,
                       coredump_script,
                       'info_corefile',
                       '--core', coredump_file.name,
                       '--core-format', 'b64',
                       self.elf_file
                       ]
                output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
                self._output_enabled = True
                self._print(output)
                self._output_enabled = False  # Will be reenabled in check_coredump_trigger_after_print
        except subprocess.CalledProcessError as e:
            yellow_print('Failed to run espcoredump script: {}\n{}\n\n'.format(e, e.output))
            self._output_enabled = True
            self._print(COREDUMP_UART_START + b'\n')
            self._print(self._coredump_buffer)
            # end line will be printed in handle_serial_input
        finally:
            if coredump_file is not None:
                try:
                    os.unlink(coredump_file.name)
                except OSError as e:
                    yellow_print("Couldn't remote temporary core dump file ({})".format(e))

    def check_panic_decode_trigger(self, line):
        if self._decode_panic == PANIC_DECODE_DISABLE:
            return

        if self._reading_panic == PANIC_IDLE and re.search(PANIC_START, line.decode('ascii', errors='ignore')):
            self._reading_panic = PANIC_READING
            yellow_print('Stack dump detected')

        if self._reading_panic == PANIC_READING and PANIC_STACK_DUMP in line:
            self._output_enabled = False

        if self._reading_panic == PANIC_READING:
            self._panic_buffer += line.replace(b'\r', b'') + b'\n'

        if self._reading_panic == PANIC_READING and PANIC_END in line:
            self._reading_panic = PANIC_IDLE
            self._output_enabled = True
            self.process_panic_output(self._panic_buffer)
            self._panic_buffer = b''

    def process_panic_output(self, panic_output):
        panic_output_decode_script = os.path.join(os.path.dirname(__file__), '..', 'tools', 'gdb_panic_server.py')
        panic_output_file = None
        try:
            # On Windows, the temporary file can't be read unless it is closed.
            # Set delete=False and delete the file manually later.
            with tempfile.NamedTemporaryFile(mode='wb', delete=False) as panic_output_file:
                panic_output_file.write(panic_output)
                panic_output_file.flush()

            cmd = [self.toolchain_prefix + 'gdb',
                   '--batch', '-n',
                   self.elf_file,
                   '-ex', "target remote | \"{python}\" \"{script}\" --target {target} \"{output_file}\""
                   .format(python=sys.executable,
                           script=panic_output_decode_script,
                           target=self.target,
                           output_file=panic_output_file.name),
                   '-ex', 'bt']

            output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
            yellow_print('\nBacktrace:\n\n')
            self._print(output)
        except subprocess.CalledProcessError as e:
            yellow_print('Failed to run gdb_panic_server.py script: {}\n{}\n\n'.format(e, e.output))
            self._print(panic_output)
        finally:
            if panic_output_file is not None:
                try:
                    os.unlink(panic_output_file.name)
                except OSError as e:
                    yellow_print("Couldn't remove temporary panic output file ({})".format(e))

    def run_gdb(self):
        with self:  # disable console control
            sys.stderr.write(ANSI_NORMAL)
            try:
                cmd = ['%sgdb' % self.toolchain_prefix,
                       '-ex', 'set serial baud %d' % self.serial.baudrate,
                       '-ex', 'target remote %s' % self.serial.port,
                       '-ex', 'interrupt',  # monitor has already parsed the first 'reason' command, need a second
                       self.elf_file]
                process = subprocess.Popen(cmd, cwd='.')
                process.wait()
            except OSError as e:
                red_print('%s: %s' % (' '.join(cmd), e))
            except KeyboardInterrupt:
                pass  # happens on Windows, maybe other OSes
            finally:
                try:
                    # on Linux, maybe other OSes, gdb sometimes seems to be alive even after wait() returns...
                    process.terminate()
                except Exception:
                    pass
                try:
                    # also on Linux, maybe other OSes, gdb sometimes exits uncleanly and breaks the tty mode
                    subprocess.call(['stty', 'sane'])
                except Exception:
                    pass  # don't care if there's no stty, we tried...
            self.prompt_next_action('gdb exited')

    def output_enable(self, enable):
        self._output_enabled = enable

    def output_toggle(self):
        self._output_enabled = not self._output_enabled
        yellow_print('\nToggle output display: {}, Type Ctrl-T Ctrl-Y to show/disable output again.'.format(self._output_enabled))

    def toggle_logging(self):
        if self._log_file:
            self.stop_logging()
        else:
            self.start_logging()

    def start_logging(self):
        if not self._log_file:
            try:
                name = 'log.{}.{}.txt'.format(os.path.splitext(os.path.basename(self.elf_file))[0],
                                              datetime.datetime.now().strftime('%Y%m%d%H%M%S'))
                self._log_file = open(name, 'wb+')
                yellow_print('\nLogging is enabled into file {}'.format(name))
            except Exception as e:
                red_print('\nLog file {} cannot be created: {}'.format(name, e))

    def stop_logging(self):
        if self._log_file:
            try:
                name = self._log_file.name
                self._log_file.close()
                yellow_print('\nLogging is disabled and file {} has been closed'.format(name))
            except Exception as e:
                red_print('\nLog file cannot be closed: {}'.format(e))
            finally:
                self._log_file = None

    def _print(self, string, console_printer=None):
        if console_printer is None:
            console_printer = self.console.write_bytes
        if self._output_enabled:
            console_printer(string)
        if self._log_file:
            try:
                if isinstance(string, type(u'')):
                    string = string.encode()
                self._log_file.write(string)
            except Exception as e:
                red_print('\nCannot write to file: {}'.format(e))
                # don't fill-up the screen with the previous errors (probably consequent prints would fail also)
                self.stop_logging()

    def handle_commands(self, cmd):
        if cmd == CMD_STOP:
            self.console_reader.stop()
            self.serial_reader.stop()
        elif cmd == CMD_RESET:
            self.serial.setRTS(True)
            self.serial.setDTR(self.serial.dtr)  # usbser.sys workaround
            time.sleep(0.2)
            self.serial.setRTS(False)
            self.serial.setDTR(self.serial.dtr)  # usbser.sys workaround
            self.output_enable(True)
        elif cmd == CMD_MAKE:
            self.run_make('encrypted-flash' if self.encrypted else 'flash')
        elif cmd == CMD_APP_FLASH:
            self.run_make('encrypted-app-flash' if self.encrypted else 'app-flash')
        elif cmd == CMD_OUTPUT_TOGGLE:
            self.output_toggle()
        elif cmd == CMD_TOGGLE_LOGGING:
            self.toggle_logging()
        elif cmd == CMD_ENTER_BOOT:
            self.serial.setDTR(False)  # IO0=HIGH
            self.serial.setRTS(True)   # EN=LOW, chip in reset
            self.serial.setDTR(self.serial.dtr)  # usbser.sys workaround
            time.sleep(1.3)  # timeouts taken from esptool.py, includes esp32r0 workaround. defaults: 0.1
            self.serial.setDTR(True)   # IO0=LOW
            self.serial.setRTS(False)  # EN=HIGH, chip out of reset
            self.serial.setDTR(self.serial.dtr)  # usbser.sys workaround
            time.sleep(0.45)  # timeouts taken from esptool.py, includes esp32r0 workaround. defaults: 0.05
            self.serial.setDTR(False)  # IO0=HIGH, done
        else:
            raise RuntimeError('Bad command data %d' % (cmd))


def main():
    parser = argparse.ArgumentParser('idf_monitor - a serial output monitor for esp-idf')

    parser.add_argument(
        '--port', '-p',
        help='Serial port device',
        default=os.environ.get('ESPTOOL_PORT', '/dev/ttyUSB0')
    )

    parser.add_argument(
        '--disable-address-decoding', '-d',
        help="Don't print lines about decoded addresses from the application ELF file.",
        action='store_true',
        default=True if os.environ.get('ESP_MONITOR_DECODE') == 0 else False
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
        type=argparse.FileType('rb'))

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
        required=False,
        help='Target name (used when stack dump decoding is enabled)'
    )

    parser.add_argument(
        '--ws',
        default=os.environ.get('ESP_IDF_MONITOR_WS', None),
        help='WebSocket URL for communicating with IDE tools for debugging purposes'
    )

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

    serial_instance = serial.serial_for_url(args.port, args.baud,
                                            do_not_open=True)
    serial_instance.dtr = False
    serial_instance.rts = False

    args.elf_file.close()  # don't need this as a file

    # remove the parallel jobserver arguments from MAKEFLAGS, as any
    # parent make is only running 1 job (monitor), so we can re-spawn
    # all of the child makes we need (the -j argument remains part of
    # MAKEFLAGS)
    try:
        makeflags = os.environ['MAKEFLAGS']
        makeflags = re.sub(r'--jobserver[^ =]*=[0-9,]+ ?', '', makeflags)
        os.environ['MAKEFLAGS'] = makeflags
    except KeyError:
        pass  # not running a make jobserver

    # Pass the actual used port to callee of idf_monitor (e.g. make) through `ESPPORT` environment
    # variable
    # To make sure the key as well as the value are str type, by the requirements of subprocess
    espport_key = str('ESPPORT')
    espport_val = str(args.port)
    os.environ.update({espport_key: espport_val})

    ws = WebSocketClient(args.ws) if args.ws else None
    try:
        monitor = Monitor(serial_instance, args.elf_file.name, args.print_filter, args.make, args.encrypted,
                          args.toolchain_prefix, args.eol,
                          args.decode_coredumps, args.decode_panic, args.target,
                          ws, enable_address_decoding=not args.disable_address_decoding)

        yellow_print('--- idf_monitor on {p.name} {p.baudrate} ---'.format(
            p=serial_instance))
        yellow_print('--- Quit: {} | Menu: {} | Help: {} followed by {} ---'.format(
            key_description(monitor.console_parser.exit_key),
            key_description(monitor.console_parser.menu_key),
            key_description(monitor.console_parser.menu_key),
            key_description(CTRL_H)))
        if args.print_filter != DEFAULT_PRINT_FILTER:
            yellow_print('--- Print filter: {} ---'.format(args.print_filter))

        monitor.main_loop()
    finally:
        if ws:
            ws.close()


class WebSocketClient(object):
    """
    WebSocket client used to advertise debug events to WebSocket server by sending and receiving JSON-serialized
    dictionaries.

    Advertisement of debug event:
    {'event': 'gdb_stub', 'port': '/dev/ttyUSB1', 'prog': 'build/elf_file'} for GDB Stub, or
    {'event': 'coredump', 'file': '/tmp/xy', 'prog': 'build/elf_file'} for coredump,
    where 'port' is the port for the connected device, 'prog' is the full path to the ELF file and 'file' is the
    generated coredump file.

    Expected end of external debugging:
    {'event': 'debug_finished'}
    """

    RETRIES = 3
    CONNECTION_RETRY_DELAY = 1

    def __init__(self, url):
        self.url = url
        self._connect()

    def _connect(self):
        """
        Connect to WebSocket server at url
        """
        self.close()

        for _ in range(self.RETRIES):
            try:
                self.ws = websocket.create_connection(self.url)
                break  # success
            except NameError:
                raise RuntimeError('Please install the websocket_client package for IDE integration!')
            except Exception as e:
                red_print('WebSocket connection error: {}'.format(e))
            time.sleep(self.CONNECTION_RETRY_DELAY)
        else:
            raise RuntimeError('Cannot connect to WebSocket server')

    def close(self):
        try:
            self.ws.close()
        except AttributeError:
            # Not yet connected
            pass
        except Exception as e:
            red_print('WebSocket close error: {}'.format(e))

    def send(self, payload_dict):
        """
        Serialize payload_dict in JSON format and send it to the server
        """
        for _ in range(self.RETRIES):
            try:
                self.ws.send(json.dumps(payload_dict))
                yellow_print('WebSocket sent: {}'.format(payload_dict))
                break
            except Exception as e:
                red_print('WebSocket send error: {}'.format(e))
                self._connect()
        else:
            raise RuntimeError('Cannot send to WebSocket server')

    def wait(self, expect_iterable):
        """
        Wait until a dictionary in JSON format is received from the server with all (key, value) tuples from
        expect_iterable.
        """
        for _ in range(self.RETRIES):
            try:
                r = self.ws.recv()
            except Exception as e:
                red_print('WebSocket receive error: {}'.format(e))
                self._connect()
                continue
            obj = json.loads(r)
            if all([k in obj and obj[k] == v for k, v in expect_iterable]):
                yellow_print('WebSocket received: {}'.format(obj))
                break
            red_print('WebSocket expected: {}, received: {}'.format(dict(expect_iterable), obj))
        else:
            raise RuntimeError('Cannot receive from WebSocket server')


if os.name == 'nt':
    # Windows console stuff

    STD_OUTPUT_HANDLE = -11
    STD_ERROR_HANDLE = -12

    # wincon.h values
    FOREGROUND_INTENSITY = 8
    FOREGROUND_GREY = 7

    # matches the ANSI color change sequences that IDF sends
    RE_ANSI_COLOR = re.compile(b'\033\\[([01]);3([0-7])m')

    # list mapping the 8 ANSI colors (the indexes) to Windows Console colors
    ANSI_TO_WINDOWS_COLOR = [0, 4, 2, 6, 1, 5, 3, 7]

    GetStdHandle = ctypes.windll.kernel32.GetStdHandle
    SetConsoleTextAttribute = ctypes.windll.kernel32.SetConsoleTextAttribute

    class ANSIColorConverter(object):
        """Class to wrap a file-like output stream, intercept ANSI color codes,
        and convert them into calls to Windows SetConsoleTextAttribute.

        Doesn't support all ANSI terminal code escape sequences, only the sequences IDF uses.

        Ironically, in Windows this console output is normally wrapped by winpty which will then detect the console text
        color changes and convert these back to ANSI color codes for MSYS' terminal to display. However this is the
        least-bad working solution, as winpty doesn't support any "passthrough" mode for raw output.
        """

        def __init__(self, output=None, decode_output=False):
            self.output = output
            self.decode_output = decode_output
            self.handle = GetStdHandle(STD_ERROR_HANDLE if self.output == sys.stderr else STD_OUTPUT_HANDLE)
            self.matched = b''

        def _output_write(self, data):
            try:
                if self.decode_output:
                    self.output.write(data.decode())
                else:
                    self.output.write(data)
            except (IOError, OSError):
                # Windows 10 bug since the Fall Creators Update, sometimes writing to console randomly throws
                # an exception (however, the character is still written to the screen)
                # Ref https://github.com/espressif/esp-idf/issues/1163
                #
                # Also possible for Windows to throw an OSError error if the data is invalid for the console
                # (garbage bytes, etc)
                pass
            except UnicodeDecodeError:
                # In case of double byte Unicode characters display '?'
                self.output.write('?')

        def write(self, data):
            if isinstance(data, bytes):
                data = bytearray(data)
            else:
                data = bytearray(data, 'utf-8')
            for b in data:
                b = bytes([b])
                length = len(self.matched)
                if b == b'\033':  # ESC
                    self.matched = b
                elif (length == 1 and b == b'[') or (1 < length < 7):
                    self.matched += b
                    if self.matched == ANSI_NORMAL.encode('latin-1'):  # reset console
                        # Flush is required only with Python3 - switching color before it is printed would mess up the console
                        self.flush()
                        SetConsoleTextAttribute(self.handle, FOREGROUND_GREY)
                        self.matched = b''
                    elif len(self.matched) == 7:     # could be an ANSI sequence
                        m = re.match(RE_ANSI_COLOR, self.matched)
                        if m is not None:
                            color = ANSI_TO_WINDOWS_COLOR[int(m.group(2))]
                            if m.group(1) == b'1':
                                color |= FOREGROUND_INTENSITY
                            # Flush is required only with Python3 - switching color before it is printed would mess up the console
                            self.flush()
                            SetConsoleTextAttribute(self.handle, color)
                        else:
                            self._output_write(self.matched)  # not an ANSI color code, display verbatim
                        self.matched = b''
                else:
                    self._output_write(b)
                    self.matched = b''

        def flush(self):
            try:
                self.output.flush()
            except OSError:
                # Account for Windows Console refusing to accept garbage bytes (serial noise, etc)
                pass


if __name__ == '__main__':
    main()
