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
                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    '-C',
                    current_project,
                    'mcp-server',
                ]
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

        # === TOOLS (Actions) ===
        @mcp.tool()
        def build_project() -> str:
            """Build ESP-IDF project"""
            try:
                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    'build',
                ]
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
                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    'set-target',
                    target,
                ]
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
                flash_args = []
                if port:
                    flash_args.extend(['-p', port])
                flash_args.append('flash')

                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                ] + flash_args
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
                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    'clean',
                ]
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
