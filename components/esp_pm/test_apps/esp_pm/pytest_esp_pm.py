# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import re

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

# FreeRTOS idle hooks are dispatched via weak <-> strong symbols: weak defaults
# live in esp_system/freertos_hooks.c, strong implementations in esp_pm/pm_impl.c.
# When CONFIG_PM_ENABLE is set, the link must resolve to the strong (pm_impl.o)
# definitions. Verified post-build via the app .map cross-reference section,
# e.g. "esp_pm_impl_idle_hook  esp-idf/esp_pm/libesp_pm.a(pm_impl.c.obj)".
_PM_HOOK_SYMBOLS = ('esp_pm_impl_idle_hook', 'esp_pm_impl_waiti')
_PM_TICKLESS_SYMBOLS = ('esp_pm_impl_tickless_waiti',)


def _map_symbol_providers(map_path: str, symbol: str) -> list[str]:
    providers: list[str] = []
    pat = re.compile(r'^\s*' + re.escape(symbol) + r'\s+(.+)$')
    with open(map_path) as f:
        for line in f:
            m = pat.match(line.rstrip())
            if m:
                providers.append(m.group(1).strip())
    return providers


def _assert_strong_pm_hooks(dut: Dut) -> None:
    if not dut.app.sdkconfig.get('PM_ENABLE'):
        pytest.skip('CONFIG_PM_ENABLE not set, weak freertos_hooks defaults expected')
    map_file = os.path.splitext(dut.app.elf_file)[0] + '.map'
    assert os.path.isfile(map_file), f'map file not found: {map_file}'
    symbols = list(_PM_HOOK_SYMBOLS)
    if dut.app.sdkconfig.get('PM_TICKLESS_IDLE_WAITI'):
        symbols += list(_PM_TICKLESS_SYMBOLS)
    for sym in symbols:
        providers = _map_symbol_providers(map_file, sym)
        assert providers, f'{sym} not found in {map_file}'
        assert any('pm_impl' in p for p in providers), f'{sym} not provided by pm_impl.o (weak fallback?): {providers}'
        assert not any('freertos_hooks' in p for p in providers), (
            f'{sym} wrongly provided by freertos_hooks.o: {providers}'
        )


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'slp_iram_opt',
        'limits',
        'options',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='bringup on this module is not done')
def test_esp_pm(dut: Dut) -> None:
    _assert_strong_pm_hooks(dut)
    dut.run_all_single_board_cases()


# psram attr tests with xip_psram
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['pm_xip_psram_esp32s2'],
    indirect=True,
)
@idf_parametrize('target', ['esp32s2'], indirect=['target'])
def test_esp_attr_xip_psram_esp32s2(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# psram attr tests with xip_psram
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['pm_xip_psram_esp32s3'],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_esp_attr_xip_psram_esp32s3(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# power down CPU and TOP domain in auto-lightsleep
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['pm_pd_top_sleep'],
    indirect=True,
)
@idf_parametrize('target', ['esp32c5', 'esp32c6', 'esp32h2', 'esp32p4'], indirect=['target'])
def test_esp_pd_top_and_cpu_sleep(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# Tickless IDLE without lightsleep
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['tickless_waiti'],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32', 'esp32s2'], reason='PM_TICKLESS_IDLE_WAITI requires systimer')
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='bringup on this module is not done')
def test_ticlkess_waiti(dut: Dut) -> None:
    _assert_strong_pm_hooks(dut)
    dut.run_all_single_board_cases()
