# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pexpect
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.ethernet
def test_coap_example(dut: Dut) -> None:
    dut.expect('Loaded app from partition at offset', timeout=30)
    try:
        dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)
    except pexpect.exceptions.TIMEOUT:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')

    dut.expect('DNS lookup succeeded', timeout=30)
    dut.expect('Received', timeout=30)
    dut.expect(r'This server is using the Eclipse Californium \(Cf\) CoAP framework', timeout=30)
    dut.expect(r'published under EPL\+EDL: http://www\.eclipse\.org/californium/', timeout=30)
    dut.expect('Starting again!', timeout=30)
