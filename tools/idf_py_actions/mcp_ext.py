# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import json
import os
import subprocess
import sys
from pathlib import Path
from typing import Any

from click.core import Context

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

    try:
        with open(str(cmakelists_path), encoding='utf-8') as f:
            for line in f:
                if any(proj_line in line for proj_line in CMAKE_PROJECT_LINE):
                    return True
    except Exception:
        return False

    return False


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

        # Resolve the default project directory. Then derive the startup log line, and the
        # bound_hint that is appended to every tool's description so the LLM driving
        # the MCP client knows when (not) to pass project_dir.
        startup_default_dir = resolve_default_project_dir(project_path)
        if startup_default_dir is not None:
            print(f'INFO: Starting ESP-IDF MCP Server. Default project: {startup_default_dir}', file=sys.stderr)
            bound_hint = (
                f"This MCP server was launched with '{startup_default_dir}' as the default ESP-IDF "
                'project. Leave project_dir as None to operate on this project. Only set project_dir '
                '(absolute path to a directory containing a CMakeLists.txt with project()) when the '
                'user explicitly asks to operate on a different ESP-IDF project.'
            )
        else:
            print(
                'INFO: Starting ESP-IDF MCP Server. No project directory configured at startup. '
                'Pass project_dir in each tool call, or set IDF_MCP_WORKSPACE_FOLDER, '
                'or restart with: idf.py -C <project_dir> mcp-server',
                file=sys.stderr,
            )
            bound_hint = (
                'This MCP server was launched without a project context. You MUST pass project_dir '
                '(absolute path to a directory containing a CMakeLists.txt with project()) on every '
                'call, otherwise the call will fail.'
            )

        # Initialize MCP server — project validity is checked per-tool call
        mcp = FastMCP('ESP-IDF')

        # === TOOLS (Actions) ===
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
                print(f'INFO: Building project with command: {" ".join(cmd)} in path: {effective_dir}', file=sys.stderr)
                result = subprocess.run(cmd, capture_output=True, text=True)
                if result.returncode == 0:
                    print('INFO: Build successful', file=sys.stderr)
                    return 'Successfully built project'
                else:
                    print(f'ERROR: Build failed: {result.stderr}', file=sys.stderr)
                    return f'Build failed: {result.stderr}'
            except Exception as e:
                print(f'ERROR: Build failed: {str(e)}', file=sys.stderr)
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
                print(f'INFO: Setting target with command: {" ".join(cmd)} in path: {effective_dir}', file=sys.stderr)
                result = subprocess.run(cmd, capture_output=True, text=True)
                if result.returncode == 0:
                    print(f'INFO: Target set to: {target}', file=sys.stderr)
                    return f'Target set to: {target}'
                else:
                    print(f'ERROR: Failed to set target: {result.stderr}', file=sys.stderr)
                    return f'Failed to set target: {result.stderr}'
            except Exception as e:
                print(f'ERROR: Failed to set target: {str(e)}', file=sys.stderr)
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
                print(f'INFO: Flashing project with command: {" ".join(cmd)} in path: {effective_dir}', file=sys.stderr)
                result = subprocess.run(cmd, capture_output=True, text=True)

                if result.returncode == 0:
                    print('INFO: Flash successful', file=sys.stderr)
                    return f'Successfully flashed project{" to port " + port if port else ""}'
                else:
                    print(f'ERROR: Flash failed: {result.stderr}', file=sys.stderr)
                    return f'Flash failed: {result.stderr}'
            except Exception as e:
                print(f'ERROR: Flash failed: {str(e)}', file=sys.stderr)
                return f'Error flashing: {str(e)}'

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
                print(f'INFO: Cleaning project with command: {" ".join(cmd)} in path: {effective_dir}', file=sys.stderr)
                result = subprocess.run(cmd, capture_output=True, text=True)
                if result.returncode == 0:
                    print('INFO: Project cleaned successfully', file=sys.stderr)
                    return 'Project cleaned successfully'
                else:
                    print(f'ERROR: Clean failed: {result.stderr}', file=sys.stderr)
                    return f'Clean failed: {result.stderr}'
            except Exception as e:
                print(f'ERROR: Error cleaning: {str(e)}', file=sys.stderr)
                return f'Error cleaning: {str(e)}'

        # === RESOURCES (Data Access) ===
        @mcp.resource('project://config')
        def get_project_config() -> str:
            """Get current project configuration"""
            effective_dir = resolve_default_project_dir(project_path)
            config: dict[str, Any] = {}

            if effective_dir is None:
                config['error'] = (
                    'No valid ESP-IDF project directory found. '
                    'Set IDF_MCP_WORKSPACE_FOLDER or restart with: idf.py -C <project_dir> mcp-server'
                )
                return json.dumps(config, indent=2)
            config['project_path'] = effective_dir

            # Use the build_dir from idf.py args when the project matches; otherwise derive it.
            build_dir = (
                args.get('build_dir', '') if project_path == effective_dir else os.path.join(effective_dir, 'build')
            )

            if not os.path.exists(build_dir):
                config['build_dir_exists'] = False
                return json.dumps(config, indent=2)

            config['build_dir'] = build_dir
            proj_desc_fn = os.path.join(build_dir, 'project_description.json')
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

                # Use the build_dir from idf.py args when the project matches; otherwise derive it.
                build_dir = (
                    args.get('build_dir', '') if project_path == effective_dir else os.path.join(effective_dir, 'build')
                )
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
