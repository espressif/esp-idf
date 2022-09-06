# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded_idf.dut import IdfDut

# Note that support for Linux target console applications hasn't been implemented for pytest-embedded yet
# (https://github.com/espressif/pytest-embedded/issues/106)


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
def test_hello_world_linux_compatible(dut: IdfDut) -> None:
    dut.expect('Hello world!')
