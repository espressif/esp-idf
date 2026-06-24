# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'defaults',
        'always_skip',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_startup_time_example(dut: Dut) -> None:
    res = dut.expect(r'\((\d+)\) [^:]+: App started!')
    time = int(res[1])

    logging.info(f'[Performance][startup_time]: {time}')
