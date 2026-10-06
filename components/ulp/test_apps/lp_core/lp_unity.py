# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
"""Unity menu helpers shared by LP-core test apps.

``conftest.py`` in this directory loads for every app under ``test_apps/lp_core``.
Request ``two_board_sleep`` or ``unity_board`` from a test, or import the classes
below. The classes themselves know nothing about a particular peripheral.

``UnityBoard`` drives one DUT: reset, start a menu case, select a multi-stage
index, match a line, and complete a ``unity_wait_for_signal`` handshake.

``TwoBoardSleep`` runs one deep-sleep scenario on two boards. DUT 0 is the
sleeper (one ``multi_stage`` case) and DUT 1 is the peer (one ``normal``
case). Both cases belong to the same Unity group.

Retained cycles: the peer stays awake. The sleeper prints ``SLEEPER_SLEEP``
twice, without re-initializing the peripheral on the second cycle, then a
third stage checks the result. The peer prints ``"<armed> <cycle>"`` before
each sleep and ``"<ok> <cycle>"`` after it. An optional signal is released
only after the sleeper has printed the marker.

Wakeup: the peer prints the armed line and waits on a signal. The sleeper
then sleeps. The host releases the signal after ``delay_s``.
"""

import time
from typing import TYPE_CHECKING
from typing import Any

from pytest_embedded_idf import UnittestMenuCase

if TYPE_CHECKING:
    from pytest_embedded_idf.dut import IdfDut
    from pytest_embedded_idf.unity_tester import CaseTester


def find_menu_case(
    case_tester: 'CaseTester',
    case_type: str,
    group: str,
    name_fragment: str | None = None,
) -> UnittestMenuCase:
    """Return the single Unity case of ``case_type`` in ``group``."""
    cases = [
        case
        for case in case_tester.test_menu
        if case.type == case_type and group in case.groups and (name_fragment is None or name_fragment in case.name)
    ]
    names = [case.name for case in cases]
    assert len(cases) == 1, f"Expected 1 {case_type} case in '{group}', found {len(cases)}: {names}"
    return cases[0]


class UnityBoard:
    """One DUT sitting on the Unity menu."""

    def __init__(self, dut: 'IdfDut', timeout: float = 120.0) -> None:
        self.dut = dut
        self.timeout = timeout

    def reset(self) -> None:
        self.dut.serial.hard_reset()

    def ready(self, timeout: float | None = None) -> None:
        self.dut._get_ready(self.timeout if timeout is None else timeout)

    def start(self, case: UnittestMenuCase, *, wait_ready: bool = True) -> None:
        if wait_ready:
            self.ready()
        self.dut.confirm_write(case.index, expect_str=f'Running {case.name}...')

    def select_stage(self, case: UnittestMenuCase, stage_index: int, *, wait_ready: bool = True) -> None:
        self.start(case, wait_ready=wait_ready)
        self.dut.write(str(stage_index))

    def run(self, case: UnittestMenuCase, *, wait_ready: bool = True) -> None:
        """Start ``case`` and wait until Unity prints the result."""
        self.start(case, wait_ready=wait_ready)
        self.unity_output()

    def expect(self, text: str) -> None:
        self.dut.expect_exact(text, timeout=self.timeout)

    def unity_output(self) -> None:
        self.dut.expect_unity_test_output(timeout=self.timeout)

    def wait_signal(self, signal: str) -> None:
        """Wait until the DUT prints ``unity_wait_for_signal`` for ``signal``."""
        self.expect(f'Waiting for signal: [{signal}]!')

    def proceed(self, delay_s: float = 0) -> None:
        """Press Enter on this DUT, after ``delay_s`` seconds."""
        if delay_s:
            time.sleep(delay_s)
        self.dut.write('')

    def release_signal(self, signal: str, delay_s: float = 0) -> None:
        """Wait for ``signal``, then press Enter."""
        self.wait_signal(signal)
        self.proceed(delay_s)


class TwoBoardSleep:
    """Sleeper plus peer for one Unity group."""

    def __init__(
        self,
        case_tester: 'CaseTester',
        group: str,
        *,
        timeout: float = 120.0,
        sleep_marker: str = 'SLEEPER_SLEEP',
        sleeper_index: int = 0,
        peer_index: int = 1,
    ) -> None:
        self.group = group
        self.sleep_marker = sleep_marker
        self.sleeper_case = find_menu_case(case_tester, 'multi_stage', group)
        self.peer_case = find_menu_case(case_tester, 'normal', group)
        self.sleeper = UnityBoard(case_tester.dut[sleeper_index], timeout)
        self.peer = UnityBoard(case_tester.dut[peer_index], timeout)

    def run_retained(
        self,
        armed: str,
        ok: str,
        *,
        after_sleep_signal: str | None = None,
        delay_s: float = 0,
    ) -> None:
        """Two sleep cycles, then the sleeper's final stage.

        The second cycle must not re-initialize the peripheral. When
        ``after_sleep_signal`` is set, Enter is pressed on the peer only
        after the sleeper has printed the sleep marker.
        """
        assert len(self.sleeper_case.subcases) == 3, f'{self.group} expected 3 sleeper stages'
        stage0, stage1, stage2 = self.sleeper_case.subcases
        self._reset()
        self.peer.start(self.peer_case)
        for cycle, stage in enumerate((stage0, stage1)):
            self.peer.expect(f'{armed} {cycle}')
            self._enter_sleep(stage)
            if after_sleep_signal is not None:
                self.peer.release_signal(after_sleep_signal, delay_s)
            self.peer.expect(f'{ok} {cycle}')
        self.peer.unity_output()
        self._finish_stage(stage2)

    def run_wakeup(self, armed: str, signal: str, delay_s: float) -> None:
        """Peer arms and waits. Sleeper sleeps. Host releases ``signal`` after ``delay_s``."""
        assert len(self.sleeper_case.subcases) == 2, f'{self.group} expected 2 sleeper stages'
        stage0, stage1 = self.sleeper_case.subcases
        self._reset()
        self.peer.start(self.peer_case)
        self.peer.expect(armed)
        self.peer.wait_signal(signal)
        self._enter_sleep(stage0)
        self.peer.proceed(delay_s)
        self.peer.unity_output()
        self._finish_stage(stage1)

    def _reset(self) -> None:
        self.sleeper.reset()
        self.peer.reset()

    def _enter_sleep(self, stage: Any) -> None:
        self.sleeper.select_stage(self.sleeper_case, int(stage['index']))
        self.sleeper.expect(self.sleep_marker)

    def _finish_stage(self, stage: Any) -> None:
        self.sleeper.ready()
        self.sleeper.select_stage(self.sleeper_case, int(stage['index']), wait_ready=False)
        self.sleeper.unity_output()
