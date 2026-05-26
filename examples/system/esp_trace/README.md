| Supported Targets | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | --------- |

# ESP Trace External Library Integration Example

This example shows the **minimal** set of files and configuration needed to plug a third-party trace library into the [`esp_trace`](../../../components/esp_trace) component using the public `CONFIG_ESP_TRACE_LIB_EXTERNAL` extension point. It is meant as a copy-paste starting point for vendors and users who want to integrate their own trace recorder (e.g. Percepio TraceRecorder, a custom CTF emitter, a printf-style logger, …) without patching ESP-IDF itself.

The example covers:

* How to expose a custom **encoder** to `esp_trace` via `ESP_TRACE_REGISTER_ENCODER()`.
* How to provide an **`esp_trace_freertos_impl.h`** header that injects your trace hooks into FreeRTOS without breaking the `FreeRTOSConfig.h` include chain.
* How to wire everything up in **CMake** so the registration is not stripped by the linker, and so your header is visible to the FreeRTOS kernel.
* How to override the trace session parameters from the application via **`esp_trace_get_user_params()`**.

## How to Use

### Hardware Required

By default this example targets devices with built-in USB Serial JTAG (ESP32-C3/C5/C6/C61/H2/P4/S3, …). For other transports, see [Changing the Transport](#changing-the-transport).

You only need a development board and a USB cable.

### Configure the Project

```
idf.py set-target <esp_target>
idf.py menuconfig
```

The defaults in [`sdkconfig.defaults`](sdkconfig.defaults) already enable everything the example needs:

```ini
CONFIG_ESP_TRACE_ENABLE=y
CONFIG_ESP_TRACE_LIB_EXTERNAL=y                # use an external encoder
CONFIG_ESP_TRACE_TRANSPORT_USB_SERIAL_JTAG=y   # transport: built-in USB-Serial-JTAG
CONFIG_ESP_TRACE_TS_SOURCE_ESP_TIMER=y         # timestamp source
CONFIG_ESP_CONSOLE_SECONDARY_NONE=y            # free up USB-Serial-JTAG for trace
```

`CONFIG_ESP_CONSOLE_SECONDARY_NONE=y` is required so the USB-Serial-JTAG peripheral is not claimed by the secondary console — otherwise the transport option is not selectable.

### Build, Flash, and Monitor

```
idf.py -p PORT flash monitor
```

You should see the app start up and create a task. Whatever trace bytes your encoder produces will be emitted over the configured transport — in this example, the encoder writes a short string to the transport on every task switch.

(To exit the serial monitor, type `Ctrl-]`.)

## Project Layout

```
esp_trace/
├── CMakeLists.txt
├── sdkconfig.defaults
├── main/
│   ├── CMakeLists.txt
│   └── app_main.c                              # overrides esp_trace_get_user_params()
└── components/
    └── ext_trace_lib/                          # the external trace library component
        ├── CMakeLists.txt                      # WHOLE_ARCHIVE + freertos include trick
        ├── include/
        │   ├── esp_trace_freertos_impl.h       # entry point pulled in by FreeRTOSConfig.h
        │   └── trace_FreeRTOS.h                # trace*() macros + forward declarations
        └── src/
            ├── adapter_encoder_ext_trace_lib.c # vtable + ESP_TRACE_REGISTER_ENCODER()
            └── trace_FreeRTOS.c                # hook implementations (may include FreeRTOS.h)
```

## How the Integration Works

### 1. Selecting the external library

`CONFIG_ESP_TRACE_LIB_EXTERNAL=y` tells `esp_trace` that the encoder lives in a separate component. Internally, `CONFIG_ESP_TRACE_LIB_NAME` resolves to `"ext"`. You can either:

* register your encoder under that default name — `ESP_TRACE_REGISTER_ENCODER("ext", &vt);` — and the system picks it up automatically, **or**
* register under any name you like (this example uses `"ext_trace_lib"`) and override the session parameters at runtime via `esp_trace_get_user_params()`. See [`main/app_main.c`](main/app_main.c):

  ```c
  esp_trace_open_params_t esp_trace_get_user_params(void)
  {
      esp_trace_open_params_t trace_params = {
          .core_cfg = NULL,
          .encoder_name = "ext_trace_lib",
          .encoder_cfg = NULL,
          .transport_name = "usb_serial_jtag",
          .transport_cfg = NULL,
      };
      return trace_params;
  }
  ```

### 2. Providing the FreeRTOS trace hooks

`esp_trace`'s public header [`esp_trace_freertos.h`](../../../components/esp_trace/include/esp_trace_freertos.h) is included from `FreeRTOSConfig.h`. When `CONFIG_ESP_TRACE_LIB_EXTERNAL=y` is set, it pulls in **your** `esp_trace_freertos_impl.h`:

```c
#if CONFIG_ESP_TRACE_LIB_EXTERNAL
#include "esp_trace_freertos_impl.h"
#endif
```

The example splits the contract into two files:

* [`esp_trace_freertos_impl.h`](components/ext_trace_lib/include/esp_trace_freertos_impl.h) — a one-line shim that pulls in `trace_FreeRTOS.h`.
* [`trace_FreeRTOS.h`](components/ext_trace_lib/include/trace_FreeRTOS.h) — defines only the `trace*()` macros this example actually hooks into, plus forward declarations of the helper functions called from them. **No FreeRTOS includes.** Anything left undefined here falls back to FreeRTOS's own empty default (every trace macro is guarded by `#ifndef traceXXX / #define traceXXX() / #endif` in `freertos/FreeRTOS.h`), so you only need to declare what you actually intercept. Trace macros are allowed to reference FreeRTOS identifiers like `pxTCB` or `xTicksToWait` by name — they are resolved later, when the macro is expanded inside the FreeRTOS kernel `.c` files where those names are already in scope.

The actual hook implementation lives in [`trace_FreeRTOS.c`](components/ext_trace_lib/src/trace_FreeRTOS.c) and is free to `#include "freertos/FreeRTOS.h"`. By the time a `.c` file is compiled, `FreeRTOSConfig.h` has been fully parsed.

### 3. Registering the encoder

[`adapter_encoder_ext_trace_lib.c`](components/ext_trace_lib/src/adapter_encoder_ext_trace_lib.c) implements the encoder vtable (`init`, `write`, `panic_handler`) and registers it at link time:

```c
ESP_TRACE_REGISTER_ENCODER("ext_trace_lib", &s_ext_trace_lib_vt);
```

The registration places a descriptor into a dedicated linker section that `esp_trace_core` scans during startup. Because nothing in the application references that descriptor directly, the linker would normally garbage-collect it — `WHOLE_ARCHIVE TRUE` in the component's `CMakeLists.txt` prevents that.

### 4. CMake setup

[`components/ext_trace_lib/CMakeLists.txt`](components/ext_trace_lib/CMakeLists.txt) shows the two pieces of CMake plumbing every external trace library needs:

```cmake
if(CONFIG_ESP_TRACE_LIB_EXTERNAL)
    idf_component_register(SRC_DIRS ${src_dirs}
                           INCLUDE_DIRS ${include_dirs}
                           PRIV_REQUIRES esp_trace
                           WHOLE_ARCHIVE TRUE)            # keep ESP_TRACE_REGISTER_* symbols

    # Expose esp_trace_freertos_impl.h to the freertos component
    idf_component_get_property(freertos_lib freertos COMPONENT_LIB)
    target_include_directories(${freertos_lib} INTERFACE ${include_dirs})
else()
    idf_component_register(PRIV_REQUIRES esp_trace)
endif()
```

The second `target_include_directories(...)` call is what makes `esp_trace_freertos_impl.h` resolvable from inside the FreeRTOS kernel's translation units.

## Changing the Transport

The example defaults to USB Serial JTAG. To use a different transport, edit `sdkconfig.defaults` (or run `idf.py menuconfig` → *Component config → ESP Trace Configuration → Trace transport*):

| Transport | Config | Notes |
| --- | --- | --- |
| USB Serial JTAG | `CONFIG_ESP_TRACE_TRANSPORT_USB_SERIAL_JTAG=y` | Default. Requires `ESP_CONSOLE_SECONDARY_NONE=y`. |
| apptrace over JTAG | `CONFIG_ESP_TRACE_TRANSPORT_APPTRACE=y` + `CONFIG_APPTRACE_DEST_JTAG=y` | Needs OpenOCD on the host to drain the buffer. |
| apptrace over UART | `CONFIG_ESP_TRACE_TRANSPORT_APPTRACE=y` + `CONFIG_APPTRACE_DEST_UART=y` | Pick a UART different from the console. |
| External transport | `CONFIG_ESP_TRACE_TRANSPORT_EXTERNAL=y` | Another component must register a transport with `ESP_TRACE_REGISTER_TRANSPORT(...)`. |
| None | `CONFIG_ESP_TRACE_TRANSPORT_NONE=y` | Useful if your library streams data over its own channel and just needs the FreeRTOS hooks. |

Don't forget to update the `transport_name` field in `esp_trace_get_user_params()` to match (e.g. `"apptrace"`, `"usb_serial_jtag"`, or your custom transport's registered name).

## What the Demo Emits

`encode()` in [`trace_FreeRTOS.c`](components/ext_trace_lib/src/trace_FreeRTOS.c) writes one line per trace event in the form:

```
[+   9933 us] ISR_IN       irq=63
[+     29 us] ISR_IN       irq=57
[+     21 us] ISR_YIELD
[+     16 us] ISR_OUT
[+   1234 us] Q_CREATE     q=0x3fc8a210
[+    167 us] TASK_IN      producer
[+     54 us] Q_SEND       q=0x3fc8a210
[+     32 us] TASK_IN      consumer
```

The leading number is the time elapsed since the previous traced event (microseconds when `CONFIG_ESP_TRACE_TS_SOURCE_ESP_TIMER` is selected). Eight FreeRTOS hooks are wired up — see the *active hooks* block at the top of [`trace_FreeRTOS.h`](components/ext_trace_lib/include/trace_FreeRTOS.h). The rest stay as no-ops (FreeRTOS still expects every `trace*()` macro to be defined).

`ISR_OUT` vs `ISR_YIELD` reflects how FreeRTOS leaves the interrupt: `ISR_OUT` when the handler returns to the interrupted task without scheduling, `ISR_YIELD` when it calls `portYIELD_FROM_ISR()` (triggering `traceISR_EXIT_TO_SCHEDULER`). On a busy SMP target the yield path dominates; on a mostly-idle single-core target the plain `ISR_OUT` path does.

Because the transport is USB-Serial-JTAG and the console is on UART (`CONFIG_ESP_CONSOLE_SECONDARY_NONE=y`), `idf.py monitor` shows ESP-IDF logs while the trace stream is on a separate USB endpoint — open it in any serial terminal (`screen /dev/cu.usbmodem...`, picocom, etc.) to read the output above.

## Runtime Control — `esp_trace_start` / `_stop` / `_flush`

[`esp_trace.h`](../../../components/esp_trace/include/esp_trace.h) exposes three generic lifecycle calls that dispatch to the active encoder's vtable. The application uses only the public API — it never reaches into the external library:

```c
esp_trace_start();    // resume emission (also resets the delta baseline)
// ... do stuff ...
esp_trace_stop();     // pause emission
esp_trace_flush();    // drain transport buffers
```

In this example the library boots with `s_enabled = false`, so nothing is emitted until `app_main()` calls `esp_trace_start()`. The trailing pair `esp_trace_flush(); esp_trace_stop();` makes sure the last events reach the host before the trace channel goes silent. Adapter wiring lives in [`adapter_encoder_ext_trace_lib.c`](components/ext_trace_lib/src/adapter_encoder_ext_trace_lib.c) (`start` / `stop` / `flush` callbacks); flush forwards to the transport's `flush_nolock`.

## Cross-Core Serialization

`encode()` wraps its body in the encoder's `take_lock` / `give_lock` vtable entries (an `esp_trace_lock_t` allocated in the adapter's `init()`). See [`trace_FreeRTOS.c`](components/ext_trace_lib/src/trace_FreeRTOS.c) and [`adapter_encoder_ext_trace_lib.c`](components/ext_trace_lib/src/adapter_encoder_ext_trace_lib.c).

## Other `esp_trace` Helpers

Beyond what this example uses, [`esp_trace.h`](../../../components/esp_trace/include/esp_trace.h) and [`esp_trace_util.h`](../../../components/esp_trace/include/esp_trace_util.h) also expose:

* `esp_trace_is_host_connected()` — gate expensive work when no host is listening.
* `esp_trace_get_link_type()` — returns `ESP_TRACE_LINK_DEBUG_PROBE`, `_UART`, or `_USB_SERIAL_JTAG`.
* `esp_trace_rb_*()` — power-of-2, FreeRTOS-free ring buffer for trace hot paths.
* `esp_trace_tmo_init/check()` — cooperative timeouts for flush loops.

## See Also

* [`components/esp_trace/README.md`](../../../components/esp_trace/README.md) — full architecture overview and adapter API reference.
* [`examples/system/sysview_tracing`](../sysview_tracing) — a production-grade integration of SEGGER SystemView built on the same extension points.
