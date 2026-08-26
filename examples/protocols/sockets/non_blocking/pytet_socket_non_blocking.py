# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_examples_non_block_socket_localhost(dut: Dut) -> None:
    dut.expect(r'nonblocking-socket-client: Received: GET / HTTP/1.1', timeout=30)
