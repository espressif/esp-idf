# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.qemu
@pytest.mark.parametrize('config', ['secure_update_with_fe'], indirect=True)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_examples_secure_update_with_fe_qemu(dut: Dut) -> None:
    dut.expect('Example for secured signed with no secure boot', timeout=60)
