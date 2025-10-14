| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Heap Task Tracking Basic Example

## Overview

The example creates a task which allocates random amount of memory and frees it and another task that allocates random amount of memory but never frees it.
The main then goes into a loop printing the overview information of each task that allocated memory dynamically.
The information include:
- The task name
- The task status
- The current memory usage
- The peak memory usage
- The number of heaps currently used by the task

Because the heap task tracking feature requires additional metadata to be allocated for each memory allocations, the overall heap usage of the application is
greater than when the feature is disabled. For this reason, it is highly recommended to use the task tracking for debugging purpose only.

### Configure the project

- Enable thee option `Enable heap task tracking` by opening the project configuration menu (`idf.py menuconfig`) and navigate to `Component config -> Heap memory debugging` menu.
- (optional) Enable the option `Keep information about the memory usage on deleted tasks` if you wish to keep track of the information of a task after it has been deleted.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build and flash the project..

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
Starting task: no_leak_task
Starting task: leaking_task

 PRINTING OVERVIEW STATISTICS OF EACH TASK
┌────────────────────┬─────────┬──────────────────────┬───────────────────┬─────────────────┐
│ TASK               │ STATUS  │ CURRENT MEMORY USAGE │ PEAK MEMORY USAGE │ TOTAL HEAP USED │
├────────────────────┼─────────┼──────────────────────┼───────────────────┼─────────────────┤
│       leaking_task │ ALIVE   │                 6656 │              8064 │               1 │
│       no_leak_task │ ALIVE   │                    0 │              7152 │               1 │
│               main │ ALIVE   │                 7412 │              7412 │               2 │
│               ipc1 │ ALIVE   │                   32 │                44 │               1 │
│               ipc0 │ ALIVE   │                10080 │             10092 │               1 │
│      Pre-scheduler │ ALIVE   │                 2236 │              2236 │               1 │
└────────────────────┴─────────┴──────────────────────┴───────────────────┴─────────────────┘

 PRINTING OVERVIEW STATISTICS OF NO LEAK TASK
┌────────────────────┬─────────┬──────────────────────┬───────────────────┬─────────────────┐
│ TASK               │ STATUS  │ CURRENT MEMORY USAGE │ PEAK MEMORY USAGE │ TOTAL HEAP USED │
├────────────────────┼─────────┼──────────────────────┼───────────────────┼─────────────────┤
│       no_leak_task │ ALIVE   │                    0 │              7152 │               1 │
└────────────────────┴─────────┴──────────────────────┴───────────────────┴─────────────────┘

 PRINTING OVERVIEW STATISTICS OF LEAKING TASK
┌────────────────────┬─────────┬──────────────────────┬───────────────────┬─────────────────┐
│ TASK               │ STATUS  │ CURRENT MEMORY USAGE │ PEAK MEMORY USAGE │ TOTAL HEAP USED │
├────────────────────┼─────────┼──────────────────────┼───────────────────┼─────────────────┤
│       leaking_task │ ALIVE   │                 6656 │              8064 │               1 │
└────────────────────┴─────────┴──────────────────────┴───────────────────┴─────────────────┘

[...]
Deleting task: leaking_task

 PRINTING OVERVIEW STATISTICS OF EACH TASK
┌────────────────────┬─────────┬──────────────────────┬───────────────────┬─────────────────┐
│ TASK               │ STATUS  │ CURRENT MEMORY USAGE │ PEAK MEMORY USAGE │ TOTAL HEAP USED │
├────────────────────┼─────────┼──────────────────────┼───────────────────┼─────────────────┤
│       leaking_task │ DELETED │                11392 │             11616 │               1 │
│       no_leak_task │ ALIVE   │                    0 │              9408 │               2 │
│               main │ ALIVE   │                 3860 │              7412 │               2 │
│               ipc1 │ ALIVE   │                   32 │                44 │               1 │
│               ipc0 │ ALIVE   │                10080 │             10092 │               1 │
│      Pre-scheduler │ ALIVE   │                 2236 │              2236 │               1 │
└────────────────────┴─────────┴──────────────────────┴───────────────────┴─────────────────┘

 PRINTING OVERVIEW STATISTICS OF NO LEAK TASK
┌────────────────────┬─────────┬──────────────────────┬───────────────────┬─────────────────┐
│ TASK               │ STATUS  │ CURRENT MEMORY USAGE │ PEAK MEMORY USAGE │ TOTAL HEAP USED │
├────────────────────┼─────────┼──────────────────────┼───────────────────┼─────────────────┤
│       no_leak_task │ ALIVE   │                    0 │              9408 │               2 │
└────────────────────┴─────────┴──────────────────────┴───────────────────┴─────────────────┘

 PRINTING OVERVIEW STATISTICS OF LEAKING TASK
┌────────────────────┬─────────┬──────────────────────┬───────────────────┬─────────────────┐
│ TASK               │ STATUS  │ CURRENT MEMORY USAGE │ PEAK MEMORY USAGE │ TOTAL HEAP USED │
├────────────────────┼─────────┼──────────────────────┼───────────────────┼─────────────────┤
│       leaking_task │ DELETED │                11392 │             11616 │               1 │
└────────────────────┴─────────┴──────────────────────┴───────────────────┴─────────────────┘
Deleting task: no_leak_task

 PRINTING OVERVIEW STATISTICS OF EACH TASK
┌────────────────────┬─────────┬──────────────────────┬───────────────────┬─────────────────┐
│ TASK               │ STATUS  │ CURRENT MEMORY USAGE │ PEAK MEMORY USAGE │ TOTAL HEAP USED │
├────────────────────┼─────────┼──────────────────────┼───────────────────┼─────────────────┤
│       leaking_task │ DELETED │                11392 │             11616 │               1 │
│       no_leak_task │ DELETED │                    0 │              9408 │               2 │
│               main │ ALIVE   │                  308 │              7412 │               2 │
│               ipc1 │ ALIVE   │                   32 │                44 │               1 │
│               ipc0 │ ALIVE   │                10080 │             10092 │               1 │
│      Pre-scheduler │ ALIVE   │                 2236 │              2236 │               1 │
└────────────────────┴─────────┴──────────────────────┴───────────────────┴─────────────────┘

 PRINTING DETAILED STATISTICS OF EACH TASK
├ DELETED: leaking_task, CURRENT MEMORY USAGE 11392, PEAK MEMORY USAGE 11616, TOTAL HEAP USED 1:
│    └ HEAP: RAM, CAPS: 0x0010580e, SIZE: 22308, USAGE: CURRENT 11392 (51%), PEAK 11616 (52%), ALLOC COUNT: 3
│         ├ ALLOC 0x3fcebbb8, SIZE 6656
│         ├ ALLOC 0x3fced5bc, SIZE 3584
│         ├ ALLOC 0x3fceb718, SIZE 1152
├ DELETED: no_leak_task, CURRENT MEMORY USAGE 0, PEAK MEMORY USAGE 9408, TOTAL HEAP USED 2:
│    ├ HEAP: RAM, CAPS: 0x0010580e, SIZE: 22308, USAGE: CURRENT 0 (0%), PEAK 7152 (32%), ALLOC COUNT: 0
│    └ HEAP: RAM, CAPS: 0x0010580e, SIZE: 344400, USAGE: CURRENT 0 (0%), PEAK 9216 (2%), ALLOC COUNT: 0
├ ALIVE: main, CURRENT MEMORY USAGE 308, PEAK MEMORY USAGE 7412, TOTAL HEAP USED 2:
│    ├ HEAP: RAM, CAPS: 0x0010580e, SIZE: 344400, USAGE: CURRENT 220 (0%), PEAK 220 (0%), ALLOC COUNT: 2
│    │    ├ ALLOC 0x3fc99024, SIZE 88
│    │    ├ ALLOC 0x3fc99124, SIZE 132
│    └ HEAP: RAM, CAPS: 0x0010580e, SIZE: 22308, USAGE: CURRENT 88 (0%), PEAK 7192 (32%), ALLOC COUNT: 5
│         ├ ALLOC 0x3fce99f8, SIZE 20
│         ├ ALLOC 0x3fce9a10, SIZE 12
│         ├ ALLOC 0x3fce9a20, SIZE 16
│         ├ ALLOC 0x3fce9a34, SIZE 20
│         ├ ALLOC 0x3fce9a4c, SIZE 20
├ ALIVE: ipc1, CURRENT MEMORY USAGE 32, PEAK MEMORY USAGE 44, TOTAL HEAP USED 1:
│    └ HEAP: RAM, CAPS: 0x0010580e, SIZE: 344400, USAGE: CURRENT 32 (0%), PEAK 44 (0%), ALLOC COUNT: 2
│         ├ ALLOC 0x3fc990fc, SIZE 16
│         ├ ALLOC 0x3fc991c0, SIZE 16
├ ALIVE: ipc0, CURRENT MEMORY USAGE 10080, PEAK MEMORY USAGE 10092, TOTAL HEAP USED 1:
│    └ HEAP: RAM, CAPS: 0x0010580e, SIZE: 344400, USAGE: CURRENT 10080 (2%), PEAK 10092 (2%), ALLOC COUNT: 10
│         ├ ALLOC 0x3fc966e0, SIZE 1312
│         ├ ALLOC 0x3fc96c80, SIZE 344
│         ├ ALLOC 0x3fc96e14, SIZE 16
│         ├ ALLOC 0x3fc96e3c, SIZE 4224
│         ├ ALLOC 0x3fc97ec0, SIZE 344
│         ├ ALLOC 0x3fc98030, SIZE 1568
│         ├ ALLOC 0x3fc98668, SIZE 344
│         ├ ALLOC 0x3fc987d8, SIZE 1568
│         ├ ALLOC 0x3fc98e10, SIZE 344
│         ├ ALLOC 0x3fc98f94, SIZE 16
└ ALIVE: Pre-scheduler, CURRENT MEMORY USAGE 2236, PEAK MEMORY USAGE 2236, TOTAL HEAP USED 1:
     └ HEAP: RAM, CAPS: 0x0010580e, SIZE: 344400, USAGE: CURRENT 2236 (0%), PEAK 2236 (0%), ALLOC COUNT: 11
          ├ ALLOC 0x3fc95cb0, SIZE 164
          ├ ALLOC 0x3fc95dd8, SIZE 12
          ├ ALLOC 0x3fc95dfc, SIZE 12
          ├ ALLOC 0x3fc95e20, SIZE 16
          ├ ALLOC 0x3fc95e48, SIZE 24
          ├ ALLOC 0x3fc95e78, SIZE 88
          ├ ALLOC 0x3fc95ee8, SIZE 88
          ├ ALLOC 0x3fc95f58, SIZE 88
          ├ ALLOC 0x3fc95fc8, SIZE 88
          ├ ALLOC 0x3fc96038, SIZE 1312
          ├ ALLOC 0x3fc96570, SIZE 344
I (5949) main_task: Returned from app_main()
```
