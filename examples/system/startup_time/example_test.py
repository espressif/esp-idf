from __future__ import print_function

import re

import ttfw_idf
from tiny_test_fw import TinyFW


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32s2', 'esp32c3'])
def test_startup_time_example(env, _):
    key = 'startup_time'
    dut = env.get_dut(key, 'examples/system/startup_time')
    dut.start_app()

    res = dut.expect(re.compile(r'\((\d+)\) [^:]+: App started!'))
    time = int(res[0])

    TinyFW.JunitReport.update_performance([(key, time)])


if __name__ == '__main__':
    test_startup_time_example()
