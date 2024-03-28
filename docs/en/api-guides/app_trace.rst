Application Level Tracing Library
=================================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

ESP-IDF provides a useful feature for program behavior analysis: application level tracing. It is implemented in the corresponding library and can be enabled in menuconfig. This feature allows to transfer arbitrary data between host and {IDF_TARGET_NAME} via JTAG, UART, or USB interfaces with small overhead on program execution. It is possible to use JTAG and UART interfaces simultaneously. The UART interface is mostly used for connection with SEGGER SystemView tool (see `SystemView <https://www.segger.com/products/development-tools/systemview/>`_).

Developers can use this library to send application-specific state of execution to the host and receive commands or other types of information from the opposite direction at runtime. The main use cases of this library are:

1. Collecting application-specific data. See :ref:`app_trace-application-specific-tracing`.
2. Lightweight logging to the host. See :ref:`app_trace-logging-to-host`.
3. System behavior analysis. See :ref:`app_trace-system-behaviour-analysis-with-segger-systemview`.
4. Source code coverage. See :ref:`app_trace-gcov-source-code-coverage`.

Tracing components used when working over JTAG interface are shown in the figure below.

.. figure:: ../../_static/app_trace-overview.jpg
    :align: center
    :alt: Tracing Components When Working Over JTAG

    Tracing Components Used When Working Over JTAG


Modes of Operation
------------------

The library supports two modes of operation:

**Post-mortem mode:** This is the default mode. The mode does not need interaction with the host side. In this mode, tracing module does not check whether the host has read all the data from *HW UP BUFFER*, but directly overwrites old data with the new ones. This mode is useful when only the latest trace data is interesting to the user, e.g., for analyzing program's behavior just before the crash. The host can read the data later on upon user request, e.g., via special OpenOCD command in case of working via JTAG interface.

**Streaming mode:** Tracing module enters this mode when the host connects to {IDF_TARGET_NAME}. In this mode, before writing new data to *HW UP BUFFER*, the tracing module checks that whether there is enough space in it and if necessary, waits for the host to read data and free enough memory. Maximum waiting time is controlled via timeout values passed by users to corresponding API routines. So when application tries to write data to the trace buffer using the finite value of the maximum waiting time, it is possible that this data will be dropped. This is especially true for tracing from time critical code (ISRs, OS scheduler code, etc.) where infinite timeouts can lead to system malfunction. In order to avoid loss of such critical data, developers can enable additional data buffering via menuconfig option :ref:`CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX`. This macro specifies the size of data which can be buffered in above conditions. The option can also help to overcome situation when data transfer to the host is temporarily slowed down, e.g., due to USB bus congestions. But it will not help when the average bitrate of the trace data stream exceeds the hardware interface capabilities.


Configuration Options and Dependencies
--------------------------------------

Using of this feature depends on two components:

1. **Host side:** Application tracing is done over JTAG, so it needs OpenOCD to be set up and running on host machine. For instructions on how to set it up, please see :doc:`JTAG Debugging <../api-guides/jtag-debugging/index>` for details.

2. **Target side:** Application tracing functionality can be enabled in menuconfig. Please go to ``Component config`` > ``Application Level Tracing`` menu, which allows selecting destination for the trace data (hardware interface for transport: JTAG or/and UART). Choosing any of the destinations automatically enables the ``CONFIG_APPTRACE_ENABLE`` option. For UART interfaces, users have to define baud rate, TX and RX pins numbers, and additional UART-related parameters.

.. note::

    In order to achieve higher data rates and minimize the number of dropped packets, it is recommended to optimize the setting of JTAG clock frequency, so that it is at maximum and still provides stable operation of JTAG. See :ref:`jtag-debugging-tip-optimize-jtag-speed`.

There are two additional menuconfig options not mentioned above:

1. *Threshold for flushing last trace data to host on panic* (:ref:`CONFIG_APPTRACE_POSTMORTEM_FLUSH_THRESH`). This option is necessary due to the nature of working over JTAG. In this mode, trace data is exposed to the host in 16 KB blocks. In post-mortem mode, when one block is filled, it is exposed to the host and the previous one becomes unavailable. In other words, the trace data is overwritten in 16 KB granularity. On panic, the latest data from the current input block is exposed to the host and the host can read them for post-analysis. System panic may occur when a very small amount of data are not exposed to the host yet. In this case, the previous 16 KB of collected data will be lost and the host will see the latest, but very small piece of the trace. It can be insufficient to diagnose the problem. This menuconfig option allows avoiding such situations. It controls the threshold for flushing data in case of apanic. For example, users can decide that it needs no less than 512 bytes of the recent trace data, so if there is less then 512 bytes of pending data at the moment of panic, they will not be flushed and will not overwrite the previous 16 KB. The option is only meaningful in post-mortem mode and when working over JTAG.

2. *Timeout for flushing last trace data to host on panic* (:ref:`CONFIG_APPTRACE_ONPANIC_HOST_FLUSH_TMO`). The option is only meaningful in streaming mode and it controls the maximum time that the tracing module will wait for the host to read the last data in case of panic.

3. *UART RX/TX ring buffer size* (:ref:`CONFIG_APPTRACE_UART_TX_BUFF_SIZE`). The size of the buffer depends on the amount of data transferred through the UART.

4. *UART TX message size* (:ref:`CONFIG_APPTRACE_UART_TX_MSG_SIZE`). The maximum size of the single message to transfer.


How to Use This Library
-----------------------

This library provides APIs for transferring arbitrary data between the host and {IDF_TARGET_NAME}. When enabled in menuconfig, the target application tracing module is initialized automatically at the system startup, so all what the user needs to do is to call corresponding APIs to send, receive or flush the data.


.. _app_trace-application-specific-tracing:

Application Specific Tracing
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In general, users should decide what type of data should be transferred in every direction and how these data must be interpreted (processed). The following steps must be performed to transfer data between the target and the host:

1. On the target side, users should implement algorithms for writing trace data to the host. Piece of code below shows an example on how to do this.

    .. code-block:: c

        #include "esp_app_trace.h"
        ...
        char buf[] = "Hello World!";
        esp_err_t res = esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, buf, strlen(buf), ESP_APPTRACE_TMO_INFINITE);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write data to host!");
            return res;
        }

    ``esp_apptrace_write()`` function uses memcpy to copy user data to the internal buffer. In some cases, it can be more optimal to use ``esp_apptrace_buffer_get()`` and ``esp_apptrace_buffer_put()`` functions. They allow developers to allocate buffer and fill it themselves. The following piece of code shows how to do this.

    .. code-block:: c

        #include "esp_app_trace.h"
        ...
        int number = 10;
        char *ptr = (char *)esp_apptrace_buffer_get(ESP_APPTRACE_DEST_TRAX, 32, 100/*tmo in us*/);
        if (ptr == NULL) {
            ESP_LOGE(TAG, "Failed to get buffer!");
            return ESP_FAIL;
        }
        sprintf(ptr, "Here is the number %d", number);
        esp_err_t res = esp_apptrace_buffer_put(ESP_APPTRACE_DEST_TRAX, ptr, 100/*tmo in us*/);
        if (res != ESP_OK) {
            /* in case of error host tracing tool (e.g., OpenOCD) will report incomplete user buffer */
            ESP_LOGE(TAG, "Failed to put buffer!");
            return res;
        }

    Also according to his needs, the user may want to receive data from the host. Piece of code below shows an example on how to do this.

    .. code-block:: c

        #include "esp_app_trace.h"
        ...
        char buf[32];
        char down_buf[32];
        size_t sz = sizeof(buf);

        /* config down buffer */
        esp_apptrace_down_buffer_config(down_buf, sizeof(down_buf));
        /* check for incoming data and read them if any */
        esp_err_t res = esp_apptrace_read(ESP_APPTRACE_DEST_TRAX, buf, &sz, 0/*do not wait*/);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read data from host!");
            return res;
        }
        if (sz > 0) {
            /* we have data, process them */
            ...
        }

    ``esp_apptrace_read()`` function uses memcpy to copy host data to user buffer. In some casesm it can be more optimal to use ``esp_apptrace_down_buffer_get()`` and ``esp_apptrace_down_buffer_put()`` functions. They allow developers to occupy chunk of read buffer and process it in-place. The following piece of code shows how to do this.

    .. code-block:: c

        #include "esp_app_trace.h"
        ...
        char down_buf[32];
        uint32_t *number;
        size_t sz = 32;

        /* config down buffer */
        esp_apptrace_down_buffer_config(down_buf, sizeof(down_buf));
        char *ptr = (char *)esp_apptrace_down_buffer_get(ESP_APPTRACE_DEST_TRAX, &sz, 100/*tmo in us*/);
        if (ptr == NULL) {
            ESP_LOGE(TAG, "Failed to get buffer!");
            return ESP_FAIL;
        }
        if (sz > 4) {
            number = (uint32_t *)ptr;
            printf("Here is the number %d", *number);
        } else {
            printf("No data");
        }
        esp_err_t res = esp_apptrace_down_buffer_put(ESP_APPTRACE_DEST_TRAX, ptr, 100/*tmo in us*/);
        if (res != ESP_OK) {
            /* in case of error host tracing tool (e.g., OpenOCD) will report incomplete user buffer */
            ESP_LOGE(TAG, "Failed to put buffer!");
            return res;
        }

2. The next step is to build the program image and download it to the target as described in the :ref:`Getting Started Guide <get-started-build>`.

3. Run OpenOCD (see :doc:`JTAG Debugging <../api-guides/jtag-debugging/index>`).

4. Connect to OpenOCD telnet server. It can be done using the following command in terminal ``telnet <oocd_host> 4444``. If telnet session is opened on the same machine which runs OpenOCD, you can use ``localhost`` as ``<oocd_host>`` in the command above.

5. Start trace data collection using special OpenOCD command. This command will transfer tracing data and redirect them to the specified file or socket (currently only files are supported as trace data destination). For description of the corresponding commands, see `OpenOCD Application Level Tracing Commands`_.

6. The final step is to process received data. Since the format of data is defined by users, the processing stage is out of the scope of this document. Good starting points for data processor are python scripts in ``$IDF_PATH/tools/esp_app_trace``: ``apptrace_proc.py`` (used for feature tests) and ``logtrace_proc.py`` (see more details in section `Logging to Host`_).


OpenOCD Application Level Tracing Commands
""""""""""""""""""""""""""""""""""""""""""

*HW UP BUFFER* is shared between user data blocks and the filling of the allocated memory is performed on behalf of the API caller (in task or ISR context). In multithreading environment, it can happen that the task/ISR which fills the buffer is preempted by another high priority task/ISR. So it is possible that the user data preparation process is not completed at the moment when that chunk is read by the host. To handle such conditions, the tracing module prepends all user data chunks with header which contains the allocated user buffer size (2 bytes) and the length of the actually written data (2 bytes). So the total length of the header is 4 bytes. OpenOCD command which reads trace data reports error when it reads incomplete user data chunk, but in any case, it puts the contents of the whole user chunk (including unfilled area) to the output file.

Below is the description of available OpenOCD application tracing commands.

.. note::

    Currently, OpenOCD does not provide commands to send arbitrary user data to the target.


Command usage:

``esp apptrace [start <options>] | [stop] | [status] | [dump <cores_num> <outfile>]``

Sub-commands:

``start``
    Start tracing (continuous streaming).
``stop``
    Stop tracing.
``status``
    Get tracing status.
``dump``
    Dump all data from  (post-mortem dump).


Start command syntax:

  ``start <outfile> [poll_period [trace_size [stop_tmo [wait4halt [skip_size]]]]``

``outfile``
    Path to file to save data from both CPUs. This argument should have the following format: ``file://path/to/file``.
``poll_period``
    Data polling period (in ms) for available trace data. If greater than 0, then command runs in non-blocking mode. By default, 1 ms.
``trace_size``
    Maximum size of data to collect (in bytes). Tracing is stopped after specified amount of data is received. By default, -1 (trace size stop trigger is disabled).
``stop_tmo``
    Idle timeout (in sec). Tracing is stopped if there is no data for specified period of time. By default, -1 (disable this stop trigger). Optionally set it to value longer than longest pause between tracing commands from target.
``wait4halt``
    If 0, start tracing immediately, otherwise command waits for the target to be halted (after reset, by breakpoint etc.) and then automatically resumes it and starts tracing. By default, 0.
``skip_size``
    Number of bytes to skip at the start. By default, 0.

.. note::

    If ``poll_period`` is 0, OpenOCD telnet command line will not be available until tracing is stopped. You must stop it manually by resetting the board or pressing Ctrl+C in OpenOCD window (not one with the telnet session). Another option is to set ``trace_size`` and wait until this size of data is collected. At this point, tracing stops automatically.

Command usage examples:

.. highlight:: none

1. Collect 2048 bytes of tracing data to the file ``trace.log``. The file will be saved in the ``openocd-esp32`` directory.

    ::

        esp apptrace start file://trace.log 1 2048 5 0 0

    The tracing data will be retrieved and saved in non-blocking mode. This process will stop automatically after 2048 bytes are collected, or if no data are available for more than 5 seconds.

    .. note::

        Tracing data is buffered before it is made available to OpenOCD. If you see "Data timeout!" message, then it is likely that the target is not sending enough data to empty the buffer to OpenOCD before the timeout. Either increase the timeout or use the function ``esp_apptrace_flush()`` to flush the data on specific intervals.

2.  Retrieve tracing data indefinitely in non-blocking mode.

    ::

        esp apptrace start file://trace.log 1 -1 -1 0 0

    There is no limitation on the size of collected data and there is no data timeout set. This process may be stopped by issuing ``esp apptrace stop`` command on OpenOCD telnet prompt, or by pressing Ctrl+C in OpenOCD window.

3.  Retrieve tracing data and save them indefinitely.

    ::

        esp apptrace start file://trace.log 0 -1 -1 0 0

    OpenOCD telnet command line prompt will not be available until tracing is stopped. To stop tracing, press Ctrl+C in the OpenOCD window.

4.  Wait for the target to be halted. Then resume the target's operation and start data retrieval. Stop after collecting 2048 bytes of data:

    ::

        esp apptrace start file://trace.log 0 2048 -1 1 0

    To configure tracing immediately after reset, use the OpenOCD ``reset halt`` command.


.. _app_trace-logging-to-host:

Logging to Host
^^^^^^^^^^^^^^^

ESP-IDF implements a useful feature: logging to the host via application level tracing library. This is a kind of semihosting when all `ESP_LOGx` calls send strings to be printed to the host instead of UART. This can be useful because "printing to host" eliminates some steps performed when logging to UART. Most part of the work is done on the host.

By default, ESP-IDF's logging library uses vprintf-like function to write formatted output to dedicated UART. In general, it involves the following steps:

1. Format string is parsed to obtain type of each argument.
2. According to its type, every argument is converted to string representation.
3. Format string combined with converted arguments is sent to UART.

Though the implementation of the vprintf-like function can be optimized to a certain level, all steps above have to be performed in any case and every step takes some time (especially item 3). So it frequently occurs that with additional log added to the program to identify the problem, the program behavior is changed and the problem cannot be reproduced. And in the worst cases, the program cannot work normally at all and ends up with an error or even hangs.

Possible ways to overcome this problem are to use higher UART bitrates (or another faster interface) and/or to move string formatting procedure to the host.

The application level tracing feature can be used to transfer log information to the host using ``esp_apptrace_vprintf`` function. This function does not perform full parsing of the format string and arguments. Instead, it just calculates the number of arguments passed and sends them along with the format string address to the host. On the host, log data is processed and printed out by a special Python script.


Limitations
"""""""""""

Current implementation of logging over JTAG has some limitations:

1. No support for tracing from ``ESP_EARLY_LOGx`` macros.
2. No support for printf arguments whose size exceeds 4 bytes (e.g., ``double`` and ``uint64_t``).
3. Only strings from the .rodata section are supported as format strings and arguments.
4. The maximum number of printf arguments is 256.


How To Use It
"""""""""""""

In order to use logging via trace module, users need to perform the following steps:

1. On the target side, the special vprintf-like function :cpp:func:`esp_apptrace_vprintf` needs to be installed. It sends log data to the host. An example is ``esp_log_set_vprintf(esp_apptrace_vprintf);``. To send log data to UART again, use ``esp_log_set_vprintf(vprintf);``.
2. Follow instructions in items 2-5 in `Application Specific Tracing`_.
3. To print out collected log records, run the following command in terminal: ``$IDF_PATH/tools/esp_app_trace/logtrace_proc.py /path/to/trace/file /path/to/program/elf/file``.


Log Trace Processor Command Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Command usage:

``logtrace_proc.py [-h] [--no-errors] <trace_file> <elf_file>``

Positional arguments:

``trace_file``
    Path to log trace file.
``elf_file``
    Path to program ELF file.

Optional arguments:

``-h``, ``--help``
    Show this help message and exit.
``--no-errors``, ``-n``
    Do not print errors.


.. _app_trace-system-behaviour-analysis-with-segger-systemview:

System Behavior Analysis with SEGGER SystemView
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Another useful ESP-IDF feature built on top of application tracing library is the system level tracing which produces traces compatible with SEGGER SystemView tool (see `SystemView <https://www.segger.com/products/development-tools/systemview/>`_). SEGGER SystemView is a real-time recording and visualization tool that allows to analyze runtime behavior of an application. It is possible to view events in real-time through the UART interface.


How To Use It
"""""""""""""

Support for this feature is enabled by ``Component config`` > ``Application Level Tracing`` > ``FreeRTOS SystemView Tracing`` (:ref:`CONFIG_APPTRACE_SV_ENABLE`) menuconfig option. There are several other options enabled under the same menu:

1. SytemView destination. Select the destination interface: JTAG or UART. In case of UART, it will be possible to connect SystemView application to the {IDF_TARGET_NAME} directly and receive data in real-time.

2. {IDF_TARGET_NAME} timer to use as SystemView timestamp source: (:ref:`CONFIG_APPTRACE_SV_TS_SOURCE`) selects the source of timestamps for SystemView events. In the single core mode, timestamps are generated using {IDF_TARGET_NAME} internal cycle counter running at maximum 240 Mhz (about 4 ns granularity). In the dual-core mode, external timer working at 40 Mhz is used, so the timestamp granularity is 25 ns.

3. Individually enabled or disabled collection of SystemView events (``CONFIG_APPTRACE_SV_EVT_XXX``):

    - Trace Buffer Overflow Event
    - ISR Enter Event
    - ISR Exit Event
    - ISR Exit to Scheduler Event
    - Task Start Execution Event
    - Task Stop Execution Event
    - Task Start Ready State Event
    - Task Stop Ready State Event
    - Task Create Event
    - Task Terminate Event
    - System Idle Event
    - Timer Enter Event
    - Timer Exit Event

ESP-IDF has all the code required to produce SystemView compatible traces, so users can just configure necessary project options (see above), build, download the image to target, and use OpenOCD to collect data as described in the previous sections.

4. Select Pro or App CPU in menuconfig options ``Component config`` > ``Application Level Tracing`` > ``FreeRTOS SystemView Tracing`` to trace over the UART interface in real-time.


OpenOCD SystemView Tracing Command Options
""""""""""""""""""""""""""""""""""""""""""

Command usage:

``esp sysview [start <options>] | [stop] | [status]``

Sub-commands:

``start``
    Start tracing (continuous streaming).
``stop``
    Stop tracing.
``status``
    Get tracing status.

Start command syntax:

  ``start <outfile1> [outfile2] [poll_period [trace_size [stop_tmo]]]``

``outfile1``
    Path to file to save data from PRO CPU. This argument should have the following format: ``file://path/to/file``.
``outfile2``
    Path to file to save data from APP CPU. This argument should have the following format: ``file://path/to/file``.
``poll_period``
    Data polling period (in ms) for available trace data. If greater than 0, then command runs in non-blocking mode. By default, 1 ms.
``trace_size``
    Maximum size of data to collect (in bytes). Tracing is stopped after specified amount of data is received. By default, -1 (trace size stop trigger is disabled).
``stop_tmo``
    Idle timeout (in sec). Tracing is stopped if there is no data for specified period of time. By default, -1 (disable this stop trigger).

.. note::

    If ``poll_period`` is 0, OpenOCD telnet command line will not be available until tracing is stopped. You must stop it manually by resetting the board or pressing Ctrl+C in the OpenOCD window (not the one with the telnet session). Another option is to set ``trace_size`` and wait until this size of data is collected. At this point, tracing stops automatically.

Command usage examples:

.. highlight:: none

1.  Collect SystemView tracing data to files ``pro-cpu.SVDat`` and ``app-cpu.SVDat``. The files will be saved in ``openocd-esp32`` directory.

    ::

        esp sysview start file://pro-cpu.SVDat file://app-cpu.SVDat

    The tracing data will be retrieved and saved in non-blocking mode. To stop this process, enter ``esp sysview stop`` command on OpenOCD telnet prompt, optionally pressing Ctrl+C in the OpenOCD window.

2.  Retrieve tracing data and save them indefinitely.

    ::

        esp sysview start file://pro-cpu.SVDat file://app-cpu.SVDat 0 -1 -1

    OpenOCD telnet command line prompt will not be available until tracing is stopped. To stop tracing, press Ctrl+C in the OpenOCD window.


Data Visualization
""""""""""""""""""

After trace data are collected, users can use a special tool to visualize the results and inspect behavior of the program.

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

    Unfortunately, SystemView does not support tracing from multiple cores. So when tracing from {IDF_TARGET_NAME} with JTAG interfaces in the dual-core mode, two files are generated: one for PRO CPU and another for APP CPU. Users can load each file into separate instances of the tool. For tracing over UART, users can select ``Component config`` > ``Application Level Tracing`` > ``FreeRTOS SystemView Tracing`` in menuconfig Pro or App to choose which CPU has to be traced.

It is uneasy and awkward to analyze data for every core in separate instance of the tool. Fortunately, there is an Eclipse plugin called *Impulse* which can load several trace files, thus making it possible to inspect events from both cores in one view. Also, this plugin has no limitation of 1,000,000 events as compared to the free version of SystemView.

Good instructions on how to install, configure, and visualize data in Impulse from one core can be found `here <https://mcuoneclipse.com/2016/07/31/impulse-segger-systemview-in-eclipse/>`_.

.. note::

    ESP-IDF uses its own mapping for SystemView FreeRTOS events IDs, so users need to replace the original file mapping ``$SYSVIEW_INSTALL_DIR/Description/SYSVIEW_FreeRTOS.txt`` with ``$IDF_PATH/tools/esp_app_trace/SYSVIEW_FreeRTOS.txt``. Also, contents of that ESP-IDF-specific file should be used when configuring SystemView serializer using the above link.

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

    Configure Impulse for Dual Core Traces
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    After installing Impulse and ensuring that it can successfully load trace files for each core in separate tabs, users can add special Multi Adapter port and load both files into one view. To do this, users need to do the following steps in Eclipse:

    1. Open the ``Signal Ports`` view. Go to ``Windows`` > ``Show View`` > ``Other menu``. Find the ``Signal Ports`` view in Impulse folder and double-click it.
    2. In the ``Signal Ports`` view, right-click ``Ports`` and select ``Add`` > ``New Multi Adapter Port``.
    3. In the open dialog box, click ``Add`` and select ``New Pipe/File``.
    4. In the open dialog box, select ``SystemView Serializer`` as Serializer and set path to PRO CPU trace file. Click ``OK``.
    5. Repeat the steps 3-4 for APP CPU trace file.
    6. Double-click the created port. View for this port should open.
    7. Click the ``Start/Stop Streaming`` button. Data should be loaded.
    8. Use the ``Zoom Out``, ``Zoom In`` and ``Zoom Fit`` buttons to inspect data.
    9. For settings measurement cursors and other features, please see `Impulse documentation <https://toem.de/index.php/products/impulse>`_).

    .. note::

        If you have problems with visualization (no data is shown or strange behaviors of zoom action are observed), you can try to delete current signal hierarchy and double-click on the necessary file or port. Eclipse will ask you to create a new signal hierarchy.


.. _app_trace-gcov-source-code-coverage:

Gcov (Source Code Coverage)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Basics of Gcov and Gcovr
""""""""""""""""""""""""

Source code coverage is data indicating the count and frequency of every program execution path that has been taken within a program's runtime. `Gcov <https://en.wikipedia.org/wiki/Gcov>`_ is a GCC tool that, when used in concert with the compiler, can generate log files indicating the execution count of each line of a source file. The `Gcovr <https://gcovr.com/>`_ tool is a utility for managing Gcov and generating summarized code coverage results.

Generally, using Gcov to compile and run programs on the host will undergo these steps:

1. Compile the source code using GCC with the ``--coverage`` option enabled. This will cause the compiler to generate a ``.gcno`` notes files during compilation. The notes files contain information to reconstruct execution path block graphs and map each block to source code line numbers. Each source file compiled with the ``--coverage`` option should have their own ``.gcno`` file of the same name (e.g., a ``main.c`` will generate a ``main.gcno`` when compiled).

2. Execute the program. During execution, the program should generate ``.gcda`` data files. These data files contain the counts of the number of times an execution path was taken. The program will generate a ``.gcda`` file for each source file compiled with the ``--coverage`` option (e.g., ``main.c`` will generate a ``main.gcda``).

3. Gcov or Gcovr can be used to generate a code coverage based on the ``.gcno``, ``.gcda``, and source files. Gcov will generate a text-based coverage report for each source file in the form of a ``.gcov`` file, whilst Gcovr will generate a coverage report in HTML format.


Gcov and Gcovr in ESP-IDF
"""""""""""""""""""""""""""

Using Gcov in ESP-IDF is complicated due to the fact that the program is running remotely from the host (i.e., on the target). The code coverage data (i.e., the ``.gcda`` files) is initially stored on the target itself. OpenOCD is then used to dump the code coverage data from the target to the host via JTAG during runtime. Using Gcov in ESP-IDF can be split into the following steps.

1. :ref:`app_trace-gcov-setup-project`
2. :ref:`app_trace-gcov-dumping-data`
3. :ref:`app_trace-gcov-generate-report`


.. _app_trace-gcov-setup-project:

Setting Up a Project for Gcov
"""""""""""""""""""""""""""""

Compiler Option
~~~~~~~~~~~~~~~

In order to obtain code coverage data in a project, one or more source files within the project must be compiled with the ``--coverage`` option. In ESP-IDF, this can be achieved at the component level or the individual source file level:

- To cause all source files in a component to be compiled with the ``--coverage`` option, you can add ``target_compile_options(${COMPONENT_LIB} PRIVATE --coverage)`` to the ``CMakeLists.txt`` file of the component.
- To cause a select number of source files (e.g., ``source1.c`` and ``source2.c``) in the same component to be compiled with the ``--coverage`` option, you can add ``set_source_files_properties(source1.c source2.c PROPERTIES COMPILE_FLAGS --coverage)`` to the ``CMakeLists.txt`` file of the component.

When a source file is compiled with the ``--coverage`` option (e.g., ``gcov_example.c``), the compiler will generate the ``gcov_example.gcno`` file in the project's build directory.


Project Configuration
~~~~~~~~~~~~~~~~~~~~~

Before building a project with source code coverage, make sure that the following project configuration options are enabled by running ``idf.py menuconfig``.

- Enable the application tracing module by selecting ``Trace Memory`` for the :ref:`CONFIG_APPTRACE_DESTINATION1` option.
- Enable Gcov to the host via the :ref:`CONFIG_APPTRACE_GCOV_ENABLE`.


.. _app_trace-gcov-dumping-data:

Dumping Code Coverage Data
""""""""""""""""""""""""""

Once a project has been complied with the ``--coverage`` option and flashed onto the target, code coverage data will be stored internally on the target (i.e., in trace memory) whilst the application runs. The process of transferring code coverage data from the target to the host is known as dumping.

The dumping of coverage data is done via OpenOCD (see :doc:`JTAG Debugging <../api-guides/jtag-debugging/index>` on how to setup and run OpenOCD). A dump is triggered by issuing commands to OpenOCD, therefore a telnet session to OpenOCD must be opened to issue such commands (run ``telnet localhost 4444``). Note that GDB could be used instead of telnet to issue commands to OpenOCD, however all commands issued from GDB will need to be prefixed as ``mon <oocd_command>``.

When the target dumps code coverage data, the ``.gcda`` files are stored in the project's build directory. For example, if ``gcov_example_main.c`` of the ``main`` component is compiled with the ``--coverage`` option, then dumping the code coverage data would generate a ``gcov_example_main.gcda`` in ``build/esp-idf/main/CMakeFiles/__idf_main.dir/gcov_example_main.c.gcda``. Note that the ``.gcno`` files produced during compilation are also placed in the same directory.

The dumping of code coverage data can be done multiple times throughout an application's lifetime. Each dump will simply update the ``.gcda`` file with the newest code coverage information. Code coverage data is accumulative, thus the newest data will contain the total execution count of each code path over the application's entire lifetime.

ESP-IDF supports two methods of dumping code coverage data form the target to the host:

* Instant Run-Time Dumpgit
* Hard-coded Dump


Instant Run-Time Dump
~~~~~~~~~~~~~~~~~~~~~

An Instant Run-Time Dump is triggered by calling the ``{IDF_TARGET_NAME} gcov`` OpenOCD command (via a telnet session). Once called, OpenOCD will immediately preempt the {IDF_TARGET_NAME}'s current state and execute a built-in ESP-IDF Gcov debug stub function. The debug stub function will handle the dumping of data to the host. Upon completion, the {IDF_TARGET_NAME} will resume its current state.


Hard-coded Dump
~~~~~~~~~~~~~~~

A Hard-coded Dump is triggered by the application itself by calling :cpp:func:`esp_gcov_dump` from somewhere within the application. When called, the application will halt and wait for OpenOCD to connect and retrieve the code coverage data. Once :cpp:func:`esp_gcov_dump` is called, the host must execute the ``esp gcov dump`` OpenOCD command (via a telnet session). The ``esp gcov dump`` command will cause OpenOCD to connect to the {IDF_TARGET_NAME}, retrieve the code coverage data, then disconnect from the {IDF_TARGET_NAME}, thus allowing the application to resume. Hard-coded Dumps can also be triggered multiple times throughout an application's lifetime.

Hard-coded dumps are useful if code coverage data is required at certain points of an application's lifetime by placing :cpp:func:`esp_gcov_dump` where necessary (e.g., after application initialization, during each iteration of an application's main loop).

GDB can be used to set a breakpoint on :cpp:func:`esp_gcov_dump`, then call ``mon esp gcov dump`` automatically via the use a ``gdbinit`` script (see Using GDB from :ref:`jtag-debugging-using-debugger-command-line`).

The following GDB script will add a breakpoint at :cpp:func:`esp_gcov_dump`, then call the ``mon esp gcov dump`` OpenOCD command.

.. code-block:: none

    b esp_gcov_dump
    commands
    mon esp gcov dump
    end


.. note::
    Note that all OpenOCD commands should be invoked in GDB as: ``mon <oocd_command>``.


.. _app_trace-gcov-generate-report:

Generating Coverage Report
""""""""""""""""""""""""""

Once the code coverage data has been dumped, the ``.gcno``, ``.gcda`` and the source files can be used to generate a code coverage report. A code coverage report is simply a report indicating the number of times each line in a source file has been executed.

Both Gcov and Gcovr can be used to generate code coverage reports. Gcov is provided along with the Xtensa toolchain, whilst Gcovr may need to be installed separately. For details on how to use Gcov or Gcovr, refer to `Gcov documentation <https://gcc.gnu.org/onlinedocs/gcc/Gcov.html>`_ and `Gcovr documentation <https://gcovr.com/>`_.


Adding Gcovr Build Target to Project
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To make report generation more convenient, users can define additional build targets in their projects such that the report generation can be done with a single build command.

Add the following lines to the ``CMakeLists.txt`` file of your project.

.. code-block:: none

    include($ENV{IDF_PATH}/tools/cmake/gcov.cmake)
    idf_create_coverage_report(${CMAKE_CURRENT_BINARY_DIR}/coverage_report)
    idf_clean_coverage_report(${CMAKE_CURRENT_BINARY_DIR}/coverage_report)

The following commands can now be used:

    * ``cmake --build build/ --target gcovr-report`` will generate an HTML coverage report in ``$(BUILD_DIR_BASE)/coverage_report/html`` directory.
    * ``cmake --build build/ --target cov-data-clean`` will remove all coverage data files.
