# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32s2', 'esp32c3'], indirect=['target'])
@pytest.mark.parametrize(
    'config',
    [
        'defaults',
        'no_isr_post',
    ],
    indirect=True,
)
def test_esp_event(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@idf_parametrize('target', ['esp32'], indirect=['target'])
@pytest.mark.parametrize('config', ['ext_ram'], indirect=True)
def test_esp_event_ext_ram(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.xfail('config.getvalue("target") == "esp32c3"', reason='Unstable on QEMU, needs investigation')
@pytest.mark.parametrize(
    'config',
    [
        'defaults',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_esp_event_qemu(dut: Dut) -> None:
    for case in dut.test_menu:
        if 'qemu-ignore' not in case.groups and not case.is_ignored and case.type == 'normal':
            dut._run_normal_case(case)


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
@pytest.mark.parametrize(
    'config',
    [
        'defaults',
    ],
    indirect=True,
)
def test_esp_event_posix_simulator(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('*')
    dut.expect(r'\d{2} Tests 0 Failures 0 Ignored', timeout=120)


@pytest.mark.generic
@idf_parametrize('target', ['esp32'], indirect=['target'])
@pytest.mark.parametrize(
    'config',
    [
        'defaults',
    ],
    indirect=True,
)
def test_esp_event_profiling(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('"profiling reports valid values"')
    # look for all references of handlers invoked at least 1 time
    # with an execution time superior to 0 us
    matches = dut.expect(r'HANDLER .+ inv:[1-9][0-9]{0,} time:[1-9][0-9]{0,} us', timeout=2)
    matches_arr = matches.group().split(b'\r\n')
    assert len(matches_arr) == 3
    dut.expect('1 Tests 0 Failures 0 Ignored', timeout=120)
    dut.expect_exact("Enter next test, or 'enter' to see menu")

    dut.write('"esp_event_dump does not show self unregistered handler"')
    # look for 1 handlers never invoked
    matches = dut.expect(r'HANDLER .+ inv:0 time:0 us', timeout=2)
    matches_arr = matches.group().split(b'\r\n')
    assert len(matches_arr) == 1
    dut.expect('1 Tests 0 Failures 0 Ignored', timeout=120)
    dut.expect_exact("Enter next test, or 'enter' to see menu")

    dut.write('"self unregistered handlers are never called again after they return"')
    dut.expect('1 Tests 0 Failures 0 Ignored', timeout=120)
