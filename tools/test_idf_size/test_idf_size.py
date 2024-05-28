#!/usr/bin/env python
# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import sys
from pathlib import Path
from shutil import copytree
from subprocess import PIPE
from subprocess import run
from subprocess import STDOUT
from tempfile import TemporaryDirectory
from typing import Any
from typing import Optional
from typing import Tuple
from typing import Union


IDF_PATH = Path(os.environ['IDF_PATH'])
IDF_PY_PATH = IDF_PATH / 'tools' / 'idf.py'
IDF_SIZE_PY_PATH = IDF_PATH / 'tools' / 'idf_size.py'
HELLO_WORLD_PATH = IDF_PATH / 'examples' / 'get-started' / 'hello_world'

PathLike = Union[str, Path]


def run_cmd(*cmd: PathLike, cwd: Optional[PathLike]=None, check: bool=True, text: bool=True) -> Tuple[int, str]:
    logging.info('running: {}'.format(' '.join([str(arg) for arg in cmd])))
    p = run(cmd, stdout=PIPE, stderr=STDOUT, cwd=cwd, check=check, text=text)
    return p.returncode, p.stdout


def run_idf_py(*args: PathLike, **kwargs: Any) -> Tuple[int, str]:
    return run_cmd(sys.executable, IDF_PY_PATH, *args, **kwargs)


def run_idf_size_py(*args: PathLike, **kwargs: Any) -> Tuple[int, str]:
    return run_cmd(sys.executable, IDF_SIZE_PY_PATH, *args, **kwargs)


def test_idf_size() -> None:
    # Simple test to make sure that the idf_size.py wrapper is compatible
    # with idf.py minimum required python version.
    logging.info('idf_size.py python compatibility check')
    run_idf_size_py('--help')


def test_idf_py_size_diff() -> None:
    # Test idf.py size with diff option, utilizing the same map file, as the focus
    # of the test lies solely on the option, not on the resulting output.
    logging.info('idf.py size --diff option test')
    tmpdir = TemporaryDirectory()
    app_path = Path(tmpdir.name) / 'app'
    copytree(HELLO_WORLD_PATH, app_path, symlinks=True)
    run_idf_py('fullclean', cwd=app_path)
    run_idf_py('build', cwd=app_path)
    run_idf_py('size', '--diff', '.', cwd=app_path)
    # The diff map file should be found automatically in project or project's build directory
    run_idf_py('size', '--diff', 'build', cwd=app_path)
    run_idf_py('size', '--diff', Path('build') / 'hello_world.map', cwd=app_path)
