# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_linux_freertos_SMP(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('![ignore]')
    dut.expect('[0-9][0-9] Tests 0 Failures 0 Ignored', timeout=120)
