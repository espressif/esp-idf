# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

from typing import Any

import pytest


@pytest.mark.esp32c6
def test_usb_serial_jtag_repl_no_input(dut: Any) -> None:
    """
    Test that console REPL doesn't starve scheduler when no USB input available.

    Both blinker_task and verbose_task must keep running after REPL starts.
    ROM heartbeats are printed to UART0 every 100 iterations.
    """
    # Wait for REPL init to complete
    dut.expect('Starting verbose task after esp_console_start_repl', timeout=10)

    # Capture two consecutive heartbeats and verify both counters are increasing.
    # Each heartbeat fires every 100 blinker iterations × 50 ms = ~5 s.
    hb_re = r'rom hb blink=(\d+) verbose=(\d+)'

    m1 = dut.expect(hb_re, timeout=15)
    blink1, verbose1 = int(m1.group(1)), int(m1.group(2))

    m2 = dut.expect(hb_re, timeout=15)
    blink2, verbose2 = int(m2.group(1)), int(m2.group(2))

    assert blink2 > blink1, f'blinker counter not increasing: {blink1} -> {blink2}'
    assert verbose2 > verbose1, f'verbose counter not increasing: {verbose1} -> {verbose2}'
