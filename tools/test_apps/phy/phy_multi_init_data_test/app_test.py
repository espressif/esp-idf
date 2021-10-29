#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import glob
import os

import tiny_test_fw
import ttfw_idf
from tiny_test_fw import Utility


@ttfw_idf.idf_custom_test(env_tag='Example_GENERIC', group='test-apps')
def test_phy_multi_init_data_bin(env, _):
    # type: (tiny_test_fw.Env.Env, None) -> None
    config_files = glob.glob(os.path.join(os.path.dirname(__file__), 'sdkconfig.ci.*'))
    config_names = [os.path.basename(s).replace('sdkconfig.ci.', '') for s in config_files]
    for name in config_names:
        dut = env.get_dut('phy_multi_init_data_test', 'tools/test_apps/phy/phy_multi_init_data_test',app_config_name=name)
        dut.start_app()

        if 'CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN_EMBED' in dut.app.get_sdkconfig().keys():
            Utility.console_log('multi init data bin embed test')
            dut.expect('load embedded multi phy init data')
        else:
            Utility.console_log('multi init data bin test')
            dut.expect('Support multiple PHY init data bins')

        dut.expect('wifi_init finished')
        Utility.console_log('Test Success')


if __name__ == '__main__':
    test_phy_multi_init_data_bin()
