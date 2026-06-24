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
IDF_DIAG_PY_PATH = IDF_PATH / 'tools' / 'idf_diag.py'
HELLO_WORLD_PATH = IDF_PATH / 'examples' / 'get-started' / 'hello_world'

PathLike = Union[str, Path]


def run_cmd(*cmd: PathLike, cwd: Optional[PathLike]=None, check: bool=True, text: bool=True) -> Tuple[int, str]:
    logging.info('running: {}'.format(' '.join([str(arg) for arg in cmd])))
    p = run(cmd, stdout=PIPE, stderr=STDOUT, cwd=cwd, check=check, text=text)
    return p.returncode, p.stdout


def run_idf_py(*args: PathLike, **kwargs: Any) -> Tuple[int, str]:
    return run_cmd(sys.executable, IDF_PY_PATH, *args, **kwargs)


def test_idf_diag() -> None:
    # Basic test, compile the hello_world example, generate a report directory, and archive it.

    # temporary directories
    tmpdir = TemporaryDirectory()
    app_path = Path(tmpdir.name) / 'app'
    report_path = Path(tmpdir.name) / 'report'

    # build hello world example
    logging.info('building testing hello_world example')
    copytree(HELLO_WORLD_PATH, app_path)
    run_idf_py('fullclean', cwd=app_path)
    run_idf_py('build', cwd=app_path)

    # create report
    logging.info('creating report')
    run_idf_py('diag', '--output', report_path, cwd=app_path)

    # archive report
    logging.info('creating report archive')
    run_idf_py('diag', '--zip', report_path)

    # list recipes
    logging.info('list recipes')
    run_idf_py('diag', '--list')

    # check recipes
    logging.info('check recipes')
    run_idf_py('diag', '--check')

    # check redaction
    logging.info('check redaction')
    idf_component_path = app_path / 'idf_component.yml'
    idf_component_path.write_text('https://username:password@github.com/username/repository.git')
    run_idf_py('diag', '--force', '--output', report_path, cwd=app_path)
    idf_component_path.unlink()
    with open(report_path / 'manager' / 'idf_component' / 'idf_component.yml', 'r') as f:
        data = f.read()
        assert 'https://[XXX]@github.com/username/repository.git' in data
