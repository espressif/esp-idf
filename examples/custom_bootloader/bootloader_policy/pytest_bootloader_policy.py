# SPDX-FileCopyrightText: 2026 Matterize Labs (matterizelabs.com)
#
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

MAX_ATTEMPTS = 3

BOOT_LINE = (
    r'bootloader_policy: bootable=(\d+) attempts=(\d+)->(\d+) target=(\S+) reason=(\d+) '
    r'fallback=(\d) safe=(\d) journal=(\w+)'
)
APP_LINE = (
    r'bootloader_policy: records=(\d+) latest_seq=(\d+) attempts=(\d+) reason=(\d+) safe_mode=(\d) confirmed=(\d)'
)


@pytest.mark.generic
@idf_parametrize('target', ['esp32c3', 'esp32s3'], indirect=['target'])
def test_bootloader_policy(dut: Dut) -> None:
    """The policy walks the attempt counter up and then enters safe mode.

    The example does not confirm the image, so every reset is an unconfirmed boot. The phase
    is not fixed, because flashing the example already boots the board once, so this checks
    the shape of the cycle instead of absolute counter values.
    """
    dut.expect(BOOT_LINE)
    dut.expect(APP_LINE)

    hit_limit = False
    for _ in range(MAX_ATTEMPTS + 2):
        dut.serial.hard_reset()
        match = dut.expect(BOOT_LINE)
        before, after = int(match.group(2)), int(match.group(3))
        reason, safe = int(match.group(5)), int(match.group(7))

        if reason == 3:  # crash loop
            assert after == 0, f'a crash loop must clear the attempt counter, got {after}'
            assert safe == 1, 'a crash loop must set safe mode'
            report = dut.expect(APP_LINE)
            assert int(report.group(4)) == 3, 'the application must see the crash loop decision'
            assert int(report.group(5)) == 1, 'the application must see safe mode'
            hit_limit = True
            break

        assert after == before + 1, f'the attempt counter must step by one, got {before}->{after}'

    assert hit_limit, 'the attempt limit was never reached'
