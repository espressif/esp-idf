# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='esp32p4 support TBD')  # TODO: IDF-8994
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'pure_ram',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_pure_ram_loadable_app(dut: IdfDut) -> None:
    dut.expect('main_task: Calling app_main()', timeout=10)
    dut.expect('Time since boot: 3 seconds...', timeout=10)


@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='esp32p4 support TBD')  # TODO: IDF-8994
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'defaults',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_ram_loadable_app(dut: IdfDut) -> None:
    dut.expect('spi_flash: detected chip', timeout=10)
    dut.expect('Time since boot: 3 seconds...', timeout=30)
