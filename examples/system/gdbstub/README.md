# GDBstub example

This example shows how to use gdbstub and it's functionality at runtime to debug an application with GDB. 
With the gdbstub component it is possible to run GDB from IDF Monitor by pressing Ctrl+C and debug  
the application using GDB. It is also possible to read/modify memory values, interrupt and continue the application.
Upon exit from GDB, the application will continue to work in IDF Monitor as before.

## How to use example
### Hardware Required

he example can run on any commonly available ESP32 development board.  
There are two possible ways to execute gdbstub with GDB: from IDF Monitor and as standalone application.
gdbstub support ESP32, ESP32-S2 and ESP32-S3 chips. 

### Configure the project

By default, the example is already pre-configured, but the user can change configuration options with the following command:
```
idf.py menuconfig
``` 
Current example is pre-configured. The user can scroll through the system parameters and see the settings.
Most important one is:
-> Component Config -> ESP System Settings -> Panic handler behaviour -> GDBStub on runtime
This selection switches gdbstub to runtime mode.
Depending on the project, following settings could be used:
-> Component Config -> GDB Stub -> ...
The user can enable or disable task list handling and define a maximum amount of tasks.
Note that gdbstub can now only be used when FreeRTOS is run on the first core only. 
This setting is located here:
-> Component Config -> FreeRTOS -> Run FreeRTOS only on first core.

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
A typical console output for such a scenario is shown below:
```
I (300) cpu_start: Starting scheduler on PRO CPU.
Hello GDB example!
I (307) gdbstub_example: INFO  mode enabled. Call - 0. To enter GDB please press "Ctrl+C"
W (317) gdbstub_example: WARN  mode enabled. Call - 0. To enter GDB please press "Ctrl+C"
I (1317) gdbstub_example: INFO  mode enabled. Call - 1. To enter GDB please press "Ctrl+C"
W (1317) gdbstub_example: WARN  mode enabled. Call - 1. To enter GDB please press "Ctrl+C"
To exit from the idf.py please use "Ctrl+]"
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
Remote debugging using \\.\COM15
0x400dff0a in esp_pm_impl_waiti () at C:/esp-idf/components/esp_pm/pm_impl.c:533
533         asm("waiti 0");
(gdb) print call_count
$1 = 2
(gdb) set call_count = 100
(gdb) print call_count
$2 = 100
(gdb) print update_log_level
$3 = ESP_LOG_DEBUG
(gdb) set update_log_level = ESP_LOG_WARN
(gdb) print update_log_level
$4 = ESP_LOG_WARN
(gdb) c
Continuing.

Thread 1 received signal SIGINT, Interrupt.
0x400dff0a in esp_pm_impl_waiti () at C:/esp-idf/components/esp_pm/pm_impl.c:533
533         asm("waiti 0");
(gdb) print call_count
$6 = 108
(gdb) quit
A debugging session is active.

        Inferior 1 [Remote target] will be killed.

Quit anyway? (y or n) y
W (13977) gdbstub_example: WARN  mode enabled. Call - 108. To enter GDB please press "Ctrl+C"
W (14977) gdbstub_example: WARN  mode enabled. Call - 109. To enter GDB please press "Ctrl+C"
W (15977) gdbstub_example: WARN  mode enabled. Call - 110. To enter GDB please press "Ctrl+C"
W (16977) gdbstub_example: WARN  mode enabled. Call - 111. To enter GDB please press "Ctrl+C"
```

To reproduce this scenario run the application by: idf.py -P PORT flash monitor
Then:
1. Interrupt the application by pressing Ctrl+C
2. In GDB, print the application values by typing in GDB command line "print call_count" or "print update_log_level"  
3. Modify the application values by typing in GDB command line "set call_count = 100" or "set update_log_level = ESP_LOG_WARN"
4. Continue the application by typing in GDB command line "continue"
5. Interrupt application by pressing Ctrl+C
6. Check the value by typing in GDB command line "print call_count" or "print update_log_level"
7. Exit GDB by typing "quit" and then "y"

To exit from monitor please use Ctrl+]

