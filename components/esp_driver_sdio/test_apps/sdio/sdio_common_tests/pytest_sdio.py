# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import os.path
from typing import Tuple

import pytest
from pytest_embedded_idf import IdfDut


# Normal tests
def test_sdio_flow(dut:Tuple[IdfDut, IdfDut]) -> None:
    dut[1].expect('Press ENTER to see the list of tests')
    dut[1].write('[sdio]')
    dut[1].expect('test_sdio: slave ready')

    dut[0].expect('Press ENTER to see the list of tests')
    dut[0].write('[sdio]')

    dut[1].expect_unity_test_output()
    dut[0].expect_unity_test_output()


@pytest.mark.esp32c6
@pytest.mark.sdio_multidev_32_c6
@pytest.mark.parametrize('count', [2,], indirect=True)
@pytest.mark.parametrize('app_path, target', [
    pytest.param(
        f'{os.path.join(os.path.dirname(__file__), "host_sdmmc")}|{os.path.join(os.path.dirname(__file__), "sdio")}',
        'esp32|esp32c6'),
], indirect=True)
def test_sdio_esp32_esp32c6(dut:Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_flow(dut)


@pytest.mark.esp32
@pytest.mark.sdio_master_slave
@pytest.mark.parametrize('count', [2,], indirect=True)
@pytest.mark.parametrize('app_path, target', [
    pytest.param(
        f'{os.path.join(os.path.dirname(__file__), "host_sdmmc")}|{os.path.join(os.path.dirname(__file__), "sdio")}',
        'esp32|esp32'),
], indirect=True)
def test_sdio_esp32_esp32(dut:Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_flow(dut)


# From host speed tests
def test_sdio_speed_frhost_flow(dut:Tuple[IdfDut, IdfDut], expected_4b_speed:int, expected_1b_speed:int) -> None:
    dut[1].expect('Press ENTER to see the list of tests')
    dut[1].write('"SDIO_Slave: test from host (Performance)"')
    dut[1].expect('test_sdio: slave ready')

    dut[0].expect('Press ENTER to see the list of tests')
    dut[0].write('"SDIO_SDMMC: test from host (Performance)"')

    dut[0].expect('Probe using SD 4-bit')
    res = dut[0].expect(r'Throughput: compensated (\d+)')
    frhost_speed_4bit = res.group(1).decode('utf8')
    assert (int(frhost_speed_4bit) > expected_4b_speed)

    dut[0].expect('Probe using SD 1-bit')
    res = dut[0].expect(r'Throughput: compensated (\d+)')
    frhost_speed_1bit = res.group(1).decode('utf8')
    assert (int(frhost_speed_1bit) > expected_1b_speed)


@pytest.mark.esp32c6
@pytest.mark.sdio_multidev_32_c6
@pytest.mark.parametrize('count', [2,], indirect=True)
@pytest.mark.parametrize('app_path, target', [
    pytest.param(
        f'{os.path.join(os.path.dirname(__file__), "host_sdmmc")}|{os.path.join(os.path.dirname(__file__), "sdio")}',
        'esp32|esp32c6'),
], indirect=True)
def test_sdio_speed_frhost_esp32_esp32c6(dut:Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_speed_frhost_flow(dut, 10000, 4000)


@pytest.mark.esp32
@pytest.mark.sdio_master_slave
@pytest.mark.parametrize('count', [2,], indirect=True)
@pytest.mark.parametrize('app_path, target', [
    pytest.param(
        f'{os.path.join(os.path.dirname(__file__), "host_sdmmc")}|{os.path.join(os.path.dirname(__file__), "sdio")}',
        'esp32|esp32'),
], indirect=True)
def test_sdio_speed_frhost_esp32_esp32(dut:Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_speed_frhost_flow(dut, 12200, 4000)


# To host speed tests
def test_sdio_speed_tohost_flow(dut:Tuple[IdfDut, IdfDut], expected_4b_speed:int, expected_1b_speed:int) -> None:
    dut[1].expect('Press ENTER to see the list of tests')
    dut[1].write('"SDIO_Slave: test to host (Performance)"')
    dut[1].expect('test_sdio: slave ready')

    dut[0].expect('Press ENTER to see the list of tests')
    dut[0].write('"SDIO_SDMMC: test to host (Performance)"')

    dut[0].expect('Probe using SD 4-bit')
    res = dut[0].expect(r'Throughput: compensated (\d+)')
    tohost_speed_4bit = res.group(1).decode('utf8')
    assert (int(tohost_speed_4bit) > expected_4b_speed)

    dut[0].expect('Probe using SD 1-bit')
    res = dut[0].expect(r'Throughput: compensated (\d+)')
    tohost_speed_1bit = res.group(1).decode('utf8')
    assert (int(tohost_speed_1bit) > expected_1b_speed)


@pytest.mark.esp32c6
@pytest.mark.sdio_multidev_32_c6
@pytest.mark.parametrize('count', [2,], indirect=True)
@pytest.mark.parametrize('app_path, target', [
    pytest.param(
        f'{os.path.join(os.path.dirname(__file__), "host_sdmmc")}|{os.path.join(os.path.dirname(__file__), "sdio")}',
        'esp32|esp32c6'),
], indirect=True)
def test_sdio_speed_tohost_esp32_esp32c6(dut:Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_speed_tohost_flow(dut, 9000, 4000)


@pytest.mark.esp32
@pytest.mark.sdio_master_slave
@pytest.mark.parametrize('count', [2,], indirect=True)
@pytest.mark.parametrize('app_path, target', [
    pytest.param(
        f'{os.path.join(os.path.dirname(__file__), "host_sdmmc")}|{os.path.join(os.path.dirname(__file__), "sdio")}',
        'esp32|esp32'),
], indirect=True)
def test_sdio_speed_tohost_esp32_esp32(dut:Tuple[IdfDut, IdfDut]) -> None:
    test_sdio_speed_tohost_flow(dut, 12200, 4000)
