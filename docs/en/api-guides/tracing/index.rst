Tracing
===========

:link_to_translation:`zh_CN:[中文]`

Overview
--------

ESP-IDF provides a tracing system for program behavior analysis and debugging. It lets you collect runtime data from {IDF_TARGET_NAME} and send it to a host computer with minimal overhead.

The system is centered on the **esp_trace** component. It owns the public tracing API, manages the active trace session, and connects trace encoders with trace transports. Other tracing features, such as SEGGER SystemView, Gcov, and the apptrace transport, plug into this model.

The ``esp_trace`` component supports common trace formats and transports, and is designed to be extensible. New trace formats and transports can be added without modifying ESP-IDF. For more information about the design, see :doc:`architecture`.

- **Trace formats**: SEGGER SystemView for industry-standard FreeRTOS analysis (see :doc:`sysview`), or your own recorder (see :doc:`custom-trace-library`).
- **Transports**:

  .. list::

      - the :doc:`apptrace transport <transports>` (``app_trace`` component) over JTAG or UART
      :SOC_USB_SERIAL_JTAG_SUPPORTED: - the USB Serial JTAG transport

Choosing Your Path
------------------

.. list-table::
    :header-rows: 1
    :widths: 40 60

    * - Goal
      - Where to look
    * - Analyze FreeRTOS task/ISR behavior
      - :doc:`SEGGER SystemView <sysview>`
    * - Send/receive arbitrary application data, or log to host
      - :doc:`Application Level Tracing transport <transports>`
    * - Collect source code coverage
      - :doc:`Gcov <gcov>`
    * - Trace every function entry and exit automatically
      - :doc:`Function tracing <function-tracing>`
    * - Integrate a third-party trace recorder
      - :doc:`Custom trace library <custom-trace-library>`

Choosing a Transport
--------------------

The trace format and the transport are selected independently. Pick the host link based on your available hardware:

.. list::

    - **apptrace over JTAG**: Highest throughput and host-initiated control (start / stop / dump). Requires a JTAG adapter and OpenOCD on the host. Best for SystemView and on-demand Gcov dumps.
    - **apptrace over UART**: Uses a spare UART instead of a debug probe, at lower throughput than JTAG. Pick a UART that is not used by the console.
    :SOC_USB_SERIAL_JTAG_SUPPORTED: - **USB Serial JTAG**: Uses the chip's built-in USB peripheral over a single USB cable, with no external adapter. Trace data flows over the peripheral's serial (CDC) interface, not its JTAG interface. Available when USB Serial JTAG is not already taken by the console.

Key Features
------------

- **Automatic initialization**: Tracing is configured automatically at startup
- **Multi-core support**: Works on single and dual-core chips
- **Extensible**: Add custom trace formats or transports; see :doc:`architecture`

Quick Start: SystemView Tracing
-------------------------------

To enable SEGGER SystemView tracing for FreeRTOS system analysis:

1. Add the ``espressif/esp_sysview`` dependency to your project's ``idf_component.yml``.

2. Select the external trace library by enabling :ref:`CONFIG_ESP_TRACE_LIB_EXTERNAL <CONFIG_ESP_TRACE_LIB_EXTERNAL>`.

3. Select the apptrace transport by enabling :ref:`CONFIG_ESP_TRACE_TRANSPORT_APPTRACE <CONFIG_ESP_TRACE_TRANSPORT_APPTRACE>`.

4. Set the data destination to JTAG by enabling :ref:`CONFIG_APPTRACE_DEST_JTAG <CONFIG_APPTRACE_DEST_JTAG>`.

5. Build and flash your application:

   .. code-block:: bash

       idf.py build flash

For detailed SystemView usage, OpenOCD setup, and host-side visualization, see :doc:`sysview`.

Detailed Guides
---------------

.. toctree::
    :maxdepth: 1

    architecture
    transports
    sysview
    gcov
    function-tracing
    custom-trace-library

Related Documentation
---------------------

- :doc:`/api-reference/system/esp_trace`: ESP Trace API reference
- :doc:`/api-reference/system/app_trace`: Application Level Tracing (transport) API reference
- :doc:`/api-guides/jtag-debugging/index`: JTAG debugging setup and hardware configuration
- `SEGGER SystemView <https://www.segger.com/products/development-tools/systemview/>`_: Official SystemView tool and documentation
- `OpenOCD <https://openocd.org/>`_: Open On-Chip Debugger

Examples
--------

- :example:`system/app_trace_basic`: Basic application tracing
- :example:`system/sysview_tracing`: SystemView tracing example
- :example:`system/sysview_tracing_heap_log`: Heap tracing with SystemView
- :example:`system/gcov`: Source code coverage over JTAG
- :example:`system/function_tracing`: Compiler-instrumented function entry/exit tracing
- :example:`system/esp_trace_custom_library`: External trace library integration template
