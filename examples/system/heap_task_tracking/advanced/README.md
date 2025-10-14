| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Heap Task Tracking Basic Example

## Overview

The example creates a task which allocates random amount of memory and frees it and another task that allocates random amount of memory but never frees it.
The main then goes into a loop calling functions retrieving statistics for the "no leak" task, the "leaking" tasks and all tasks and printing them.
For each tasks, the following information is retrieved and printed:
    - the task name
    - the task status (running or deleted)
    - the overall peak memory usage of the task 
    - the overall current memory usage of the task
For each heap used by a given task, the following information is printed:
    - the heap name
    - the heap caps
    - the heap size
    - the heap current memory usage by the task
    - the heap peak memory usage by the task
    - the number of blocks currently allocated in the heap by the task
For each block of memory allocated in a given heap by a given task, the following information is printed:
    - the allocation address
    - the allocation size

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
--------------------------------------------------------------------------------
PRINTING SINGLE TASK INFO
--------------------------------------------------------------------------------
no_leak_task: ALIVE  : Peak Usage 10128, Current Usage 0
    RAM: Caps: 1071118. Size 22308, Current Usage 0, Peak Usage 10128, alloc count 0

--------------------------------------------------------------------------------
PRINTING SINGLE TASK INFO
--------------------------------------------------------------------------------
leaking_task: ALIVE  : Peak Usage 7232, Current Usage 6656
    RAM: Caps: 1071118. Size 22308, Current Usage 6656, Peak Usage 7232, alloc count 1
        0x3fceb878: Size: 6656

--------------------------------------------------------------------------------
PRINTING SINGLE TASK INFO
--------------------------------------------------------------------------------
no_leak_task: ALIVE  : Peak Usage 10128, Current Usage 0
    RAM: Caps: 1071118. Size 22308, Current Usage 0, Peak Usage 10128, alloc count 0
    RAM: Caps: 1071118. Size 14832, Current Usage 0, Peak Usage 8960, alloc count 0

--------------------------------------------------------------------------------
PRINTING SINGLE TASK INFO
--------------------------------------------------------------------------------
leaking_task: ALIVE  : Peak Usage 15040, Current Usage 9664
    RAM: Caps: 1071118. Size 22308, Current Usage 6656, Peak Usage 12032, alloc count 1
        0x3fceb878: Size: 6656
    RAM: Caps: 1071118. Size 14832, Current Usage 3008, Peak Usage 3008, alloc count 1
        0x3fc9a0e4: Size: 3008

[...]

--------------------------------------------------------------------------------
PRINTING ALL TASKS INFO
--------------------------------------------------------------------------------
leaking_task: DELETED: Peak Usage 19248, Current Usage 13616
    RAM: Caps: 1071118. Size 22308, Current Usage 10608, Peak Usage 13296, alloc count 3
        0x3fceb878: Size: 6656
        0x3fceb634: Size: 368
        0x3fcedd00: Size: 3584
    RAM: Caps: 1071118. Size 14832, Current Usage 3008, Peak Usage 12224, alloc count 1
        0x3fc9a0e4: Size: 3008
no_leak_task: DELETED: Peak Usage 10128, Current Usage 0
    RAM: Caps: 1071118. Size 22308, Current Usage 0, Peak Usage 10128, alloc count 0
    RAM: Caps: 1071118. Size 14832, Current Usage 0, Peak Usage 9728, alloc count 0
main: ALIVE  : Peak Usage 7456, Current Usage 352
    RAM: Caps: 1071118. Size 14832, Current Usage 264, Peak Usage 264, alloc count 3
        0x3fc99cf4: Size: 88
        0x3fc99e1c: Size: 88
        0x3fc99e78: Size: 88
    RAM: Caps: 1071118. Size 22308, Current Usage 88, Peak Usage 7192, alloc count 5
        0x3fce99f8: Size: 20
        0x3fce9a10: Size: 12
        0x3fce9a20: Size: 16
        0x3fce9a34: Size: 20
        0x3fce9a4c: Size: 20
ipc1: ALIVE  : Peak Usage 44, Current Usage 32
    RAM: Caps: 1071118. Size 14832, Current Usage 32, Peak Usage 44, alloc count 2
        0x3fc99dcc: Size: 16
        0x3fc99df4: Size: 16
ipc0: ALIVE  : Peak Usage 10092, Current Usage 10080
    RAM: Caps: 1071118. Size 14832, Current Usage 10080, Peak Usage 10092, alloc count 10
        0x3fc973b0: Size: 1312
        0x3fc97950: Size: 344
        0x3fc97ae4: Size: 16
        0x3fc97b0c: Size: 4224
        0x3fc98b90: Size: 344
        0x3fc98d00: Size: 1568
        0x3fc99338: Size: 344
        0x3fc994a8: Size: 1568
        0x3fc99ae0: Size: 344
        0x3fc99c64: Size: 16
Pre-scheduler: ALIVE  : Peak Usage 3364, Current Usage 3364
    RAM: Caps: 1071118. Size 14832, Current Usage 3364, Peak Usage 3364, alloc count 22
        0x3fc96410: Size: 164
        0x3fc96538: Size: 12
        0x3fc9655c: Size: 12
        0x3fc96580: Size: 16
        0x3fc965a8: Size: 24
        0x3fc965d8: Size: 36
        0x3fc96614: Size: 40
        0x3fc96654: Size: 36
        0x3fc96690: Size: 40
        0x3fc966d0: Size: 88
        0x3fc96740: Size: 88
        0x3fc967b0: Size: 88
        0x3fc96820: Size: 432
        0x3fc969e8: Size: 88
        0x3fc96a58: Size: 88
        0x3fc96ac8: Size: 88
        0x3fc96b38: Size: 132
        0x3fc96bd4: Size: 132
        0x3fc96c70: Size: 88
        0x3fc96ce0: Size: 16
        0x3fc96d08: Size: 1312
        0x3fc97240: Size: 344
I (4504) main_task: Returned from app_main()
```
