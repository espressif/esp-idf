# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# !/usr/bin/env python3
import pathlib
import time

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

CURRENT_DIR_LIGHT = str(pathlib.Path(__file__).parent / 'HA_on_off_light')
CURRENT_DIR_SWITCH = str(pathlib.Path(__file__).parent / 'HA_on_off_switch')
pytest_build_dir = CURRENT_DIR_LIGHT + '|' + CURRENT_DIR_SWITCH


@pytest.mark.zigbee_multi_dut
@pytest.mark.parametrize(
    ' count, app_path, erase_all',
    [
        (2, pytest_build_dir, 'y'),
    ],
    indirect=True,
)
# config Zigbee network
@idf_parametrize('target', ['esp32h2'], indirect=['target'])
def test_config_zigbee_network(dut: tuple[Dut, Dut]) -> None:
    light = dut[0]
    switch = dut[1]
    time.sleep(3)
    light_network = light.expect(
        r'ON_OFF_LIGHT: Formed network successfully: PAN ID\(0x([a-fA-F0-9]+), EXT: 0x([a-fA-F0-9]+)\)',
        timeout=30,
    )
    light_panid = light_network[1].decode()
    light_extpanid = light_network[2].decode()

    switch_network = switch.expect(
        r'ON_OFF_SWITCH: Joined network successfully: PAN ID\(0x([a-fA-F0-9]+), EXT: 0x([a-fA-F0-9]+)\)',
        timeout=30,
    )
    switch_panid = switch_network[1].decode()
    switch_extpanid = switch_network[2].decode()

    light.expect(r'New device commissioned or rejoined ?\(short: 0x([a-fA-F0-9]+)\)', timeout=30)

    assert switch_panid == light_panid
    assert switch_extpanid == light_extpanid
