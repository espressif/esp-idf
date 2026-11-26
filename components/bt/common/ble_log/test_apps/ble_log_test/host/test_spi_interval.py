# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Exercise the production SPI callbacks across low-32-bit timer wraps."""

import pathlib
import subprocess
import tempfile

source = (pathlib.Path(__file__).resolve().parents[3] / 'src/prph/ble_log_prph_spi_master_dma.c').read_text()
callbacks = source[source.index('/* PRIVATE FUNCTION */') : source.index('/* INTERFACE */')]
timestamp = next(line for line in source.splitlines() if 'last_tx_done_ts = 0;' in line)
interval = next(line for line in source.splitlines() if line.startswith('#define BLE_LOG_SPI_TRANS_ITVL_MIN_US'))
harness = r"""
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#define BLE_LOG_STATIC static
#define BLE_LOG_DRAM_ATTR
#define BLE_LOG_SPI_MASTER_DMA_CB_ATTR
typedef struct { void *user; } spi_transaction_t;
typedef void ble_log_prph_trans_t;
static int64_t now;
static unsigned reads, recycled;
static int64_t esp_timer_get_time(void) { ++reads; return now++; }
static void ble_log_lbm_recycle_trans(ble_log_prph_trans_t *trans)
{
    (void)trans;
    ++recycled;
}
"""
main = r"""
int main(void)
{
    const int64_t wrap = INT64_C(1) << 32;
    const int64_t done[] = {100, wrap - 10, wrap - 1, wrap, wrap + 100, 2 * wrap + 100};
    const int64_t gaps[] = {0, 1, 5, 29, 30, 31, 100, wrap - 1};
    spi_transaction_t trans = {0};
    for (unsigned i = 0; i < sizeof(done) / sizeof(done[0]); ++i) {
        for (unsigned j = 0; j < sizeof(gaps) / sizeof(gaps[0]); ++j) {
            now = done[i];
            spi_master_dma_tx_done_cb(&trans);
            now = done[i] + gaps[j];
            reads = 0;
            spi_master_dma_pre_tx_cb(&trans);
            unsigned expected = gaps[j] < 30 ? 31 - gaps[j] : 1;
            if (reads != expected) {
                fprintf(stderr, "done=%lld gap=%lld: expected %u timer reads, got %u\n",
                        (long long)done[i], (long long)gaps[j], expected, reads);
                return 1;
            }
        }
    }
    assert(recycled == 48);
    puts("PASS: 48 SPI interval cases, including timer wraps and 30-us boundaries");
}
"""

with tempfile.TemporaryDirectory(prefix='ble-log-spi-interval-') as directory:
    c_file = pathlib.Path(directory) / 'interval.c'
    binary = pathlib.Path(directory) / 'interval'
    c_file.write_text(harness + timestamp + '\n' + interval + '\n' + callbacks + main)
    subprocess.run(
        [
            'cc',
            '-std=c11',
            '-Wall',
            '-Wextra',
            '-Werror',
            '-fsanitize=undefined',
            '-fno-sanitize-recover=undefined',
            str(c_file),
            '-o',
            str(binary),
        ],
        check=True,
        timeout=30,
    )
    subprocess.run([str(binary)], check=True, timeout=5)
