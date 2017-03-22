Application Level Tracing
=========================

Overview
--------

IDF provides useful feature for program behaviour analysis: application level tracing. It is implemented in the corresponding library and can be enabled via menuconfig. This feature allows to transfer arbitrary data between host and ESP32 via JTAG interface with small overhead on program execution.
Developers can use this library to send application specific state of execution to the host and receive commands or other type of info in the opposite direction at runtime. The main use cases of this library are:

1. System behaviour analysis.
2. Lightweight logging to the host.
3. Collecting application specific data.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`app_trace/include/esp_app_trace.h`

Macros
^^^^^^

.. doxygendefine:: ESP_APPTRACE_TMO_INFINITE

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: esp_apptrace_dest_t


Functions
^^^^^^^^^

.. doxygenfunction:: esp_apptrace_init
.. doxygenfunction:: esp_apptrace_buffer_get
.. doxygenfunction:: esp_apptrace_buffer_put
.. doxygenfunction:: esp_apptrace_write
.. doxygenfunction:: esp_apptrace_vprintf_to
.. doxygenfunction:: esp_apptrace_vprintf
.. doxygenfunction:: esp_apptrace_read
.. doxygenfunction:: esp_apptrace_flush
.. doxygenfunction:: esp_apptrace_flush_nolock
