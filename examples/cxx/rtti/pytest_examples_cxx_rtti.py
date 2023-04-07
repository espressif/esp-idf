# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_cpp_rtti_example(dut: IdfDut) -> None:
    dut.expect_exact('Type name of std::cout is: std::ostream')
    dut.expect_exact('Type name of std::cin is: std::istream')
    dut.expect_exact('Type of app_main is: void ()')
    dut.expect_exact('Type name of a lambda function is: app_main::{lambda(int, int)#1}')

    dut.expect_exact('dynamic_cast<DerivedA*>(obj)=0')
    dut.expect_exact('dynamic_cast<DerivedB*>(obj)=0x')
    dut.expect_exact('dynamic_cast<DerivedB*>(obj)=0')
    dut.expect_exact('dynamic_cast<DerivedA*>(obj)=0x')

    dut.expect_exact('Example finished.')
