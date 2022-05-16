#!/usr/bin/env python

import ttfw_idf
from tiny_test_fw import Utility

MEM_TEST_S2 = [
    ['IRAM0_SRAM', 'WRX'],
    ['IRAM0_RTCFAST', 'WRX'],
    ['DRAM0_SRAM', 'WR'],
    ['DRAM0_RTCFAST', 'WR'],
    ['PERI1_RTCSLOW', 'WR'],
    ['PERI2_RTCSLOW_0', 'WRX'],
    ['PERI2_RTCSLOW_1', 'WRX']
]

MEM_TEST_C3 = [
    ['IRAM0_SRAM', 'WRX'],
    ['DRAM0_SRAM', 'WR'],
    ['IRAM0_RTCFAST', 'WRX'],
]


@ttfw_idf.idf_custom_test(env_tag='Example_GENERIC', target=['esp32c3', 'esp32s2'], group='test-apps')
def test_memprot(env, extra_data):

    dut = env.get_dut('memprot', 'tools/test_apps/system/memprot')
    dut.start_app()

    mem_test_cfg = []
    current_target = dut.app.get_sdkconfig()['CONFIG_IDF_TARGET'].replace('"','').lower()

    if current_target == 'esp32c3':
        mem_test_cfg = MEM_TEST_C3
    elif current_target == 'esp32s2':
        mem_test_cfg = MEM_TEST_S2

    Utility.console_log('Test cfg: ' + current_target)

    for i in mem_test_cfg:
        if 'R' in i[1]:
            dut.expect(i[0] + ' read low: OK')
            dut.expect(i[0] + ' read high: OK')
        if 'W' in i[1]:
            dut.expect(i[0] + ' write low: OK')
            dut.expect(i[0] + ' write high: OK')
        if 'X' in i[1]:
            dut.expect(i[0] + ' exec low: OK')
            dut.expect(i[0] + ' exec high: OK')

    Utility.console_log('Memprot test done')


if __name__ == '__main__':
    test_memprot()
