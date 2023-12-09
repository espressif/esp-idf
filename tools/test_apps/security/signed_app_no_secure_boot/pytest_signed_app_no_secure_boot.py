# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.generic
def test_examples_security_on_update_no_secure_boot(dut: Dut) -> None:
    dut.expect("This app is not signed, but check signature on update is enabled in config. It won't be possible to verify any update.", timeout=10)
