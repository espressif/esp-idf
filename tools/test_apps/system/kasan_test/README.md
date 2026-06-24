| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# KASAN Test Application

This test application validates the Kernel Address Sanitizer (KASAN) integration
in ESP-IDF by deliberately triggering memory safety bugs and checking that KASAN
detects and reports them before calling the panic handler.

## Test Cases

| Test | Description | Expected Outcome |
|------|-------------|-----------------|
| `overflow` | 1-byte write past end of 16-byte heap allocation | KASAN WRITE error + panic |
| `use_after_free` | Write to freed heap block | KASAN WRITE error + panic |
| `uaf_read` | Read from freed heap block | KASAN READ error + panic |
| `underflow` | Write before start of allocation (into left redzone) | KASAN WRITE error + panic |
| `large_overflow` | `memset` of 16 bytes into an 8-byte buffer | KASAN WRITE error + panic |
| `no_bug` | Clean alloc/use/free cycle | Completes without panic |
| `asan stubs valid access no error` | Direct calls to every sized `__asan_load<N>_noabort` and `__asan_store<N>_noabort` stub (N in {1, 2, 4, 8, 16, N}) on a valid buffer | Completes without panic; covers all 12 stubs |
| `asan stubs poisoned access all sizes` (no_halt only) | Same 12 stubs called on a freed pointer | Exactly 12 KASAN errors reported |

## Building

```bash
cd tools/test_apps/system/kasan_test
idf.py set-target esp32c6
idf.py build
```

Or to select a specific test case:

```bash
idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.overflow" build
```

## Prerequisites

The following Kconfig options must be set (they are pre-configured in
`sdkconfig.defaults`):

- `CONFIG_IDF_EXPERIMENTAL_FEATURES=y` – Required to expose KASAN in menuconfig
- `CONFIG_COMPILER_KASAN=y` – Enable KASAN instrumentation
- `CONFIG_ESP_TASK_WDT_EN=n` – Unity menu blocks IDLE until you press Enter or
  select a test; required for manual `idf.py monitor` as well as pytest

`CONFIG_COMPILER_KASAN` automatically selects `CONFIG_HEAP_USE_HOOKS`.  Redzone
size (8 bytes), quarantine size (8192 bytes), and heap poisoning (disabled) use
their Kconfig defaults — no extra overrides are needed.

The `sdkconfig.ci.*` files add only mode-specific options (e.g. `CONFIG_KASAN_NO_HALT`
for the all-in-one run, `CONFIG_ESP_SYSTEM_PANIC_PRINT_HALT` for halt-mode pytest).

## Running pytest

```bash
pytest pytest_kasan.py --target esp32c6 -v
```

## Memory and Performance Impact

| Target | Shadow Memory | Code Size Overhead | Free Heap Impact |
|--------|--------------|-------------------|-----------------|
| ESP32 | ~42 KiB (internal SRAM) | ~1.5-3x instrumented components | ~14% of free heap |
| ESP32-S3 | ~60 KiB | ~1.5-3x | ~16% of free heap |
| ESP32-C3 | ~54 KiB | ~1.5-3x | ~16% of free heap |
| ESP32-C6 | ~64 KiB | ~1.5-3x | ~14% of free heap |

The shadow array is placed in DRAM via the `DRAM_ATTR` attribute on
`kasan_shadow_mem` (mapped into `dram0_data` by the standard `.dram1` linker
mapping). On targets with PSRAM the shadow currently stays in internal SRAM;
placing it in external RAM is not yet supported.
