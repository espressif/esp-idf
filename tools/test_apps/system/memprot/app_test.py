#!/usr/bin/env python

import ttfw_idf
from tiny_test_fw import Utility


mem_test = [
    ['IRAM0_SRAM', 'WRX'],
    ['IRAM0_RTCFAST', 'WRX'],
    ['DRAM0_SRAM', 'WR'],
    ['DRAM0_RTCFAST', 'WR'],
    ['PERI1_RTCSLOW', 'WR'],
    ['PERI2_RTCSLOW_0', 'WRX'],
    ['PERI2_RTCSLOW_1', 'WRX']
]


@ttfw_idf.idf_custom_test(env_tag="Example_GENERIC", target="esp32s2", group="test-apps")
def test_memprot(env, extra_data):

    dut = env.get_dut("memprot", "tools/test_apps/system/memprot")
    dut.start_app()

    for i in mem_test:
        if 'R' in i[1]:
            dut.expect(i[0] + " read low: OK")
            dut.expect(i[0] + " read high: OK")
        if 'W' in i[1]:
            dut.expect(i[0] + " write low: OK")
            dut.expect(i[0] + " write high: OK")
        if 'X' in i[1]:
            dut.expect(i[0] + " exec low: OK")
            dut.expect(i[0] + " exec high: OK")

    Utility.console_log("Memprot test done")


if __name__ == '__main__':
    test_memprot()
