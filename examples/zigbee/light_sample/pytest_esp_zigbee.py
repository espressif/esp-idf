# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# !/usr/bin/env python3

import pathlib
import time
from typing import Tuple

import pytest
from pytest_embedded import Dut

CURRENT_DIR_LIGHT = str(pathlib.Path(__file__).parent / 'HA_on_off_light')
CURRENT_DIR_SWITCH = str(pathlib.Path(__file__).parent / 'HA_on_off_switch')
pytest_build_dir = CURRENT_DIR_LIGHT + '|' + CURRENT_DIR_SWITCH


@pytest.mark.esp32h2
@pytest.mark.zigbee_multi_dut
@pytest.mark.parametrize(
    ' count, app_path, erase_all', [
        (2, pytest_build_dir, 'y'),
    ],
    indirect=True,
)
# config Zigbee network
def test_config_zigbee_network(dut:Tuple[Dut, Dut]) -> None:
    light = dut[0]
    switch = dut[1]
    time.sleep(3)
    switch.expect('ESP_ZB_ON_OFF_SWITCH: Formed network successfully',timeout=30)
    # get the switch extpanid
    switch_node_expanid = switch.expect(r'Extended PAN ID: (([a-z0-9]{2}:?){8})',timeout=3)[1].decode()
    switch_node_expanid = switch_node_expanid.replace(':','')
    # get the switch panid
    switch_node_panid = switch.expect(r'PAN ID: 0x([a-z0-9]+:?)',timeout=2)[1].decode()
    # new device commissioned successfully
    switch.expect(r'New device commissioned or rejoined \(short: 0x([a-z0-9]+)[^a-z0-9]',timeout=30)[1].decode()
    # get the light node extpanid
    light.expect('ESP_ZB_ON_OFF_LIGHT: Joined network successfully',timeout=20)
    light_node_expanid = light.expect(r'Extended PAN ID: (([a-z0-9]{2}:?){8})',timeout=3)[1].decode()
    light_node_expanid = light_node_expanid.replace(':','')
    # get the light panid
    light_node_panid = light.expect(r'PAN ID: 0x([a-z0-9]+:?)',timeout=2)[1].decode()
    # make sure the light node join the network that switch node formed (same expanid)
    if ((light_node_expanid != switch_node_expanid) or (light_node_panid != switch_node_panid)):
        assert False
