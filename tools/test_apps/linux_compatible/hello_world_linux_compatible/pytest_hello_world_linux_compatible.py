# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_hello_world_linux_compatible(dut: IdfDut) -> None:
    dut.expect('Hello world!')


@pytest.mark.linux
@pytest.mark.host_test
def test_hello_world_linux(dut: IdfDut) -> None:
    dut.expect('Hello world!')
