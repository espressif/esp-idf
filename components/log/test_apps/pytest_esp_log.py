# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from elftools.elf.elffile import ELFFile
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


def check_no_log_handler_linked(dut: Dut) -> None:
    # This app never installs a handler. The weak reference must not pull in
    # registration, callback dispatch or their synchronization state.
    with open(dut.app.elf_file, 'rb') as binary:
        elf = ELFFile(binary)
        symbols = elf.get_section_by_name('.symtab')
        assert symbols is not None, 'The test requires an unstripped ELF'
        defined = {symbol.name for symbol in symbols.iter_symbols() if symbol['st_shndx'] != 'SHN_UNDEF'}
        unexpected = defined.intersection(
            {
                'esp_log_set_handler',
                'esp_log_handler_process',
                'esp_log_handler_process_v1',
                'esp_log_output_default',
            }
        )
        assert not unexpected, f'Unused log handler linked into the app: {sorted(unexpected)}'


@pytest.mark.require_elf
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'v2_constrained_env_safe',
        'v2_constrained_env_unsafe',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_log(dut: Dut) -> None:
    check_no_log_handler_linked(dut)
    dut.run_all_single_board_cases()
