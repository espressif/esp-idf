Application Level Tracing
=========================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

ESP-IDF provides a useful feature for application behavior analysis called **Application Level Tracing**. The feature can be enabled in menuconfig and allows transfer of arbitrary data between the host and {IDF_TARGET_NAME} via JTAG interface with minimal overhead on program execution.

Developers can use this library to send application specific state of execution to the host, and receive commands or other types of information in the opposite direction at runtime. The main use cases of this library are:

1. Collecting application specific data, see :ref:`app_trace-application-specific-tracing`.
2. Lightweight logging to the host, see :ref:`app_trace-logging-to-host`.
3. System behaviour analysis, see :ref:`app_trace-system-behaviour-analysis-with-segger-systemview`.

API Reference
-------------

.. include-build-file:: inc/esp_app_trace.inc
.. include-build-file:: inc/esp_sysview_trace.inc
