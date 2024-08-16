# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# This test checks the behavior of reproducible builds option.
import logging
import os
import shutil
import subprocess
from pathlib import Path

import pytest
from test_build_system_helpers import append_to_file
from test_build_system_helpers import bin_files_differ
from test_build_system_helpers import BOOTLOADER_BINS
from test_build_system_helpers import IdfPyFunc


@pytest.mark.parametrize(
    'app_name', [
        pytest.param('blink', marks=[pytest.mark.test_app_copy('examples/get-started/blink')]),
        pytest.param('blecent', marks=[pytest.mark.test_app_copy('examples/bluetooth/nimble/blecent')]),
    ]
)
def test_reproducible_builds(app_name: str, idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    append_to_file(test_app_copy / 'sdkconfig', 'CONFIG_APP_REPRODUCIBLE_BUILD=y')
    build_first = test_app_copy / 'build_first'
    build_second = test_app_copy / 'build_second'

    logging.info(f'Building in {build_first} directory')
    idf_py('-B', str(build_first), 'build')

    elf_file = build_first / f'{app_name}.elf'
    logging.info(f'Checking that various paths are not included in the ELF file')
    strings_output = subprocess.check_output(
        ['xtensa-esp32-elf-strings', str(elf_file)],
        encoding='utf-8'
    )
    idf_path = os.environ['IDF_PATH']
    assert str(idf_path) not in strings_output, f'{idf_path} found in {elf_file}'
    assert str(test_app_copy) not in strings_output, f'{test_app_copy} found in {elf_file}'
    compiler_path = shutil.which('xtensa-esp32-elf-gcc')
    assert compiler_path is not None, 'failed to determine compiler path'
    toolchain_path = Path(compiler_path).parent.parent
    assert str(toolchain_path) not in strings_output, f'{toolchain_path} found in {elf_file}'

    logging.info(f'Building in {build_second} directory')
    idf_py('-B', str(build_second), 'build')

    logging.info(f'Comparing build artifacts')
    artifacts_to_check = [
        f'build/{app_name}.map',
        f'build/{app_name}.elf',
        f'build/{app_name}.bin',
    ] + BOOTLOADER_BINS

    for artifact in artifacts_to_check:
        path_first = artifact.replace('build/', f'{build_first}/')
        path_second = artifact.replace('build/', f'{build_second}/')

        assert not bin_files_differ(path_first, path_second), f'{path_first} and {path_second} differ'

    logging.info(f'Checking that GDB works with CONFIG_APP_REPRODUCIBLE_BUILD=y')
    gdb_output = subprocess.check_output([
        'xtensa-esp32-elf-gdb',
        '--batch', '--quiet',
        '-x', f'{build_first}/prefix_map_gdbinit',
        '-ex', 'set logging enabled',
        '-ex', 'set pagination off',
        '-ex', 'list app_main',
        str(elf_file)
    ], encoding='utf-8', stderr=subprocess.STDOUT, cwd=str(build_first))

    assert 'No such file or directory' not in gdb_output, f'GDB failed to find app_main in {elf_file}:\n{gdb_output}'
