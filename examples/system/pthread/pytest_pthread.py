# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@idf_parametrize(
    'target,markers',
    [
        ('esp32', pytest.mark.generic),
        ('esp32c3', pytest.mark.generic),
        ('linux', pytest.mark.host_test),
    ],
    indirect=['target'],
)
def test_pthread(dut: Dut) -> None:
    # Note: this test doesn't really confirm anything, except that threads are created
    # and stdout is not being corrupted by multiple threads printing ot it.
    dut.expect(r'Created thread 0x[\da-f]+')
    dut.expect(r'Created larger stack thread 0x[\da-f]+')
    dut.expect(r'Threads have exited')
    dut.expect(r'Created thread 0x[\da-f]+ with new default config')
    dut.expect('Thread has exited')
