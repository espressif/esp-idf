| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# SystemView Heap and Log Tracing Example

Heap memory leaking is quite widespread software bug. IDF provides [heap tracing feature](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/heap_debug.html#heap-tracing) which allows to collect information related to heap operations (allocations/deallocations) and detect potential memory leaks. This feature can be used in two modes: standalone and host-based. In standalone mode collected data are kept on-board, so this mode is limited by available memory in the system. Host-based mode does not have such limitation because collected data are sent to the host and can be analysed there using special tools. One of such tool is SEGGER SystemView. For description of [SystemView tracing feature](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#system-behaviour-analysis-with-segger-systemview) please refer to **ESP32 Programming Guide**, section **Application Level Tracing library**. SystemView is also can be useful to show log message sent from the target.
This example shows how to use this tool and IDF's scripts for host-based heap and log tracing analysis.

Consider the following situation. User program have two tasks. One task allocates memory and puts obtained addresses into the queue. Another task waits on that queue, reads sent pointers and frees memory. The first task queues only part of the pointers so some of the allocated blocks are not freed and become leaked. Both tasks uses IDF's logging API to report their actions. This example uses IDF's heap tracing module to record allocations and deallocations to detect memory leaks. Both heap tracing records and log messages are redirected to the host.

## How to use example

### Hardware and tools required

This example does not require any special hardware, and can be run on any common development board.
This example requires the following tools:
1. [OpenOCD](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html#run-openocd).
NOTE: In order to run this example you need OpenOCD version `v0.10.0-esp32-20190313` or later.

2. [GDB](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#setup-toolchain) can be used to start and/or stop tracing automatically. To do this you need to prepare special GDB command file. Having provided with `gdbinit` file from the example project directory GDB will connect to the target, reset it, start and stop tracing automatically.
when program hits breakpoint at `heap_trace_start`. Trace data will be saved to `/tmp/hesp_log.svdat`. Tracing will be stopped when program hits breakpoint at `heap_trace_stop`.

3. [SEGGER SystemView tool](https://www.segger.com/products/development-tools/systemview/). By default SystemView shows only numeric values of IDs and parameters for IDF's heap messages in `Events` view. To make them pretty-looking you need to copy `SYSVIEW_FreeRTOS.txt` from the project's root directory to SystemView installation one.

### Build and flash

```
idf.py -p PORT flash monitor
```

(Replace PORT with serial port name.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### Collect And View Trace Data

To run the example and collect trace data:

1. Run GDB using the following command from the project root directory:

    ```
    xtensa-esp32-elf-gdb -x gdbinit build/sysview_tracing_heap_log.elf
    ```

2. When program stops at `heap_trace_stop`  quit GDB.

3. Open trace data file in SystemView tool.

4. Now you can inspect all collected events. Log messages are shown in `Terminal` view.

5. You can filter out API related and heap events by right-clicking on any item in `Events` view and select `Show APIs only`.

### Auto-detect Heap Leaks

Since SystemView tool is mostly intended for OS level analysis. It allows just to inspect custom events' timestamps and parameters. So it can require some efforts to analyse heap operations flow. IDF provides special script to make the life a bit more easy. This script parses SystemView trace file sand reports detected memory leaks. The script also prints found log messages. To run it type the following from the project root directory:

```
$IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -p -b build/sysview_tracing_heap_log.elf /tmp/heap_log.svdat
```

Below is the sample scripts output.

```
[0.002272225] HEAP: Allocated 8 bytes @ 0x3ffaff6c from task "main" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:85
/home/user/projects/esp/esp-idf/components/esp32/cpu_start.c:570

[0.002307300] HEAP: Allocated 2500 bytes @ 0x3ffb580c from task "main" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:804
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:84 (discriminator 2)

[0.002323775] HEAP: Allocated 356 bytes @ 0x3ffb61d4 from task "main" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:809
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:84 (discriminator 2)

[0.002427700] HEAP: Allocated 120 bytes @ 0x3ffaff78 from task "alloc0" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/queue.c:391
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:46

[0.002471225] HEAP: Allocated 2500 bytes @ 0x3ffb633c from task "alloc0" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:804
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:55

[0.002487725] HEAP: Allocated 356 bytes @ 0x3ffb6d04 from task "alloc0" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:809
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:55

[0.002569725] HEAP: Allocated 2 bytes @ 0x3ffafff4 from task "alloc0" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:59
/home/user/projects/esp/esp-idf/components/freertos/port.c:145

[0.002835275] LOG: I (298) example: Task[0x3ffb61d4]: allocated 2 bytes @ 0x3ffafff4
[0.002974600] LOG: I (299) example: Task[0x3ffb6d04]: free memory @ 0x3ffafff4

....

[2.942891550] LOG: I (3239) example: Task[0x3ffb7840]: allocated 396 bytes @ 0x3ffb9d08
[2.943024150] LOG: I (3239) example: Task[0x3ffb6d04]: free memory @ 0x3ffb9c3c
[2.943035600] HEAP: Freed bytes @ 0x3ffb9c3c from task "free0" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:24 (discriminator 9)
/home/user/projects/esp/esp-idf/components/freertos/port.c:145

[2.943212125] LOG: I (3239) example: Task[0x3ffb83ec]: free memory @ 0x3ffb9d08
[2.943223500] HEAP: Freed bytes @ 0x3ffb9d08 from task "free1" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:24 (discriminator 9)
/home/user/projects/esp/esp-idf/components/freertos/port.c:145

[2.943649025] HEAP: Allocated 594 bytes @ 0x3ffb9c3c from task "alloc2" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:59
/home/user/projects/esp/esp-idf/components/freertos/port.c:145

[2.943734250] LOG: I (3240) example: Task[0x3ffb8f28]: allocated 594 bytes @ 0x3ffb9c3c
[2.943867850] LOG: I (3240) example: Task[0x3ffb9ad4]: free memory @ 0x3ffb9c3c
[2.943879200] HEAP: Freed bytes @ 0x3ffb9c3c from task "free2" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:24 (discriminator 9)
/home/user/projects/esp/esp-idf/components/freertos/port.c:145

[2.972813425] LOG: I (3269) example: Got notify val 2
[2.972870400] LOG: I (3269) example: Wait notify 1
[2.972932800] LOG: I (3269) example: Got notify val 1
[2.972989825] LOG: I (3269) example: Wait notify 2
[2.973756125] LOG: I (3270) example: Got notify val 1
Processed 13467 events
=============== LOG TRACE REPORT ===============
Processed 600 log messages.
=============== HEAP TRACE REPORT ===============
Processed 612 heap events.
[0.002272225] HEAP: Allocated 8 bytes @ 0x3ffaff6c from task "main" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:85
/home/user/projects/esp/esp-idf/components/esp32/cpu_start.c:570

[0.002307300] HEAP: Allocated 2500 bytes @ 0x3ffb580c from task "main" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:804
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:84 (discriminator 2)

[0.002323775] HEAP: Allocated 356 bytes @ 0x3ffb61d4 from task "main" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:809
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:84 (discriminator 2)

[0.002427700] HEAP: Allocated 120 bytes @ 0x3ffaff78 from task "alloc0" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/queue.c:391
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:46

[0.002471225] HEAP: Allocated 2500 bytes @ 0x3ffb633c from task "alloc0" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:804
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:55

[0.002487725] HEAP: Allocated 356 bytes @ 0x3ffb6d04 from task "alloc0" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:809
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:55

[0.003102175] HEAP: Allocated 8 bytes @ 0x3ffb6e6c from task "main" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:85
/home/user/projects/esp/esp-idf/components/esp32/cpu_start.c:570

....

[0.003713175] HEAP: Allocated 356 bytes @ 0x3ffb8f28 from task "main" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:809
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:84 (discriminator 2)

[0.003814375] HEAP: Allocated 120 bytes @ 0x3ffb9090 from task "alloc2" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/queue.c:391
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:46

[0.003845875] HEAP: Allocated 2500 bytes @ 0x3ffb910c from task "alloc2" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:804
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:55

[0.003862350] HEAP: Allocated 356 bytes @ 0x3ffb9ad4 from task "alloc2" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:809
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/build/../main/sysview_heap_log.c:55

Found 17520 leaked bytes in 18 blocks.
```
