import os
import re
import subprocess
import sys
import tempfile

from .constants import PANIC_OUTPUT_DECODE_SCRIPT
from .logger import Logger
from .output_helpers import normal_print, red_print, yellow_print
from .web_socket_client import WebSocketClient


class GDBHelper:
    def __init__(self, toolchain_prefix, websocket_client, elf_file, port, baud_rate):
        # type: (str, WebSocketClient, str, int, int) -> None
        self._gdb_buffer = b''  # type: bytes
        self._gdb_exit = False  # type: bool
        self.toolchain_prefix = toolchain_prefix
        self.websocket_client = websocket_client
        self.elf_file = elf_file
        self.port = port
        self.baud_rate = baud_rate

    @property
    def gdb_buffer(self):  # type: () -> bytes
        return self._gdb_buffer

    @gdb_buffer.setter
    def gdb_buffer(self, value):  # type: (bytes) -> None
        self._gdb_buffer = value

    @property
    def gdb_exit(self):  # type: () -> bool
        return self._gdb_exit

    @gdb_exit.setter
    def gdb_exit(self, value):  # type: (bool) -> None
        self._gdb_exit = value

    def run_gdb(self):
        # type: () -> None
        normal_print('')
        try:
            cmd = ['%sgdb' % self.toolchain_prefix,
                   '-ex', 'set serial baud %d' % self.baud_rate,
                   '-ex', 'target remote %s' % self.port,
                   self.elf_file]
            # Here we handling GDB as a process
            # Open GDB process
            try:
                process = subprocess.Popen(cmd, cwd='.')
            except KeyboardInterrupt:
                pass
            # We ignore Ctrl+C interrupt form external process abd wait response util GDB will be finished.
            while True:
                try:
                    process.wait()
                    break
                except KeyboardInterrupt:
                    pass  # We ignore the Ctrl+C
            self.gdb_exit = True
        except OSError as e:
            red_print('%s: %s' % (' '.join(cmd), e))
        except KeyboardInterrupt:
            pass  # happens on Windows, maybe other OSes
        finally:
            try:
                # on Linux, maybe other OSes, gdb sometimes seems to be alive even after wait() returns...
                process.terminate()
            except Exception:  # noqa
                pass
            try:
                # also on Linux, maybe other OSes, gdb sometimes exits uncleanly and breaks the tty mode
                subprocess.call(['stty', 'sane'])
            except Exception:  # noqa
                pass  # don't care if there's no stty, we tried...

    def check_gdb_stub_trigger(self, line):
        # type: (bytes) -> bool
        line = self.gdb_buffer + line
        self.gdb_buffer = b''
        m = re.search(b'\\$(T..)#(..)', line)  # look for a gdb "reason" for a break
        if m is not None:
            try:
                chsum = sum(ord(bytes([p])) for p in m.group(1)) & 0xFF
                calc_chsum = int(m.group(2), 16)
            except ValueError:  # payload wasn't valid hex digits
                return False
            if chsum == calc_chsum:
                if self.websocket_client:
                    yellow_print('Communicating through WebSocket')
                    self.websocket_client.send({'event': 'gdb_stub',
                                                'port': self.port,
                                                'prog': self.elf_file})
                    yellow_print('Waiting for debug finished event')
                    self.websocket_client.wait([('event', 'debug_finished')])
                    yellow_print('Communications through WebSocket is finished')
                else:
                    return True
            else:
                red_print('Malformed gdb message... calculated checksum %02x received %02x' % (chsum, calc_chsum))
        return False

    def process_panic_output(self, panic_output, logger, target):  # type: (bytes, Logger, str) -> None
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
                               script=PANIC_OUTPUT_DECODE_SCRIPT,
                               target=target,
                               output_file=panic_output_file.name),
                   '-ex', 'bt']
            output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
            yellow_print('\nBacktrace:\n\n')
            logger.print(output)  # noqa: E999
        except subprocess.CalledProcessError as e:
            yellow_print('Failed to run gdb_panic_server.py script: {}\n{}\n\n'.format(e, e.output))
            logger.print(panic_output)
        finally:
            if panic_output_file is not None:
                try:
                    os.unlink(panic_output_file.name)
                except OSError as e:
                    yellow_print('Couldn\'t remove temporary panic output file ({})'.format(e))
