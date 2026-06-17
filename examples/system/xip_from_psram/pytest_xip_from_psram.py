# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded.dut import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
# in order to build the default sdkconfig(the CI won't build the sdkconfig.defaults if there is a sdkconfig.ci.xx)
@pytest.mark.parametrize(
    'config',
    [
        'generic',
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_SPIRAM_XIP_SUPPORTED == 1'), indirect=['target'])
def test_xip_from_psram_example_generic(dut: Dut) -> None:
    dut.expect_exact('found partition')

    res = dut.expect(r'callback\(in PSRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 12

    res = dut.expect(r'callback\(in IRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 12


@pytest.mark.MSPI_F4R8
@pytest.mark.parametrize(
    'config',
    [
        'esp32s3_f4r8',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_xip_from_psram_example_f4r8(dut: Dut) -> None:
    dut.expect_exact('found partition')

    res = dut.expect(r'callback\(in PSRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 12

    res = dut.expect(r'callback\(in IRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 12


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32p4_200m',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_xip_from_psram_example_p4_200m(dut: Dut) -> None:
    dut.expect_exact('found partition')

    res = dut.expect(r'callback\(in PSRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 10

    res = dut.expect(r'callback\(in IRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 10


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32s31_250m',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s31'], indirect=['target'])
def test_xip_from_psram_example_s31(dut: Dut) -> None:
    dut.expect_exact('found partition')

    res = dut.expect(r'callback\(in PSRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 5

    res = dut.expect(r'callback\(in IRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 5
