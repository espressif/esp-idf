# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_examples_cpp_exceptions(dut: IdfDut) -> None:
    lines = [
        'app_main starting',
        'In constructor, arg=42',
        'In constructor, arg=0',
        'In destructor, m_arg=42',
        'Exception caught: Exception in constructor',
        'app_main done',
    ]
    for line in lines:
        dut.expect(line, timeout=2)
