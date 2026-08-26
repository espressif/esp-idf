# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['default'],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_regi2c(dut: Dut) -> None:
    dut.run_all_single_board_cases()
