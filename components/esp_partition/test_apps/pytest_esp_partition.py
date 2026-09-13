# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.flaky(reruns=2, reruns_delay=5)
@idf_parametrize(
    'config,target',
    [
        *(('default', target) for target in ['esp32', 'esp32c3']),
        *(('md5_tolerate_missing', target) for target in ['esp32', 'esp32c3']),
    ],
    indirect=['config', 'target'],
)
def test_esp_partition(dut: Dut) -> None:
    dut.expect_unity_test_output()
