# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0


import glob
import os
from typing import Any

import ttfw_idf
from tiny_test_fw import Utility


def test_loop(env, config_names):  # type: (Any, Any) -> None

    for name in config_names:
        Utility.console_log("Checking config \"{}\"... ".format(name), end='')
        dut = env.get_dut('xip_from_psram', 'tools/test_apps/system/xip_from_psram', app_config_name=name)
        dut.start_app()
        dut.expect('Finish')
        env.close_dut(dut.name)
        Utility.console_log('done')


@ttfw_idf.idf_custom_test(env_tag='Example_GENERIC', target=['esp32s2'])
def test_xip_psram_esp32s2(env, _):  # type: (Any, Any) -> None

    config_files = glob.glob(os.path.join(os.path.dirname(__file__), 'sdkconfig.ci.esp32s2'))
    config_names = [os.path.basename(s).replace('sdkconfig.ci.', '') for s in config_files]
    test_loop(env, config_names)


@ttfw_idf.idf_custom_test(env_tag='MSPI_F4R8', target=['esp32s3'])
def test_xip_psram_esp32s3(env, _):  # type: (Any, Any) -> None

    config_files = glob.glob(os.path.join(os.path.dirname(__file__), 'sdkconfig.ci.esp32s3'))
    config_names = [os.path.basename(s).replace('sdkconfig.ci.', '') for s in config_files]
    test_loop(env, config_names)


if __name__ == '__main__':
    test_xip_psram_esp32s2()
    test_xip_psram_esp32s3()
