Debugging
=========

OpenOCD setup for ESP32
-----------------------

The ESP31 and ESP32 have two powerful Xtensa cores, allowing for a great variety of program architectures. The FreeRTOS
OS that comes with ESP-IDF is capable of multi-core pre-emptive multithreading, allowing for an intuitive way of writing software. 

The downside of the ease of programming is that debugging without the right tools is harder: figuring out a bug that is caused 
by two threads, maybe even running simultaneously on two different CPU cores, can take a long time when all you have are printf 
statements. A better and in many cases quicker way to debug such problems is by using a debugger, connected to the processors over
a debug port. 

Espressif has ported OpenOCD to support the ESP32 processor and the multicore FreeRTOS that will be the foundation of most ESP32
apps, and has written some tools to help with features OpenOCD does not support natively. These are all available for free, and 
this document describes how to install and use them.

JTAG adapter hardware
---------------------

You will need a JTAG adapter that is compatible with both the voltage levels on the ESP32 as well as with the OpenOCD software. 
The JTAG port on the ESP32 is an industry-standard JTAG port which lacks (and does not need) the TRST pin. The JTAG I/O pins
all are powered from the VDD_3P3_RTC pin (which normally would be powered by a 3.3V rail) so the JTAG adapter needs to be
able to work with JTAG pins in that voltage range. On the software side, OpenOCD supports a fair amount of JTAG adapters.
See http://openocd.org/doc/html/Debug-Adapter-Hardware.html for an (unfortunately slightly incomplete) list of the adapters 
OpenOCD works with. This page lists SWD-compatible adapters as well; take note that the ESP32 does not support SWD.

At Espressif, we have tested the TIAO USB Multi-protocol Adapter board as well as the Flyswatter2, which are both USB2.0 high-speed
devices and give a good throughput. We also tested a J-link-compatible and an EasyOpenJTAG adapter; both worked as well but are 
somewhat slower.

The minimal signalling to get a working JTAG connection are TDI, TDO, TCK, TMS and Gnd. Some JTAG debuggers also need a connection 
from the ESP32 power line to a line called e.g. Vtar to set the working voltage. SRST can optionally be connected to the CH_PD of 
the ESP32, although for now, support in OpenOCD for that line is pretty minimal.

Installing OpenOCD
------------------

The sources for the ESP32-enabled variant of OpenOCD are available from `Espressifs Github <https://github.com/espressif/openocd-esp32>`_. 
To download the source, use the following commands::

    git clone --recursive https://github.com/espressif/openocd-esp32.git
    cd openocd-esp32

For compilation of OpenOCD, please refer to the README, README.OSX and README.Windows file in the openocd-esp32 directory. You can skip
the ``make install`` step if you want.

Configuring the ESP32 target in OpenOCD
---------------------------------------

After OpenOCD is compiled (and optionally installed) and the JTAG adapter is connected to the ESP32 board, everything is ready to
invoke OpenOCD for the first time. To do this, OpenOCD needs to be told what JTAG adapter to use as well as what type of board
and processor the JTAG adapter is connected to. It is the easiest to do both using a configuration file. A template configuration
file (esp32.cfg) is included in the same directory as this file. A way to use this would be:

- Copy esp32.cfg to the openocd-esp32 directory
- Edit the copied esp32.cfg file. Most importantly, change the ``source [find interface/ftdi/tumpa.cfg]`` line to reflect the
  physical JTAG adapter connected.
- Open a terminal and ``cd`` to the openocd-esp32 directory.
- Run ``./src/openocd -s ./tcl -f ./esp32.cfg`` to start OpenOCD

You should now see something like this::

    user@machine:~/esp32/openocd-esp32$ ./src/openocd -s ./tcl/ -f ../openocd-esp32-tools/esp32.cfg 
    Open On-Chip Debugger 0.10.0-dev-00446-g6e13a97-dirty (2016-08-23-16:36)
    Licensed under GNU GPL v2
    For bug reports, read
    http://openocd.org/doc/doxygen/bugs.html
    none separate
    adapter speed: 200 kHz
    Info : clock speed 200 kHz
    Info : JTAG tap: esp32.cpu0 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
    Info : JTAG tap: esp32.cpu1 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
    Info : esp32.cpu0: Debug controller was reset (pwrstat=0x5F, after clear 0x0F).
    Info : esp32.cpu0: Core was reset (pwrstat=0x5F, after clear 0x0F).


- If you see an error indicating permission problems, please see the 'Permissions delegation' bit in the OpenOCD README
- If you see JTAG errors (...all ones/...all zeroes) please check your connections and see if everything is powered on.

Connecting a debugger to OpenOCD
--------------------------------

OpenOCD should now be ready to accept gdb connections. If you have compiled the ESP32 toolchain using Crosstool-NG, or
if you have downloaded a precompiled toolchain from the Espressif website, you should already have xtensa-esp32-elf-gdb, 
a version of gdb that can be used for this. First, make sure the project you want to debug is compiled and flashed 
into the ESP32's SPI flash. Then, in a different console than OpenOCD is running in, invoke gdb. For example, for the 
template app, you would do this like such::

    cd esp-idf-template
    xtensa-esp32-elf-gdb -ex 'target remote localhost:3333' ./build/app-template.elf 


This should give you a gdb prompt.

FreeRTOS support
----------------

OpenOCD has explicit support for the ESP-IDF FreeRTOS; FreeRTOS detection can be disabled in esp32.conf. When enabled,
gdb can see FreeRTOS tasks as threads. Viewing them all can be done using the gdb ``i threads`` command, changing
to a certain task is done with ``thread x``, with x being the number of the thread. All threads can be switched to
except for a thread actually running on the other CPU, please see ``ESP32 quirks`` for more information.


ESP32 quirks
------------

Normal gdb breakpoints (``b myFunction``) can only be set in IRAM, because that memory is writable. Setting these types of
breakpoints in code in flash will not work. Instead, use a hardware breakpoint (``hb myFunction``). The esp32 supports
2 hardware breakpoints. It also supports two watchpoint, so two variables can be watched for change or read by the gdb
command ``watch myVariable``.

Connecting gdb to the APP or PRO cpu happens by changing the port gdb connects to. ``target remote localhost:3333`` connects
to the PRO CPU, ``target remote localhost:3334`` to the APP CPU. Hardware-wise, when one CPU is halted because of debugging
reasons, the other one will be halted as well; resuming also happens simultaneously.

Because gdb only sees the system from the point of view of the selected CPU, only the FreeRTOS tasks that are suspended
and the task running on the CPU gdb is connected to, will be shown correctly. The task that was active on the other
cpu can be inspected, but its state may be wildly inconsistent.

The ESP-IDF code has the option of compiling in various support options for OpenOCD: it can stop execution when the first 
thread is started and break the system if a panic or unhandled exception is thrown. Both options are enabled by default 
but can be disabled using the esp-idf configuration menu. Please see the ``make menuconfig`` menu for more details.

Normally, under OpenOCD, a board can be reset by entering 'mon reset' or 'mon reset halt' into gdb. For
the ESP32, these commands work more or less, but have side effects. First of all, an OpenOCD reset only
resets the CPU cores, not the peripherals, which may lead to undefined behaviour if software assumes the
after-reset state of peripherals. Secondly, 'mon reset halt' stops before FreeRTOS is initialized. 
OpenOCD assumes (in the default configuration, you can change this by editing esp32.cfg) a running 
FreeRTOS and may get confused.
