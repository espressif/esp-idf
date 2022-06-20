# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.generic
def test_i2s_pdm_example(dut: Dut) -> None:
    dut.expect(r'I2S PDM TX example start', timeout=5)
    dut.expect(r'---------------------------', timeout=5)
    dut.expect(r'D \(([0-9]+)\) i2s_common: tx channel is registered on I2S0 successfully', timeout=5)
    dut.expect(r'D \(([0-9]+)\) i2s_common: DMA malloc info: dma_desc_num = ([0-9]+), '
               r'dma_desc_buf_size = dma_frame_num \* slot_num \* data_bit_width = ([0-9]+)', timeout=5)
    dut.expect(r'D \(([0-9]+)\) i2s_pdm: Clock division info: \[sclk\] ([0-9]+) Hz '
               r'\[mdiv\] ([0-9]+) \[mclk\] ([0-9]+) Hz \[bdiv\] ([0-9]+) \[bclk\] ([0-9]+) Hz', timeout=5)
    dut.expect(r'D \(([0-9]+)\) i2s_pdm: The tx channel on I2S0 has been initialized to PDM TX mode successfully', timeout=5)
    dut.expect(r'D \(([0-9]+)\) i2s_common: i2s tx channel enabled', timeout=5)
    dut.expect(r'Playing bass `twinkle twinkle little star`', timeout=5)
