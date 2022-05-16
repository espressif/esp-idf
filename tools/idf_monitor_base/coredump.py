import os
import queue
import subprocess
import sys
import tempfile
from contextlib import contextmanager
from typing import Generator

from .constants import COREDUMP_SCRIPT, TAG_KEY
from .logger import Logger
from .output_helpers import yellow_print
from .web_socket_client import WebSocketClient

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


class CoreDump:
    def __init__(self, decode_coredumps, event_queue, logger, websocket_client, elf_file):
        # type: (str, queue.Queue, Logger, WebSocketClient, str) -> None

        self._coredump_buffer = b''
        self._decode_coredumps = decode_coredumps
        self.event_queue = event_queue
        self._reading_coredump = COREDUMP_IDLE
        self.logger = logger
        self.websocket_client = websocket_client
        self.elf_file = elf_file

    @property
    def in_progress(self) -> bool:
        return bool(self._coredump_buffer)

    def _process_coredump(self):  # type: () -> None
        if self._decode_coredumps != COREDUMP_DECODE_INFO:
            raise NotImplementedError('process_coredump: %s not implemented' % self._decode_coredumps)
        coredump_file = None
        try:
            # On Windows, the temporary file can't be read unless it is closed.
            # Set delete=False and delete the file manually later.
            with tempfile.NamedTemporaryFile(mode='wb', delete=False) as coredump_file:
                coredump_file.write(self._coredump_buffer)
                coredump_file.flush()

            if self.websocket_client:
                self.logger.output_enabled = True
                yellow_print('Communicating through WebSocket')
                self.websocket_client.send({'event': 'coredump',
                                            'file': coredump_file.name,
                                            'prog': self.elf_file})
                yellow_print('Waiting for debug finished event')
                self.websocket_client.wait([('event', 'debug_finished')])
                yellow_print('Communications through WebSocket is finished')
            else:
                cmd = [sys.executable,
                       COREDUMP_SCRIPT,
                       'info_corefile',
                       '--core', coredump_file.name,
                       '--core-format', 'b64',
                       self.elf_file
                       ]
                output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
                self.logger.output_enabled = True
                self.logger.print(output)  # noqa: E999
                self.logger.output_enabled = False  # Will be reenabled in check_coredump_trigger_after_print
        except subprocess.CalledProcessError as e:
            yellow_print('Failed to run espcoredump script: {}\n{}\n\n'.format(e, e.output))
            self.logger.output_enabled = True
            self.logger.print(COREDUMP_UART_START + b'\n')
            self.logger.print(self._coredump_buffer)
            # end line will be printed in handle_serial_input
        finally:
            if coredump_file is not None:
                try:
                    os.unlink(coredump_file.name)
                except OSError as e:
                    yellow_print('Couldn\'t remote temporary core dump file ({})'.format(e))

    def _check_coredump_trigger_before_print(self, line):  # type: (bytes) -> None
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
            self.logger.output_enabled = False
            return
        if COREDUMP_UART_END in line:
            self._reading_coredump = COREDUMP_DONE
            yellow_print('\nCore dump finished!')
            self._process_coredump()
            return
        if self._reading_coredump == COREDUMP_READING:
            kb = 1024
            buffer_len_kb = len(self._coredump_buffer) // kb
            self._coredump_buffer += line.replace(b'\r', b'') + b'\n'
            new_buffer_len_kb = len(self._coredump_buffer) // kb
            if new_buffer_len_kb > buffer_len_kb:
                yellow_print('Received %3d kB...' % new_buffer_len_kb, newline='\r')

    def _check_coredump_trigger_after_print(self):  # type: () -> None
        if self._decode_coredumps == COREDUMP_DECODE_DISABLE:
            return

        # Re-enable output after the last line of core dump has been consumed
        if not self.logger.output_enabled and self._reading_coredump == COREDUMP_DONE:
            self._reading_coredump = COREDUMP_IDLE
            self.logger.output_enabled = True
            self._coredump_buffer = b''

    @contextmanager
    def check(self, line):  # type: (bytes) -> Generator
        self._check_coredump_trigger_before_print(line)
        try:
            yield
        finally:
            self._check_coredump_trigger_after_print()
