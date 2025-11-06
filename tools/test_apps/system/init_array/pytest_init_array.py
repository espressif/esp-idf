# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets', 'preview_targets', 'linux'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration # TODO: IDF-14425')
def test_init_array(dut: Dut) -> None:
    dut.expect_exact('preinit_func')
    dut.expect_exact('init_prio_101')
    dut.expect_exact('init_prio_102')
    dut.expect_exact('init_prio_103')
    dut.expect_exact('foo')
    dut.expect_exact('bar')
    dut.expect_exact('app_main running')
