# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import re

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

LOCK_PCT_TOL = 5

LOCK_EXPECT_PCT = {
    'demo_no_ls': 60,
    'demo_apb': 40,
    'demo_cpu': 20,
}


def _parse_lock_stats(dut: Dut, timeout: int = 15) -> dict[str, int]:
    dut.expect_exact('Lock stats:', timeout=timeout)
    dut.expect(re.compile(rb'Name\s+Type\s+Arg\s+Active\s+Total_count\s+Time'), timeout=2)
    lock_pct: dict[str, int] = {}
    pattern = re.compile(rb'(demo_no_ls|demo_apb|demo_cpu)\s+\S+\s+\d+\s+\d+\s+\d+\s+\d+\s+(\d+)\s*%')
    while len(lock_pct) < len(LOCK_EXPECT_PCT):
        m = dut.expect(pattern, timeout=timeout)
        lock_pct[m.group(1).decode()] = int(m.group(2))
    return lock_pct


def _run_pm_example_test(dut: Dut) -> None:
    dut.expect_exact('=== Power Management Example (nested locks, single task) ===', timeout=10)
    dut.expect(re.compile(rb'--- PM: max_freq=\d+ min_freq=\d+ MHz, light_sleep=\d+ ---'), timeout=15)

    lock_pct = _parse_lock_stats(dut, timeout=5)
    for name, expected in LOCK_EXPECT_PCT.items():
        pct = lock_pct[name]
        assert (expected - LOCK_PCT_TOL) <= pct <= (expected + LOCK_PCT_TOL)


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_PM_SUPPORTED == 1'), indirect=['target'])
def test_esp_pm_mode_stats(dut: Dut) -> None:
    _run_pm_example_test(dut)


@pytest.mark.generic
@pytest.mark.parametrize('config', ['pd_top'], indirect=True)
@idf_parametrize(
    'target',
    list(
        filter(
            lambda t: t != 'esp32c5',
            soc_filtered_targets('SOC_PM_SUPPORTED == 1 and SOC_PM_SUPPORT_TOP_PD == 1'),
        )
    ),
    indirect=['target'],
)
def test_esp_pm_mode_stats_pd_top(dut: Dut) -> None:
    _run_pm_example_test(dut)


@pytest.mark.generic
@pytest.mark.esp32c5_rev1
@pytest.mark.parametrize('config', ['pd_top'], indirect=True)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_esp_pm_mode_stats_pd_top_esp32c5_eco3(dut: Dut) -> None:
    _run_pm_example_test(dut)
