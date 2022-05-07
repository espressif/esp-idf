# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import os.path
from typing import Tuple

import pytest
from pytest_embedded_idf.dut import IdfDut

# @pytest.mark.supported_targets
# This test should support all targets, even between different target types
# For now our CI only support multi dut with esp32
# If you want to enable different target type, please use the following param
# @pytest.mark.parametrize(
#     'count, app_path, target', [
#         (2,
#          f'{os.path.join(os.path.dirname(__file__), "softAP")}|{os.path.join(os.path.dirname(__file__), "station")}',
#          'esp32|esp32s2'),
#     ],
#     indirect=True,
# )


@pytest.mark.esp32
@pytest.mark.multi_dut_generic
@pytest.mark.parametrize(
    'count, app_path', [
        (2,
         f'{os.path.join(os.path.dirname(__file__), "softAP")}|{os.path.join(os.path.dirname(__file__), "station")}'),
    ], indirect=True
)
def test_wifi_getting_started(dut: Tuple[IdfDut, IdfDut]) -> None:
    softap = dut[0]
    station = dut[1]

    ssid = 'myssid'
    password = 'mypassword'
    tag = 'wifi station'

    station.expect(f'{tag}: got ip:', timeout=60)
    station.expect(f'{tag}: connected to ap SSID:{ssid} password:{password}', timeout=60)
    softap.expect('station .+ join, AID=', timeout=60)
