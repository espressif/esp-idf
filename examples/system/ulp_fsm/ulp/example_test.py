from __future__ import unicode_literals

import re
import time

import ttfw_idf
from tiny_test_fw import Utility


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32'])
def test_examples_ulp(env, extra_data):

    dut = env.get_dut('ulp', 'examples/system/ulp_fsm/ulp')
    dut.start_app()

    dut.expect_all('Not ULP wakeup, initializing ULP',
                   'Entering deep sleep',
                   timeout=30)

    def generate_gpio0_events():
        for _ in range(5):
            dut.port_inst.setDTR(True)  # Pulling GPIO0 low using DTR
            time.sleep(0.25)
            dut.port_inst.setDTR(False)
            time.sleep(0.25)

    nvs_value = None
    for _ in range(5):
        generate_gpio0_events()
        dut.expect('ULP wakeup, saving pulse count', timeout=5)
        Utility.console_log('Woke up...')
        init_count = int(dut.expect(re.compile(r'Read pulse count from NVS:\s+(\d+)'), timeout=5)[0], 10)
        assert nvs_value in (init_count, None), ('Read count is {} and previously written value is {}'
                                                 ''.format(init_count, nvs_value))
        inc = int(dut.expect(re.compile(r'Pulse count from ULP:\s+(\d+)'), timeout=5)[0], 10)
        assert inc in (5, 6), 'pulse count is {}'.format(inc)
        new_count = int(dut.expect(re.compile(r'Wrote updated pulse count to NVS:\s+(\d+)'), timeout=5)[0], 10)
        assert init_count + inc == new_count, '{} + {} != {}'.format(init_count, inc, new_count)
        nvs_value = new_count
        Utility.console_log('Pulse count written to NVS: {}. Entering deep sleep...'.format(nvs_value))
        dut.expect('Entering deep sleep', timeout=5)


if __name__ == '__main__':
    test_examples_ulp()
