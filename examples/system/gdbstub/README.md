| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# GDBstub example

This example shows how to use gdbstub and it's functionality at runtime to debug an application with GDB.
With the gdbstub component it is possible to run GDB from IDF Monitor by pressing Ctrl+C and debug
the application using GDB. It is also possible to read/modify memory values, interrupt and continue
the application, set breakpoints, make steps and so on.
Upon exit from GDB, the application will continue to work in IDF Monitor as before.

## How to use example
### Hardware Required

The example can run on any commonly available ESP32 development board.
There are two possible ways to execute gdbstub with GDB: from IDF Monitor and as standalone application.
GDBStub is supported for all ESP chips.

NOTE: On chips with an integrated USB Serial/JTAG Controller, it is reasonable to use OpenOCD + GDB for debugging.

### Configure the project

By default, the example is already pre-configured, but the user can change configuration options with the following command:
```
idf.py menuconfig
```
Current example is pre-configured. The user can scroll through the system parameters and see the settings.
Most important one is:
-> Component Config -> GDB Stub -> GDBStub on runtime
This selection switches gdbstub to runtime mode.
Using another options in this menu, the user can also enable or disable task list handling and define a maximum amount of tasks.

### Build and Flash

Build the project and flash it to the board, then run IDF Monitor to view the serial output:

```
idf.py -p PORT flash monitor
```
Replace PORT with the name of the serial port to use, for example COM4 for Windows or /dev/ttyUSB0 for Linux.
To exit the serial monitor, type ``Ctrl-]``.
See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

In addition, it is also possible to run GDB and connect to the Esp32 directly, without IDF Monitor.
```
xtensa-esp32-elf-gdb ./build/gdbstub.elf -ex "set serial baud 115200" -ex "target remote \\.\COM10"
```
This will execute GDB and GDB will connect to your Esp32 by serial port COM10 with baudrate 115200.

## Example Output

The example demonstrates how to switch to GDB, watch values, change values, continue to run, and exit from GDB to the application.
To switch to GDB, the user presses Ctrl+C. This will stop the application and run the GDB.
In GDB, the user can print values "print call_count" and "print update_log_level" and then
change them  "set call_count 100" and "set update_log_level = ESP_LOG_WARN".
The user can continue running the application in GDB by entering "continue" and then interrupt the application by pressing Ctrl+C.
The user can check again that the application has worked by checking variable "print call_count".
The user can exit from GDB to continue seeing the trace from IDF Monitor by pressing "quit" and then "y".
The user will see in IDF Monitor that call_count and logging level have changed.
The user can add breakpoint to the label test_point2 by entering "break test_point2" and then enter "continue" or "c". The application will break at this line.
If user will continue again, the application will break at this line again.
Also, user can try to step application by entering "si".
A typical console output for such a scenario is shown below:
```
I (312) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
Hello GDB example!
CPU 0: To enter GDB please press "Ctrl+C"
I (4317) gdbstub_example: INFO  mode enabled. Task 0, Core ID 0, Call - 0.
W (4317) gdbstub_example: WARN  mode enabled. Task 0, Core ID 0, Call - 0.
CPU 1: To enter GDB please press "Ctrl+C"
I (4327) gdbstub_example: INFO  mode enabled. Task 1, Core ID 1, Call - 0.
W (4337) gdbstub_example: WARN  mode enabled. Task 1, Core ID 1, Call - 0.
CPU 0: To enter GDB please press "Ctrl+C"
I (5137) gdbstub_example: INFO  mode enabled. Task 0, Core ID 0, Call - 1.
W (5137) gdbstub_example: WARN  mode enabled. Task 0, Core ID 0, Call - 1.
CPU 1: To enter GDB please press "Ctrl+C"
I (5157) gdbstub_example: INFO  mode enabled. Task 1, Core ID 1, Call - 1.
W (5157) gdbstub_example: WARN  mode enabled. Task 1, Core ID 1, Call - 1.
To exit from IDF monitor please use "Ctrl+]"
$T02#b6GNU gdb (crosstool-NG esp-2020r3) 8.1.0.20180627-git
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "--host=x86_64-host_w64-mingw32 --target=xtensa-esp32-elf".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from c:\esp-idf\examples\system\gdbstub\build\gdbstub.elf...done.
Remote debugging using \\.\COM16
task4test (param=0x0) at ../main/gdbstub_main.c:41
41                  asm("               nop;");
(gdb) print call_counts[0]
$1 = 2
(gdb) print call_counts[1]
$2 = 2
(gdb) set call_counts[0] = 100
(gdb) set call_counts[0] = 100(gdb) print call_counts[0]
$3 = 100
(gdb) print update_log_level
$4 = ESP_LOG_DEBUG
(gdb) set update_log_level = ESP_LOG_WARN
(gdb) print update_log_level
$5 = ESP_LOG_WARN
(gdb) c
Continuing.
CPU 0: To enter GDB please press "Ctrl+C"
W (5927) gdbstub_example: WARN  mode enabled. Task 0, Core ID 0, Call - 100.
CPU 1: To enter GDB please press "Ctrl+C"
W (5927) gdbstub_example: WARN  mode enabled. Task 1, Core ID 1, Call - 2.
CPU 0: To enter GDB please press "Ctrl+C"
W (5927) gdbstub_example: WARN  mode enabled. Task 0, Core ID 0, Call - 101.
CPU 1: To enter GDB please press "Ctrl+C"
W (5927) gdbstub_example: WARN  mode enabled. Task 1, Core ID 1, Call - 3.
CPU 0: To enter GDB please press "Ctrl+C"
W (5927) gdbstub_example: WARN  mode enabled. Task 0, Core ID 0, Call - 102.
CPU 1: To enter GDB please press "Ctrl+C"
W (5927) gdbstub_example: WARN  mode enabled. Task 1, Core ID 1, Call - 4.

Thread 1 received signal SIGINT, Interrupt.
task4test (param=0x0) at ../main/gdbstub_main.c:38
38              for (int i=0 ; i< 10000000; i++)
(gdb) break test_point2
Breakpoint 1 at 0x400d511d: file ../main/gdbstub_main.c, line 40.
(gdb) c
Continuing.

Thread 1 hit Breakpoint 1, task4test (param=0x1) at ../main/gdbstub_main.c:40
40                  asm("test_point2:   nop;");
(gdb) si
41                  asm("               nop;");
(gdb) si
42                  asm("               nop;");
(gdb) c
Continuing.

Thread 1 hit Breakpoint 1, task4test (param=0x1) at ../main/gdbstub_main.c:40
40                  asm("test_point2:   nop;");
(gdb) delete 1
(gdb) c
Continuing.
CPU 0: To enter GDB please press "Ctrl+C"
W (36077) gdbstub_example: WARN  mode enabled. Task 0, Core ID 0, Call - 103.
CPU 1: To enter GDB please press "Ctrl+C"
W (36107) gdbstub_example: WARN  mode enabled. Task 1, Core ID 1, Call - 5.
CPU 0: To enter GDB please press "Ctrl+C"
W (36917) gdbstub_example: WARN  mode enabled. Task 0, Core ID 0, Call - 104.
CPU 1: To enter GDB please press "Ctrl+C"
W (36947) gdbstub_example: WARN  mode enabled. Task 1, Core ID 1, Call - 6.

Thread 1 received signal SIGINT, Interrupt.
task4test (param=0x0) at ../main/gdbstub_main.c:38
38              for (int i=0 ; i< 10000000; i++)
(gdb)
```

To reproduce this scenario run the application by: idf.py -P PORT flash monitor
Then:
1. Interrupt the application by pressing Ctrl+C
2. In GDB, print the application values by typing in GDB command line "print call_count" or "print update_log_level"
3. Modify the application values by typing in GDB command line "set call_count = 100" or "set update_log_level = ESP_LOG_WARN"
4. Continue the application by typing in GDB command line "continue"
5. Interrupt application by pressing Ctrl+C
6. Check the value by typing in GDB command line "print call_count" or "print update_log_level"
7. Continue the application by typing in GDB command line "continue"
8. Interrupt the application by pressing Ctrl+C
9. Add breakpoint by typing in GDB command line "break test_point2" or break 40 (break at line 40).
10. Continue the application by typing in GDB command line "continue"
11. After application stop at label "test_point2".
12. Make stepping by typing "si"
13. To exit from GDB to monitor type "exit" and press "y"

To exit from monitor please use Ctrl+]

