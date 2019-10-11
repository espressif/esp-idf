# SystemView Heap and Log Tracing Example

Heap memory leaking is quite widespread software bug. IDF provides [heap tracing feature](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/heap_debug.html#heap-tracing) which allows to collect information related to heap operations (allocations/deallocations) and detect potential memory leaks. This feature can be used in two modes: standalone and host-based. In standalone mode collected data are kept on-board, so this mode is limited by avaialable memory in the system. Host-based mode does not have such limitation because collected data are sent to the host and can be analysed there using special tools. One of such tool is SEGGER SystemView. For description of [SystemView tracing feature](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/app_trace.html#system-behaviour-analysis-with-segger-systemview) please refer to **ESP32 Programming Guide**, section **Application Level Tracing library**. SystemView is also can be useful to show log message sent from the target.
This example shows how to use this tool and IDF's scripts for host-based heap and log tracing analysis.

Consider the following situation. User program have two tasks. One task allocates memory and puts obtained addresses into the queue. Another task waits on that queue, reads sent pointers and frees memory. The first task queues only part of the pointers so some of the allocated blocks are not freed and become leaked. Both tasks uses IDF's logging API to report their actions. This example uses IDF's heap tracing module to record allocations and deallocations to detect memory leaks. Both heap tracing records and log mesages are redirected to the host.

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
$IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -p /tmp/heap_log.svdat build/sysview_tracing_heap_log.elf
```

Below is the sample scripts output.

```
Parse trace from '/tmp/heap_log.svdat'...
Stop parsing trace. (Timeout 0.000000 sec while reading 1 bytes!)
Process events from '['/tmp/heap_log.svdat']'...
[0.002244575] HEAP: Allocated 1 bytes @ 0x3ffaffd8 from task "alloc" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.002258425] HEAP: Allocated 2 bytes @ 0x3ffaffe0 from task "alloc" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:48
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.002405000] LOG: I (264) example: Task[0x3ffb6cf4]: allocated 2 bytes @ 0x3ffaffe0
[0.002553425] LOG: I (265) example: Task[0x3ffb7660]: free memory @ 0x3ffaffe0
[0.002563725] HEAP: Freed bytes @ 0x3ffaffe0 from task "free" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:31 (discriminator 9)
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.002782950] HEAP: Freed bytes @ 0x3ffb40b8 from task "main" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:4590
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:4590

[0.002798700] HEAP: Freed bytes @ 0x3ffb50bc from task "main" on core 0 by:
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:4590
/home/user/projects/esp/esp-idf/components/freertos/tasks.c:4590

[0.102436025] HEAP: Allocated 2 bytes @ 0x3ffaffe0 from task "alloc" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.102449800] HEAP: Allocated 4 bytes @ 0x3ffaffe8 from task "alloc" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:48
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.102536400] LOG: I (365) example: Task[0x3ffb6cf4]: allocated 4 bytes @ 0x3ffaffe8
[0.102655625] LOG: I (365) example: Task[0x3ffb7660]: free memory @ 0x3ffaffe8
[0.102666150] HEAP: Freed bytes @ 0x3ffaffe8 from task "free" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:31 (discriminator 9)
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.202436200] HEAP: Allocated 3 bytes @ 0x3ffaffe8 from task "alloc" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.202451725] HEAP: Allocated 6 bytes @ 0x3ffafff0 from task "alloc" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:48
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.202538225] LOG: I (465) example: Task[0x3ffb6cf4]: allocated 6 bytes @ 0x3ffafff0
[0.202654475] LOG: I (465) example: Task[0x3ffb7660]: free memory @ 0x3ffafff0
[0.202667075] HEAP: Freed bytes @ 0x3ffafff0 from task "free" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:31 (discriminator 9)
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.302436000] HEAP: Allocated 4 bytes @ 0x3ffafff0 from task "alloc" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.302451475] HEAP: Allocated 8 bytes @ 0x3ffb40b8 from task "alloc" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:48
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.302540900] LOG: I (565) example: Task[0x3ffb6cf4]: allocated 8 bytes @ 0x3ffb40b8
[0.302657050] LOG: I (565) example: Task[0x3ffb7660]: free memory @ 0x3ffb40b8
[0.302667500] HEAP: Freed bytes @ 0x3ffb40b8 from task "free" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:31 (discriminator 9)
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

Processing completed.
Processed 1027 events
=============== LOG TRACE REPORT ===============
Processed 8 log messages.
=============== HEAP TRACE REPORT ===============
Processed 14 heap events.
[0.002244575] HEAP: Allocated 1 bytes @ 0x3ffaffd8 from task "alloc" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.102436025] HEAP: Allocated 2 bytes @ 0x3ffaffe0 from task "alloc" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.202436200] HEAP: Allocated 3 bytes @ 0x3ffaffe8 from task "alloc" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

[0.302436000] HEAP: Allocated 4 bytes @ 0x3ffafff0 from task "alloc" on core 0 by:
/home/user/projects/esp/esp-idf/examples/system/sysview_tracing_heap_log/main/sysview_heap_log.c:47
/home/user/projects/esp/esp-idf/components/freertos/port.c:355 (discriminator 1)

Found 10 leaked bytes in 4 blocks.
```
