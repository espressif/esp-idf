# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration # TODO: IDF-14422')
def test_lp_mailbox(dut: Dut) -> None:
    # Wait for LP core to be loaded and running
    dut.expect_exact('LP Mailbox initialized successfully')
    dut.expect_exact('Final sum: 2957')
