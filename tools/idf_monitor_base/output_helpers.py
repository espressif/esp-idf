# SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import subprocess
import sys
from typing import BinaryIO, Callable, Optional, Union  # noqa: F401

# ANSI terminal codes (if changed, regular expressions in LineMatcher need to be updated)
ANSI_RED = '\033[1;31m'
ANSI_YELLOW = '\033[0;33m'
ANSI_NORMAL = '\033[0m'


def color_print(message, color, newline='\n'):  # type: (str, str, Optional[str]) -> None
    """ Print a message to stderr with colored highlighting """
    sys.stderr.write('%s%s%s%s' % (color, message, ANSI_NORMAL, newline))
    sys.stderr.flush()


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
