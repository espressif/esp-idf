| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

# USB-Serial-JTAG REPL no-input test

Regression test for ESP-IDF issue: **console REPL starves FreeRTOS scheduler when USB-Serial-JTAG host is not connected**.

## Root cause

When no host is connected, `linenoise()` returns `NULL` immediately on every call. Without a yield, `console_repl` task spins in a tight loop, starves the Idle task, and triggers the Task Watchdog (TWDT).

## Fix

`vTaskDelay(1)` on `NULL` return from `linenoise()` in `esp_console_common.c` — yields to the scheduler each iteration.

## Test scenario

1. USB-Serial-JTAG **not connected** at power-on (worst case)
2. `blinker_task` created **before** `esp_console_start_repl()` — proves blinker runs regardless
3. `verbose_task` created **after** `esp_console_start_repl()` — the task that couldn't start without the fix
4. Both tasks print a combined heartbeat to UART0 every 100 iterations (~5 s): `rom hb blink=N verbose=M`

**Without fix:** only blinker runs; `verbose_task` never gets CPU; TWDT fires.  
**With fix:** both counters increase; no TWDT.

## Running the test

```bash
# Flash and monitor manually
idf.py -p PORT flash monitor

# Run automated pytest (reads UART0)
pytest test_usb_serial_jtag_repl_no_input.py -v --target esp32c6 --port PORT
```
