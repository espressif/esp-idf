# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Exercise the production console dump with retained transport contents."""

import pathlib
import subprocess
import tempfile

source = (pathlib.Path(__file__).resolve().parents[3] / 'src/ble_log_lbm_v2.c').read_text()
dump = source[source.index('void ble_log_dump_to_console(void)') :]
harness = r"""
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define BLE_LOG_POOL_TRANS_CNT 2
#define BLE_LOG_CONSOLE printf
#define BLE_LOG_FEED_WDT() ((void)0)
#define BLE_LOG_ENTER_CRITICAL() (++depth)
#define BLE_LOG_EXIT_CRITICAL() (--depth)
#define BLE_LOG_REF_COUNT_RELEASE(ref) (--*(ref))
typedef struct { uint8_t *buf; int size, pos; } ble_log_prph_trans_t;
static unsigned lbm_ref_count, lbm_inited = 1, depth;
static struct { ble_log_prph_trans_t *trans[2]; } g_pool;
static ble_log_prph_trans_t *internal_trans;
static bool ble_log_ref_count_try_acquire(unsigned *ref, unsigned *inited)
{
    if (!*inited) { return false; }
    ++*ref;
    return true;
}
"""
main = r"""
int main(int argc, char **argv)
{
    uint8_t first[] = {0x12, 0x34}, second[] = {0x56, 0x78, 0x9a};
    uint8_t snapshot[180];
    for (unsigned i = 0; i < sizeof(snapshot); i++) { snapshot[i] = i; }
    ble_log_prph_trans_t pool[] = {{first, sizeof(first), 0}, {second, sizeof(second), 0}};
    ble_log_prph_trans_t internal = {snapshot, sizeof(snapshot), 179};
    g_pool.trans[0] = &pool[0];
    g_pool.trans[1] = &pool[1];
    internal_trans = &internal;
    if (argc > 1) { internal.pos = 0; } /* Recycle retains bytes, but clears pos. */
    if (argc > 2) { lbm_inited = 0; internal_trans = NULL; }
    ble_log_dump_to_console();
    assert(lbm_ref_count == 0 && depth == 0);
    return 0;
}
"""

with tempfile.TemporaryDirectory(prefix='ble-log-dump-test-') as directory:
    c_file = pathlib.Path(directory) / 'dump.c'
    binary = pathlib.Path(directory) / 'dump'
    c_file.write_text(harness + dump + main)
    subprocess.run(
        [
            'cc',
            '-std=c11',
            '-Wall',
            '-Wextra',
            '-Werror',
            '-Wno-unused-parameter',
            '-fsanitize=undefined',
            '-fno-sanitize-recover=undefined',
            str(c_file),
            '-o',
            str(binary),
        ],
        check=True,
        timeout=30,
    )
    for args in ([], ['recycled']):
        output = subprocess.check_output([str(binary), *args], text=True, timeout=5)
        start, end = '[BLE_LOG_DUMP_START:\n', '\n:BLE_LOG_DUMP_END]\n\n'
        assert output.startswith(start) and output.endswith(end)
        payload = bytes.fromhex(output[len(start) : -len(end)])
        assert payload == bytes.fromhex('12 34 56 78 9a') + bytes(range(180)), (
            'Console dump must preserve pool contents and include the dedicated snapshot, even after recycle'
        )
    assert not subprocess.check_output([str(binary), 'recycled', 'deinitialized'], timeout=5)
print('PASS: dump includes retained snapshot bytes; closed lifecycle emits nothing')
