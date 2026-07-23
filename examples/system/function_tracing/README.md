| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Example: Compiler-Instrumented Function Tracing (function_tracing)

This example shows how to trace function entry/exit automatically with the `esp_trace` compiler-instrumented function tracing feature, and how to control **which** code is instrumented.

When a source file is built with the GCC flag `-finstrument-functions`, the compiler inserts a call at the start and end of every function. Those calls go to hooks provided by `esp_trace`, which forward the events to the active trace encoder (SystemView here). The events carry the raw function and call-site addresses. SystemView shows the enter/exit sequence with those addresses. Resolving the addresses to function names is done separately against the ELF file (for example with `addr2line` or a custom tool).

Unlike RTOS-aware tracing (task switches, semaphores, etc.), function tracing needs no manual trace points. You add one compile flag to a component and the whole call flow is captured.

## What this example demonstrates

- Instrument a whole component (`components/ft_demo`) by adding `-finstrument-functions` to its build.
- Instrument only selected sources (`main`) by setting the flag per source file.
- `-finstrument-functions-exclude-file-list` — exclude whole files as a comma-separated list (`ft_demo_hot.c`, `ft_demo_quiet.c`).
- `-finstrument-functions-exclude-function-list` — exclude a single function by name (`ft_demo_secret`).

The workload (`example_workload()`) calls four things every iteration:

| Call                  | Instrumented? | Why                                              |
| --------------------- | ------------- | ------------------------------------------------ |
| `ft_demo_run()`       | yes           | component is instrumented, appears as enter/exit |
| `ft_demo_secret()`    | no            | excluded by `-...-exclude-function-list`         |
| `ft_demo_hot_loop()`  | no            | its file is excluded by `-...-exclude-file-list` |
| `ft_demo_quiet_path()`| no            | its file is the 2nd `-...-exclude-file-list` entry |

So in the captured trace you should see `ft_demo_run -> ft_demo_level1 -> ft_demo_level2`, but **not** `ft_demo_secret`, `ft_demo_hot_loop` or `ft_demo_quiet_path`.

The same workload task is created pinned to each core, so on a dual-core target the multi-core capture shows function tracing on both cores (one task per core). On a single-core target it is one task on core 0.

## Project layout

```
function_tracing/
├── main/
│   ├── function_tracing_example_main.c   # workload + trace setup
│   └── CMakeLists.txt                     # instruments this source only
├── components/
│   └── ft_demo/
│       ├── ft_demo.c                      # instrumented call graph + excluded-by-name function
│       ├── ft_demo_hot.c                  # excluded-by-file (1st exclude-file-list entry)
│       ├── ft_demo_quiet.c                # excluded-by-file (2nd exclude-file-list entry)
│       └── CMakeLists.txt                 # instruments the whole component + exclude flags
├── sdkconfig.defaults                     # enables function tracing
├── sdkconfig.ci.jtag                      # apptrace over JTAG
├── SYSVIEW_FreeRTOS.txt                   # event names for the function-trace module
└── gdbinit
```

## Instrumenting your code

A component is instrumented from its own `CMakeLists.txt`, after `idf_component_register()`, by adding `-finstrument-functions` to its build. Guard it with the config so the flag is absent when the feature is off — otherwise the `__cyg_profile_*` hooks are undefined at link time:

```cmake
# whole component (see components/ft_demo/CMakeLists.txt)
if(CONFIG_ESP_TRACE_FUNCTION_TRACE)
    target_compile_options(${COMPONENT_LIB} PRIVATE -finstrument-functions)
endif()

# or only specific sources (see main/CMakeLists.txt)
if(CONFIG_ESP_TRACE_FUNCTION_TRACE)
    set_source_files_properties(my_file.c PROPERTIES
        COMPILE_OPTIONS "-finstrument-functions")
endif()
```

Instrumentation is applied per component, so you trace only your own code. ESP-IDF internals are not instrumented (this keeps event volume manageable and avoids tracing code that runs with the flash cache disabled).

To exclude individual files or functions from an otherwise-instrumented component, add the GCC blocklist flags:

```cmake
if(CONFIG_ESP_TRACE_FUNCTION_TRACE)
    target_compile_options(${COMPONENT_LIB} PRIVATE
        -finstrument-functions
        -finstrument-functions-exclude-file-list=ft_demo_hot,ft_demo_quiet
        -finstrument-functions-exclude-function-list=ft_demo_secret)
endif()
```

Each list is a comma-separated set of substrings matched against the source file path / function name, so a single entry can cover several files and you can list several at once.

## How recording starts

This example records when the **host** starts the SystemView session over JTAG (OpenOCD `mon esp sysview_mcore start`). Because `CONFIG_ESP_TRACE_FUNCTION_TRACE_AUTO_START` is enabled, no explicit `esp_trace_function_trace_start()` call is needed. Recording follows the encoder's recording state.

## Build, run and capture (JTAG + OpenOCD)

1. Connect a JTAG interface and [run OpenOCD](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html#run-openocd).

2. Build and flash with the JTAG config:

   ```
   idf.py set-target <target>
   idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.jtag" build flash monitor
   ```

3. Start tracing automatically from GDB using the provided `gdbinit` (it breaks at `app_main` and runs `mon esp sysview_mcore start`):

   ```
   riscv32-esp-elf-gdb -x gdbinit build/function_tracing.elf
   ```

   Replace the GDB binary with the one matching your target (e.g. `xtensa-esp32-elf-gdb`). Trace data is written to `/tmp/function_tracing.svdat`.

   This example uses `esp sysview_mcore`, which captures all cores into a single file in SEGGER's official multi-core format. It requires SystemView **v3.60 or later**. On a dual-core target the one file holds both cores. For older SystemView versions, use `mon esp sysview start <core0_file> [core1_file]` instead to write a separate file per core.

4. When enough data is captured, stop tracing:

   ```
   mon esp sysview_mcore stop
   ```

## Viewing in SystemView and naming the events

Open the `.svdat` file in the SEGGER SystemView application.

The function-trace events arrive as numeric module event IDs. To show them as names, copy this example's `SYSVIEW_FreeRTOS.txt` into your SystemView installation directory (or merge its entries):

```
512       function_enter      func=%p call_site=%p
513       function_exit       func=%p call_site=%p
```

`func` is the traced function start address. `call_site` is the caller return address. SystemView records both as raw addresses. Map them to function names offline against the ELF file (for example with `addr2line`).

The IDs are not chosen by the application: SystemView assigns each registered module an `EventOffset` (the first module gets `512`) and events are `EventOffset + index`. In this example the function-trace module is the only one registered, so it occupies `512-513`.

## Configuration

Function tracing is configured under **Component config → ESP Trace Configuration → Function Tracing** in `menuconfig`. Each option has built-in help. This example enables it and relies on the default `CONFIG_ESP_TRACE_FUNCTION_TRACE_AUTO_START` so recording follows the host session. File/function exclusion uses the `-finstrument-functions-exclude-*` compile flags (see [Instrumenting your code](#instrumenting-your-code)).

For the full feature description and option reference, see the [Compiler-Instrumented Function Tracing guide](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/tracing/function-tracing.html).

## Limitations

- Instrumentation adds overhead to every traced function call and increases code size and stack usage.
- Do not instrument code that runs with the flash cache disabled (IRAM ISRs, SPI flash operations). Keep instrumentation scoped to your own components.
