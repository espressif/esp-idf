# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.flaky(reruns=2, reruns_delay=5)
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_spiffs_generic(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('')
    dut.expect_exact('Enter test for running.')
    dut.write('*')
    dut.expect_unity_test_output(timeout=120)


@pytest.mark.quad_psram
@pytest.mark.flaky(reruns=2, reruns_delay=5)
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_spiffs_psram(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('')
    dut.expect_exact('Enter test for running.')
    dut.write('*')
    dut.expect_unity_test_output(timeout=120)
