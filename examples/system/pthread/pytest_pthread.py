# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


# IDF-5054
@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.generic
def test_pthread(dut: Dut) -> None:

    # Note: this test doesn't really confirm anything, except that threads are created
    # and stdout is not being corrupted by multiple threads printing ot it.
    dut.expect(r'Created thread 0x[\da-f]+')
    dut.expect(r'Created larger stack thread 0x[\da-f]+')
    dut.expect(r'Threads have exited')
    dut.expect(r'Created thread 0x[\da-f]+ with new default config')
    dut.expect('Thread has exited')
