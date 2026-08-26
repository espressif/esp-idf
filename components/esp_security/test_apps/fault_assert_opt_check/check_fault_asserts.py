# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Post-build regression guard for ESP_FAULT_ASSERT().

Disassembles the test app and verifies that the ESP_FAULT_ASSERT() calls in the
known test functions still emit their reset blocks. One intact assert produces
three independent reset-on-failure paths, i.e. three references to
``esp_rom_software_reset_system`` in the function. If the macro ever regresses
and the optimizer folds the checks away, the count drops and this exits non-zero,
failing the build.

Usage: check_fault_asserts.py <app.elf> <objdump>
"""

import re
import subprocess
import sys

RESET_SYM = 'esp_rom_software_reset_system'

# function name -> number of ESP_FAULT_ASSERT calls it contains (x3 reset blocks each)
EXPECTED = {
    'test_fa_guarded_flag': 1,
    'test_fa_guarded_status': 1,
}

FUNC_RE = re.compile(r'^[0-9a-fA-F]+ <(.+)>:$')


def reset_counts(elf: str, objdump: str) -> dict:
    dis = subprocess.run([objdump, '-d', elf], capture_output=True, text=True, check=True).stdout
    counts: dict = {}
    cur = None
    for line in dis.splitlines():
        m = FUNC_RE.match(line)
        if m:
            cur = m.group(1)
            counts.setdefault(cur, 0)
        elif cur and RESET_SYM in line:
            counts[cur] += 1
    return counts


def main() -> int:
    if len(sys.argv) != 3:
        print(__doc__)
        return 2
    elf, objdump = sys.argv[1], sys.argv[2]
    counts = reset_counts(elf, objdump)

    failed = False
    for fn, n_asserts in EXPECTED.items():
        expected = 3 * n_asserts
        found = counts.get(fn)
        if found is None:
            print(f'ERROR: {fn} not found in {elf} (renamed/removed?)')
            failed = True
        elif found < expected:
            print(
                f'ERROR: ESP_FAULT_ASSERT optimized away in {fn}: '
                f'{found} reset checks, expected {expected}. '
                f'See components/esp_common/include/esp_fault.h'
            )
            failed = True
        else:
            print(f'OK: {fn} -> {found} reset checks ({found // 3} assert(s) x3)')

    if failed:
        print('FAILED: ESP_FAULT_ASSERT regression check')
        return 1
    print('PASSED: ESP_FAULT_ASSERT checks survived optimization')
    return 0


if __name__ == '__main__':
    sys.exit(main())
