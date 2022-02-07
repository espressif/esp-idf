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
        dut = env.get_dut('flash_psram', 'tools/test_apps/system/flash_psram', app_config_name=name)
        dut.start_app()
        dut.expect('flash psram test success')
        env.close_dut(dut.name)
        Utility.console_log('done')


# For F8R8 board (Octal Flash and Octal PSRAM)
@ttfw_idf.idf_custom_test(env_tag='MSPI_F8R8', target=['esp32s3'])
def test_flash8_psram8(env, _):  # type: (Any, Any) -> None

    config_files = glob.glob(os.path.join(os.path.dirname(__file__), 'sdkconfig.ci.f8r8*'))
    config_names = [os.path.basename(s).replace('sdkconfig.ci.', '') for s in config_files]
    test_loop(env, config_names)


# For F4R8 board (Quad Flash and Octal PSRAM)
@ttfw_idf.idf_custom_test(env_tag='MSPI_F4R8', target=['esp32s3'])
def test_flash4_psram8(env, _):  # type: (Any, Any) -> None

    config_files = glob.glob(os.path.join(os.path.dirname(__file__), 'sdkconfig.ci.f4r8*'))
    config_names = [os.path.basename(s).replace('sdkconfig.ci.', '') for s in config_files]
    test_loop(env, config_names)


# For F4R4 board (Quad Flash and Quad PSRAM)
@ttfw_idf.idf_custom_test(env_tag='MSPI_F4R4', target=['esp32s3'])
def test_flash4_psram4(env, _):  # type: (Any, Any) -> None

    config_files = glob.glob(os.path.join(os.path.dirname(__file__), 'sdkconfig.ci.f4r4*'))
    config_names = [os.path.basename(s).replace('sdkconfig.ci.', '') for s in config_files]
    test_loop(env, config_names)


if __name__ == '__main__':
    test_flash8_psram8()
    test_flash4_psram8()
    test_flash4_psram4()
