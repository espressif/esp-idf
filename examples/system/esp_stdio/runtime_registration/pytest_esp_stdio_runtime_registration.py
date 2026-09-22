# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_esp_stdio_runtime_registration(dut: Dut) -> None:
    # The capture backend recorded every line written to /dev/console while it
    # was registered, and the example dumps that buffer back at the end.
    dut.expect('capture backend recorded')
    dut.expect('Written while the capture backend is an auxiliary')
    dut.expect('Written while the capture backend is the primary')
    dut.expect('Written after restoring the original primary')
    dut.expect('end of captured data')
