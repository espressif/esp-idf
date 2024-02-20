# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='esp32p4 support TBD')  # TODO: IDF-8994
@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize('config', ['pure_ram',], indirect=True,)
def test_pure_ram_loadable_app(dut: IdfDut) -> None:
    dut.expect('main_task: Calling app_main()', timeout=10)
    dut.expect('Time since boot: 3 seconds...', timeout=10)


@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='esp32p4 support TBD')  # TODO: IDF-8994
@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize('config', ['defaults',], indirect=True,)
def test_ram_loadable_app(dut: IdfDut) -> None:
    dut.expect('spi_flash: detected chip', timeout=10)
    dut.expect('Time since boot: 3 seconds...', timeout=30)
