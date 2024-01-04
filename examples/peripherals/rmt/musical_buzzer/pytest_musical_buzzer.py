# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
def test_musical_buzzer_example(dut: Dut) -> None:
    dut.expect_exact('example: Create RMT TX channel')
    dut.expect_exact('example: Install musical score encoder')
    dut.expect_exact('example: Enable RMT TX channel')
    dut.expect_exact("example: Playing Beethoven's Ode to joy")
