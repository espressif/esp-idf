#!/usr/bin/env python
# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Ensure that the Python version used to initiate this script is appropriate for
running the ESP-IDF shell activation. The primary goal is to perform the minimum
necessary checks to identify the virtual environment with the default user Python
and then launch activate.py using the ESP-IDF Python virtual environment.
"""
import os
import sys
from subprocess import run
from subprocess import SubprocessError


def die(msg: str) -> None:
    sys.exit(f'\nERROR: {msg}')


idf_tools_path = os.path.realpath(os.path.dirname(__file__))
idf_path = os.path.dirname(idf_tools_path)
sys.path.insert(0, idf_tools_path)

try:
    # The idf_tools module checks for Python version compatibility.
    import idf_tools
except ImportError as e:
    die(f'Unable to import the idf_tools module: {e}')

# Get ESP-IDF venv python path
idf_tools.g.idf_path = idf_path
idf_tools.g.idf_tools_path = os.environ.get('IDF_TOOLS_PATH') or os.path.expanduser(idf_tools.IDF_TOOLS_PATH_DEFAULT)
idf_python_env_path, idf_python_export_path, virtualenv_python, idf_version = idf_tools.get_python_env_path()

os.environ['IDF_PATH_OLD'] = os.environ.get('IDF_PATH', '')
os.environ['IDF_PATH'] = idf_path
os.environ['IDF_PYTHON_ENV_PATH'] = idf_python_env_path
os.environ['ESP_IDF_VERSION'] = idf_version

if not os.path.exists(virtualenv_python):
    die((f'ESP-IDF Python virtual environment "{virtualenv_python}" '
         f'not found. Please run the install script to set it up before '
         f'proceeding.'))

try:
    run([virtualenv_python, os.path.join(idf_path, 'tools', 'export_utils', 'activate_venv.py')] + sys.argv[1:], check=True, env=os.environ.copy())
except (OSError, SubprocessError) as e:
    die('\n'.join(['Activation script failed', str(e),
                   'To view detailed debug information, set ESP_IDF_EXPORT_DEBUG=1 and run the export script again.']))
