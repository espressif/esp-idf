# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging

import pytest
from pytest_embedded import Dut

MEM_TEST_UNICORE = {
    'esp32s2': [
        ['IRAM0_SRAM', 'WRX'],
        ['IRAM0_RTCFAST', 'WRX'],
        ['DRAM0_SRAM', 'WR'],
        ['DRAM0_RTCFAST', 'WR'],
        ['PERI1_RTCSLOW', 'WR'],
        ['PERI2_RTCSLOW_0', 'WRX'],
        ['PERI2_RTCSLOW_1', 'WRX']
    ],
    'esp32c3': [
        ['IRAM0_SRAM', 'WRX'],
        ['DRAM0_SRAM', 'WR'],
        ['IRAM0_RTCFAST', 'WRX']
    ],
    'esp32s3': [
        ['IRAM0_SRAM (core 0)', 'WRX'],
        ['DRAM0_SRAM (core 0)', 'WR'],
        ['IRAM0_RTCFAST (core 0)', 'WRX']
    ],
}

MEM_TEST_MULTICORE = {
    'esp32s3': [
        ['IRAM0_SRAM (core 0)', 'WRX'],
        ['DRAM0_SRAM (core 0)', 'WR'],
        ['IRAM0_RTCFAST (core 0)', 'WRX'],
        ['IRAM0_SRAM (core 1)', 'WRX'],
        ['DRAM0_SRAM (core 1)', 'WR'],
        ['IRAM0_RTCFAST (core 1)', 'WRX']
    ]
}


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.generic
def test_sys_memprot(dut: Dut) -> None:

    current_target = dut.target

    unicore = dut.app.sdkconfig.get('FREERTOS_UNICORE')

    mem_test_cfg = MEM_TEST_UNICORE if unicore else MEM_TEST_MULTICORE

    logging.info(f'Test cfg: {current_target}')

    for i in mem_test_cfg[current_target]:
        if 'R' in i[1]:
            dut.expect_exact(i[0] + ' read low: OK')
            dut.expect_exact(i[0] + ' read high: OK')
        if 'W' in i[1]:
            dut.expect_exact(i[0] + ' write low: OK')
            dut.expect_exact(i[0] + ' write high: OK')
        if 'X' in i[1]:
            dut.expect_exact(i[0] + ' exec low: OK')
            dut.expect_exact(i[0] + ' exec high: OK')
