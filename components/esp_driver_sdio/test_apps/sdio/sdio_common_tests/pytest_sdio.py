# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os.path
from typing import List
from typing import Tuple

import pytest
from pytest_embedded_idf import IdfDut


def parameter_expand(existing_parameters: List[List[str]], value_list: List[str]) -> List[List[str]]:
    ret = []
    for param in existing_parameters:
        ret.extend([param + [value] for value in value_list])

    return ret


esp32_32_param = [
    [
        f'{os.path.join(os.path.dirname(__file__), "host_sdmmc")}|{os.path.join(os.path.dirname(__file__), "sdio")}',
        'esp32|esp32',
    ]
]
esp32_c6_param = [
    [
        f'{os.path.join(os.path.dirname(__file__), "host_sdmmc")}|{os.path.join(os.path.dirname(__file__), "sdio")}',
        'esp32|esp32c6',
    ]
]

esp32p4_c5_param = [
    [
        f'{os.path.join(os.path.dirname(__file__), "host_sdmmc")}|{os.path.join(os.path.dirname(__file__), "sdio")}',
        'esp32p4|esp32c5',
    ]
]

esp32_param_default = [pytest.param(*param) for param in parameter_expand(esp32_32_param, ['default|default'])]
c6_param_default = [pytest.param(*param) for param in parameter_expand(esp32_c6_param, ['default|default'])]
c5_param_default = [pytest.param(*param) for param in parameter_expand(esp32p4_c5_param, ['esp32p4_esp32c5|default'])]

c6_param_retention = [pytest.param(*param) for param in parameter_expand(esp32_c6_param, ['default|sleep_retention'])]


# Normal tests
def test_sdio_flow(dut: Tuple[IdfDut, IdfDut]) -> None:
    dut[1].expect('Press ENTER to see the list of tests')
    dut[1].write('[sdio]')
    dut[1].expect('test_sdio: slave ready')

    dut[0].expect('Press ENTER to see the list of tests')
    dut[0].write('[sdio]')

    dut[1].expect_unity_test_output()
    dut[0].expect_unity_test_output()


@pytest.mark.sdio_multidev_32_c6
@pytest.mark.parametrize(
    'count',
    [
        2,
    ],
    indirect=True,
)
@pytest.mark.parametrize('app_path, target, config', c6_param_default, indirect=True)
def test_sdio_esp32_esp32c6(dut: Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_flow(dut)


@pytest.mark.sdio_master_slave
@pytest.mark.parametrize(
    'count',
    [
        2,
    ],
    indirect=True,
)
@pytest.mark.parametrize('app_path, target, config', esp32_param_default, indirect=True)
def test_sdio_esp32_esp32(dut: Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_flow(dut)


@pytest.mark.sdio_multidev_p4_c5
@pytest.mark.parametrize(
    'count',
    [
        2,
    ],
    indirect=True,
)
@pytest.mark.parametrize('app_path, target, config', c5_param_default, indirect=True)
def test_sdio_esp32p4_esp32c5(dut: Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_flow(dut)


# From host speed tests
def test_sdio_speed_frhost_flow(dut: Tuple[IdfDut, IdfDut], expected_4b_speed: int, expected_1b_speed: int) -> None:
    dut[1].expect('Press ENTER to see the list of tests')
    dut[1].write('"SDIO_Slave: test from host (Performance)"')
    dut[1].expect('test_sdio: slave ready')

    dut[0].expect('Press ENTER to see the list of tests')
    dut[0].write('"SDIO_SDMMC: test from host (Performance)"')

    dut[0].expect('Probe using SD 4-bit')
    res = dut[0].expect(r'Throughput: compensated (\d+)KB\/s')
    frhost_speed_4bit = res.group(1).decode('utf8')
    assert int(frhost_speed_4bit) > expected_4b_speed

    dut[0].expect('Probe using SD 1-bit')
    res = dut[0].expect(r'Throughput: compensated (\d+)KB\/s')
    frhost_speed_1bit = res.group(1).decode('utf8')
    assert int(frhost_speed_1bit) > expected_1b_speed


@pytest.mark.sdio_multidev_32_c6
@pytest.mark.parametrize(
    'count',
    [
        2,
    ],
    indirect=True,
)
@pytest.mark.parametrize('app_path, target, config', c6_param_default, indirect=True)
def test_sdio_speed_frhost_esp32_esp32c6(dut: Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_speed_frhost_flow(dut, 10000, 4000)


@pytest.mark.sdio_master_slave
@pytest.mark.parametrize(
    'count',
    [
        2,
    ],
    indirect=True,
)
@pytest.mark.parametrize('app_path, target, config', esp32_param_default, indirect=True)
def test_sdio_speed_frhost_esp32_esp32(dut: Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_speed_frhost_flow(dut, 12200, 4000)


@pytest.mark.sdio_multidev_p4_c5
@pytest.mark.parametrize(
    'count',
    [
        2,
    ],
    indirect=True,
)
@pytest.mark.parametrize('app_path, target, config', c5_param_default, indirect=True)
def test_sdio_speed_frhost_esp32p4_esp32c5(dut: Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_speed_frhost_flow(dut, 10000, 4000)


# To host speed tests
def test_sdio_speed_tohost_flow(dut: Tuple[IdfDut, IdfDut], expected_4b_speed: int, expected_1b_speed: int) -> None:
    dut[1].expect('Press ENTER to see the list of tests')
    dut[1].write('"SDIO_Slave: test to host (Performance)"')
    dut[1].expect('test_sdio: slave ready')

    dut[0].expect('Press ENTER to see the list of tests')
    dut[0].write('"SDIO_SDMMC: test to host (Performance)"')

    dut[0].expect('Probe using SD 4-bit')
    res = dut[0].expect(r'Throughput: compensated (\d+)KB\/s')
    tohost_speed_4bit = res.group(1).decode('utf8')
    assert int(tohost_speed_4bit) > expected_4b_speed

    dut[0].expect('Probe using SD 1-bit')
    res = dut[0].expect(r'Throughput: compensated (\d+)KB\/s')
    tohost_speed_1bit = res.group(1).decode('utf8')
    assert int(tohost_speed_1bit) > expected_1b_speed


@pytest.mark.sdio_multidev_32_c6
@pytest.mark.parametrize(
    'count',
    [
        2,
    ],
    indirect=True,
)
@pytest.mark.parametrize('app_path, target, config', c6_param_default, indirect=True)
def test_sdio_speed_tohost_esp32_esp32c6(dut: Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_speed_tohost_flow(dut, 9000, 4000)


@pytest.mark.sdio_master_slave
@pytest.mark.parametrize(
    'count',
    [
        2,
    ],
    indirect=True,
)
@pytest.mark.parametrize('app_path, target, config', esp32_param_default, indirect=True)
def test_sdio_speed_tohost_esp32_esp32(dut: Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_speed_tohost_flow(dut, 12200, 4000)


@pytest.mark.sdio_multidev_p4_c5
@pytest.mark.parametrize(
    'count',
    [
        2,
    ],
    indirect=True,
)
@pytest.mark.parametrize('app_path, target, config', c5_param_default, indirect=True)
def test_sdio_speed_tohost_esp32p4_esp32c5(dut: Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_speed_tohost_flow(dut, 8500, 4000)


# Retention tests
def test_sdio_retention(dut: Tuple[IdfDut, IdfDut]) -> None:
    dut[1].expect('Press ENTER to see the list of tests')
    dut[1].write('[sdio_retention]')
    dut[1].expect('test_sdio: slave ready')

    dut[0].expect('Press ENTER to see the list of tests')
    dut[0].write('[sdio_retention]')

    dut[1].expect_unity_test_output()
    dut[0].expect_unity_test_output()


@pytest.mark.sdio_multidev_32_c6
@pytest.mark.parametrize(
    'count',
    [
        2,
    ],
    indirect=True,
)
@pytest.mark.parametrize('app_path, target, config', c6_param_retention, indirect=True)
def test_sdio_retention_esp32_esp32c6(dut: Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_retention(dut)
