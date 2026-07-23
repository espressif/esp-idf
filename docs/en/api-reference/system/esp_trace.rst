ESP Trace
=========

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The ``esp_trace`` component is the entry point for ESP-IDF tracing. It provides the public tracing API, manages the active trace session, and connects the selected encoder, such as SEGGER SystemView, with the selected transport, such as apptrace.

For a conceptual overview, architecture, and usage guides, see :doc:`/api-guides/tracing/index`.

Application Examples
--------------------

- :example:`system/esp_trace_custom_library` demonstrates how to integrate an external trace library (encoder) with the ``esp_trace`` core.

API Reference
-------------

Types
^^^^^

.. doxygentypedef:: esp_trace_handle_t

.. doxygenstruct:: esp_trace_open_params_t
    :members:

.. doxygenstruct:: esp_trace_config
    :members:

.. doxygenenum:: esp_trace_link_types_t

Adapter Types
^^^^^^^^^^^^^

.. doxygenstruct:: esp_trace_encoder_vtable_t
    :members:

.. doxygenstruct:: esp_trace_encoder
    :members:

.. doxygenenum:: esp_trace_transport_cfg_key_t

.. doxygenstruct:: esp_trace_transport_vtable_t
    :members:

.. doxygenstruct:: esp_trace_transport
    :members:

Functions
^^^^^^^^^

.. doxygenfunction:: esp_trace_get_user_params

.. doxygenfunction:: esp_trace_get_active_handle

.. doxygenfunction:: esp_trace_write

.. doxygenfunction:: esp_trace_start

.. doxygenfunction:: esp_trace_stop

.. doxygenfunction:: esp_trace_flush

.. doxygenfunction:: esp_trace_is_host_connected

.. doxygenfunction:: esp_trace_get_link_type

.. doxygenfunction:: esp_trace_panic_handler
