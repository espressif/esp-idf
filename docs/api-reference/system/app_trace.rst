Application Level Tracing
=========================

Overview
--------

IDF provides useful feature for program behaviour analysis: application level tracing. It is implemented in the corresponding library and can be enabled via menuconfig. This feature allows to transfer arbitrary data between host and ESP32 via JTAG interface with small overhead on program execution.
Developers can use this library to send application specific state of execution to the host and receive commands or other type of info in the opposite direction at runtime. The main use cases of this library are:

1. Collecting application specific data, see :ref:`app_trace-application-specific-tracing`
2. Lightweight logging to the host, see :ref:`app_trace-logging-to-host`
3. System behaviour analysis, see :ref:`app_trace-system-behaviour-analysis-with-segger-systemview`

API Reference
-------------

.. include:: /_build/inc/esp_app_trace.inc

