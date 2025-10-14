# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
        'cache_safe',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32s3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4'], indirect=['target'])
def test_mcpwm(dut: Dut) -> None:
    dut.run_all_single_board_cases()
