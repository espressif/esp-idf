# SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import ctypes
import os
import re
import sys
from io import TextIOBase
from typing import Any, Optional, TextIO, Union

from .output_helpers import ANSI_NORMAL

STD_OUTPUT_HANDLE = -11
STD_ERROR_HANDLE = -12

# wincon.h values
FOREGROUND_INTENSITY = 8
FOREGROUND_GREY = 7

# matches the ANSI color change sequences that IDF sends
RE_ANSI_COLOR = re.compile(b'\033\\[([01]);3([0-7])m')

# list mapping the 8 ANSI colors (the indexes) to Windows Console colors
ANSI_TO_WINDOWS_COLOR = [0, 4, 2, 6, 1, 5, 3, 7]

if os.name == 'nt':
    GetStdHandle = ctypes.windll.kernel32.GetStdHandle  # type: ignore
    SetConsoleTextAttribute = ctypes.windll.kernel32.SetConsoleTextAttribute  # type: ignore


def get_ansi_converter(orig_output_method=None, force_color=False):
    # type: (Any[TextIO, Optional[TextIOBase]], bool) -> Union[ANSIColorConverter, Optional[TextIOBase]]
    """
    Returns an ANSIColorConverter on Windows and the original output method (orig_output_method) on other platforms.
    The ANSIColorConverter with force_color=True will be forced to use ANSI color codes
    """
    if os.name == 'nt' and not force_color:
        return ANSIColorConverter(orig_output_method, force_color)
    return orig_output_method


class ANSIColorConverter(object):
    """Class to wrap a file-like output stream, intercept ANSI color codes,
    and convert them into calls to Windows SetConsoleTextAttribute.

    Doesn't support all ANSI terminal code escape sequences, only the sequences IDF uses.

    Ironically, in Windows this console output is normally wrapped by winpty which will then detect the console text
    color changes and convert these back to ANSI color codes for MSYS' terminal to display. However this is the
    least-bad working solution, as winpty doesn't support any "passthrough" mode for raw output.
    """

    def __init__(self, output=None, force_color=False):
        # type: (TextIOBase, bool) -> None
        self.output = output
        # check if output supports writing bytes or if decoding before writing is necessary
        try:
            output.write(b'')  # type: ignore
        except Exception:
            self.decode_output = True
        else:
            self.decode_output = False
        self.handle = GetStdHandle(STD_ERROR_HANDLE if self.output == sys.stderr else STD_OUTPUT_HANDLE)
        self.matched = b''
        self.decode_buffer = b''
        self.force_color = force_color  # always print ANSI for colors if true

    def _output_write(self, data):  # type: (Union[str, bytes]) -> None
        try:
            if self.decode_output:
                data = self.decode_buffer + data  # type: ignore
                self.output.write(data.decode())  # type: ignore
                self.decode_buffer = b''
            else:
                self.output.write(data)  # type: ignore
        except (IOError, OSError):
            # Windows 10 bug since the Fall Creators Update, sometimes writing to console randomly throws
            # an exception (however, the character is still written to the screen)
            # Ref https://github.com/espressif/esp-idf/issues/1163
            #
            # Also possible for Windows to throw an OSError error if the data is invalid for the console
            # (garbage bytes, etc)
            pass
        except UnicodeDecodeError:
            self.decode_buffer += data  # type: ignore
            if len(self.decode_buffer) > 4:
                # Multi-byte character contain up to 4 bytes and if buffer have more then 4 bytes
                # and still can not decode it we can just ignore some bytes
                self.output.write(self.decode_buffer.decode(errors='replace'))  # type: ignore
                self.decode_buffer = b''

    def write(self, data):  # type: ignore
        if isinstance(data, bytes):
            data = bytearray(data)
        else:
            data = bytearray(data, 'utf-8')

        for b in data:
            b = bytes([b])
            length = len(self.matched)

            if b == b'\033':  # ESC
                self._output_write(self.matched)
                self.matched = b
            elif (length == 1 and b == b'[') or (1 < length < 7):
                self.matched += b
                if self.matched == ANSI_NORMAL.encode('latin-1'):  # reset console
                    # Flush is required only with Python3 - switching color before it is printed would mess up the console
                    self.flush()
                    SetConsoleTextAttribute(self.handle, FOREGROUND_GREY)
                    self.matched = b''
                elif (length == 2 and b not in [b'0', b'1']) or (length == 3 and b != b';'):
                    # other escape sequences like cursor position and status reports
                    self._output_write(self.matched)
                    self.flush()
                    self.matched = b''
                elif length == 6:  # could be an ANSI sequence
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
                        self.flush()
                    self.matched = b''
            else:
                self._output_write(b)
                self.matched = b''

    def flush(self):  # type: () -> None
        try:
            self.output.flush()  # type: ignore
        except OSError:
            # Account for Windows Console refusing to accept garbage bytes (serial noise, etc)
            pass
