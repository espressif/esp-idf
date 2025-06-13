# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

# ---------------- Pytest build parameters ----------------

SOC_SYS_APM_SUPPORTED = ['esp32c6', 'esp32h2', 'esp32c5', 'esp32c61']
SOC_PERI_APM_SUPPORTED = ['esp32c5']

CONFIG_SYS_APM = [
    # 'config, target, markers',
    ('default', target, (pytest.mark.generic,))
    for target in SOC_SYS_APM_SUPPORTED
]

CONFIG_PERI_APM = [
    # 'config, target, markers',
    ('default', target, (pytest.mark.generic,))
    for target in SOC_PERI_APM_SUPPORTED
]

# ---------------- TEE default tests ----------------


@idf_parametrize(
    'config, target, markers',
    CONFIG_SYS_APM,
    indirect=['config', 'target'],
)
def test_tee_sys_apm(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='SYS_APM')


@idf_parametrize(
    'config, target, markers',
    CONFIG_PERI_APM,
    indirect=['config', 'target'],
)
def test_tee_peri_apm(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='PERI_APM')
