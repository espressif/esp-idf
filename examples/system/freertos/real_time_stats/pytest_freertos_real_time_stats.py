# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_freertos_real_time_stats(dut: Dut) -> None:

    for _ in range(0, 3):
        dut.expect_exact('Real time stats obtained')
