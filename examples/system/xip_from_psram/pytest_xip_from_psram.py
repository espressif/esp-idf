# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded.dut import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.esp32c5
@pytest.mark.esp32c61
@pytest.mark.generic
# in order to build the default sdkconfig(the CI won't build the sdkconfig.defaults if there is a sdkconfig.ci.xx)
@pytest.mark.parametrize(
    'config',
    [
        'generic',
    ],
    indirect=True,
)
def test_xip_from_psram_example_generic(dut: Dut) -> None:
    dut.expect_exact('found partition')

    res = dut.expect(r'callback\(in PSRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 12

    res = dut.expect(r'callback\(in IRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 12


@pytest.mark.esp32s3
@pytest.mark.MSPI_F4R8
@pytest.mark.parametrize(
    'config',
    [
        'esp32s3_f4r8',
    ],
    indirect=True,
)
def test_xip_from_psram_example_f4r8(dut: Dut) -> None:
    dut.expect_exact('found partition')

    res = dut.expect(r'callback\(in PSRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 12

    res = dut.expect(r'callback\(in IRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 12


@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32p4_200m',
    ],
    indirect=True,
)
def test_xip_from_psram_example_p4_200m(dut: Dut) -> None:
    dut.expect_exact('found partition')

    res = dut.expect(r'callback\(in PSRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 10

    res = dut.expect(r'callback\(in IRAM\) response time: (\d{1,3}) us')
    response_time = res.group(1).decode('utf8')
    assert float(response_time) <= 10
