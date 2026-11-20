#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Compile sync sampling and sending with deterministic peripheral/time fakes."""

import pathlib
import subprocess
import tempfile

HERE = pathlib.Path(__file__).resolve().parent
SRC = HERE.parents[2] / 'src'
STUBS = (
    'sdkconfig.h',
    'soc/soc_caps.h',
    'esp_bit_defs.h',
    'freertos/FreeRTOS.h',
    'freertos/task.h',
    'esp_bt.h',
    'esp_heap_caps.h',
    'esp_rom_serial_output.h',
    'driver/gpio.h',
    'driver/uart.h',
    'esp_log.h',
    'esp_timer.h',
    'esp_rom_sys.h',
)

with tempfile.TemporaryDirectory(prefix='ble-log-sync-test-') as directory:
    temp = pathlib.Path(directory)
    for name in STUBS:
        path = temp / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.touch()
    for sync, xtal, ll in ((1, 1, 1), (1, 0, 1), (0, 0, 1), (0, 0, 0)):
        binary = temp / 'sync_test'
        subprocess.run(
            [
                'cc',
                '-std=gnu11',
                '-Wall',
                '-Wextra',
                '-Werror',
                '-Wno-unused-parameter',
                '-fsanitize=undefined',
                '-fno-sanitize-recover=undefined',
                '-g',
                f'-DSOC_UART_SUPPORT_XTAL_CLK={xtal}',
                f'-DCONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED={sync}',
                f'-DCONFIG_BLE_LOG_LL_ENABLED={ll}',
                '-I',
                str(temp),
                '-I',
                str(SRC / 'internal_include'),
                str(HERE / 'test_sync.c'),
                '-o',
                str(binary),
            ],
            check=True,
            timeout=30,
        )
        subprocess.run([str(binary)], check=True, timeout=10)
        print(f'PASS: SYNC={sync}, UART_XTAL={xtal}, LL={ll}', flush=True)
