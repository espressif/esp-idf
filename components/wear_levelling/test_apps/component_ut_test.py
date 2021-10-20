# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import glob
import os

import ttfw_idf
from tiny_test_fw import Utility


@ttfw_idf.idf_component_unit_test(env_tag='COMPONENT_UT_GENERIC', target=['esp32', 'esp32c3'])
def test_component_ut_wear_levelling(env, _):   # type: (ttfw_idf.TinyFW.Env, None) -> None
    # Get the names of all configs (sdkconfig.ci.* files)
    config_files = glob.glob(os.path.join(os.path.dirname(__file__), 'sdkconfig.ci.*'))
    config_names = [os.path.basename(s).replace('sdkconfig.ci.', '') for s in config_files]

    # Run test once with binaries built for each config
    for name in config_names:
        Utility.console_log("Checking config \"{}\"... ".format(name), end='')
        dut = env.get_dut('wear_levelling', 'components/wear_levelling/test_apps', app_config_name=name)
        dut.start_app()
        stdout = dut.expect('Tests finished', full_stdout=True, timeout=30)
        ttfw_idf.ComponentUTResult.parse_result(stdout)
        env.close_dut(dut.name)
        Utility.console_log('done')


if __name__ == '__main__':
    test_component_ut_wear_levelling()
