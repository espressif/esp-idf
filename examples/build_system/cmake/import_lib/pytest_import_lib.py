# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded_qemu.dut import QemuDut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.host_test
@pytest.mark.qemu
def test_pytest_host(dut: QemuDut) -> None:
    dut.expect_exact('Initializing the filesystem')
    dut.expect_exact('Read XML data:')
    dut.expect_exact('Parsed XML data:')
    dut.expect_exact('To: Tove')
    dut.expect_exact('From: Jani')
    dut.expect_exact('Heading: Reminder')
    dut.expect_exact('Body: Don\'t forget me this weekend!')
    dut.expect_exact('Example end', timeout=20)
