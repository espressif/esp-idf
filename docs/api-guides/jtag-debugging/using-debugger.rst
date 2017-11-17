Using Debugger
--------------

This section covers configuration and running debugger either from :ref:`jtag-debugging-using-debugger-eclipse`
or :ref:`jtag-debugging-using-debugger-command-line`. It is recommended to first check if debugger works from :ref:`jtag-debugging-using-debugger-command-line` and then move to using Eclipse.


.. _jtag-debugging-using-debugger-eclipse:

Eclipse
^^^^^^^

Debugging functionality is provided out of box in standard Eclipse installation. Another option is to use pluggins like "GDB Hardware Debugging" plugin. We have found this plugin quite convenient and decided to use throughout this guide.

To begin with, install "GDB Hardware Debugging" plugin by opening Eclipse and going to `Help` > `Install` New Software.

Once installation is complete, configure debugging session following steps below. Please note that some of configuration parameters are generic and some are project specific. This will be shown below by configuring debugging for "blink" example project. If not done already, add this project to Eclipse workspace following guidance in section :doc:`Build and Flash with Eclipse IDE <../../get-started/eclipse-setup>`.  The source of :example:`get-started/blink` application is available  in :idf:`examples` directory of ESP-IDF repository.

1.  In Eclipse go to `Run` > `Debug Configuration`. A new window will open. In the window's left pane double click "GDB Hardware Debugging" (or select "GDB Hardware Debugging" and press the "New" button) to create a new configuration.

2.  In a form that will show up on the right, enter the "Name:" of this configuration, e.g. "Blink checking".

3.  On the "Main" tab below, under "Project:", press "Browse" button and select the "blink" project.

4.  In next line "C/C++ Application:" press "Browse" button and select "blink.elf" file. If "blink.elf" is not there, then likely this project has not been build yet. See :doc:`Build and Flash with Eclipse IDE <../../get-started/eclipse-setup>` how to do it.

5.  Finally, under "Build (if required) before launching" click "Disable auto build".

    A sample window with settings entered in points 1 - 5 is shown below.

    .. figure:: ../../_static/hw-debugging-main-tab.jpg
        :align: center
        :alt: Configuration of GDB Hardware Debugging - Main tab
        :figclass: align-center

        Configuration of GDB Hardware Debugging - Main tab

6.  Click "Debugger" tab. In field "GDB Command" enter ``xtensa-esp32-elf-gdb`` to invoke debugger.

7.  Change default configuration of "Remote host" by entering ``3333`` under the "Port number". 

    Configuration entered in points 6 and 7 is shown on the following picture.

    .. figure:: ../../_static/hw-debugging-debugger-tab.jpg
        :align: center
        :alt: Configuration of GDB Hardware Debugging - Debugger tab
        :figclass: align-center

        Configuration of GDB Hardware Debugging - Debugger tab

8.  The last tab to that requires changing of default configuration is "Startup". Under "Initialization Commands" uncheck "Reset and Delay (seconds)" and "Halt"". Then, in entry field below, type ``mon reset halt`` and ``x $a1=0`` (in two separate lines).

    .. note::
        If you want to update image in the flash automatically before starting new debug session add the following lines of commands at the beginning of "Initialization Commands" textbox::

            mon reset halt
            mon program_esp32 ${workspace_loc:blink/build/blink.bin} 0x10000 verify


    For description of ``program_esp32`` command see :ref:`jtag-upload-app-debug`.

9.  Under "Load Image and Symbols" uncheck "Load image" option. 

10. Further down on the same tab, establish an initial breakpoint to halt CPUs after they are reset by debugger. The plugin will set this breakpoint at the beginning of the function entered under "Set break point at:". Checkout this option and enter ``app_main`` in provided field.

11. Checkout "Resume" option. This will make the program to resume after ``mon reset halt`` is invoked per point 8. The program will then stop at breakpoint inserted at ``app_main``.

    Configuration described in points 8 - 11 is shown below.

    .. figure:: ../../_static/hw-debugging-startup-tab.jpg
        :align: center
        :alt: Configuration of GDB Hardware Debugging - Startup tab
        :figclass: align-center

        Configuration of GDB Hardware Debugging - Startup tab

    If the "Startup" sequence looks convoluted and respective "Initialization Commands" are not clear to you, check :ref:`jtag-debugging-tip-debugger-startup-commands` for additional explanation.

12. If you previously completed :ref:`jtag-debugging-configuring-esp32-target` steps described above, so the target is running and ready to talk to debugger, go right to debugging by pressing "Debug" button. Otherwise press "Apply" to save changes, go back to :ref:`jtag-debugging-configuring-esp32-target` and return here to start debugging.

Once all 1 - 12 configuration steps are satisfied, the new Eclipse perspective called "Debug" will open as shown on example picture below. 

.. figure:: ../../_static/debug-perspective.jpg
    :align: center
    :alt: Debug Perspective in Eclipse
    :figclass: align-center

    Debug Perspective in Eclipse

If you are not quite sure how to use GDB, check :ref:`jtag-debugging-examples-eclipse` example debugging session in section :ref:`jtag-debugging-examples`.


.. _jtag-debugging-using-debugger-command-line:

Command Line
^^^^^^^^^^^^

1.  To be able start debugging session, the target should be up and running. If not done already, complete steps described under :ref:`jtag-debugging-configuring-esp32-target`.

.. highlight:: bash

2.  Open a new terminal session and go to directory that contains project for debugging, e.g.

    ::

        cd ~/esp/blink

.. highlight:: none

3.  When launching a debugger, you will need to provide couple of configuration parameters and commands. Instead of entering them one by one in command line, create a configuration file and name it ``gdbinit``:

    ::

        target remote :3333
        mon reset halt
        thb app_main
        x $a1=0
        c

    Save this file in current directory. 

    For more details what's inside ``gdbinit`` file, see :ref:`jtag-debugging-tip-debugger-startup-commands`

.. highlight:: bash

4.  Now you are ready to launch GDB. Type the following in terminal:

    ::

        xtensa-esp32-elf-gdb -x gdbinit build/blink.elf

.. highlight:: none

5.  If previous steps have been done correctly, you will see a similar log concluded with ``(gdb)`` prompt:

    ::

        user-name@computer-name:~/esp/blink$ xtensa-esp32-elf-gdb -x gdbinit build/blink.elf
        GNU gdb (crosstool-NG crosstool-ng-1.22.0-61-gab8375a) 7.10
        Copyright (C) 2015 Free Software Foundation, Inc.
        License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
        This is free software: you are free to change and redistribute it.
        There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
        and "show warranty" for details.
        This GDB was configured as "--host=x86_64-build_pc-linux-gnu --target=xtensa-esp32-elf".
        Type "show configuration" for configuration details.
        For bug reporting instructions, please see:
        <http://www.gnu.org/software/gdb/bugs/>.
        Find the GDB manual and other documentation resources online at:
        <http://www.gnu.org/software/gdb/documentation/>.
        For help, type "help".
        Type "apropos word" to search for commands related to "word"...
        Reading symbols from build/blink.elf...done.
        0x400d10d8 in esp_vApplicationIdleHook () at /home/user-name/esp/esp-idf/components/esp32/./freertos_hooks.c:52
        52          asm("waiti 0");
        JTAG tap: esp32.cpu0 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
        JTAG tap: esp32.slave tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
        esp32: Debug controller was reset (pwrstat=0x5F, after clear 0x0F).
        esp32: Core was reset (pwrstat=0x5F, after clear 0x0F).
        Target halted. PRO_CPU: PC=0x5000004B (active)    APP_CPU: PC=0x00000000 
        esp32: target state: halted
        esp32: Core was reset (pwrstat=0x1F, after clear 0x0F).
        Target halted. PRO_CPU: PC=0x40000400 (active)    APP_CPU: PC=0x40000400 
        esp32: target state: halted
        Hardware assisted breakpoint 1 at 0x400db717: file /home/user-name/esp/blink/main/./blink.c, line 43.
        0x0:    0x00000000
        Target halted. PRO_CPU: PC=0x400DB717 (active)    APP_CPU: PC=0x400D10D8 
        [New Thread 1073428656]
        [New Thread 1073413708]
        [New Thread 1073431316]
        [New Thread 1073410672]
        [New Thread 1073408876]
        [New Thread 1073432196]
        [New Thread 1073411552]
        [Switching to Thread 1073411996]

        Temporary breakpoint 1, app_main () at /home/user-name/esp/blink/main/./blink.c:43
        43      xTaskCreate(&blink_task, "blink_task", 512, NULL, 5, NULL);
        (gdb) 

Note the third line from bottom that shows debugger halting at breakpoint established in ``gdbinit`` file at function ``app_main()``. Since the processor is halted, the LED should not be blinking. If this is what you see as well, you are ready to start debugging.

If you are not quite sure how to use GDB, check :ref:`jtag-debugging-examples-command-line` example debugging session in section :ref:`jtag-debugging-examples`.
