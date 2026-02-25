# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import pytest
from crc import Calculator
from crc import Configuration
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

EXPECT_TIMEOUT = 20


def calculate_crc8(data: str) -> int:
    config = Configuration(8, 0x07, init_value=0x00, final_xor_value=0x00, reverse_input=False, reverse_output=False)
    calculator = Calculator(config)
    return int(calculator.checksum(data.encode('utf-8')))


def calculate_crc16_ccitt(data: str) -> int:
    config = Configuration(
        16, 0x1021, init_value=0x0000, final_xor_value=0x0000, reverse_input=False, reverse_output=False
    )
    calculator = Calculator(config)
    return int(calculator.checksum(data.encode('utf-8')))


def calculate_crc32(data: str) -> int:
    config = Configuration(
        32, 0x04C11DB7, init_value=0xFFFFFFFF, final_xor_value=0xFFFFFFFF, reverse_input=True, reverse_output=True
    )
    calculator = Calculator(config)
    return int(calculator.checksum(data.encode('utf-8')))


def calculate_crc32_no_reflect(data: str) -> int:
    config = Configuration(
        32, 0x04C11DB7, init_value=0x00000000, final_xor_value=0x0000, reverse_input=False, reverse_output=False
    )
    calculator = Calculator(config)
    return int(calculator.checksum(data.encode('utf-8')))


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_GDMA_SUPPORT_CRC == 1'), indirect=['target'])
def test_async_crc_example(dut: IdfDut) -> None:
    dut.expect_exact('crc>', timeout=EXPECT_TIMEOUT)

    # Test CRC-8 (default)
    dut.write('crc "test"')
    expected_crc8 = calculate_crc8('test')
    dut.expect(f'CRC result: 0x{expected_crc8:02X}', timeout=EXPECT_TIMEOUT)

    # Test CRC-8 with explicit polynomial
    dut.write('crc --width 8 --poly 0x07 "test"')
    expected_crc8 = calculate_crc8('test')
    dut.expect(f'CRC result: 0x{expected_crc8:02X}', timeout=EXPECT_TIMEOUT)

    # Test CRC-16/CCITT
    dut.write('crc --width 16 --poly 0x1021 "test"')
    expected_crc16 = calculate_crc16_ccitt('test')
    dut.expect(f'CRC result: 0x{expected_crc16:04X}', timeout=EXPECT_TIMEOUT)

    # Test CRC-32 without reflection
    dut.write('crc --width 32 --poly 0x04C11DB7 --init 0 --xor 0 --reverse-input 0 --reverse-output 0 "test"')
    expected_crc32 = calculate_crc32_no_reflect('test')
    dut.expect(f'CRC result: 0x{expected_crc32:08X}', timeout=EXPECT_TIMEOUT)

    # Test CRC-32 with reflection (standard CRC-32)
    dut.write(
        'crc --width 32 --poly 0x04C11DB7 --init 0xFFFFFFFF --xor 0xFFFFFFFF '
        '--reverse-input 1 --reverse-output 1 "test"'
    )
    expected_crc32 = calculate_crc32('test')
    dut.expect(f'CRC result: 0x{expected_crc32:08X}', timeout=EXPECT_TIMEOUT)
