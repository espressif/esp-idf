# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0


import glob
import os
from typing import Any

import ttfw_idf
from tiny_test_fw import Utility


def test_func(env, dut):  # type: (Any, Any) -> None

    dut.start_app()
    dut.expect('success', timeout=60)
    env.close_dut(dut.name)


def test_loop(env, config_names):  # type: (Any, Any) -> None

    for name in config_names:
        Utility.console_log("Checking config \"{}\"... ".format(name), end='')
        dut = env.get_dut('flash_mmap', 'tools/test_apps/system/flash_mmap', app_config_name=name)
        test_func(env, dut)
        Utility.console_log('done')


@ttfw_idf.idf_custom_test(env_tag='MSPI_F4R8', target=['esp32s3'])
def test_mmap_esp32s3(env, _):  # type: (Any, Any) -> None

    config_files = glob.glob(os.path.join(os.path.dirname(__file__), 'sdkconfig.ci.esp32s3_f4r8*'))
    config_names = [os.path.basename(s).replace('sdkconfig.ci.', '') for s in config_files]
    test_loop(env, config_names)


@ttfw_idf.idf_custom_test(env_tag='Example_GENERIC', target=['esp32s2'])
def test_mmap_esp32s2(env, _):  # type: (Any, Any) -> None

    config_files = glob.glob(os.path.join(os.path.dirname(__file__), 'sdkconfig.ci.esp32s2_*'))
    config_names = [os.path.basename(s).replace('sdkconfig.ci.', '') for s in config_files]
    test_loop(env, config_names)


@ttfw_idf.idf_custom_test(env_tag='psram', target=['esp32'])
def test_mmap_esp32(env, _):  # type: (Any, Any) -> None

    config_files = glob.glob(os.path.join(os.path.dirname(__file__), 'sdkconfig.ci.esp32_*'))
    config_names = [os.path.basename(s).replace('sdkconfig.ci.', '') for s in config_files]
    test_loop(env, config_names)


if __name__ == '__main__':
    test_mmap_esp32s3()
    test_mmap_esp32s2()
    test_mmap_esp32()
