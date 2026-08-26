# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

_EXPECTED = {
    'default': ('Blinky Development Board', 'func() from func_dev.c (Development) called.'),
    'prod1': ('Blinky Smart Light', 'func() from func_prod.c (Production) called.'),
    'prod2': ('Blinky Smart Switch', 'func() from func_prod.c (Production) called.'),
}


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default', 'prod1', 'prod2'], indirect=True)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_cmakev2_multi_config(dut: IdfDut, config: str) -> None:
    product_name, func_line = _EXPECTED[config]
    dut.expect_exact(f'This app is built for running on: {product_name}')
    dut.expect_exact(func_line)
    dut.expect_exact('See README.md for building and running other app configurations.')
