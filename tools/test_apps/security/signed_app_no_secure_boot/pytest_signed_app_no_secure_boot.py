# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32c2', 'esp32c3'], indirect=['target'])
def test_examples_security_on_update_no_secure_boot(dut: Dut) -> None:
    dut.expect(
        "This app is not signed, but check signature on update is enabled in config. It won't be possible to verify any update.",
        timeout=10,
    )
