#!/usr/bin/env python

from typing import Any

import ttfw_idf


@ttfw_idf.idf_custom_test(env_tag='Example_GENERIC', group='test-apps', target=['esp32', 'esp32s3'])
def test_multicore_app_and_unicore_bootloader(env, _):  # type: (Any, Any) -> None
    dut = env.get_dut('unicore_bootloader', 'tools/test_apps/system/unicore_bootloader', app_config_name='multicore_cfg')
    dut.start_app()
    dut.expect('Multicore bootloader')
    dut.expect('Multicore app')
    dut.expect('App is running')
    env.close_dut(dut.name)

    dut = env.get_dut('unicore_bootloader', 'tools/test_apps/system/unicore_bootloader', app_config_name='unicore_cfg')
    dut.bootloader_flash()
    dut.expect('Unicore bootloader')
    dut.expect('Multicore app')
    dut.expect('App is running')


@ttfw_idf.idf_custom_test(env_tag='Example_GENERIC', group='test-apps', target=['esp32', 'esp32s3'])
def test_unicore_app_and_multicore_bootloader(env, _):  # type: (Any, Any) -> None
    dut = env.get_dut('unicore_bootloader', 'tools/test_apps/system/unicore_bootloader', app_config_name='unicore_cfg')
    dut.start_app()
    dut.expect('Unicore bootloader')
    dut.expect('Unicore app')
    dut.expect('App is running')
    env.close_dut(dut.name)

    dut = env.get_dut('unicore_bootloader', 'tools/test_apps/system/unicore_bootloader', app_config_name='multicore_cfg')
    dut.bootloader_flash()
    dut.expect('Multicore bootloader')
    dut.expect('Unicore app')
    dut.expect('App is running')


if __name__ == '__main__':
    test_multicore_app_and_unicore_bootloader()
    test_unicore_app_and_multicore_bootloader()
