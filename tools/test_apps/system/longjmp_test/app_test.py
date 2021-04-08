#!/usr/bin/env python

import ttfw_idf
from tiny_test_fw import Utility


@ttfw_idf.idf_custom_test(env_tag='Example_GENERIC', target=['esp32', 'esp32s2'], group='test-apps')
def test_longjmp(env, _):

    dut = env.get_dut('longjmp_test', 'tools/test_apps/system/longjmp_test')
    dut.start_app()
    dut.expect('Test successful', 15)

    Utility.console_log('longjmp test done.')


if __name__ == '__main__':
    test_longjmp()
