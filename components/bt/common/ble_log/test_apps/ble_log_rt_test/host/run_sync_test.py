#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Compile the real sync sender with deterministic peripheral/time fakes."""

import pathlib
import subprocess
import tempfile

HERE = pathlib.Path(__file__).resolve().parent
SRC = HERE.parents[2] / 'src'
STUBS = (
    'sdkconfig.h',
    'soc/soc_caps.h',
    'ble_log_rt.h',
    'esp_bit_defs.h',
    'freertos/FreeRTOS.h',
    'freertos/semphr.h',
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
    for xtal in (1, 0):
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
                '-I',
                str(temp),
                '-I',
                str(SRC / 'internal_include'),
                '-I',
                str(SRC.parent / 'include'),
                str(HERE / 'test_sync.c'),
                '-o',
                str(binary),
            ],
            check=True,
            timeout=30,
        )
        subprocess.run([str(binary)], check=True, timeout=10)
        print(f'PASS: UART_XTAL={xtal}', flush=True)
