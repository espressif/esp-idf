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


import datetime
import os
import subprocess
import sys

from serial.tools import miniterm

try:
    from typing import BinaryIO, Callable, Optional, Union  # noqa
except ImportError:
    pass

# ANSI terminal codes (if changed, regular expressions in LineMatcher need to be updated)
ANSI_RED = '\033[1;31m'
ANSI_YELLOW = '\033[0;33m'
ANSI_NORMAL = '\033[0m'


def color_print(message, color, newline='\n'):  # type: (str, str, Optional[str]) -> None
    """ Print a message to stderr with colored highlighting """
    sys.stderr.write('%s%s%s%s' % (color, message, ANSI_NORMAL, newline))


def normal_print(message):  # type: (str) -> None
    sys.stderr.write(ANSI_NORMAL + message)


def yellow_print(message, newline='\n'):  # type: (str, Optional[str]) -> None
    color_print(message, ANSI_YELLOW, newline)


def red_print(message, newline='\n'):  # type: (str, Optional[str]) -> None
    color_print(message, ANSI_RED, newline)


def lookup_pc_address(pc_addr, toolchain_prefix, elf_file):  # type: (str, str, str) -> Optional[str]
    cmd = ['%saddr2line' % toolchain_prefix, '-pfiaC', '-e', elf_file, pc_addr]

    try:
        translation = subprocess.check_output(cmd, cwd='.')
        if b'?? ??:0' not in translation:
            return translation.decode()
    except OSError as e:
        red_print('%s: %s' % (' '.join(cmd), e))
    return None


class Logger:
    def __init__(self, elf_file, console, timestamps, timestamp_format):
        # type: (str, miniterm.Console, bool, str) -> None
        self.log_file = None  # type: Optional[BinaryIO]
        self._output_enabled = True  # type: bool
        self.elf_file = elf_file
        self.console = console
        self.timestamps = timestamps
        self.timestamp_format = timestamp_format

    @property
    def output_enabled(self):  # type: () -> bool
        return self._output_enabled

    @output_enabled.setter
    def output_enabled(self, value):  # type: (bool) -> None
        self._output_enabled = value

    @property
    def log_file(self):  # type: () -> Optional[BinaryIO]
        return self._log_file

    @log_file.setter
    def log_file(self, value):  # type: (Optional[BinaryIO]) -> None
        self._log_file = value

    def toggle_logging(self):  # type: () -> None
        if self._log_file:
            self.stop_logging()
        else:
            self.start_logging()

    def toggle_timestamps(self):  # type: () -> None
        self.timestamps = not self.timestamps

    def start_logging(self):  # type: () -> None
        if not self._log_file:
            name = 'log.{}.{}.txt'.format(os.path.splitext(os.path.basename(self.elf_file))[0],
                                          datetime.datetime.now().strftime('%Y%m%d%H%M%S'))
            try:
                self.log_file = open(name, 'wb+')
                yellow_print('\nLogging is enabled into file {}'.format(name))
            except Exception as e:
                red_print('\nLog file {} cannot be created: {}'.format(name, e))

    def stop_logging(self):  # type: () -> None
        if self._log_file:
            try:
                name = self._log_file.name
                self._log_file.close()
                yellow_print('\nLogging is disabled and file {} has been closed'.format(name))
            except Exception as e:
                red_print('\nLog file cannot be closed: {}'.format(e))
            finally:
                self._log_file = None

    def print(self, string, console_printer=None):  # noqa: E999
        # type: (Union[str, bytes], Optional[Callable]) -> None
        if console_printer is None:
            console_printer = self.console.write_bytes

        if self.timestamps and (self._output_enabled or self._log_file):
            t = datetime.datetime.now().strftime(self.timestamp_format)
            # "string" is not guaranteed to be a full line. Timestamps should be only at the beginning of lines.
            if isinstance(string, type(u'')):
                search_patt = '\n'
                replacement = '\n' + t + ' '
            else:
                search_patt = b'\n'  # type: ignore
                replacement = b'\n' + t.encode('ascii') + b' '  # type: ignore
            string = string.replace(search_patt, replacement)  # type: ignore
        if self._output_enabled:
            console_printer(string)
        if self._log_file:
            try:
                if isinstance(string, type(u'')):
                    string = string.encode()  # type: ignore
                self._log_file.write(string)  # type: ignore
            except Exception as e:
                red_print('\nCannot write to file: {}'.format(e))
                # don't fill-up the screen with the previous errors (probably consequent prints would fail also)
                self.stop_logging()

    def output_toggle(self):  # type: () -> None
        self.output_enabled = not self.output_enabled
        yellow_print('\nToggle output display: {}, Type Ctrl-T Ctrl-Y to show/disable output again.'.format(
            self.output_enabled))
