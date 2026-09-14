# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

# Names of the units observing core 0 instruction and data traffic, per target
INST_UNIT = {
    'esp32c5': 'l1-cache-ibus',
    'esp32c6': 'l1-cache-ibus',
    'esp32c61': 'l1-cache-ibus',
    'esp32h2': 'l1-cache-ibus',
    'esp32h4': 'l1-cache-inst-core0',
    'esp32p4': 'l1-icache-core0',
    'esp32s31': 'l1-cache-inst-core0',
}
DATA_UNIT = {
    'esp32c5': 'l1-cache-dbus',
    'esp32c6': 'l1-cache-dbus',
    'esp32c61': 'l1-cache-dbus',
    'esp32h2': 'l1-cache-dbus',
    'esp32h4': 'l1-cache-data-core0',
    'esp32p4': 'l1-dcache-core0',
    'esp32s31': 'l1-cache-data-core0',
}

# The one workload every target runs. Reading .rodata goes through the cache on
# every chip, and 128 KB exceeds the data cache everywhere, so each pass has to
# fetch the lines from flash again.
FLASH_PHASE = 'flash rodata, 128 KB x 10 passes'


def expect_counter_row(dut: Dut, unit: str) -> dict:
    """Match one row of the esp_cache_cnt_dump() table, return the counter values.

    Counters which the unit does not provide (printed as '-') are returned as None.
    """
    num = r'(\d+|-)'
    match = dut.expect(rf'{unit} +{num} +{num} +{num} +{num} +[0-9.]+%')

    def field(idx: int) -> int | None:
        value = match.group(idx).decode()
        return None if value == '-' else int(value)

    return {
        'accesses': field(1),
        'line_fills': field(2),
        'writebacks': field(3),
        'conflicts': field(4),
    }


def check_phase(dut: Dut, workload: str, nonzero: dict) -> None:
    """Expect one 'Workload: ...' line followed by a counter table.

    nonzero maps unit names (in dump order) to the list of counters which
    must read non-zero after the workload.
    """
    dut.expect_exact(f'Workload: {workload}')
    for unit, counters in nonzero.items():
        row = expect_counter_row(dut, unit)
        for counter in counters:
            assert row[counter], f'{workload}: expected {unit} {counter} > 0, got {row[counter]}'


@pytest.mark.generic
@idf_parametrize('config', ['default'], indirect=['config'])
@idf_parametrize('target', ['esp32c6', 'esp32h2'], indirect=['target'])
def test_cache_counters(dut: Dut) -> None:
    # No PSRAM: only the flash workload runs. Its code is fetched from flash as
    # well, so the instruction unit counts accesses too.
    check_phase(
        dut,
        FLASH_PHASE,
        {
            INST_UNIT[dut.target]: ['accesses'],
            DATA_UNIT[dut.target]: ['accesses', 'line_fills'],
        },
    )
    dut.expect_exact('Cache counters example done')


@pytest.mark.generic
@idf_parametrize('config', ['psram'], indirect=['config'])
@idf_parametrize('target', ['esp32c5', 'esp32c61', 'esp32h4', 'esp32s31'], indirect=['target'])
def test_cache_counters_psram(dut: Dut) -> None:
    inst = INST_UNIT[dut.target]
    data = DATA_UNIT[dut.target]
    check_phase(dut, FLASH_PHASE, {inst: ['accesses'], data: ['accesses', 'line_fills']})
    check_phase(dut, 'PSRAM, 8 KB x 500 passes (fits in cache)', {inst: ['accesses'], data: ['accesses']})
    # A working set larger than the cache must be fetched from PSRAM on every pass
    check_phase(dut, 'PSRAM, 512 KB x 5 passes (exceeds cache)', {inst: ['accesses'], data: ['accesses', 'line_fills']})
    dut.expect_exact('Cache counters example done')


@pytest.mark.generic
@idf_parametrize('config', ['psram'], indirect=['config'])
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_cache_counters_psram_esp32p4(dut: Dut) -> None:
    # 128 KB is larger than the L1 data cache but fits in L2, so L1 refills on
    # every pass while L2 only has to fill once
    check_phase(
        dut,
        FLASH_PHASE,
        {
            'l1-icache-core0': ['accesses'],
            'l1-dcache-core0': ['accesses', 'line_fills'],
            'l2-cache-data': ['accesses'],
        },
    )
    # Internal SRAM is reached through the L1 cache on this chip, and the whole
    # working set stays in it, so the L1 data unit counts accesses but no fills
    check_phase(
        dut,
        'internal SRAM, 8 KB x 1000 passes',
        {
            'l1-icache-core0': ['accesses'],
            'l1-dcache-core0': ['accesses'],
        },
    )
    check_phase(
        dut,
        'PSRAM, 48 KB x 100 passes (fits in L1)',
        {
            'l1-icache-core0': ['accesses'],
            'l1-dcache-core0': ['accesses'],
        },
    )
    # Larger than the L1 data cache: every pass refills L1 from L2
    check_phase(
        dut,
        'PSRAM, 192 KB x 20 passes (fits in L2)',
        {
            'l1-dcache-core0': ['accesses', 'line_fills'],
            'l2-cache-data': ['accesses'],
        },
    )
    # Larger than the L2 cache: lines are continuously fetched from PSRAM
    check_phase(
        dut,
        'PSRAM, 2 MB x 5 passes (exceeds L2)',
        {
            'l1-dcache-core0': ['accesses', 'line_fills'],
            'l2-cache-data': ['accesses', 'line_fills'],
        },
    )
    dut.expect_exact('Cache counters example done')
