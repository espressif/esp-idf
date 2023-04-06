# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import re
import shutil
import stat
import subprocess
import sys
import textwrap
from pathlib import Path
from typing import List

import pytest
from _pytest.monkeypatch import MonkeyPatch
from test_build_system_helpers import IdfPyFunc, find_python, get_snapshot, replace_in_file, run_idf_py


def get_subdirs_absolute_paths(path: Path) -> List[str]:
    """
    Get a list of files with absolute path in a given `path` folder
    """
    return [
        '{}/{}'.format(dir_path, file_name)
        for dir_path, _, file_names in os.walk(path)
        for file_name in file_names
    ]


@pytest.mark.usefixtures('test_app_copy')
@pytest.mark.test_app_copy('examples/get-started/blink')
def test_compile_commands_json_updated_by_reconfigure(idf_py: IdfPyFunc) -> None:
    output = idf_py('reconfigure')
    assert 'Building ESP-IDF components for target esp32' in output.stdout
    snapshot_1 = get_snapshot(['build/compile_commands.json'])
    snapshot_2 = get_snapshot(['build/compile_commands.json'])
    snapshot_2.assert_same(snapshot_1)
    idf_py('reconfigure')
    snapshot_3 = get_snapshot(['build/compile_commands.json'])
    snapshot_3.assert_different(snapshot_2)


@pytest.mark.usefixtures('test_app_copy')
def test_of_test_app_copy(idf_py: IdfPyFunc) -> None:
    p = Path('main/idf_component.yml')
    p.write_text('syntax_error\n')
    try:
        with (pytest.raises(subprocess.CalledProcessError)) as exc_info:
            idf_py('reconfigure')
        assert 'ERROR: Unknown format of the manifest file:' in exc_info.value.stderr
    finally:
        p.unlink()


@pytest.mark.usefixtures('test_app_copy')
def test_hints_no_color_output_when_noninteractive(idf_py: IdfPyFunc) -> None:
    """Check that idf.py hints don't include color escape codes in non-interactive builds"""

    # make the build fail in such a way that idf.py shows a hint
    replace_in_file('main/build_test_app.c', '// placeholder_inside_main',
                    'esp_chip_info_t chip_info; esp_chip_info(&chip_info);')

    with (pytest.raises(subprocess.CalledProcessError)) as exc_info:
        idf_py('build')

    # Should not actually include a color escape sequence!
    # Change the assert to the correct value once the bug is fixed.
    assert '\x1b[0;33mHINT: esp_chip_info.h' in exc_info.value.stderr


@pytest.mark.usefixtures('test_app_copy')
def test_idf_copy(idf_copy: Path, idf_py: IdfPyFunc) -> None:
    # idf_copy is the temporary IDF copy.
    # For example, we can check if idf.py build can work without the .git directory:
    shutil.rmtree(idf_copy / '.git', ignore_errors=True)
    idf_py('build')


def test_idf_build_with_env_var_sdkconfig_defaults(
    test_app_copy: Path,
    idf_py: IdfPyFunc,
    monkeypatch: MonkeyPatch,
) -> None:
    with open(test_app_copy / 'sdkconfig.test', 'w') as fw:
        fw.write('CONFIG_BT_ENABLED=y')

    monkeypatch.setenv('SDKCONFIG_DEFAULTS', 'sdkconfig.test')
    idf_py('build')

    with open(test_app_copy / 'sdkconfig') as fr:
        assert 'CONFIG_BT_ENABLED=y' in fr.read()


@pytest.mark.usefixtures('test_app_copy')
@pytest.mark.test_app_copy('examples/system/efuse')
def test_efuse_symmary_cmake_functions(
    idf_py: IdfPyFunc,
    monkeypatch: MonkeyPatch
) -> None:
    monkeypatch.setenv('IDF_CI_BUILD', '1')
    output = idf_py('efuse-summary')
    assert 'FROM_CMAKE: MAC: 00:00:00:00:00:00' in output.stdout
    assert 'FROM_CMAKE: WR_DIS: 0' in output.stdout


def test_custom_build_folder(test_app_copy: Path, idf_py: IdfPyFunc) -> None:
    idf_py('-BBuiLDdiR', 'build')
    assert (test_app_copy / 'BuiLDdiR').is_dir()


def test_python_clean(idf_py: IdfPyFunc) -> None:
    logging.info('Cleaning Python bytecode')
    idf_path = Path(os.environ['IDF_PATH'])
    abs_paths = get_subdirs_absolute_paths(idf_path)
    abs_paths_suffix = [path for path in abs_paths if path.endswith(('.pyc', '.pyo'))]
    assert len(abs_paths_suffix) != 0
    idf_py('python-clean')
    abs_paths = get_subdirs_absolute_paths(idf_path)
    abs_paths_suffix = [path for path in abs_paths if path.endswith(('.pyc', '.pyo'))]
    assert len(abs_paths_suffix) == 0


@pytest.mark.usefixtures('test_app_copy')
def test_partition_table(idf_py: IdfPyFunc) -> None:
    logging.info('Displays partition table when executing target partition_table')
    output = idf_py('partition-table')
    assert re.search('# ESP-IDF.+Partition Table', output.stdout)


@pytest.mark.skipif(sys.platform == 'win32', reason='Windows does not support executing bash script')
def test_python_interpreter_unix(test_app_copy: Path) -> None:
    logging.info("Make sure idf.py never runs '/usr/bin/env python' or similar")
    env_dict = dict(**os.environ)
    python = find_python(env_dict['PATH'])
    (test_app_copy / 'python').write_text(textwrap.dedent("""#!/bin/sh
                                                          echo "idf.py has executed '/usr/bin/env python' or similar"
                                                          exit 1
                                                          """))
    st = os.stat(test_app_copy / 'python')
    # equivalent to 'chmod +x ./python'
    os.chmod((test_app_copy / 'python'), st.st_mode | stat.S_IEXEC)

    env_dict['PATH'] = str(test_app_copy) + os.pathsep + env_dict['PATH']
    # python is loaded from env:$PATH, but since false interpreter is provided there, python needs to be specified as argument
    # if idf.py is reconfigured during it's execution, it would load a false interpreter
    run_idf_py('reconfigure', env=env_dict, python=python)


@pytest.mark.skipif(sys.platform != 'win32', reason='Linux does not support executing .exe files')
def test_python_interpreter_win(test_app_copy: Path) -> None:
    logging.info('Make sure idf.py never runs false python interpreter')
    env_dict = dict(**os.environ)
    python = find_python(env_dict['PATH'])

    # on windows python interpreter has compiled code '.exe' format, so this false file can be empty
    (test_app_copy / 'python.exe').write_text('')
    env_dict['PATH'] = str(test_app_copy) + os.pathsep + env_dict['PATH']
    # python is loaded from env:$PATH, but since false interpreter is provided there, python needs to be specified as argument
    # if idf.py is reconfigured during it's execution, it would load a false interpreter
    run_idf_py('reconfigure', env=env_dict, python=python)
