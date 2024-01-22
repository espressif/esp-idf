# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
def test_i2s_basic_example(dut: Dut) -> None:

    dut.expect(r'D \(([0-9]+)\) i2s_common: tx channel is registered on I2S0 successfully', timeout=5)
    dut.expect(r'D \(([0-9]+)\) i2s_common: rx channel is registered on I2S0 successfully', timeout=5)
    dut.expect(r'D \(([0-9]+)\) i2s_common: DMA malloc info: dma_desc_num = ([0-9]+), '
               r'dma_desc_buf_size = dma_frame_num \* slot_num \* data_bit_width = ([0-9]+)', timeout=5)
    dut.expect(r'D \(([0-9]+)\) i2s_std: Clock division info: \[sclk\] ([0-9]+) Hz '
               r'\[mdiv\] ([0-9]+) \[mclk\] ([0-9]+) Hz \[bdiv\] ([0-9]+) \[bclk\] ([0-9]+) Hz', timeout=5)
    dut.expect(r'D \(([0-9]+)\) i2s_std: The tx channel on I2S0 has been initialized to STD mode successfully', timeout=5)
    dut.expect(r'D \(([0-9]+)\) i2s_common: DMA malloc info: dma_desc_num = ([0-9]+), '
               r'dma_desc_buf_size = dma_frame_num \* slot_num \* data_bit_width = ([0-9]+)', timeout=5)
    dut.expect(r'D \(([0-9]+)\) i2s_std: Clock division info: \[sclk\] ([0-9]+) Hz '
               r'\[mdiv\] ([0-9]+) \[mclk\] ([0-9]+) Hz \[bdiv\] ([0-9]+) \[bclk\] ([0-9]+) Hz', timeout=5)
    dut.expect(r'D \(([0-9]+)\) i2s_std: The rx channel on I2S0 has been initialized to STD mode successfully', timeout=5)
    chan_enable_pattern = [
        r'D \(([0-9]+)\) i2s_common: i2s tx channel enabled',
        r'D \(([0-9]+)\) i2s_common: i2s rx channel enabled'
    ]
    dut.expect(chan_enable_pattern, timeout=5)
    dut.expect(chan_enable_pattern, timeout=5)
    dut.expect(r'Write Task: i2s write ([0-9]+) bytes', timeout=5)
    dut.expect(r'Read Task: i2s read ([0-9]+) bytes', timeout=5)
    dut.expect(r'-----------------------------------', timeout=5)
    dut.expect(r'\[0\] 12 \[1\] 34 \[2\] 56 \[3\] 78', timeout=10)
    dut.expect(r'\[4\] 9a \[5\] bc \[6\] de \[7\] f0', timeout=10)
