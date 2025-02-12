#!/usr/bin/env python
# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import re
import subprocess
import sys
from typing import Dict
from typing import List

from click.core import Context
from idf_py_actions.errors import FatalError
from idf_py_actions.tools import PropertyDict
from idf_py_actions.tools import red_print
from packaging.version import Version

WOKWI_CLI_PATH = os.getenv('WOKWI_CLI_PATH', 'wokwi-cli')
WOKWI_CLI_MINIMUM_VERSION = '0.15.0'


def check_wokwi_cli() -> bool:
    try:
        stdout = subprocess.check_output([WOKWI_CLI_PATH, '--version']).decode()
        match = re.match(r'Wokwi CLI v(\d+\.\d+\.\d+)', stdout)
        if not match:
            raise FileNotFoundError('wokwi-cli not found.')
        version = match.group(1)
        if Version(version) < Version(WOKWI_CLI_MINIMUM_VERSION):
            red_print(
                f'wokwi-cli version {version} is less than minimum required version {WOKWI_CLI_MINIMUM_VERSION}'
            )
            red_print('Please update wokwi-cli by running: idf.py wokwi-install')
            return False
    except (subprocess.CalledProcessError, FileNotFoundError):
        red_print('wokwi-cli not found.')
        red_print('Please install it using: idf.py wokwi-install')
        return False
    return True


def action_extensions(base_actions: Dict, project_path: List) -> Dict:
    def install_wokwi_cli(action: str, ctx: Context, args: PropertyDict) -> None:
        if sys.platform == 'win32':
            cli_setup_cmd = 'irm https://wokwi.com/ci/install.ps1 | iex'
        else:
            cli_setup_cmd = 'curl -L https://wokwi.com/ci/install.sh | sh'
        print('Running command: ', cli_setup_cmd)
        if sys.platform == 'win32':
            subprocess.run(['powershell', '-Command', cli_setup_cmd], check=True)
        else:
            subprocess.run(cli_setup_cmd, check=True, shell=True)

    def wokwi(action: str, ctx: Context, args: PropertyDict, diagram_file: str) -> None:
        if not check_wokwi_cli():
            raise FatalError('Please run: idf.py wokwi-install')

        wokwi_cmd = [WOKWI_CLI_PATH, args.project_dir]
        if diagram_file:
            wokwi_cmd.append('--diagram-file')
            wokwi_cmd.append(diagram_file)

        try:
            subprocess.run(wokwi_cmd, check=True)
        except subprocess.CalledProcessError as e:
            raise FatalError(f'Error running wokwi-cli: {e}')

    return {
        'actions': {
            'wokwi': {
                'callback': wokwi,
                'help': 'Launch current project in Wokwi simulator',
                'options': [
                    {
                        'names': ['--diagram-file'],
                        'help': 'Allows to specify a custom diagram file to use (e.g. --diagram-file diagram.esp32c3.json)',
                    }
                ],
                'dependencies': ['all'],
            },
            'wokwi-install': {
                'callback': install_wokwi_cli,
                'help': 'Install wokwi-cli',
                'options': [],
            },
        },
    }
