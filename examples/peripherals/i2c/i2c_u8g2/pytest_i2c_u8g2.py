# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

EXPECT_TIMEOUT = 10
ERROR_PATTERNS = [  # error patterns, indicates test failed
    r'error',
    r'fail',
]


def expect_success_or_fail(dut: Dut, success_pattern: str, error_context: str, timeout: int = EXPECT_TIMEOUT) -> None:
    """Expect success pattern, fail test if error pattern is detected"""
    # Build combined regex: success_pattern | error_pattern1 | error_pattern2 | ...
    combined_pattern = f'({success_pattern})|({"|".join(ERROR_PATTERNS)})'

    result = dut.expect(combined_pattern, timeout=timeout)

    if result.group(1):  # Matched success pattern (first group)
        return
    else:  # Matched error pattern
        pytest.fail(f'{error_context} failed, detected error: {result.group()}')


@pytest.mark.i2c_oled
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_i2c_u8g2(dut: Dut) -> None:
    expect_success_or_fail(dut, r'Starting U8G2 display demo program', 'Program startup')
    expect_success_or_fail(dut, r'I2C Configuration:', 'I2C configuration')
    expect_success_or_fail(dut, r'Display Configuration:', 'Display configuration')
    expect_success_or_fail(dut, r'Display initialization completed', 'Display initialization')
    expect_success_or_fail(dut, r'Demo cycle: 1', 'Demo cycle 1 startup')

    demo_functions = [
        ('Text Display', 'Text display demo'),
        ('Geometric Shapes', 'Geometric shapes demo'),
        ('Pixel Operations', 'Pixel operations demo'),
        ('Progress Bar', 'Progress bar demo'),
        ('Animation Effects', 'Animation effects demo'),
        ('Bitmap Display', 'Bitmap display demo'),
    ]

    for pattern, description in demo_functions:
        expect_success_or_fail(dut, pattern, description)

    expect_success_or_fail(dut, r'Demo cycle: 2', 'Demo cycle 2 startup')
