# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import json
import os
import subprocess
import sys
from typing import Any

from click.core import Context

from idf_py_actions.errors import FatalError
from idf_py_actions.tools import PropertyDict
from idf_py_actions.tools import get_build_context
from idf_py_actions.tools import get_target
from idf_py_actions.tools import idf_version

try:
    from mcp.server.fastmcp import FastMCP

    MCP_AVAILABLE = True
except ImportError:
    MCP_AVAILABLE = False


def action_extensions(base_actions: dict, project_path: str) -> dict:
    """ESP-IDF MCP Server Extension"""

    def start_mcp_server(action_name: str, ctx: Context, args: PropertyDict, **kwargs: Any) -> None:
        """Start MCP server for ESP-IDF project integration"""
        if not MCP_AVAILABLE:
            raise FatalError('MCP dependencies not available. Install with: ./install.sh --enable-mcp')

        current_project = None
        # Use current working directory if available, fallback to project_path
        for project_dir in [os.getcwd(), project_path]:
            if project_dir and os.path.exists(os.path.join(project_dir, 'CMakeLists.txt')):
                current_project = project_dir
                break

        if not current_project:
            raise FatalError('Open the MCP server in a valid ESP-IDF project directory.')

        print(f'Starting ESP-IDF MCP Server for project: {current_project}')
        print(f'Target: {get_target(current_project)}')
        print(f'ESP-IDF Version: {idf_version()}')
        print(f'Working Directory: {os.getcwd()}')

        # Initialize MCP server
        mcp = FastMCP('ESP-IDF')

        # === TOOLS (Actions) ===
        @mcp.tool()
        def build_project(target: str = 'all') -> str:
            """Build ESP-IDF project with specified target"""
            try:
                # Use the current project directory
                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    '-C',
                    current_project,
                    'build',
                ]
                result = subprocess.run(cmd, capture_output=True, text=True, cwd=current_project)
                if result.returncode == 0:
                    return f'Successfully built target: {target}'
                else:
                    return f'Build failed: {result.stderr}'
            except Exception as e:
                return f'Build failed: {str(e)}'

        @mcp.tool()
        def set_target(target: str) -> str:
            """Set the ESP-IDF target (esp32, esp32s3, esp32c6, etc.)"""
            try:
                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    '-C',
                    current_project,
                    'set-target',
                    target,
                ]
                result = subprocess.run(cmd, capture_output=True, text=True, cwd=current_project)
                if result.returncode == 0:
                    return f'Target set to: {target}'
                else:
                    return f'Failed to set target: {result.stderr}'
            except Exception as e:
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
                    '-C',
                    current_project,
                ] + flash_args
                result = subprocess.run(cmd, capture_output=True, text=True, cwd=current_project)

                if result.returncode == 0:
                    return f'Successfully flashed project{" to port " + port if port else ""}'
                else:
                    return f'Flash failed: {result.stderr}'
            except Exception as e:
                return f'Error flashing: {str(e)}'

        @mcp.tool()
        def monitor_serial(port: str | None = None) -> str:
            """Start serial monitor (returns immediately, monitor runs in background)"""
            try:
                monitor_args = []
                if port:
                    monitor_args.extend(['-p', port])
                monitor_args.append('monitor')

                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    '-C',
                    current_project,
                ] + monitor_args

                # Start monitor in background
                subprocess.Popen(cmd, cwd=current_project)
                return f'Serial monitor started{" on port " + port if port else ""}'
            except Exception as e:
                return f'Error starting monitor: {str(e)}'

        @mcp.tool()
        def clean_project() -> str:
            """Clean build artifacts"""
            try:
                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    '-C',
                    current_project,
                    'clean',
                ]
                result = subprocess.run(cmd, capture_output=True, text=True, cwd=current_project)
                if result.returncode == 0:
                    return 'Project cleaned successfully'
                else:
                    return f'Clean failed: {result.stderr}'
            except Exception as e:
                return f'Error cleaning: {str(e)}'

        @mcp.tool()
        def menuconfig() -> str:
            """Open menuconfig interface (terminal-based)"""
            try:
                cmd = [
                    sys.executable,
                    os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py'),
                    '-C',
                    current_project,
                    'menuconfig',
                ]
                subprocess.run(cmd, cwd=current_project)
                return 'Menuconfig completed'
            except Exception as e:
                return f'Error opening menuconfig: {str(e)}'

        # === RESOURCES (Data Access) ===

        @mcp.resource('project://config')
        def get_project_config() -> str:
            """Get current project configuration"""
            try:
                config_data = {}

                # Get target
                config_data['target'] = get_target(current_project)

                # Get build directory info
                build_dir = os.path.join(current_project, 'build')
                config_data['build_dir'] = build_dir
                config_data['build_exists'] = os.path.exists(build_dir)

                # Get project description if available
                try:
                    ctx = get_build_context()
                    if 'proj_desc' in ctx:
                        config_data['project_description'] = ctx['proj_desc']
                except Exception:
                    pass

                # Get sdkconfig info
                sdkconfig_path = os.path.join(current_project, 'sdkconfig')
                config_data['sdkconfig_exists'] = os.path.exists(sdkconfig_path)

                return json.dumps(config_data, indent=2)
            except Exception as e:
                return f'Error getting config: {str(e)}'

        @mcp.resource('project://status')
        def get_project_status() -> str:
            """Get current project build status"""
            try:
                status = {
                    'project_path': current_project,
                    'target': get_target(current_project),
                    'idf_version': idf_version(),
                    'build_dir': os.path.join(current_project, 'build'),
                }

                # Check if built
                build_dir = os.path.join(current_project, 'build')
                if os.path.exists(build_dir):
                    status['built'] = True

                    # Check for common build artifacts
                    artifacts = ['bootloader', 'partition_table', 'app-flash', 'flash_args']
                    status['artifacts'] = {}
                    for artifact in artifacts:
                        artifact_path = os.path.join(build_dir, artifact)
                        status['artifacts'][artifact] = os.path.exists(artifact_path)
                else:
                    status['built'] = False

                return json.dumps(status, indent=2)
            except Exception as e:
                return f'Error getting status: {str(e)}'

        @mcp.resource('project://devices')
        def get_connected_devices() -> str:
            """Get list of connected ESP devices"""
            try:
                # Use esptool to list ports
                cmd = [sys.executable, '-m', 'esptool', '--list-ports']
                result = subprocess.run(cmd, capture_output=True, text=True)

                devices = {
                    'available_ports': result.stdout.strip().split('\n') if result.stdout else [],
                    'esptool_available': result.returncode == 0,
                }

                return json.dumps(devices, indent=2)
            except Exception as e:
                return f'Error getting devices: {str(e)}'

        # Start the MCP server
        print('MCP Server running on stdio...')
        print('Available tools: build_project, set_target, flash_project, monitor_serial, clean_project, menuconfig')
        print('Available resources: project://config, project://status, project://devices')

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
            }
        }
    }
