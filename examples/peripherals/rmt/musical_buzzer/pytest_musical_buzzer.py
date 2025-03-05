# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'target', ['esp32s2', 'esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4'], indirect=['target']
)
def test_musical_buzzer_example(dut: Dut) -> None:
    dut.expect_exact('example: Create RMT TX channel')
    dut.expect_exact('example: Install musical score encoder')
    dut.expect_exact('example: Enable RMT TX channel')
    dut.expect_exact("example: Playing Beethoven's Ode to joy")
