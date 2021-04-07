#!/usr/bin/env python

import glob
import os
import re

import ttfw_idf
from tiny_test_fw import Utility
from ttfw_idf.IDFDUT import ESP32DUT


@ttfw_idf.idf_custom_test(env_tag='Example_GENERIC', group='test-apps')
def test_startup(env, extra_data):
    config_files = glob.glob(os.path.join(os.path.dirname(__file__), 'sdkconfig.ci.*'))
    config_names = [os.path.basename(s).replace('sdkconfig.ci.', '') for s in config_files]
    for name in config_names:
        Utility.console_log("Checking config \"{}\"... ".format(name), end='')
        dut = env.get_dut('startup', 'tools/test_apps/system/startup', app_config_name=name)
        dut.start_app()

        if (name == 'single_core_variant' and isinstance(dut, ESP32DUT)):
            dut.allow_dut_exception = True
            dut.expect('Running on single core variant of a chip, but app is built with multi-core support.')
            dut.expect(re.compile(r'abort\(\) was called at PC [0-9xa-f]+ on core 0'))
        else:
            dut.expect('app_main running')

        env.close_dut(dut.name)
        Utility.console_log('done')


if __name__ == '__main__':
    test_startup()
