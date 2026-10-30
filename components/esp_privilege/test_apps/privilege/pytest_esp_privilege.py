# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

UMODE_TARGETS = soc_filtered_targets('SOC_RISCV_SV32_SUPPORTED == 1 and SOC_INT_CLIC_SUPPORTED == 1')


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default', 'release', 'unicore'], indirect=True)
@idf_parametrize('target', UMODE_TARGETS, indirect=['target'])
def test_esp_privilege(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='privilege')


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default', 'release', 'unicore'], indirect=True)
@idf_parametrize('target', UMODE_TARGETS, indirect=['target'])
@pytest.mark.parametrize(
    'case_name, reason',
    [
        ('M-mode illegal instruction still panics', 'Illegal instruction'),
        ('M-mode fault with nonzero scratch retains the machine stack', 'Illegal instruction'),
    ],
)
def test_esp_privilege_panic(dut: Dut, case_name: str, reason: str) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write(f'"{case_name}"')
    dut.expect(
        rf"Guru Meditation Error: Core\s+\d panic'ed \({reason}\)\. "
        r'Exception was unhandled\.'
    )
    dut.expect_exact('Rebooting...')
