# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import subprocess
from os import path

import pytest
import yaml
from pytest_embedded import Dut


def validate_sbom(dut: Dut) -> None:
    dirname = path.dirname(path.abspath(__file__))
    sbom_file = path.join(path.dirname(path.dirname(dirname)), 'sbom.yml')
    gcc_input_file = path.join(dirname, 'test_sbom', 'newlib_version.c')
    gcc = 'riscv32-esp-elf-gcc'
    if dut.target in dut.XTENSA_TARGETS:
        gcc = f'xtensa-{dut.target}-elf-gcc'
    gcc_process = subprocess.run(f'{gcc} -E {gcc_input_file}',
                                 shell=True,
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.PIPE,
                                 text=True,
                                 check=True)
    output_lines = gcc_process.stdout.splitlines()
    assert output_lines, 'Can not get newlib version'
    toolchain_newlib_version = output_lines[-1].replace(' ', '.')
    with open(sbom_file, 'r', encoding='utf-8') as yaml_file:
        sbom_newlib_version = yaml.safe_load(yaml_file).get('version')
        assert sbom_newlib_version, 'Can not get newlib version from sbom.yml'
        assert toolchain_newlib_version == sbom_newlib_version, 'toolchain_newlib_version != sbom_newlib_version'


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        pytest.param('default', marks=[pytest.mark.supported_targets]),
        pytest.param('options', marks=[pytest.mark.supported_targets]),
        pytest.param('single_core_esp32', marks=[pytest.mark.esp32]),
        pytest.param('psram_esp32', marks=[pytest.mark.esp32]),
        pytest.param('release_esp32', marks=[pytest.mark.esp32]),
        pytest.param('release_esp32c2', marks=[pytest.mark.esp32c2]),
    ],
    indirect=True
)
def test_newlib(dut: Dut) -> None:
    validate_sbom(dut)
    dut.run_all_single_board_cases()
