# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_section_macros(dut: Dut) -> None:
    dut.expect('SUCCESS: All 5 section entries verified.')
    dut.expect('SUCCESS: All 3 pointer section entries verified.')


@pytest.mark.host_test
@pytest.mark.macos
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_section_macros_macos(dut: Dut) -> None:
    dut.expect('SUCCESS: All 5 section entries verified.')
    dut.expect('SUCCESS: All 3 pointer section entries verified.')
