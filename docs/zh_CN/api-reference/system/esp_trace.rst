ESP Trace
=========

:link_to_translation:`en:[English]`

概述
----

``esp_trace`` 组件是 ESP-IDF 跟踪的入口。它提供公共跟踪 API，管理活动跟踪会话，并将所选编码器（如 SEGGER SystemView）与所选传输（如 apptrace）连接起来。

有关概念概览、架构和使用指南，请参阅 :doc:`/api-guides/tracing/index`。

应用示例
--------

- :example:`system/esp_trace_custom_library` 演示如何将外部跟踪库（编码器）与 ``esp_trace`` 核心集成。

API 参考
--------

类型
^^^^

.. doxygentypedef:: esp_trace_handle_t

.. doxygenstruct:: esp_trace_open_params_t
    :members:

.. doxygenstruct:: esp_trace_config
    :members:

.. doxygenenum:: esp_trace_link_types_t

适配器类型
^^^^^^^^^^

.. doxygenstruct:: esp_trace_encoder_vtable_t
    :members:

.. doxygenstruct:: esp_trace_encoder
    :members:

.. doxygenenum:: esp_trace_transport_cfg_key_t

.. doxygenstruct:: esp_trace_transport_vtable_t
    :members:

.. doxygenstruct:: esp_trace_transport
    :members:

函数
^^^^

.. doxygenfunction:: esp_trace_get_user_params

.. doxygenfunction:: esp_trace_get_active_handle

.. doxygenfunction:: esp_trace_write

.. doxygenfunction:: esp_trace_start

.. doxygenfunction:: esp_trace_stop

.. doxygenfunction:: esp_trace_flush

.. doxygenfunction:: esp_trace_is_host_connected

.. doxygenfunction:: esp_trace_get_link_type

.. doxygenfunction:: esp_trace_panic_handler
