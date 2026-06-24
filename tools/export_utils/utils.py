# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import os
from subprocess import run
from subprocess import SubprocessError
from typing import Any
from typing import Dict
from typing import List
from typing import Optional


class Config:
    """
    Config serves as global hodler for variables used across modules
    It holds also arguments from command line
    """
    def __init__(self) -> None:
        self.IDF_PATH = os.environ['IDF_PATH']
        self.IDF_PATH_OLD = os.environ['IDF_PATH_OLD']
        self.IDF_VERSION = os.environ['ESP_IDF_VERSION']
        self.IDF_PYTHON_ENV_PATH = os.environ['IDF_PYTHON_ENV_PATH']
        self.IDF_TOOLS_PY = os.path.join(self.IDF_PATH, 'tools', 'idf_tools.py')
        self.IDF_PY = os.path.join(self.IDF_PATH, 'tools', 'idf.py')
        self.ARGS: Optional[argparse.Namespace] = None


# Global variable instance
conf = Config()


def run_cmd(cmd: List[str], env: Optional[Dict[str, Any]]=None) -> str:
    new_env = os.environ.copy()
    if env is not None:
        new_env.update(env)

    cmd_str = '"{}"'.format(' '.join(cmd))
    try:
        p = run(cmd, env=new_env, text=True, capture_output=True)
    except (OSError, SubprocessError) as e:
        raise RuntimeError(f'Command {cmd_str} failed: {e}')

    stdout: str = p.stdout.strip()
    stderr: str = p.stderr.strip()
    if p.returncode:
        raise RuntimeError(f'Command {cmd_str} failed with error code {p.returncode}\n{stdout}\n{stderr}')

    return stdout
