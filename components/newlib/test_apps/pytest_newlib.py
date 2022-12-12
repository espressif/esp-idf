# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.temp_skip_ci(targets=['esp32c6'], reason='c6 support TBD')
@pytest.mark.generic
def test_newlib(dut: Dut) -> None:
    dut.expect_unity_test_output()
