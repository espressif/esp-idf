Using Debugger
--------------

:link_to_translation:`zh_CN:[中文]`

This section covers the steps to configure and run a debugger using various methods, including:

* :ref:`jtag-debugging-using-debugger-eclipse`
* :ref:`jtag-debugging-using-debugger-command-line`
* :ref:`jtag-debugging-with-idf-py`

For how to run a debugger from VS Code, see `Configuration for Visual Studio Code Debug <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/DEBUGGING.md>`__.


.. _jtag-debugging-using-debugger-eclipse:

Eclipse
^^^^^^^

.. note::

    It is recommended to first check if debugger works using :ref:`jtag-debugging-with-idf-py` or from :ref:`jtag-debugging-using-debugger-command-line` and then move to using Eclipse.

Eclipse is an integrated development environment (IDE) that provides a powerful set of tools for developing and debugging software applications. For ESP-IDF applications, `IDF Eclipse plugin <https://github.com/espressif/idf-eclipse-plugin>`_ provides two ways of debugging:

1.  `ESP-IDF GDB OpenOCD Debugging <https://github.com/espressif/idf-eclipse-plugin/blob/master/docs/OpenOCD%20Debugging.md#esp-idf-gdb-openocd-debugging>`_
2.  GDB Hardware Debugging

By default, Eclipse supports OpenOCD Debugging via the GDB Hardware Debugging plugin, which requires starting the OpenOCD server from the command line and configuring the GDB client from Eclipse to start with the debugging. This approach can be time-consuming and error-prone.

To make the debugging process easier, the IDF Eclipse plugin has a customized ESP-IDF GDB OpenOCD Debugging functionality. This functionality supports configuring the OpenOCD server and GDB client from within Eclipse. All the required configuration parameters will be pre-filled by the plugin, and you can start debugging with just a click of a button.

Therefore, it is recommended to use the `ESP-IDF GDB OpenOCD Debugging <https://github.com/espressif/idf-eclipse-plugin/blob/master/docs/OpenOCD%20Debugging.md#esp-idf-gdb-openocd-debugging>`_ via the IDF Eclipse plugin.

**GDB Hardware Debugging**

.. note::

    This approach is recommended only if you are unable to debug using `ESP-IDF GDB OpenOCD Debugging <https://github.com/espressif/idf-eclipse-plugin/blob/master/docs/OpenOCD%20Debugging.md#esp-idf-gdb-openocd-debugging>`_ for some reason.

To install the ``GDB Hardware Debugging`` plugin, open Eclipse and select ``Help`` > ``Install`` New Software.

After installation is complete, follow these steps to configure the debugging session. Please note that some configuration parameters are generic, while others are project-specific. This will be shown below by configuring debugging for "blink" example project. If not done already, add this project to Eclipse workspace following `Eclipse Plugin <https://github.com/espressif/idf-eclipse-plugin/blob/master/README.md>`_.  The source of :example:`get-started/blink` application is available in :idf:`examples` directory of ESP-IDF repository.

1.  In Eclipse, go to ``Run`` > ``Debug Configuration``. A new window will open. In the left pane of the window, double-click ``GDB Hardware Debugging`` (or select ``GDB Hardware Debugging`` and press the ``New`` button) to create a new configuration.

2.  In a form that will show up on the right, enter the ``Name:`` of this configuration, e.g., "Blink checking".

3.  On the ``Main`` tab below, under ``Project:``, press the ``Browse`` button and select the ``blink`` project.

4.  In the next line under ``C/C++ Application:``, press the ``Browse`` button and select the ``blink.elf`` file. If ``blink.elf`` is not there, it is likely that this project has not been built yet. Refer to the `Eclipse Plugin <https://github.com/espressif/idf-eclipse-plugin/blob/master/README.md>`_ for instructions.

5.  Finally, under ``Build (if required) before launching`` click ``Disable auto build``.

    A sample window with settings entered in points 1 - 5 is shown below.

    .. figure:: ../../../_static/hw-debugging-main-tab.jpg
        :align: center
        :alt: Configuration of GDB Hardware Debugging - Main tab
        :figclass: align-center

        Configuration of GDB Hardware Debugging - Main tab

6.  Click the ``Debugger`` tab. In field ``GDB Command``, enter ``{IDF_TARGET_TOOLCHAIN_PREFIX}-gdb`` to invoke the debugger.

7.  Change the default configuration of the ``Remote host`` by entering ``3333`` under the ``Port number``.

    Configuration entered in points 6 and 7 is shown on the following picture.

    .. figure:: ../../../_static/hw-debugging-debugger-tab.jpg
        :align: center
        :alt: Configuration of GDB Hardware Debugging - Debugger tab
        :figclass: align-center

        Configuration of GDB Hardware Debugging - Debugger tab

8.  The last tab that requires changing the default configuration is ``Startup``. Under ``Initialization Commands`` uncheck ``Reset and Delay (seconds)`` and ``Halt``. Then, in the entry field below, enter the following lines:

    .. code-block:: none

        mon reset halt
        maintenance flush register-cache
        set remote hardware-watchpoint-limit 2

    .. note::

        To automatically update the image in the flash before starting a new debug session, add the following command lines to the beginning of the ``Initialization Commands`` textbox:

        .. code-block:: none

            mon reset halt
            mon program_esp ${workspace_loc:blink/build/blink.bin} 0x10000 verify

    For description of ``program_esp`` command, see :ref:`jtag-upload-app-debug`.

9.  Uncheck the ``Load image`` option under ``Load Image and Symbols``.

10. Further down on the same tab, establish an initial breakpoint to halt CPUs after they are reset by debugger. The plugin will set this breakpoint at the beginning of the function entered under ``Set break point at:``. Checkout this option and enter ``app_main`` in provided field.

11. Checkout ``Resume`` option. This will make the program to resume after ``mon reset halt`` is invoked per point 8. The program will then stop at breakpoint inserted at ``app_main``.

    Configuration described in points 8 - 11 is shown below.

    .. figure:: ../../../_static/hw-debugging-startup-tab.jpg
        :align: center
        :alt: Configuration of GDB Hardware Debugging - Startup tab
        :figclass: align-center

        Configuration of GDB Hardware Debugging - Startup tab

    If the ``Startup`` sequence looks convoluted and respective ``Initialization Commands`` are unclear, check :ref:`jtag-debugging-tip-debugger-startup-commands` for additional explanation.

12. If you have completed the :ref:`jtag-debugging-configuring-target` steps described above, so the target is running and ready to talk to debugger, go right to debugging by pressing ``Debug`` button. Otherwise press ``Apply`` to save changes, go back to :ref:`jtag-debugging-configuring-target` and return here to start debugging.

    Once all configuration steps 1-12 are satisfied, the new Eclipse perspective called "Debug" will open, as shown in the example picture below.

    .. figure:: ../../../_static/debug-perspective.jpg
        :align: center
        :alt: Debug Perspective in Eclipse
        :figclass: align-center

        Debug Perspective in Eclipse

    If you are not quite sure how to use GDB, check :ref:`jtag-debugging-examples-eclipse` example debugging session in section :ref:`jtag-debugging-examples`.


.. _jtag-debugging-using-debugger-command-line:

Command Line
^^^^^^^^^^^^

1.  Begin by completing the steps described under :ref:`jtag-debugging-configuring-target`. This is prerequisite to start a debugging session.

.. highlight:: bash

2.  Open a new terminal session and go to the directory that contains the project for debugging, e.g.,

    .. code-block:: none

        cd ~/esp/blink

.. highlight:: none

3.  When launching a debugger, you will need to provide a couple of configuration parameters and commands. The build system generates several ``.gdbinit`` files to facilitate efficient debugging. Paths to these files can be found in the ``build/project_description.json``, under the ``gdbinit_files`` section. The paths to these files are defined as follows:

    .. code-block:: json

        "gdbinit_files": {
            "01_symbols": "application_path/build/gdbinit/symbols",
            "02_prefix_map": "application_path/build/gdbinit/prefix_map",
            "03_py_extensions": "application_path/build/gdbinit/py_extensions",
            "04_connect": "application_path/build/gdbinit/connect"
        }

    The ``XX_`` prefix in the JSON keys is included to have ability to sort them. Sorted fields indicate the recommended order in which to provide the data to GDB.

    Descriptions of the generated ``.gdbinit`` files:

    * ``symbols`` - contains symbol sources for debugging.
    * ``prefix_map`` - configures the prefix map to modify source paths in GDB. For more details, see :ref:`reproducible-builds-and-debugging`.
    * ``py_extensions`` - initializes Python extensions in GDB. This requires Python built with ``libpython`` and a version supported by GDB. To verify compatibility, run ``{IDF_TARGET_TOOLCHAIN_PREFIX}-gdb --batch-silent --ex "python import os"``, which should complete without errors.
    * ``connect`` - contains commands necessary for establishing a connection to the target device.

    To enhance your debugging experience, you can also create custom ``.gdbinit`` files, used either alongside or in place of the generated configurations.

.. highlight:: bash

4.  Now you are ready to launch GDB. Use the following example command to load symbols and connect to the target (``-q`` option added to minimize startup output):

    .. code-block:: none

        {IDF_TARGET_TOOLCHAIN_PREFIX}-gdb -q -x build/gdbinit/symbols -x build/gdbinit/prefix_map -x build/gdbinit/connect build/blink.elf

.. highlight:: none

5.  If the previous steps have been done correctly, you will see a similar log concluded with the ``(gdb)`` prompt:

    .. code-block:: none

        {IDF_TARGET_TOOLCHAIN_PREFIX}-gdb -q -x build/gdbinit/symbols -x build/gdbinit/prefix_map -x build/gdbinit/connect build/blink.elf

        user-name@computer-name:~/esp-idf/examples/get-started/blink$ {IDF_TARGET_TOOLCHAIN_PREFIX}-gdb -q -x build/gdbinit/symbols -x build/gdbinit/connect build/blink.elf
        Reading symbols from build/blink.elf...
        add symbol table from file "/home/user-name/esp-idf/examples/get-started/blink/build/bootloader/bootloader.elf"
        [Switching to Thread 1070141764]
        app_main () at /home/user-name/esp-idf/examples/get-started/blink/main/blink_example_main.c:95
        95          configure_led();
        add symbol table from file "/home/alex/.espressif/tools/esp-rom-elfs/20241011/{IDF_TARGET_PATH_NAME}_rev0_rom.elf"
        JTAG tap: {IDF_TARGET_PATH_NAME}.tap0 tap/device found: 0x00005c25 (mfg: 0x612 (Espressif Systems), part: 0x0005, ver: 0x0)
        [{IDF_TARGET_PATH_NAME}] Reset cause (3) - (Software core reset)
        Hardware assisted breakpoint 1 at 0x42009436: file /home/user-name/esp-idf/examples/get-started/blink/main/blink_example_main.c, line 92.
        [Switching to Thread 1070139884]

        Thread 2 "main" hit Temporary breakpoint 1, app_main () at /home/user-name/esp-idf/examples/get-started/blink/main/blink_example_main.c:92
        92      {
        (gdb)


    Note that the third-to-last line indicates the debugger has halted at a breakpoint established in ``build/gdbinit/connect`` file at function ``app_main()``. Since the processor is halted, the LED should not be blinking. If this matches what you observe, you are ready to start debugging.

    If you are not sure how to use GDB, check :ref:`jtag-debugging-examples-command-line` example debugging session in section :ref:`jtag-debugging-examples`.


.. _jtag-debugging-with-idf-py:

Idf.py Debug Targets
^^^^^^^^^^^^^^^^^^^^

It is also possible to execute the described debugging tools conveniently from ``idf.py``. These commands are supported:

1.  ``idf.py openocd``

    Runs OpenOCD in a console with configuration defined in the environment or via command line. It uses default script directory defined as ``OPENOCD_SCRIPTS`` environmental variable, which is automatically added from an Export script (``export.sh`` or ``export.bat``).
    It is possible to override the script location using command line argument ``--openocd-scripts``.

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: idf-py-openocd-default-cfg
        :end-before: ---

    To configure the JTAG configuration for the current board, please use the environmental variable ``OPENOCD_COMMANDS`` or ``--openocd-commands`` command line argument. If none of the above is defined, OpenOCD is started with |idf-py-def-cfg| board definition.


2.  ``idf.py gdb``

    Starts GDB in the same way as the :ref:`jtag-debugging-using-debugger-command-line`, using generated GDB scripts referring to the current project's ELF file. For more details, see :ref:`jtag-debugging-using-debugger-command-line`.


3. ``idf.py gdbtui``

    The same as `2`, but starts the gdb with ``tui`` argument, allowing for a simple source code view.


4.  ``idf.py gdbgui``

    Starts `gdbgui <https://www.gdbgui.com>`_ debugger frontend enabling out-of-the-box debugging in a browser window. To enable this option, run the install script with the "--enable-gdbgui" argument, e.g., ``install.sh --enable-gdbgui``.


    You can combine these debugging actions on a single command line, allowing for convenient setup of blocking and non-blocking actions in one step. ``idf.py`` implements a simple logic to move the background actions (such as openocd) to the beginning and the interactive ones (such as gdb, monitor) to the end of the action list.

    An example of a very useful combination is:

    .. code-block:: none

        idf.py openocd gdbgui monitor


    The above command runs OpenOCD in the background, starts `gdbgui <https://www.gdbgui.com>`_ to open a browser window with active debugger frontend and opens a serial monitor in the active console.
