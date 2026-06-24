# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import os
import re
import signal
import subprocess
import sys
import threading
import time
from pathlib import Path
from typing import Any

from rich_click import Context

from idf_py_actions.errors import FatalError
from idf_py_actions.tools import PropertyDict
from idf_py_actions.tools import get_target
from idf_py_actions.tools import idf_version

try:
    from idf_component_tools.build_system_tools import CMAKE_PROJECT_LINE
except ImportError:
    CMAKE_PROJECT_LINE = [
        r'include($ENV{IDF_PATH}/tools/cmake/project.cmake)',
        r'include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)',
    ]

try:
    from mcp.server.fastmcp import FastMCP
    MCP_AVAILABLE = True
except ImportError:
    MCP_AVAILABLE = False

# pty, select, and termios are Unix-only. Guard the import so that the
# existing build/flash/clean/set-target tools remain usable on Windows.
try:
    import pty
    import select
    import termios
    _PTY_SUPPORTED = True
except ImportError:
    _PTY_SUPPORTED = False

_ANSI_RE = re.compile(r'\x1b\[[0-9;]*[A-Za-z]')


def _idf_cmd(*idf_args: str) -> list[str]:
    """Build an ``idf.py`` command list.

    Raises ``FatalError`` if ``IDF_PATH`` is not set.
    """
    idf_path = os.environ.get('IDF_PATH')
    if not idf_path:
        raise FatalError(
            'IDF_PATH environment variable is not set. '
            'Source the ESP-IDF export script before starting the MCP server.'
        )
    return [sys.executable, os.path.join(idf_path, 'tools', 'idf.py'), *idf_args]


def _strip_ansi(text: str) -> str:
    return _ANSI_RE.sub('', text)


def _pty_open_noecho() -> tuple[int, int]:
    """Create a PTY pair with echo disabled on the slave side.

    Disabling echo prevents typed commands from being reflected back
    into the captured output stream.
    """
    master_fd, slave_fd = pty.openpty()
    try:
        attrs = termios.tcgetattr(slave_fd)
        attrs[3] = attrs[3] & ~termios.ECHO  # lflags
        termios.tcsetattr(slave_fd, termios.TCSANOW, attrs)
    except termios.error:
        pass  # non-fatal: echo may appear in output
    return master_fd, slave_fd


def _pty_terminate(proc: subprocess.Popen, master_fd: int) -> None:
    """Terminate a PTY-wrapped process tree and close the master fd.

    When ``start_new_session=True`` was used, the child runs in its own
    process group.  Killing the group ensures that child processes
    (e.g. ``idf_monitor.py`` spawned by ``idf.py``) are also terminated
    and do not hold the serial port open.
    """
    try:
        pgid = os.getpgid(proc.pid)
        os.killpg(pgid, signal.SIGTERM)
        proc.wait(timeout=5)
    except (ProcessLookupError, ChildProcessError):
        pass  # already exited
    except Exception:
        # SIGTERM didn't work or timed out — force kill the group
        try:
            pgid = os.getpgid(proc.pid)
            os.killpg(pgid, signal.SIGKILL)
            proc.wait(timeout=3)
        except Exception:
            pass
    try:
        os.close(master_fd)
    except OSError:
        pass


def _pty_read_for(
    master_fd: int, duration: float, wait_for: str | None = None,
) -> str:
    """Read from a PTY master fd for *duration* seconds.

    If *wait_for* is set, returns early as soon as the output contains
    the given string. Otherwise reads until the deadline.
    """
    parts: list[str] = []
    deadline = time.monotonic() + duration
    while time.monotonic() < deadline:
        remaining = max(0.1, deadline - time.monotonic())
        ready, _, _ = select.select([master_fd], [], [], min(remaining, 0.5))
        if ready:
            try:
                chunk = os.read(master_fd, 4096)
                if not chunk:
                    break
                parts.append(chunk.decode('utf-8', errors='replace'))
                if wait_for and wait_for in ''.join(parts):
                    break
            except OSError:
                break
    return ''.join(parts)


class MonitorSession:
    """Manages a background ``idf.py monitor`` process using a PTY.

    A pseudo-terminal is required because idf_monitor.py checks
    ``sys.stdin.isatty()`` and refuses to start without a real TTY.
    The PTY master fd is used to both read output and send input.
    """

    def __init__(self) -> None:
        self._proc: subprocess.Popen | None = None
        self._master_fd: int | None = None
        self._output_lines: list[str] = []
        self._lock = threading.Lock()
        self._reader_thread: threading.Thread | None = None
        self._stop_event = threading.Event()

    def _reader(self) -> None:
        assert self._master_fd is not None
        buf = b''
        while not self._stop_event.is_set():
            try:
                ready, _, _ = select.select([self._master_fd], [], [], 0.5)
                if not ready:
                    continue
                chunk = os.read(self._master_fd, 4096)
                if not chunk:
                    break
                buf += chunk
                new_lines: list[str] = []
                while b'\n' in buf:
                    line, buf = buf.split(b'\n', 1)
                    new_lines.append(line.decode('utf-8', errors='replace') + '\n')
                if new_lines:
                    with self._lock:
                        self._output_lines.extend(new_lines)
            except (OSError, TypeError):
                # OSError: PTY closed. TypeError: stop() set _master_fd
                # to None between the loop check and select() call.
                break
        # Flush any remaining partial line so it is not lost
        if buf:
            with self._lock:
                self._output_lines.append(buf.decode('utf-8', errors='replace'))

    @property
    def is_running(self) -> bool:
        """True if the monitor subprocess is still alive."""
        return self._proc is not None and self._proc.poll() is None

    @property
    def has_session(self) -> bool:
        """True if session resources exist (process, fd, thread, or buffered data).

        Unlike ``is_running``, this remains True after the subprocess
        exits until ``stop()`` is called, so buffered output can still
        be read.
        """
        if self._proc is not None or self._master_fd is not None:
            return True
        if self._reader_thread is not None:
            return True
        with self._lock:
            return len(self._output_lines) > 0

    def start(self, cmd: list[str], cwd: str) -> None:
        if self.has_session:
            raise RuntimeError('Monitor session is already active. Stop it first.')
        self._output_lines = []
        self._stop_event.clear()

        master_fd, slave_fd = _pty_open_noecho()
        try:
            self._proc = subprocess.Popen(
                cmd, stdin=slave_fd, stdout=slave_fd, stderr=slave_fd,
                cwd=cwd, start_new_session=True,
            )
        except Exception:
            os.close(slave_fd)
            os.close(master_fd)
            raise
        os.close(slave_fd)

        self._master_fd = master_fd
        self._reader_thread = threading.Thread(target=self._reader, daemon=True)
        self._reader_thread.start()

    def send(self, data: str) -> None:
        """Write data to the PTY master (forwarded to the device by idf_monitor)."""
        if self._master_fd is None:
            raise RuntimeError('No monitor session is running.')
        os.write(self._master_fd, data.encode('utf-8'))

    def read(self, max_lines: int = 200) -> tuple[list[str], int]:
        """Drain up to *max_lines* from the buffer.

        Returns ``(lines, remaining_count)`` so the caller knows
        whether the buffer was truncated.
        """
        with self._lock:
            chunk = self._output_lines[:max_lines]
            self._output_lines = self._output_lines[max_lines:]
            remaining = len(self._output_lines)
        return chunk, remaining

    def stop(self) -> list[str]:
        """Stop the session and return all remaining buffered output.

        Always cleans up all resources (process, fd, thread) even if the
        subprocess has already exited on its own.
        """
        self._stop_event.set()
        # Terminate process if it is still alive
        if self._proc is not None:
            if self._proc.poll() is None:
                if self._master_fd is not None:
                    _pty_terminate(self._proc, self._master_fd)
                    self._master_fd = None
                else:
                    try:
                        os.killpg(os.getpgid(self._proc.pid), signal.SIGKILL)
                        self._proc.wait(timeout=3)
                    except Exception:
                        pass
            else:
                # Process already exited; just reap
                try:
                    self._proc.wait(timeout=1)
                except Exception:
                    pass
            self._proc = None
        # Close master fd if still open (e.g. process died but fd leaked)
        if self._master_fd is not None:
            try:
                os.close(self._master_fd)
            except OSError:
                pass
            self._master_fd = None
        if self._reader_thread is not None:
            self._reader_thread.join(timeout=3)
            self._reader_thread = None
        with self._lock:
            remaining = list(self._output_lines)
            self._output_lines.clear()
        return remaining


def is_valid_project_dir(directory: str) -> bool:
    """
    Determine if the given directory is a valid ESP-IDF project directory.
    - Must be a directory.
    - Must contain a CMakeLists.txt.
    - CMakeLists.txt must include CMAKE_PROJECT_LINE.
    """
    root = Path(directory)
    if not root.is_dir():
        return False

    cmakelists_path = root / 'CMakeLists.txt'
    if not cmakelists_path.is_file():
        return False

    try:
        with open(str(cmakelists_path), encoding='utf-8') as f:
            for line in f:
                if any(proj_line in line for proj_line in CMAKE_PROJECT_LINE):
                    return True
    except Exception:
        return False

    return False


def action_extensions(base_actions: dict, project_path: str) -> dict:
    """ESP-IDF MCP Server Extension"""

    def start_mcp_server(action_name: str, ctx: Context, args: PropertyDict, **kwargs: Any) -> None:
        """Start MCP server for ESP-IDF project integration"""
        if not MCP_AVAILABLE:
            raise FatalError(
                'MCP dependencies not available. '
                'Install ESP-IDF using the EIM installer and select the "mcp" feature to be included. '
                'For more information, refer to the official Espressif EIM Installer documentation '
                'or use "idf.py docs" and search for EIM configuration instructions.'
            )

        # Verify that mcp-server was executed from a valid ESP-IDF project directory.
        # This is necessary to obtain the correct context such as args, project path, etc.
        if not is_valid_project_dir(project_path):
            current_project = None
            for candidate in [os.getcwd(), os.environ.get('IDF_MCP_WORKSPACE_FOLDER', '')]:
                if is_valid_project_dir(candidate):
                    current_project = candidate
                    break

            if not current_project:
                raise FatalError('Open the MCP server in a valid ESP-IDF project directory.')

            try:
                cmd = _idf_cmd('-C', current_project, 'mcp-server')
                print(
                    f'Starting ESP-IDF MCP Server with command: {" ".join(cmd)} in project path: {current_project}',
                    file=sys.stderr,
                )
                subprocess.run(cmd, cwd=current_project, check=True)
                return
            except Exception as e:
                print(f'ERROR: Failed to start ESP-IDF MCP Server: {str(e)}', file=sys.stderr)
                raise FatalError(f'Failed to start ESP-IDF MCP Server: {str(e)}') from e

        # Initialize MCP server
        mcp = FastMCP('ESP-IDF')

        # Persistent monitor session (shared across tool calls)
        monitor_session = MonitorSession()

        # === TOOLS (Actions) ===

        @mcp.tool()
        def build_project() -> str:
            """Build ESP-IDF project"""
            try:
                cmd = _idf_cmd('build')
                # Information logs are shown in some mcp clients using stderr
                print(f'INFO: Building project with command: {" ".join(cmd)} in path: {project_path}', file=sys.stderr)
                result = subprocess.run(cmd, capture_output=True, text=True, cwd=project_path)
                if result.returncode == 0:
                    print('INFO: Build successful', file=sys.stderr)
                    return 'Successfully built project'
                else:
                    print(f'ERROR: Build failed: {result.stderr}', file=sys.stderr)
                    return f'Build failed: {result.stderr}'
            except Exception as e:
                print(f'ERROR: Build failed: {str(e)}', file=sys.stderr)
                return f'Build failed: {str(e)}'

        @mcp.tool()
        def set_target(target: str) -> str:
            """Set the ESP-IDF target (esp32, esp32s3, esp32c6, etc.)"""
            try:
                cmd = _idf_cmd('set-target', target)
                print(f'INFO: Setting target with command: {" ".join(cmd)} in path: {project_path}', file=sys.stderr)
                result = subprocess.run(cmd, capture_output=True, text=True, cwd=project_path)
                if result.returncode == 0:
                    print(f'INFO: Target set to: {target}', file=sys.stderr)
                    return f'Target set to: {target}'
                else:
                    print(f'ERROR: Failed to set target: {result.stderr}', file=sys.stderr)
                    return f'Failed to set target: {result.stderr}'
            except Exception as e:
                print(f'ERROR: Failed to set target: {str(e)}', file=sys.stderr)
                return f'Error setting target: {str(e)}'

        @mcp.tool()
        def flash_project(port: str | None = None) -> str:
            """Flash the built project to connected device"""
            try:
                port_args = ['-p', port] if port else []
                cmd = _idf_cmd(*port_args, 'flash')
                print(f'INFO: Flashing project with command: {" ".join(cmd)} in path: {project_path}', file=sys.stderr)
                result = subprocess.run(cmd, capture_output=True, text=True, cwd=project_path)
                if result.returncode == 0:
                    print('INFO: Flash successful', file=sys.stderr)
                    return f'Successfully flashed project{" to port " + port if port else ""}'
                else:
                    print(f'ERROR: Flash failed: {result.stderr}', file=sys.stderr)
                    return f'Flash failed: {result.stderr}'
            except Exception as e:
                print(f'ERROR: Flash failed: {str(e)}', file=sys.stderr)
                return f'Error flashing: {str(e)}'

        @mcp.tool()
        def clean_project() -> str:
            """Clean build artifacts"""
            try:
                cmd = _idf_cmd('clean')
                print(f'INFO: Cleaning project with command: {" ".join(cmd)} in path: {project_path}', file=sys.stderr)
                result = subprocess.run(cmd, capture_output=True, text=True, cwd=project_path)
                if result.returncode == 0:
                    print('INFO: Project cleaned successfully', file=sys.stderr)
                    return 'Project cleaned successfully'
                else:
                    print(f'ERROR: Clean failed: {result.stderr}', file=sys.stderr)
                    return f'Clean failed: {result.stderr}'
            except Exception as e:
                print(f'ERROR: Error cleaning: {str(e)}', file=sys.stderr)
                return f'Error cleaning: {str(e)}'

        # === Monitor Tools ===

        def _require_pty(tool_name: str) -> str | None:
            """Return an error string if PTY is not available, else None."""
            if not _PTY_SUPPORTED:
                return (
                    f'{tool_name} requires a pseudo-terminal (pty) which is '
                    'not available on this platform.'
                )
            return None

        @mcp.tool()
        def monitor_boot(
            port: str | None = None,
            capture_duration: int = 15,
            wait_for: str | None = None,
        ) -> str:
            """Reset the device and capture the boot log.

            Launches ``idf.py monitor`` in a PTY (which resets the board
            on connect), captures output for ``capture_duration`` seconds,
            then terminates. Returns the full boot log including
            bootloader messages, component init, and startup diagnostics.

            If ``wait_for`` is set, stops capturing early as soon as the
            given string appears in the output. Useful for waiting on
            specific events like test completion, crash messages, or
            initialization markers without sitting through the full
            timeout.

            This is a one-shot tool. For interactive serial sessions,
            use ``monitor_start``/``monitor_send``/``monitor_read``/
            ``monitor_stop`` instead.

            Args:
                port:             Serial port (e.g. /dev/ttyACM0).
                                  Auto-detected if omitted.
                capture_duration: Seconds to capture (default 15, max 120).
                wait_for:         Stop early when this string appears in
                                  the output (e.g. 'Sensor scan:',
                                  'Guru Meditation Error',
                                  'Tests 0 Failures').
            """
            err = _require_pty('monitor_boot')
            if err:
                return err

            if monitor_session.has_session:
                return (
                    'A monitor session is already active on the serial port. '
                    'Call monitor_stop first.'
                )

            capture_duration = max(1, min(capture_duration, 120))
            try:
                port_args = ['-p', port] if port else []
                cmd = _idf_cmd(*port_args, 'monitor')

                print(
                    f'INFO: Resetting device and capturing boot log ({capture_duration}s) '
                    f'with command: {" ".join(cmd)} in path: {project_path}',
                    file=sys.stderr,
                )

                master_fd, slave_fd = _pty_open_noecho()
                try:
                    proc = subprocess.Popen(
                        cmd, stdin=slave_fd, stdout=slave_fd, stderr=slave_fd,
                        cwd=project_path, start_new_session=True,
                    )
                except Exception:
                    os.close(slave_fd)
                    os.close(master_fd)
                    raise
                os.close(slave_fd)

                try:
                    # Brief check: if monitor exits immediately (wrong port,
                    # device busy, etc.) report failure rather than silently
                    # returning an empty capture.
                    time.sleep(0.5)
                    if proc.poll() is not None:
                        early_output = _strip_ansi(_pty_read_for(master_fd, 1))
                        # finally block handles cleanup
                        return f'Monitor exited immediately:\n{early_output or "(no output)"}'

                    output = _pty_read_for(master_fd, capture_duration, wait_for)
                finally:
                    _pty_terminate(proc, master_fd)

                output = _strip_ansi(output)
                matched = wait_for and wait_for in output
                if not output.strip():
                    output = '(no output captured)'
                port_str = port or 'auto-detected port'
                print(f'INFO: Boot log capture complete from {port_str}', file=sys.stderr)
                header = f'Boot log from {port_str}'
                if matched:
                    header += f' (stopped on "{wait_for}")'
                else:
                    header += f' ({capture_duration}s)'
                return f'{header}:\n{output}'

            except (OSError, subprocess.SubprocessError, FatalError) as e:
                print(f'ERROR: Device reset and capture failed: {str(e)}', file=sys.stderr)
                return f'Error capturing boot log: {str(e)}'

        @mcp.tool()
        def monitor_start(port: str | None = None) -> str:
            """Start the serial monitor in the background.

            Launches ``idf.py monitor`` via a pseudo-terminal. The board
            is reset on connect. Use ``monitor_read`` to retrieve boot
            output and subsequent device logs, ``monitor_send`` to write
            commands, and ``monitor_stop`` to terminate the session.

            Args:
                port: Serial port (e.g. /dev/ttyACM0). Auto-detected if omitted.
            """
            err = _require_pty('monitor_start')
            if err:
                return err

            try:
                port_args = ['-p', port] if port else []
                cmd = _idf_cmd(*port_args, 'monitor')

                print(
                    f'INFO: Starting monitor with command: {" ".join(cmd)} in path: {project_path}',
                    file=sys.stderr,
                )
                monitor_session.start(cmd, cwd=project_path)

                # Give the board time to reset
                time.sleep(3)

                # Validate that the monitor process is still alive
                if not monitor_session.is_running:
                    startup_output = _strip_ansi(''.join(monitor_session.stop()))
                    print('ERROR: Monitor exited immediately after start', file=sys.stderr)
                    return f'Monitor failed to start:\n{startup_output}'

                return 'Monitor started. Use monitor_read to retrieve output.'

            except (RuntimeError, FatalError) as e:
                return str(e)
            except (OSError, subprocess.SubprocessError) as e:
                print(f'ERROR: Monitor start failed: {str(e)}', file=sys.stderr)
                return f'Error starting monitor: {str(e)}'

        @mcp.tool()
        def monitor_send(text: str) -> str:
            """Send text to the device through the running monitor session.

            Writes the text to the serial port via idf_monitor. A newline
            is appended if not already present. This tool only sends; use
            ``monitor_read`` to retrieve the device's response.

            Args:
                text: Text to send (e.g. 'STATUS', 'START', 'STOP').
            """
            if not monitor_session.is_running:
                return 'No monitor session is running. Call monitor_start first.'
            try:
                to_send = text if text.endswith('\n') else text + '\n'
                monitor_session.send(to_send)
                print(f'INFO: Sent to device: {text.strip()!r}', file=sys.stderr)
                return f'Sent: {text.strip()}'
            except (OSError, RuntimeError) as e:
                print(f'ERROR: Monitor send failed: {str(e)}', file=sys.stderr)
                return f'Error sending to device: {str(e)}'

        @mcp.tool()
        def monitor_read(
            max_lines: int = 200,
            timeout: float = 0,
            wait_for: str | None = None,
        ) -> str:
            """Read buffered output from the monitor session.

            Returns up to ``max_lines`` of new output since the last
            read. Can be called repeatedly to poll for new device output.
            Still works after the monitor process has exited, returning
            any remaining buffered output.

            If ``timeout`` is set (> 0), waits up to that many seconds
            for new output to appear instead of returning immediately.
            If ``wait_for`` is also set, returns as soon as the given
            string appears in the buffered output, or when the timeout
            expires, whichever comes first.

            Args:
                max_lines: Maximum lines to return (default 200, max 1000).
                timeout:   Seconds to wait for output (default 0 = instant).
                           Max 30.
                wait_for:  Return early when this string appears in the
                           output (e.g. 'OK', 'FAIL', 'Guru Meditation').
            """
            if not monitor_session.has_session:
                return 'No monitor session is active. Call monitor_start first.'

            max_lines = max(1, min(max_lines, 1000))
            timeout = max(0, min(timeout, 30))

            # Accumulate output, optionally waiting for new data or a pattern
            collected: list[str] = []
            remaining = 0
            deadline = time.monotonic() + timeout if timeout > 0 else 0

            while True:
                lines, remaining = monitor_session.read(max_lines=max_lines)
                if lines:
                    collected.extend(lines)
                    if wait_for and wait_for in ''.join(collected):
                        break
                # If no timeout, return immediately with whatever we have
                if deadline == 0:
                    break
                # If timeout active, keep polling until deadline
                if time.monotonic() >= deadline:
                    break
                if not monitor_session.is_running:
                    break
                time.sleep(0.3)

            if not collected:
                if not monitor_session.is_running:
                    return '(no new output; monitor process has exited)'
                if timeout > 0:
                    msg = f'(no output after {timeout}s'
                    if wait_for:
                        msg += f'; "{wait_for}" not found'
                    return msg + ')'
                return '(no new output)'

            output = _strip_ansi(''.join(collected))
            if remaining > 0:
                output += f'\n[{remaining} more lines in buffer]'
            if not monitor_session.is_running:
                output += '\n[note: monitor process has exited]'
            if wait_for:
                if wait_for in output:
                    output += f'\n[matched: "{wait_for}"]'
                else:
                    output += f'\n["{wait_for}" not found in output]'
            return output

        @mcp.tool()
        def monitor_stop() -> str:
            """Stop the monitor session and clean up resources.

            Works even if the monitor process has already exited.
            Use ``monitor_read`` before stopping if you need to
            retrieve remaining output.
            """
            if not monitor_session.has_session:
                return 'No monitor session is active.'
            was_running = monitor_session.is_running
            monitor_session.stop()
            status = 'Monitor stopped.' if was_running else 'Monitor had already exited; session cleaned up.'
            print(f'INFO: {status}', file=sys.stderr)
            return status

        # === RESOURCES (Data Access) ===

        @mcp.resource('project://config')
        def get_project_config() -> str:
            """Get current project configuration"""
            build_dir = args.get('build_dir', '')
            config: dict[str, Any] = {}

            if not os.path.exists(build_dir):
                config['build_dir_exists'] = False
                return json.dumps(config, indent=2)

            config['build_dir'] = build_dir

            proj_desc_fn = f'{build_dir}/project_description.json'
            config['project_description'] = 'Project description does not exist'
            try:
                with open(proj_desc_fn, encoding='utf-8') as f:
                    config['project_description'] = json.load(f)
            except (OSError, ValueError):
                pass

            return json.dumps(config, indent=2)

        @mcp.resource('project://status')
        def get_project_status() -> str:
            """Get current project build status"""
            try:
                status = {
                    'project_path': project_path,
                    'target': get_target(project_path),
                    'idf_version': idf_version(),
                }

                # Check if built
                build_dir = args.build_dir
                if os.path.exists(build_dir):
                    status['build_dir'] = build_dir
                    artifacts = ['bootloader', 'partition_table', 'app-flash', 'flash_args']
                    status['artifacts'] = {}
                    for artifact in artifacts:
                        artifact_path = os.path.join(build_dir, artifact)
                        status['artifacts'][artifact] = os.path.exists(artifact_path)
                else:
                    status['build_dir_exists'] = False

                return json.dumps(status, indent=2)
            except Exception as e:
                return f'Error getting status: {str(e)}'

        @mcp.resource('project://devices')
        def get_connected_devices() -> str:
            """Get list of connected devices"""
            try:
                import serial.tools.list_ports

                devices_on_ports = [p.device.strip() for p in serial.tools.list_ports.comports()]
                print(f'Devices: {devices_on_ports}', file=sys.stderr)
                devices = {'available_ports': devices_on_ports if devices_on_ports else []}
                return json.dumps(devices, indent=2)
            except Exception as e:
                return f'Error getting devices: {str(e)}'

        # Start the MCP server
        print('MCP Server running on stdio...')
        try:
            mcp.run()
        except KeyboardInterrupt:
            print('\nMCP Server stopped.')
        except Exception as e:
            print(f'MCP Server error: {e}')
        finally:
            if monitor_session.has_session:
                monitor_session.stop()
                print('INFO: Monitor session cleaned up on server exit', file=sys.stderr)

    # Return the action extension
    return {
        'actions': {
            'mcp-server': {
                'callback': start_mcp_server,
                'help': 'Start MCP (Model Context Protocol) server for AI integration',
                'options': [],
            },
        }
    }