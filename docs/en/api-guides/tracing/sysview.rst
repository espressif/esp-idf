.. _app_trace-system-behaviour-analysis-with-segger-systemview:

System Behavior Analysis with SEGGER SystemView
===============================================

:link_to_translation:`zh_CN:[中文]`

SEGGER SystemView is a real-time recording and visualization tool that allows you to analyze the runtime behavior of an application (task scheduling, ISRs, system events). In the :doc:`esp_trace <index>` model, SystemView is provided as an **encoder**: it formats FreeRTOS and application events into the SystemView protocol, and the data is carried to the host by a :doc:`transport <transports>` (typically apptrace over JTAG, or UART for real-time viewing).

See `SystemView <https://www.segger.com/products/development-tools/systemview/>`_ for the official tool.

Enabling SystemView
-------------------

SystemView support is provided by the managed component ``espressif/esp_sysview``. The SystemView menu becomes visible only after:

1. Adding the component dependency in ``idf_component.yml``:

   .. code-block:: yaml

       dependencies:
         espressif/esp_sysview: ^1

2. Selecting the external library in menuconfig: ``Component config`` > ``ESP Trace Configuration`` > ``Trace library`` > ``External library from component registry``.

After that, you can configure SystemView in ``Component config`` > ``SEGGER SystemView Configuration``. This menu lets you choose the timestamp source (:ref:`CONFIG_ESP_TRACE_TIMESTAMP_SOURCE`), individually enable or disable collection of SystemView events (``CONFIG_SEGGER_SYSVIEW_EVT_XXX``), and select which CPU to trace when using the UART destination.

.. note::

    For the full, up-to-date list of configuration options and host-side setup, see the component README: `esp_sysview <https://components.espressif.com/components/espressif/esp_sysview>`_.

To trace over the UART interface in real-time, first select UART as the destination in ``Component config`` > ``ESP Trace Configuration`` > ``Application Level Tracing``. Then select Pro or App CPU in ``Component config`` > ``ESP Trace Configuration`` > ``SEGGER SystemView``.

OpenOCD SystemView Tracing Command Options
------------------------------------------

When tracing over JTAG, data is collected with a dedicated OpenOCD command. For OpenOCD/JTAG setup, see :doc:`JTAG Debugging </api-guides/jtag-debugging/index>`.

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

Command usage example:

.. highlight:: none

::

    esp sysview start file://pro-cpu.SVDat file://app-cpu.SVDat

The tracing data will be retrieved and saved in non-blocking mode. To stop this process, enter ``esp sysview stop`` command on the OpenOCD telnet prompt, optionally pressing Ctrl+C in the OpenOCD window.

Multi-Core SystemView Tracing Command
"""""""""""""""""""""""""""""""""""""

For SystemView version 3.60 and later, which supports multi-core tracing, use the ``esp sysview_mcore`` command. This command is identical to ``esp sysview`` but uses the official SEGGER SystemView multi-core format. Tracing data from all cores are saved in the same file, which can be opened in SEGGER SystemView v3.60 or later.

Command usage example:

::

    esp sysview_mcore start file://heap_log_mcore.SVDat

For detailed command syntax and options, refer to the ``esp sysview`` command above, as ``esp sysview_mcore`` accepts the same parameters.

Data Visualization
------------------

After trace data are collected, use a special tool to visualize the results and inspect behavior of the program.

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

    **Multi-Core Tracing**

    SystemView version 3.60 and later supports tracing from multiple cores. For multi-core tracing, use the ``esp sysview_mcore`` command to generate a single file compatible with SystemView multi-core format. This command will create a single trace file that can be loaded directly into SystemView 3.60+ for multi-core visualization.

    **Note:** SystemView versions before 3.60 do not support multi-core tracing. For older versions, when tracing from {IDF_TARGET_NAME} with JTAG interfaces in the dual-core mode, two separate files are generated: one for PRO CPU and another for APP CPU. Users can load each file into separate instances of the tool. For tracing over UART, after selecting the external library in menuconfig, users can select ``Component config`` > ``SEGGER SystemView Configuration`` to choose which CPU (Pro or App) has to be traced.

    For older SystemView versions, analyzing data for every core in separate instances can be awkward. An alternative is to use the Eclipse plugin called *Impulse*, which can load several trace files, making it possible to inspect events from both cores in one view. This plugin also has no limitation of 1,000,000 events as compared to the free version of SystemView.

Good instructions on how to install, configure, and visualize data in Impulse from one core can be found `here <https://mcuoneclipse.com/2016/07/31/impulse-segger-systemview-in-eclipse/>`_.

.. note::

    ESP-IDF uses its own mapping for SystemView FreeRTOS events IDs, so users need to replace the original file mapping ``$SYSVIEW_INSTALL_DIR/Description/SYSVIEW_FreeRTOS.txt`` with ``$IDF_PATH/tools/esp_app_trace/SYSVIEW_FreeRTOS.txt``. Also, contents of that ESP-IDF-specific file should be used when configuring SystemView serializer using the above link.

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

    Configure Impulse for Dual-Core Traces
    """"""""""""""""""""""""""""""""""""""

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

Application Examples
--------------------

- :example:`system/sysview_tracing` demonstrates how to trace FreeRTOS task and system events using SEGGER SystemView.
- :example:`system/sysview_tracing_heap_log` demonstrates heap allocation tracing alongside SystemView events.
