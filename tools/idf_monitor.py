#!/usr/bin/env python
#
# esp-idf serial output monitor tool. Does some helpful things:
# - Looks up hex addresses in ELF file with addr2line
# - Reset ESP32 via serial RTS line (Ctrl-T Ctrl-R)
# - Run "make flash" (Ctrl-T Ctrl-F)
# - Run "make app-flash" (Ctrl-T Ctrl-A)
# - If gdbstub output is detected, gdb is automatically loaded
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
from __future__ import print_function, division
import subprocess
import argparse
import codecs
import re
import os
try:
    import queue
except ImportError:
    import Queue as queue
import time
import sys
import serial
import serial.tools.miniterm as miniterm
import threading
import ctypes
import types
from distutils.version import StrictVersion

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
CTRL_RBRACKET = '\x1d'  # Ctrl+]

# ANSI terminal codes
ANSI_RED = '\033[1;31m'
ANSI_YELLOW = '\033[0;33m'
ANSI_NORMAL = '\033[0m'

def color_print(message, color):
    """ Print a message to stderr with colored highlighting """
    sys.stderr.write("%s%s%s\n" % (color, message,  ANSI_NORMAL))

def yellow_print(message):
    color_print(message, ANSI_YELLOW)

def red_print(message):
    color_print(message, ANSI_RED)

__version__ = "1.0"

# Tags for tuples in queues
TAG_KEY = 0
TAG_SERIAL = 1

# regex matches an potential PC value (0x4xxxxxxx)
MATCH_PCADDR = re.compile(r'0x4[0-9a-f]{7}', re.IGNORECASE)

DEFAULT_TOOLCHAIN_PREFIX = "xtensa-esp32-elf-"

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
        pass # override to provide cancellation functionality

    def run(self):
        pass # override for the main thread behaviour

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
    def __init__(self, console, event_queue):
        super(ConsoleReader, self).__init__()
        self.console = console
        self.event_queue = event_queue

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
                    c = self.console.getkey()
                except KeyboardInterrupt:
                    c = '\x03'
                if c is not None:
                    self.event_queue.put((TAG_KEY, c), False)
        finally:
            self.console.cleanup()

    def _cancel(self):
        if os.name == 'posix':
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
            import fcntl, termios
            fcntl.ioctl(self.console.fd, termios.TIOCSTI, b'\0')

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
            self.serial.rts = False
        try:
            while self.alive:
                data = self.serial.read(self.serial.in_waiting or 1)
                if len(data):
                    self.event_queue.put((TAG_SERIAL, data), False)
        finally:
            self.serial.close()

    def _cancel(self):
        if hasattr(self.serial, 'cancel_read'):
            try:
                self.serial.cancel_read()
            except:
                pass


class Monitor(object):
    """
    Monitor application main class.

    This was originally derived from miniterm.Miniterm, but it turned out to be easier to write from scratch for this
    purpose.

    Main difference is that all event processing happens in the main thread, not the worker threads.
    """
    def __init__(self, serial_instance, elf_file, make="make", toolchain_prefix=DEFAULT_TOOLCHAIN_PREFIX, eol="CRLF"):
        super(Monitor, self).__init__()
        self.event_queue = queue.Queue()
        self.console = miniterm.Console()
        if os.name == 'nt':
            sys.stderr = ANSIColorConverter(sys.stderr)
            self.console.output = ANSIColorConverter(self.console.output)
            self.console.byte_output = ANSIColorConverter(self.console.byte_output)

        if StrictVersion(serial.VERSION) < StrictVersion('3.3.0'):
            # Use Console.getkey implementation from 3.3.0 (to be in sync with the ConsoleReader._cancel patch above)
            def getkey_patched(self):
                c = self.enc_stdin.read(1)
                if c == unichr(0x7f):
                    c = unichr(8)    # map the BS key (which yields DEL) to backspace
                return c
            
            self.console.getkey = types.MethodType(getkey_patched, self.console) 
        
        self.serial = serial_instance
        self.console_reader = ConsoleReader(self.console, self.event_queue)
        self.serial_reader = SerialReader(self.serial, self.event_queue)
        self.elf_file = elf_file
        self.make = make
        self.toolchain_prefix = toolchain_prefix
        self.menu_key = CTRL_T
        self.exit_key = CTRL_RBRACKET

        self.translate_eol = {
            "CRLF": lambda c: c.replace(b"\n", b"\r\n"),
            "CR":   lambda c: c.replace(b"\n", b"\r"),
            "LF":   lambda c: c.replace(b"\r", b"\n"),
        }[eol]

        # internal state
        self._pressed_menu_key = False
        self._read_line = b""
        self._gdb_buffer = b""
        self._output_enabled = True

    def main_loop(self):
        self.console_reader.start()
        self.serial_reader.start()
        try:
            while self.console_reader.alive and self.serial_reader.alive:
                (event_tag, data) = self.event_queue.get()
                if event_tag == TAG_KEY:
                    self.handle_key(data)
                elif event_tag == TAG_SERIAL:
                    self.handle_serial_input(data)
                else:
                    raise RuntimeError("Bad event data %r" % ((event_tag,data),))
        finally:
            try:
                self.console_reader.stop()
                self.serial_reader.stop()
            except:
                pass
            sys.stderr.write(ANSI_NORMAL + "\n")

    def handle_key(self, key):
        if self._pressed_menu_key:
            self.handle_menu_key(key)
            self._pressed_menu_key = False
        elif key == self.menu_key:
            self._pressed_menu_key = True
        elif key == self.exit_key:
            self.console_reader.stop()
            self.serial_reader.stop()
        else:
            try:
                key = self.translate_eol(key)
                self.serial.write(codecs.encode(key))
            except serial.SerialException:
                pass # this shouldn't happen, but sometimes port has closed in serial thread
            except UnicodeEncodeError:
                pass # this can happen if a non-ascii character was passed, ignoring

    def handle_serial_input(self, data):
        # this may need to be made more efficient, as it pushes out a byte
        # at a time to the console
        for b in data:
            if self._output_enabled:
                self.console.write_bytes(b)
            if b == b'\n': # end of line
                self.handle_serial_input_line(self._read_line.strip())
                self._read_line = b""
            else:
                self._read_line += b
            self.check_gdbstub_trigger(b)

    def handle_serial_input_line(self, line):
        for m in re.finditer(MATCH_PCADDR, line):
            self.lookup_pc_address(m.group())

    def handle_menu_key(self, c):
        if c == self.exit_key or c == self.menu_key:  # send verbatim
            self.serial.write(codecs.encode(c))
        elif c in [ CTRL_H, 'h', 'H', '?' ]:
            red_print(self.get_help_text())
        elif c == CTRL_R:  # Reset device via RTS
            self.serial.setRTS(True)
            time.sleep(0.2)
            self.serial.setRTS(False)
            self.output_enable(True)
        elif c == CTRL_F:  # Recompile & upload
            self.run_make("flash")
        elif c == CTRL_A:  # Recompile & upload app only
            self.run_make("app-flash")
        elif c == CTRL_Y:  # Toggle output display
            self.output_toggle()
        elif c == CTRL_P:
            yellow_print("Pause app (enter bootloader mode), press Ctrl-T Ctrl-R to restart")
            # to fast trigger pause without press menu key
            self.serial.setDTR(False)  # IO0=HIGH
            self.serial.setRTS(True)   # EN=LOW, chip in reset
            time.sleep(1.3) # timeouts taken from esptool.py, includes esp32r0 workaround. defaults: 0.1
            self.serial.setDTR(True)   # IO0=LOW
            self.serial.setRTS(False)  # EN=HIGH, chip out of reset
            time.sleep(0.45) # timeouts taken from esptool.py, includes esp32r0 workaround. defaults: 0.05
            self.serial.setDTR(False)  # IO0=HIGH, done
        else:
            red_print('--- unknown menu character {} --'.format(key_description(c)))

    def get_help_text(self):
        return """
--- idf_monitor ({version}) - ESP-IDF monitor tool
--- based on miniterm from pySerial
---
--- {exit:8} Exit program
--- {menu:8} Menu escape key, followed by:
--- Menu keys:
---    {menu:7} Send the menu character itself to remote
---    {exit:7} Send the exit character itself to remote
---    {reset:7} Reset target board via RTS line
---    {make:7} Run 'make flash' to build & flash
---    {appmake:7} Run 'make app-flash to build & flash app
---    {output:7} Toggle output display
---    {pause:7} Reset target into bootloader to pause app via RTS line
""".format(version=__version__,
           exit=key_description(self.exit_key),
           menu=key_description(self.menu_key),
           reset=key_description(CTRL_R),
           make=key_description(CTRL_F),
           appmake=key_description(CTRL_A),
           output=key_description(CTRL_Y),
           pause=key_description(CTRL_P),
           )

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
            red_print("""
--- {}
--- Press {} to exit monitor.
--- Press {} to run 'make flash'.
--- Press {} to run 'make app-flash'.
--- Press any other key to resume monitor (resets target).""".format(reason,
                                                                     key_description(self.exit_key),
                                                                     key_description(CTRL_F),
                                                                     key_description(CTRL_A)))
            k = CTRL_T  # ignore CTRL-T here, so people can muscle-memory Ctrl-T Ctrl-F, etc.
            while k == CTRL_T:
                k = self.console.getkey()
        finally:
            self.console.cleanup()
        if k == self.exit_key:
            self.event_queue.put((TAG_KEY, k))
        elif k in [ CTRL_F, CTRL_A ]:
            self.event_queue.put((TAG_KEY, self.menu_key))
            self.event_queue.put((TAG_KEY, k))

    def run_make(self, target):
        with self:
            yellow_print("Running make %s..." % target)
            p = subprocess.Popen([self.make,
                                  target ])
            try:
                p.wait()
            except KeyboardInterrupt:
                p.wait()
            if p.returncode != 0:
                self.prompt_next_action("Build failed")
            else:
                self.output_enable(True)

    def lookup_pc_address(self, pc_addr):
        translation = subprocess.check_output(
            ["%saddr2line" % self.toolchain_prefix,
             "-pfiaC", "-e", self.elf_file, pc_addr],
            cwd=".")
        if not "?? ??:0" in translation:
            yellow_print(translation)

    def check_gdbstub_trigger(self, c):
        self._gdb_buffer = self._gdb_buffer[-6:] + c  # keep the last 7 characters seen
        m = re.match(b"\\$(T..)#(..)", self._gdb_buffer) # look for a gdb "reason" for a break
        if m is not None:
            try:
                chsum = sum(ord(p) for p in m.group(1)) & 0xFF
                calc_chsum = int(m.group(2), 16)
            except ValueError:
                return  # payload wasn't valid hex digits
            if chsum == calc_chsum:
                self.run_gdb()
            else:
                red_print("Malformed gdb message... calculated checksum %02x received %02x" % (chsum, calc_chsum))


    def run_gdb(self):
        with self:  # disable console control
            sys.stderr.write(ANSI_NORMAL)
            try:
                subprocess.call(["%sgdb" % self.toolchain_prefix,
                                "-ex", "set serial baud %d" % self.serial.baudrate,
                                "-ex", "target remote %s" % self.serial.port,
                                "-ex", "interrupt",  # monitor has already parsed the first 'reason' command, need a second
                                self.elf_file], cwd=".")
            except KeyboardInterrupt:
                pass  # happens on Windows, maybe other OSes
            self.prompt_next_action("gdb exited")

    def output_enable(self, enable):
        self._output_enabled = enable

    def output_toggle(self):
        self._output_enabled = not self._output_enabled
        yellow_print("\nToggle output display: {}, Type Ctrl-T Ctrl-Y to show/disable output again.".format(self._output_enabled))

def main():
    parser = argparse.ArgumentParser("idf_monitor - a serial output monitor for esp-idf")

    parser.add_argument(
        '--port', '-p',
        help='Serial port device',
        default=os.environ.get('ESPTOOL_PORT', '/dev/ttyUSB0')
    )

    parser.add_argument(
        '--baud', '-b',
        help='Serial port baud rate',
        type=int,
        default=os.environ.get('MONITOR_BAUD', 115200))

    parser.add_argument(
        '--make', '-m',
        help='Command to run make',
        type=str, default='make')

    parser.add_argument(
        '--toolchain-prefix',
        help="Triplet prefix to add before cross-toolchain names",
        default=DEFAULT_TOOLCHAIN_PREFIX)

    parser.add_argument(
        "--eol",
        choices=['CR', 'LF', 'CRLF'],
        type=lambda c: c.upper(),
        help="End of line to use when sending to the serial port",
        default='CR')

    parser.add_argument(
        'elf_file', help='ELF file of application',
        type=argparse.FileType('rb'))

    args = parser.parse_args()

    if args.port.startswith("/dev/tty."):
        args.port = args.port.replace("/dev/tty.", "/dev/cu.")
        yellow_print("--- WARNING: Serial ports accessed as /dev/tty.* will hang gdb if launched.")
        yellow_print("--- Using %s instead..." % args.port)

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
        makeflags = os.environ["MAKEFLAGS"]
        makeflags = re.sub(r"--jobserver[^ =]*=[0-9,]+ ?", "", makeflags)
        os.environ["MAKEFLAGS"] = makeflags
    except KeyError:
        pass  # not running a make jobserver

    monitor = Monitor(serial_instance, args.elf_file.name, args.make, args.toolchain_prefix, args.eol)

    yellow_print('--- idf_monitor on {p.name} {p.baudrate} ---'.format(
        p=serial_instance))
    yellow_print('--- Quit: {} | Menu: {} | Help: {} followed by {} ---'.format(
        key_description(monitor.exit_key),
        key_description(monitor.menu_key),
        key_description(monitor.menu_key),
        key_description(CTRL_H)))

    monitor.main_loop()

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
    ANSI_TO_WINDOWS_COLOR = [ 0, 4, 2, 6, 1, 5, 3, 7 ]

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

        def __init__(self, output):
            self.output = output
            self.handle = GetStdHandle(STD_ERROR_HANDLE if self.output == sys.stderr else STD_OUTPUT_HANDLE)
            self.matched = b''

        def write(self, data):
            for b in data:
                l = len(self.matched)
                if b == '\033':  # ESC
                    self.matched = b
                elif (l == 1 and b == '[') or (1 < l < 7):
                    self.matched += b
                    if self.matched == ANSI_NORMAL:  # reset console
                        SetConsoleTextAttribute(self.handle, FOREGROUND_GREY)
                        self.matched = b''
                    elif len(self.matched) == 7:     # could be an ANSI sequence
                        m = re.match(RE_ANSI_COLOR, self.matched)
                        if m is not None:
                            color = ANSI_TO_WINDOWS_COLOR[int(m.group(2))]
                            if m.group(1) == b'1':
                                color |= FOREGROUND_INTENSITY
                            SetConsoleTextAttribute(self.handle, color)
                        else:
                            self.output.write(self.matched) # not an ANSI color code, display verbatim
                        self.matched = b''
                else:
                    try:
                        self.output.write(b)
                    except IOError:
                        # Windows 10 bug since the Fall Creators Update, sometimes writing to console randomly fails
                        # (but usually succeeds the second time, it seems.) Ref https://github.com/espressif/esp-idf/issues/1136
                        try:
                            self.output.write(b)
                        except IOError:
                            pass
                    self.matched = b''

        def flush(self):
            self.output.flush()


if __name__ == "__main__":
    main()
