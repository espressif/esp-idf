# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import json
import math
import os
import subprocess
import sys
import tempfile
import time
from pathlib import Path
from typing import Annotated
from typing import Any

from esp_idf_monitor.base.constants import EXIT_EXPECT_TIMEOUT
from esp_idf_monitor.base.constants import EXIT_SCRIPT_ERROR
from esp_pylib.logger import log
from esp_pylib.serial_ports import get_port_names
from rich_click import Context

from idf_py_actions.errors import FatalError
from idf_py_actions.tools import PropertyDict
from idf_py_actions.tools import get_sdkconfig_value
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
    # mcp >= 2.0: FastMCP was renamed to MCPServer
    from mcp.server import MCPServer as _MCPServer

    MCP_AVAILABLE = True
except ImportError:
    try:
        # mcp 1.x fallback
        from mcp.server.fastmcp import FastMCP as _MCPServer

        MCP_AVAILABLE = True
    except ImportError:
        MCP_AVAILABLE = False


try:
    # Field() feeds per-argument descriptions into a tool's inputSchema (see the
    # Annotated[...] argument hints on monitor_device below).
    # Annotated evaluates Field(...) at import time, and this whole module is imported
    # unconditionally by idf.py on every run, regardless of the command invoked - so without
    # this fallback, idf.py would break entirely on installs that lack the optional
    # "mcp" feature. pydantic ships as a dependency of mcp, so this import only
    # fails when mcp itself is not installed either (see MCP_AVAILABLE above).
    from pydantic import Field
except ImportError:

    def Field(**kwargs: Any) -> Any:  # type: ignore[misc] # noqa: N802
        return None


# Root folder (inside the OS temp directory) that ESP-IDF MCP tool logs are
# written under, one subfolder per tool.
MCP_LOG_ROOT_DIR_NAME = 'esp_idf_mcp_log'

# Bounded wait injected into 'expect' lines that arrive without --timeout.
DEFAULT_MONITOR_TIMEOUT_SEC = 20.0
# Whole-script cap: sum of expect and sleep durations.
MONITOR_MAX_SCRIPT_SEC = 600.0
# Commands the MCP monitor script may contain. Everything else (flash, log, ...)
# is rejected so this tool cannot trigger a rebuild or toggle monitor UI state.
MONITOR_SCRIPT_COMMANDS = frozenset({'expect', 'send', 'sleep', 'reset', 'bootloader', 'exit'})
# Size limit for text embedded directly in a tool result, in characters. Serial
# output belongs in the log file, not in the agent's context.
MONITOR_TAIL_CHARS = 1500


def _is_valid_project_dir(directory: str) -> bool:
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

    # Normalised patterns (whitespace removed) for whitespace-insensitive matching.
    # CMake treats whitespace inside include(...) as insignificant, so
    # `include( $ENV{...} )` must be accepted alongside `include($ENV{...})`.
    normalised_patterns = [''.join(p.split()) for p in CMAKE_PROJECT_LINE]

    try:
        with open(cmakelists_path, encoding='utf-8') as f:
            for line in f:
                line_normalised = ''.join(line.split())
                if any(line_normalised.startswith(pattern) for pattern in normalised_patterns):
                    return True
    except Exception:
        return False

    return False


def _derive_build_dir(effective_dir: str, launch_dir: str, args: PropertyDict) -> str:
    """Use idf.py's build_dir when it belongs to this project; otherwise ``<project>/build``."""
    if launch_dir == effective_dir:
        build_dir = args.get('build_dir', '')
        if build_dir:
            return str(build_dir)
    return os.path.join(effective_dir, 'build')


def _load_project_description(
    launch_dir: str, args: PropertyDict
) -> tuple[str | None, str | None, dict[str, Any] | None]:
    """Resolve project dir, build dir, and ``project_description.json`` without failing.

    Returns ``(project_dir, build_dir, description)``. Any piece that cannot be
    resolved is ``None``. Missing project, build directory, or description is not
    an error.
    """
    project_dir = resolve_default_project_dir(launch_dir)
    if project_dir is None:
        return None, None, None
    build_dir = _derive_build_dir(project_dir, launch_dir, args)
    if not os.path.exists(build_dir):
        return project_dir, None, None
    desc_path = os.path.join(build_dir, 'project_description.json')
    try:
        with open(desc_path, encoding='utf-8') as f:
            description = json.load(f)
    except (OSError, ValueError):
        return project_dir, build_dir, None
    if not isinstance(description, dict):
        return project_dir, build_dir, None
    return project_dir, build_dir, description


def _monitor_normalize_expect_line(line: str, default_expect_duration: float) -> tuple[str, float | None]:
    """Inject '--timeout default_expect_duration' into a bare 'expect' line.

    Returns (line, expect_duration): expect_duration is the bound this line
    ends up with, or None if the line already carries a --timeout the monitor
    will reject - left untouched so the monitor reports the script error
    itself. Timeout syntax is parsed the same way CommandReader parses it.
    """
    command, _, argument = line.partition(' ')
    argument = argument.strip()
    if argument.startswith('--timeout'):
        parts = argument.split(None, 2)
        try:
            expect_duration = float(parts[1])
        except (IndexError, ValueError):
            return line, None
        if not math.isfinite(expect_duration) or expect_duration <= 0:
            return line, None
        return line, expect_duration
    return (
        ' '.join(filter(None, [command, '--timeout', f'{default_expect_duration:g}', argument])),
        default_expect_duration,
    )


def _monitor_parse_sleep_duration(line: str) -> float | None:
    """Return the duration of a 'sleep' line, or None if the monitor would
    reject it (such a line is skipped, so it costs no time)."""
    argument = line.partition(' ')[2].strip()
    try:
        sleep_duration = float(argument)
    except ValueError:
        return None
    if not math.isfinite(sleep_duration) or sleep_duration <= 0:
        return None
    return sleep_duration


def _save_monitor_output(output: str) -> str:
    """Save the monitor's output to a log file and describe where it went, or
    inline a tail on failure - keeps the full serial dump out of the agent's context.

    *output* is the monitor's merged stdout and stderr: the chip prints serial
    data on stdout while the monitor's own '--- ' lines (decoded panic
    backtraces among them) go to stderr, so both streams are captured through
    one pipe to keep a decoded backtrace next to the raw one that triggered it.
    """
    log_dir = Path(tempfile.gettempdir()) / MCP_LOG_ROOT_DIR_NAME / 'action_monitor'
    log_path = log_dir / f'monitor_{time.strftime("%Y%m%d_%H%M%S")}.log'
    try:
        log_dir.mkdir(parents=True, exist_ok=True)
        log_path.write_text(output, encoding='utf-8', errors='replace')
        # Path on its own line so an agent cannot paraphrase the character
        # count and drop the location.
        return f'Log file: {log_path}\nMonitor output: {len(output)} characters. Read or grep that file.'
    except OSError as e:
        return f'Log file could not be written ({e}), so here is its tail instead:\n{tail(output, MONITOR_TAIL_CHARS)}'


def _monitor_status(returncode: int) -> str:
    """Explain a monitor exit code, so the agent does not have to scrape stderr."""
    if returncode == 0:
        return 'Monitor script completed successfully (exit code 0).'
    if returncode == EXIT_EXPECT_TIMEOUT:
        return (
            f'An expect pattern was not seen before its --timeout elapsed (exit code {returncode}). '
            'The monitor aborted the rest of the script.'
        )
    if returncode == EXIT_SCRIPT_ERROR:
        return (
            f'The monitor rejected the script (exit code {returncode}): invalid expect syntax, '
            'timeout value or regular expression.'
        )
    return f'The monitor exited with code {returncode}.'


def _monitor_project_args(baud: str | int | None, launch_dir: str, args: PropertyDict) -> list[str]:
    """Baud, optional decode metadata, and ELF files derived from the project build."""
    extra: list[str] = []
    _, build_dir, description = _load_project_description(launch_dir, args)

    # baud: explicit argument, else monitor_baud from the description
    if baud is not None:
        extra += ['-b', str(baud)]
    elif description is not None and description.get('monitor_baud') is not None:
        extra += ['-b', str(description['monitor_baud'])]

    if description is None or not build_dir:
        return extra

    # ELF files (app ELF first, same order as idf.py monitor)
    elf_file = os.path.join(build_dir, description.get('app_elf', '') or '')
    elf_list = [str(elf) for elf in Path(build_dir).rglob('*.elf')]
    if elf_file and elf_file in elf_list:
        elf_list.insert(0, elf_list.pop(elf_list.index(elf_file)))
    # without ELF files, metadata has no additional value to monitor
    if not elf_list:
        return extra

    # toolchain / target / decode flags (only useful with ELFs)
    toolchain_prefix = description.get('monitor_toolprefix')
    if toolchain_prefix:
        extra += ['--toolchain-prefix', str(toolchain_prefix)]
    config_file = str(description.get('config_file') or '')
    coredump_decode = get_sdkconfig_value(config_file, 'CONFIG_ESP_COREDUMP_DECODE')
    if coredump_decode is not None:
        extra += ['--decode-coredumps', coredump_decode]
    target = description.get('target')
    if target:
        extra += ['--target', str(target)]
    revision = description.get('min_rev')
    if revision:
        extra += ['--revision', str(revision)]
    if get_sdkconfig_value(config_file, 'CONFIG_IDF_TARGET_ARCH_RISCV'):
        extra += ['--decode-panic', 'backtrace']
    extra += elf_list
    return extra


def resolve_default_project_dir(launch_dir: str) -> str | None:
    """
    Returns the first valid ESP-IDF project directory from the server's launch
    context, or None if none is found.

    Priority: IDF_MCP_WORKSPACE_FOLDER env var > launch_dir

    Use this from contexts without an explicit ``project_dir`` argument
    """
    for candidate in [os.environ.get('IDF_MCP_WORKSPACE_FOLDER', ''), launch_dir]:
        if candidate and _is_valid_project_dir(candidate):
            return candidate
    return None


def resolve_tool_project_dir(explicit_dir: str | None, launch_dir: str) -> tuple[str | None, str | None]:
    """
    Resolves the effective project directory for an MCP tool call.

    Returns ``(effective_dir, None)`` on success, or ``(None, error_message)``
    on failure. When ``explicit_dir`` is provided it is validated immediately —
    the fallback chain is never tried for an explicit but invalid path.

    Use this from MCP tools that accept a ``project_dir`` argument
    """
    if explicit_dir is not None:
        if not _is_valid_project_dir(explicit_dir):
            return None, f'"{explicit_dir}" is not a valid ESP-IDF project directory.'
        return explicit_dir, None
    effective = resolve_default_project_dir(launch_dir)
    if effective is not None:
        return effective, None
    return None, (
        'No valid ESP-IDF project directory found. '
        'Pass project_dir explicitly, set IDF_MCP_WORKSPACE_FOLDER, '
        'or restart with: idf.py -C <project_dir> mcp-server'
    )


def assemble_monitor_script_from_agent_commands(
    commands: str,
    timeout_sec: float = DEFAULT_MONITOR_TIMEOUT_SEC,
) -> tuple[str, float]:
    """Frame the agent's command body into a runnable monitor script.

    Bounds every 'expect' with --timeout (default *timeout_sec*) and appends
    'exit' if missing, so the monitor always terminates on its own. A leading
    'reset' is not added: the monitor already resets the chip when it opens the
    port. Agent-supplied 'reset' lines are left in place.

    Returns (script, effective_timeout). Raises ValueError if *commands* has no
    command at all, if a line is not one of MONITOR_SCRIPT_COMMANDS, or if the
    script would run longer than MONITOR_MAX_SCRIPT_SEC.

    effective_timeout is the sum of every bounded expect duration plus every
    sleep duration. If the script has neither (for example only 'send'),
    *timeout_sec* is used instead so the monitor process still has a kill bound.
    """
    lines: list[str] = []
    effective_timeout = 0.0
    has_command = False
    allowed = ', '.join(sorted(MONITOR_SCRIPT_COMMANDS))

    for raw_line in commands.splitlines():
        line = raw_line.strip()
        if not line:
            continue
        if line.startswith('#'):
            lines.append(line)
            continue
        command = line.partition(' ')[0].lower()
        if command not in MONITOR_SCRIPT_COMMANDS:
            raise ValueError(f'Unsupported monitor command {command!r}. Allowed commands: {allowed}.')
        has_command = True
        if command == 'expect':
            line, expect_duration = _monitor_normalize_expect_line(line, timeout_sec)
            if expect_duration is not None:
                effective_timeout += expect_duration
        elif command == 'sleep':
            sleep_duration = _monitor_parse_sleep_duration(line)
            if sleep_duration is not None:
                effective_timeout += sleep_duration
        lines.append(line)

    if not has_command:
        raise ValueError(
            'No monitor commands given. Pass a commands body with at least one command, '
            'for example: expect --timeout 20 Hello world!'
        )

    if not lines or lines[-1].lower() != 'exit':
        lines.append('exit')

    if effective_timeout <= 0:
        effective_timeout = timeout_sec
    if effective_timeout > MONITOR_MAX_SCRIPT_SEC:
        raise ValueError(f'This script would run for {effective_timeout:g}s. The limit is {MONITOR_MAX_SCRIPT_SEC:g}s.')
    return '\n'.join(lines) + '\n', effective_timeout


def decode_stream(stream: Any) -> str:
    """Decode a subprocess stream that may be bytes even in text mode.

    TimeoutExpired carries the output collected so far as bytes on POSIX, while
    a completed run in text mode yields str.
    """
    if stream is None:
        return ''
    if isinstance(stream, bytes):
        return stream.decode('utf-8', errors='replace')
    return str(stream)


def tail(text: str, limit: int) -> str:
    """Return at most the last *limit* characters of *text*, marking a cut."""
    text = text.strip()
    if len(text) <= limit:
        return text
    return f'[...truncated...]\n{text[-limit:]}'


def action_extensions(base_actions: dict, project_path: str) -> dict:
    """ESP-IDF MCP Server Extension"""

    def start_mcp_server(action_name: str, ctx: Context, args: PropertyDict, **kwargs: Any) -> None:
        """Start MCP server for ESP-IDF project integration"""
        # MCP stdio transport consumes stdout; keep informational output on stderr.
        log.set_info_stream(sys.stderr)

        if not MCP_AVAILABLE:
            raise FatalError(
                'MCP dependencies not available. '
                'Install ESP-IDF using the EIM installer and select the "mcp" feature to be included. '
                'For more information, refer to the official Espressif EIM Installer documentation '
                'or use "idf.py docs" and search for EIM configuration instructions.'
            )

        # Resolve the default project directory. Then derive the startup log line, and the
        # bound_hint that is appended to every tool's description so the LLM driving
        # the MCP client knows when (not) to pass project_dir.
        startup_default_dir = resolve_default_project_dir(project_path)
        if startup_default_dir is not None:
            log.note(f'Starting ESP-IDF MCP Server. Default project: {startup_default_dir}')
            bound_hint = (
                f"This MCP server was launched with '{startup_default_dir}' as the default ESP-IDF "
                'project. Leave project_dir as None to operate on this project. Only set project_dir '
                '(absolute path to a directory containing a CMakeLists.txt with project()) when the '
                'user explicitly asks to operate on a different ESP-IDF project.'
            )
        else:
            log.note(
                'Starting ESP-IDF MCP Server. No project directory configured at startup. '
                'Pass project_dir in each tool call, or set IDF_MCP_WORKSPACE_FOLDER, '
                'or restart with: idf.py -C <project_dir> mcp-server'
            )
            bound_hint = (
                'This MCP server was launched without a project context. You MUST pass project_dir '
                '(absolute path to a directory containing a CMakeLists.txt with project()) on every '
                'call, otherwise the call will fail.'
            )

        # Initialize MCP server — project validity is checked per-tool call
        mcp = _MCPServer('ESP-IDF')

        # === TOOLS (Actions) ===
        # Tool handlers spawn idf.py with stdin=subprocess.DEVNULL. The MCP server's
        # stdin is the long-lived JSON-RPC transport pipe; a child that inherits it
        # can hang instead of exiting.
        @mcp.tool(description=f'Build the ESP-IDF project (runs `idf.py build`). {bound_hint}')
        def build_project(project_dir: str | None = None) -> str:
            """Build the ESP-IDF project.

            Args:
                project_dir: Optional absolute path to a valid ESP-IDF project directory.
                    Leave as None to use the project this MCP server was launched with
                    (or the IDF_MCP_WORKSPACE_FOLDER environment variable). Set this
                    only to override that default with another project.
            """
            effective_dir, error = resolve_tool_project_dir(project_dir, project_path)
            if error:
                return error
            assert effective_dir is not None  # mypy narrowing
            try:
                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    '-C',
                    effective_dir,
                    'build',
                ]
                log.note(f'Building project with command: {" ".join(cmd)} in path: {effective_dir}')
                result = subprocess.run(cmd, capture_output=True, text=True, stdin=subprocess.DEVNULL)
                if result.returncode == 0:
                    log.note('Build successful')
                    return 'Successfully built project'
                else:
                    log.err(f'Build failed: {result.stderr}')
                    return f'Build failed: {result.stderr}'
            except Exception as e:
                log.err(f'Build failed: {e}')
                return f'Build failed: {str(e)}'

        @mcp.tool(
            description=(
                'Set the ESP-IDF target chip (esp32, esp32s3, esp32c6, etc.) for the project '
                f'(runs `idf.py set-target`). {bound_hint}'
            )
        )
        def set_target(target: str, project_dir: str | None = None) -> str:
            """Set the ESP-IDF target for the project.

            Args:
                target: Target chip identifier (e.g. esp32, esp32s3, esp32c6).
                project_dir: Optional absolute path to a valid ESP-IDF project directory.
                    Leave as None to use the project this MCP server was launched with
                    (or the IDF_MCP_WORKSPACE_FOLDER environment variable). Set this
                    only to override that default with another project.
            """
            effective_dir, error = resolve_tool_project_dir(project_dir, project_path)
            if error:
                return error
            assert effective_dir is not None  # mypy narrowing
            try:
                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    '-C',
                    effective_dir,
                    'set-target',
                    target,
                ]
                log.note(f'Setting target with command: {" ".join(cmd)} in path: {effective_dir}')
                result = subprocess.run(cmd, capture_output=True, text=True, stdin=subprocess.DEVNULL)
                if result.returncode == 0:
                    log.note(f'Target set to: {target}')
                    return f'Target set to: {target}'
                else:
                    log.err(f'Failed to set target: {result.stderr}')
                    return f'Failed to set target: {result.stderr}'
            except Exception as e:
                log.err(f'Failed to set target: {e}')
                return f'Error setting target: {str(e)}'

        @mcp.tool(
            description=(f'Flash the built ESP-IDF project to a connected device (runs `idf.py flash`). {bound_hint}')
        )
        def flash_project(port: str | None = None, project_dir: str | None = None) -> str:
            """Flash the built ESP-IDF project to a connected device.

            Args:
                port: Optional serial port to flash through (e.g. /dev/ttyUSB0, COM3).
                    Leave as None to let idf.py auto-detect.
                project_dir: Optional absolute path to a valid ESP-IDF project directory.
                    Leave as None to use the project this MCP server was launched with
                    (or the IDF_MCP_WORKSPACE_FOLDER environment variable). Set this
                    only to override that default with another project.
            """
            effective_dir, error = resolve_tool_project_dir(project_dir, project_path)
            if error:
                return error
            assert effective_dir is not None  # mypy narrowing
            try:
                flash_args = []
                if port:
                    flash_args.extend(['-p', port])
                flash_args.append('flash')

                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    '-C',
                    effective_dir,
                ] + flash_args
                log.note(f'Flashing project with command: {" ".join(cmd)} in path: {effective_dir}')
                result = subprocess.run(cmd, capture_output=True, text=True, stdin=subprocess.DEVNULL)

                if result.returncode == 0:
                    log.note('Flash successful')
                    return f'Successfully flashed project{" to port " + port if port else ""}'
                else:
                    log.err(f'Flash failed: {result.stderr}')
                    return f'Flash failed: {result.stderr}'
            except Exception as e:
                log.err(f'Flash failed: {e}')
                return f'Error flashing: {str(e)}'

        @mcp.tool(
            description=(
                'Run a serial monitor session against a flashed device, driven by a script '
                '(runs `python -m esp_idf_monitor` in command-stream mode). '
                'Use this to observe what a device prints and to wait for specific output. '
                'Write the commands body from the project context or from the exact instructions '
                'of the user; allowed commands are expect, send, sleep, reset, bootloader and exit. '
                'The monitor already resets the chip when it opens the port, so do not start the '
                'script with reset unless you need an extra reset later. Set no_reset to skip that '
                'connection reset. ALWAYS end the script with an explicit exit as its last line. '
                'Every expect should be bounded: "expect --timeout <seconds> <regex>". '
                f'The whole script may take at most {MONITOR_MAX_SCRIPT_SEC:g} seconds. '
                'Exit code 0 means the script finished, 110 means an expect pattern never appeared, '
                '2 means the script itself was invalid. '
                'Returns a short status plus a Log file: line with the absolute path of the monitor '
                'log, which holds the serial output together with the monitor messages. '
                'Always quote that path when answering the user; read or '
                'grep the file instead of expecting the whole log inline. '
                'The port is optional and is autodetected when omitted; see project://devices '
                'when several devices are connected.'
            )
        )
        def monitor_device(
            commands: Annotated[
                str,
                Field(
                    description=(
                        'Monitor script body, one command per line (expect, send, sleep, reset, bootloader, exit). '
                        'Bound every wait as "expect --timeout <seconds> <regex>" (default 20 seconds). '
                        f'The whole script may take at most {MONITOR_MAX_SCRIPT_SEC:g} seconds. '
                        'A reset at the start is unnecessary: the monitor resets on connect unless no_reset. '
                        'Leave any later reset in the script. End with exit; the server appends one if missing. '
                        'Lines starting with # are comments.'
                    )
                ),
            ],
            timeout_sec: Annotated[
                float,
                Field(
                    description=(
                        'Wait, in seconds, injected into expect lines that omit --timeout. '
                        f'The whole script may take at most {MONITOR_MAX_SCRIPT_SEC:g} seconds, '
                        'the sum of expect and sleep durations. The monitor process is killed after '
                        'twice that time. If the script has no expect or sleep, twice timeout_sec is used.'
                    )
                ),
            ] = DEFAULT_MONITOR_TIMEOUT_SEC,
            port: str | None = None,
            baud: str | int | None = None,
            no_reset: bool = False,
        ) -> str:
            """Run a scripted esp-idf-monitor session and return a status plus a log path.

            Args:
                port: Optional serial port such as /dev/ttyUSB0 or COM3. Leave as
                    None to let the monitor autodetect it.
                baud: Optional monitor baud rate. When omitted, ``monitor_baud``
                    from ``project_description.json`` is used if that file exists.
                no_reset: Pass --no-reset so the monitor does not reset the chip
                    when it opens the port. Agent-supplied reset commands in the
                    script are left unchanged.
            """
            if not math.isfinite(timeout_sec) or timeout_sec <= 0:
                return f'timeout_sec must be a finite number greater than 0, got {timeout_sec}.'

            try:
                script, effective_timeout = assemble_monitor_script_from_agent_commands(
                    commands, timeout_sec=timeout_sec
                )
            except ValueError as e:
                return str(e)
            hard_timeout = 2 * effective_timeout

            cmd = [sys.executable, '-m', 'esp_idf_monitor']
            if port:
                cmd.extend(['-p', port])
            cmd.extend(_monitor_project_args(baud, project_path, args))
            if no_reset:
                cmd.append('--no-reset')

            log.note(f'Running monitor: {" ".join(cmd)} (hard timeout {hard_timeout:g}s)')
            log.note(f'Monitor script:\n{script}')

            try:
                result = subprocess.run(
                    cmd,
                    input=script,
                    stdout=subprocess.PIPE,
                    # One pipe for both streams keeps the monitor's decoded backtraces
                    # in order with the serial lines they belong to.
                    stderr=subprocess.STDOUT,
                    text=True,
                    timeout=hard_timeout,
                )
            except subprocess.TimeoutExpired as e:
                log.note(f'Monitor killed after {hard_timeout:g}s')
                output = decode_stream(e.output)
                parts = [
                    f'The monitor did not exit on its own and was killed after {hard_timeout:g} seconds '
                    '(twice the time the script should have taken). '
                    'The serial output captured until then was kept.',
                    _save_monitor_output(output),
                ]
                output_tail = tail(output, MONITOR_TAIL_CHARS)
                if output_tail:
                    parts.append(f'Last output before the kill:\n{output_tail}')
                return '\n'.join(parts)
            except Exception as e:
                log.err(f'Monitor failed to run: {e}')
                return f'Failed to run the monitor: {e}'

            log.note(f'Monitor exited with code {result.returncode}')
            output = result.stdout or ''
            parts = [_monitor_status(result.returncode), _save_monitor_output(output)]
            if result.returncode != 0:
                output_tail = tail(output, MONITOR_TAIL_CHARS)
                if output_tail:
                    parts.append(f'Last output before the exit:\n{output_tail}')
            return '\n'.join(parts)

        @mcp.tool(
            description=(
                'Create a new ESP-IDF project from the sample template (runs `idf.py create-project`). '
                'A directory named <name> is created inside <path>. Use this only when the user asks '
                'to bootstrap a new project; do not use it on an existing project.'
            )
        )
        def create_project(name: str, path: str | None = None) -> str:
            """Create a new ESP-IDF project from the sample template.

            Args:
                name: Name of the new project; also becomes the subdirectory name.
                path: Optional absolute path to the parent directory in which the
                    <name>/ subdirectory will be created. Leave as None to create
                    it in the directory the MCP server was launched from.
            """
            parent_dir = path or project_path or os.getcwd()
            if not os.path.isdir(parent_dir):
                return f'Parent directory does not exist: {parent_dir}'
            try:
                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    '-C',
                    parent_dir,
                    'create-project',
                    name,
                ]
                log.note(f'Creating project "{name}" in {parent_dir}')
                result = subprocess.run(cmd, capture_output=True, text=True, stdin=subprocess.DEVNULL)
                if result.returncode == 0:
                    project_path_new = os.path.join(parent_dir, name)
                    log.note(f'Project "{name}" created at {project_path_new}')
                    return f'Project "{name}" created at {project_path_new}'
                else:
                    output = result.stderr or result.stdout
                    log.err(f'Failed to create project: {output}')
                    return f'Failed to create project "{name}": {output}'
            except Exception as e:
                log.err(f'Failed to create project: {e}')
                return f'Failed to create project "{name}": {str(e)}'

        @mcp.tool(description=f'Remove build artifacts from the ESP-IDF project (runs `idf.py clean`). {bound_hint}')
        def clean_project(project_dir: str | None = None) -> str:
            """Remove build artifacts from the ESP-IDF project.

            Args:
                project_dir: Optional absolute path to a valid ESP-IDF project directory.
                    Leave as None to use the project this MCP server was launched with
                    (or the IDF_MCP_WORKSPACE_FOLDER environment variable). Set this
                    only to override that default with another project.
            """
            effective_dir, error = resolve_tool_project_dir(project_dir, project_path)
            if error:
                return error
            assert effective_dir is not None  # mypy narrowing
            try:
                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    '-C',
                    effective_dir,
                    'clean',
                ]
                log.note(f'Cleaning project with command: {" ".join(cmd)} in path: {effective_dir}')
                result = subprocess.run(cmd, capture_output=True, text=True, stdin=subprocess.DEVNULL)
                if result.returncode == 0:
                    log.note('Project cleaned successfully')
                    return 'Project cleaned successfully'
                else:
                    log.err(f'Clean failed: {result.stderr}')
                    return f'Clean failed: {result.stderr}'
            except Exception as e:
                log.err(f'Error cleaning: {e}')
                return f'Error cleaning: {str(e)}'

        # === RESOURCES (Data Access) ===
        @mcp.resource('project://config')
        def get_project_config() -> str:
            """Get current project configuration"""
            effective_dir, build_dir, description = _load_project_description(project_path, args)
            config: dict[str, Any] = {}

            if effective_dir is None:
                config['error'] = (
                    'No valid ESP-IDF project directory found. '
                    'Set IDF_MCP_WORKSPACE_FOLDER or restart with: idf.py -C <project_dir> mcp-server'
                )
                return json.dumps(config, indent=2)
            config['project_path'] = effective_dir

            if build_dir is None:
                config['build_dir_exists'] = False
                return json.dumps(config, indent=2)

            config['build_dir'] = build_dir
            if description is not None:
                config['project_description'] = description
            else:
                config['project_description'] = 'Project description does not exist'

            return json.dumps(config, indent=2)

        @mcp.resource('project://status')
        def get_project_status() -> str:
            """Get current project build status"""
            status: dict[str, Any] = {}
            try:
                effective_dir = resolve_default_project_dir(project_path)

                if effective_dir is None:
                    status['error'] = (
                        'No valid ESP-IDF project directory found. '
                        'Set IDF_MCP_WORKSPACE_FOLDER or restart with: idf.py -C <project_dir> mcp-server'
                    )
                    status['idf_version'] = idf_version()
                    return json.dumps(status, indent=2)

                status['project_path'] = effective_dir
                status['target'] = get_target(effective_dir)
                status['idf_version'] = idf_version()

                build_dir = _derive_build_dir(effective_dir, project_path, args)
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
                status['error'] = f'Error getting status: {str(e)}'
                return json.dumps(status, indent=2)

        @mcp.resource('project://devices')
        def get_connected_devices() -> str:
            """Get list of connected devices"""
            try:
                devices_on_ports = [name.strip() for name in get_port_names()]
                log.note(f'Devices: {devices_on_ports}')
                devices = {'available_ports': devices_on_ports if devices_on_ports else []}
                return json.dumps(devices, indent=2)
            except Exception as e:
                return f'Error getting devices: {str(e)}'

        # Start the MCP server. Diagnostics should go to stderr — stdout is reserved
        # for the JSON-RPC transport and any non-JSON bytes can confuse MCP clients.
        log.note('MCP Server running on stdio...')

        try:
            mcp.run()
        except KeyboardInterrupt:
            log.note('MCP Server stopped.')
        except Exception as e:
            log.err(f'MCP Server error: {e}')

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
