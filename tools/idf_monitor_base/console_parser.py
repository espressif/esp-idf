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

import textwrap

try:
    from typing import Optional
except ImportError:
    pass

import serial.tools.miniterm as miniterm

from .constants import (CMD_APP_FLASH, CMD_ENTER_BOOT, CMD_MAKE, CMD_OUTPUT_TOGGLE, CMD_RESET, CMD_STOP,
                        CMD_TOGGLE_LOGGING, CTRL_A, CTRL_F, CTRL_H, CTRL_L, CTRL_P, CTRL_R, CTRL_RBRACKET, CTRL_T,
                        CTRL_X, CTRL_Y, TAG_CMD, TAG_KEY, __version__)
from .output_helpers import red_print, yellow_print

key_description = miniterm.key_description


class ConsoleParser(object):

    def __init__(self, eol='CRLF'):  # type: (str) -> None
        self.translate_eol = {
            'CRLF': lambda c: c.replace('\n', '\r\n'),
            'CR': lambda c: c.replace('\n', '\r'),
            'LF': lambda c: c.replace('\r', '\n'),
        }[eol]
        self.menu_key = CTRL_T
        self.exit_key = CTRL_RBRACKET
        self._pressed_menu_key = False

    def parse(self, key):  # type: (str) -> Optional[tuple]
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

    def _handle_menu_key(self, c):  # type: (str) -> Optional[tuple]
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

    def get_help_text(self):  # type: () -> str
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

    def get_next_action_text(self):  # type: () -> str
        text = """\
            --- Press {} to exit monitor.
            --- Press {} to build & flash project.
            --- Press {} to build & flash app.
            --- Press any other key to resume monitor (resets target).
        """.format(key_description(self.exit_key),
                   key_description(CTRL_F),
                   key_description(CTRL_A))
        return textwrap.dedent(text)

    def parse_next_action_key(self, c):  # type: (str) -> Optional[tuple]
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
