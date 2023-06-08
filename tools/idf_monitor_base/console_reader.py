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


import os
import queue
import time

from serial.tools.miniterm import Console

from .console_parser import ConsoleParser
from .constants import CMD_STOP, TAG_CMD
from .stoppable_thread import StoppableThread


class ConsoleReader(StoppableThread):
    """ Read input keys from the console and push them to the queue,
    until stopped.
    """

    def __init__(self, console, event_queue, cmd_queue, parser, test_mode):
        # type: (Console, queue.Queue, queue.Queue, ConsoleParser, bool) -> None
        super(ConsoleReader, self).__init__()
        self.console = console
        self.event_queue = event_queue
        self.cmd_queue = cmd_queue
        self.parser = parser
        self.test_mode = test_mode

    def run(self):
        # type: () -> None
        self.console.setup()
        if os.name == 'posix':
            # Use non-blocking busy read to avoid using unsecure TIOCSTI from console.cancel().
            # TIOCSTI is not supported on kernels newer than 6.2.
            import termios
            new = termios.tcgetattr(self.console.fd)
            # new[6] - 'cc': a list of the tty special characters
            new[6][termios.VMIN] = 0  # minimum bytes to read
            new[6][termios.VTIME] = 2  # timer of 0.1 second granularity
            termios.tcsetattr(self.console.fd, termios.TCSANOW, new)
        try:
            while self.alive:
                try:
                    if os.name == 'nt':
                        # Windows kludge: because the console.cancel() method doesn't
                        # seem to work to unblock getkey() on the Windows implementation.
                        #
                        # So we only call getkey() if we know there's a key waiting for us.
                        import msvcrt
                        while not msvcrt.kbhit() and self.alive:  # type: ignore
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
                if c:
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
