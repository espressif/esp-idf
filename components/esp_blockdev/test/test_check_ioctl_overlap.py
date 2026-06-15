# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from __future__ import annotations

import shutil
import sys
import tempfile
import unittest
from pathlib import Path

COMPONENT_DIR = Path(__file__).resolve().parents[1]
TOOLS_DIR = COMPONENT_DIR / 'tools'
ESP_BLOCKDEV_H = COMPONENT_DIR / 'include' / 'esp_blockdev.h'
ESP_COMMON_INCLUDE = COMPONENT_DIR.parent / 'esp_common' / 'include'

sys.path.insert(0, str(TOOLS_DIR))
import check_ioctl_overlap as checker  # noqa: E402

COMPILER = shutil.which('gcc') or shutil.which('cc')
INCLUDE_DIRS = [str(ESP_COMMON_INCLUDE), str(ESP_BLOCKDEV_H.parent)]

# The header itself reserves [0x00..0x01] for esp_blockdev core commands.
# Tests that include esp_blockdev.h will see that reservation.
CORE_RESERVATION_END = 0x01


def run_check(contents: str) -> list[checker.Reservation]:
    with tempfile.TemporaryDirectory() as tmp:
        tmp_path = Path(tmp)
        (tmp_path / 'sdkconfig.h').write_text('/* test stub */\n')
        header = tmp_path / 'defs.h'
        header.write_text(contents)
        include_dirs = [tmp, *INCLUDE_DIRS]
        return checker.check_files([header], compiler=COMPILER, include_dirs=include_dirs)  # type: ignore[no-any-return]


class TestIoctlOverlapChecker(unittest.TestCase):
    @unittest.skipUnless(COMPILER, 'C compiler required for preprocessing')
    def test_no_overlap(self) -> None:
        # Use ranges that don't overlap with core [0x00..0x01]
        reservations = run_check(
            f'''#include "{ESP_BLOCKDEV_H}"

#define BASE 0x10
ESP_BLOCKDEV_RESERVE_CMD_RANGE(nand_flash, BASE + 0, BASE + 0x0F);
ESP_BLOCKDEV_RESERVE_CMD_RANGE(sdcard, BASE + 0x10, BASE + 0x1F);
ESP_BLOCKDEV_RESERVE_CMD(system, 0x40);
'''
        )
        # 3 from this file + 1 from esp_blockdev.h core
        self.assertEqual(len(reservations), 4)
        system_reservations = [r for r in reservations if r.component == 'system']
        self.assertEqual(len(system_reservations), 1)
        self.assertEqual(system_reservations[0].start, 0x40)
        self.assertEqual(system_reservations[0].end, 0x40)

    @unittest.skipUnless(COMPILER, 'C compiler required for preprocessing')
    def test_direct_overlap(self) -> None:
        with self.assertRaises(checker.ReservationError) as ctx:
            run_check(
                f'''#include "{ESP_BLOCKDEV_H}"

ESP_BLOCKDEV_RESERVE_CMD_RANGE(nand_flash, 0x10, 0x20);
ESP_BLOCKDEV_RESERVE_CMD_RANGE(sdcard, 0x1F, 0x30);
'''
            )
        msg = str(ctx.exception)
        self.assertIn('ioctl reservation overlap:', msg)
        self.assertIn('nand_flash', msg)
        self.assertIn('sdcard', msg)

    @unittest.skipUnless(COMPILER, 'C compiler required for preprocessing')
    def test_adjacent_non_overlap(self) -> None:
        reservations = run_check(
            f'''#include "{ESP_BLOCKDEV_H}"

ESP_BLOCKDEV_RESERVE_CMD_RANGE(a, 0x10, 0x1F);
ESP_BLOCKDEV_RESERVE_CMD_RANGE(b, 0x20, 0x2F);
'''
        )
        # 2 from this file + 1 core
        self.assertEqual(len(reservations), 3)

    @unittest.skipUnless(COMPILER, 'C compiler required for preprocessing')
    def test_single_command_reservation(self) -> None:
        reservations = run_check(
            f'''#include "{ESP_BLOCKDEV_H}"

ESP_BLOCKDEV_RESERVE_CMD(single_cmd, 0x42);
ESP_BLOCKDEV_RESERVE_CMD_RANGE(other, 0x43, 0x44);
'''
        )
        single = [r for r in reservations if r.component == 'single_cmd']
        self.assertEqual(len(single), 1)
        self.assertEqual(single[0].start, 0x42)
        self.assertEqual(single[0].end, 0x42)

    @unittest.skipUnless(COMPILER, 'C compiler required for preprocessing')
    def test_reversed_range(self) -> None:
        with self.assertRaises(checker.ReservationError) as ctx:
            run_check(
                f'''#include "{ESP_BLOCKDEV_H}"

ESP_BLOCKDEV_RESERVE_CMD_RANGE(reversed, 0x20, 0x10);
'''
            )
        self.assertIn('start 0x20 > end 0x10', str(ctx.exception))

    @unittest.skipUnless(COMPILER, 'C compiler required for preprocessing')
    def test_out_of_range_values(self) -> None:
        with self.assertRaises(checker.ReservationError) as ctx:
            run_check(
                f'''#include "{ESP_BLOCKDEV_H}"

ESP_BLOCKDEV_RESERVE_CMD_RANGE(big, 0x100, 0x101);
'''
            )
        self.assertIn('out of range', str(ctx.exception))

    @unittest.skipUnless(COMPILER, 'C compiler required for preprocessing')
    def test_macro_expanded_constants(self) -> None:
        reservations = run_check(
            f'''#include "{ESP_BLOCKDEV_H}"

#define BASE ESP_BLOCKDEV_CMD_SYSTEM_BASE
ESP_BLOCKDEV_RESERVE_CMD_RANGE(range_a, BASE + 10, BASE + 20);
ESP_BLOCKDEV_RESERVE_CMD(single, BASE + 2);
'''
        )
        # These overlap with core [0x00..0x01], but single=2 is adjacent (OK),
        # range_a=[10..20] is non-overlapping with core. However single=2 > core end=1, OK.
        range_a = [r for r in reservations if r.component == 'range_a']
        single = [r for r in reservations if r.component == 'single']
        self.assertEqual(len(range_a), 1)
        self.assertEqual(range_a[0].start, 10)
        self.assertEqual(range_a[0].end, 20)
        self.assertEqual(len(single), 1)
        self.assertEqual(single[0].start, 2)
        self.assertEqual(single[0].end, 2)

    @unittest.skipUnless(COMPILER, 'C compiler required for preprocessing')
    def test_overlap_with_core_commands(self) -> None:
        """Reserving a range that overlaps with core [0x00..0x01] must fail."""
        with self.assertRaises(checker.ReservationError) as ctx:
            run_check(
                f'''#include "{ESP_BLOCKDEV_H}"

ESP_BLOCKDEV_RESERVE_CMD_RANGE(my_comp, 0x00, 0x05);
'''
            )
        msg = str(ctx.exception)
        self.assertIn('esp_blockdev', msg)
        self.assertIn('my_comp', msg)


if __name__ == '__main__':
    unittest.main()
