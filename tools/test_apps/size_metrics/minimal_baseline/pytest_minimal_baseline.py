# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default'], indirect=True)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_minimal_baseline_boots(dut: IdfDut) -> None:
    dut.expect_exact('minimal_baseline app_main started', timeout=30)
