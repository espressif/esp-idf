# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, app_path', [
        (2,
         f'{os.path.join(os.path.dirname(__file__), "softAP")}|{os.path.join(os.path.dirname(__file__), "station")}'),
    ], indirect=True
)
def test_wifi_getting_started(dut: Tuple[IdfDut, IdfDut]) -> None:
    softap = dut[0]
    station = dut[1]

    ssid = softap.app.sdkconfig.get('ESP_WIFI_SSID')
    password = softap.app.sdkconfig.get('ESP_WIFI_PASSWORD')
    assert station.app.sdkconfig.get('ESP_WIFI_SSID') == ssid
    assert station.app.sdkconfig.get('ESP_WIFI_PASSWORD') == password

    tag = 'wifi station'
    station.expect(f'{tag}: got ip:', timeout=60)
    station.expect(f'{tag}: connected to ap SSID:{ssid} password:{password}', timeout=60)
    softap.expect('station .+ join, AID=', timeout=60)


@pytest.mark.esp32c2
@pytest.mark.wifi_two_dut
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, config, baud, app_path', [
        (2, 'esp32c2_xtal26m', '74880',
         f'{os.path.join(os.path.dirname(__file__), "softAP")}|{os.path.join(os.path.dirname(__file__), "station")}'),
    ], indirect=True
)
def test_wifi_getting_started_esp32c2_xtal_26mhz(dut: Tuple[IdfDut, IdfDut]) -> None:
    softap = dut[0]
    station = dut[1]

    assert station.app.sdkconfig['ESP_WIFI_SOFTAP_SUPPORT'] is False
    ssid = softap.app.sdkconfig.get('ESP_WIFI_SSID')
    password = softap.app.sdkconfig.get('ESP_WIFI_PASSWORD')
    assert station.app.sdkconfig.get('ESP_WIFI_SSID') == ssid
    assert station.app.sdkconfig.get('ESP_WIFI_PASSWORD') == password

    tag = 'wifi station'
    station.expect(f'{tag}: got ip:', timeout=60)
    station.expect(f'{tag}: connected to ap SSID:{ssid} password:{password}', timeout=60)
    softap.expect('station .+ join, AID=', timeout=60)
