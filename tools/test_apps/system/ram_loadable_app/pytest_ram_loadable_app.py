# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32c61
@pytest.mark.generic
@pytest.mark.parametrize('config', ['pure_ram',], indirect=True,)
def test_pure_ram_loadable_app(dut: IdfDut) -> None:
    dut.expect('main_task: Calling app_main()', timeout=10)
    dut.expect('Time since boot: 3 seconds...', timeout=10)


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32c61
@pytest.mark.generic
@pytest.mark.parametrize('config', ['defaults',], indirect=True,)
def test_ram_loadable_app(dut: IdfDut) -> None:
    dut.expect('spi_flash: detected chip', timeout=10)
    dut.expect('Time since boot: 3 seconds...', timeout=30)


# Tests with ram_app runners
@pytest.mark.esp32p4
@pytest.mark.esp32c5
@pytest.mark.ram_app
@pytest.mark.parametrize('config', ['defaults',], indirect=True,)
def test_ram_loadable_app_with_ram_app_runner(dut: IdfDut) -> None:
    dut.expect('spi_flash: detected chip', timeout=10)
    dut.expect('Time since boot: 3 seconds...', timeout=30)


@pytest.mark.esp32p4
@pytest.mark.esp32c5
@pytest.mark.ram_app
@pytest.mark.parametrize('config', ['pure_ram',], indirect=True,)
def test_pure_ram_loadable_app_with_ram_app_runner(dut: IdfDut) -> None:
    dut.expect('main_task: Calling app_main()', timeout=10)
    dut.expect('Time since boot: 3 seconds...', timeout=10)
