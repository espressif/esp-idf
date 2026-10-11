# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


def _expect_capture_output(dut: Dut) -> None:
    # The capture backend recorded every line written to /dev/console while it
    # was registered, and the example dumps that buffer back at the end. The
    # same lines appear on every target because they are read back from the
    # capture buffer, so the expectations are shared by host and chip targets.
    dut.expect('capture backend recorded')
    dut.expect('Written while the capture backend is an auxiliary')
    dut.expect('Written while the capture backend is the primary')
    dut.expect('Written after restoring the original primary')
    dut.expect('end of captured data')


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_esp_stdio_runtime_registration_host(dut: Dut) -> None:
    _expect_capture_output(dut)


@pytest.mark.generic
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_esp_stdio_runtime_registration_target(dut: Dut) -> None:
    _expect_capture_output(dut)
