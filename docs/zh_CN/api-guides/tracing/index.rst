ESP 跟踪
========

:link_to_translation:`en:[English]`

概述
----

ESP-IDF 提供了一套跟踪系统，用于程序行为分析和调试。以较小开销从 {IDF_TARGET_NAME} 收集运行时数据，并将数据发送到主机。

该系统以 **esp_trace** 组件为中心。它提供公共跟踪 API，管理活动跟踪会话，并将跟踪编码器与跟踪传输连接起来。SEGGER SystemView、Gcov 和 apptrace 传输等跟踪功能都接入这一模型。

``esp_trace`` 组件支持常用的跟踪格式和传输方式，并设计为可扩展。新的跟踪格式和传输可以在不修改 ESP-IDF 的情况下添加。有关设计的更多信息，请参阅 :doc:`architecture`。

- **跟踪格式**：SEGGER SystemView，用于业界标准的 FreeRTOS 分析（参见 :doc:`sysview`）；或你自己的记录器（参见 :doc:`custom-trace-library`）。
- **传输**：

  .. list::

      - :doc:`apptrace 传输 <transports>`\ （``app_trace`` 组件），可通过 JTAG 或 UART
      :SOC_USB_SERIAL_JTAG_SUPPORTED: - USB Serial JTAG 传输

选择适合你的路径
----------------

.. list-table::
    :header-rows: 1
    :widths: 40 60

    * - 目标
      - 参考文档
    * - 分析 FreeRTOS 任务/中断行为
      - :doc:`SEGGER SystemView <sysview>`
    * - 发送/接收任意应用程序数据，或记录日志到主机
      - :doc:`应用层跟踪传输 <transports>`
    * - 收集源代码覆盖率
      - :doc:`Gcov <gcov>`
    * - 自动跟踪每次函数进入和退出
      - :doc:`函数跟踪 <function-tracing>`
    * - 集成第三方跟踪记录器
      - :doc:`自定义跟踪库 <custom-trace-library>`

选择传输
--------

跟踪格式与传输可独立选择。请根据可用硬件选择主机链路：

.. list::

    - **apptrace（JTAG）**：吞吐量最高，并支持由主机发起的控制（start、stop 或 dump）。需要 JTAG 适配器以及主机上运行的 OpenOCD。适用于 SystemView 以及按需的 Gcov 转储。
    - **apptrace（UART）**：使用空闲的 UART 而非调试探针，吞吐量低于 JTAG。请选择未被控制台占用的 UART。
    :SOC_USB_SERIAL_JTAG_SUPPORTED: - **USB Serial JTAG**：使用芯片内置的 USB 外设，仅需一根 USB 线，无需外部适配器。跟踪数据通过该外设的串行（CDC）接口传输，而非其 JTAG 接口。当 USB Serial JTAG 未被控制台占用时可用。

主要特性
--------

- **自动初始化**：跟踪在启动时自动配置
- **多核支持**：适用于单核和双核芯片
- **可扩展**：添加自定义跟踪格式或传输；参见 :doc:`architecture`

快速入门：SystemView 跟踪
-------------------------

启用 SEGGER SystemView 跟踪以进行 FreeRTOS 系统分析：

1. 在项目的 ``idf_component.yml`` 中添加 ``espressif/esp_sysview`` 依赖。

2. 启用 :ref:`CONFIG_ESP_TRACE_LIB_EXTERNAL <CONFIG_ESP_TRACE_LIB_EXTERNAL>` 以选择外部跟踪库。

3. 启用 :ref:`CONFIG_ESP_TRACE_TRANSPORT_APPTRACE <CONFIG_ESP_TRACE_TRANSPORT_APPTRACE>` 以选择 apptrace 传输。

4. 启用 :ref:`CONFIG_APPTRACE_DEST_JTAG <CONFIG_APPTRACE_DEST_JTAG>` 以将数据目标设为 JTAG。

5. 构建并烧录应用程序：

   .. code-block:: bash

       idf.py build flash

有关 SystemView 的详细用法、OpenOCD 设置和主机端可视化，请参阅 :doc:`sysview`。

详细指南
--------

.. toctree::
    :maxdepth: 1

    architecture
    transports
    sysview
    gcov
    function-tracing
    custom-trace-library

相关文档
--------

- :doc:`/api-reference/system/esp_trace`：ESP Trace API 参考
- :doc:`/api-reference/system/app_trace`：应用层跟踪（传输）API 参考
- :doc:`/api-guides/jtag-debugging/index`：JTAG 调试设置与硬件配置
- `SEGGER SystemView <https://www.segger.com/products/development-tools/systemview/>`_：官方 SystemView 工具和文档
- `OpenOCD <https://openocd.org/>`_：开源片上调试器

示例
----

- :example:`system/app_trace_basic`：基础应用程序跟踪
- :example:`system/sysview_tracing`：SystemView 跟踪示例
- :example:`system/sysview_tracing_heap_log`：基于 SystemView 的堆跟踪
- :example:`system/gcov`：通过 JTAG 获取源代码覆盖率
- :example:`system/function_tracing`：编译器插桩的函数进入/退出跟踪
- :example:`system/esp_trace_custom_library`：外部跟踪库集成模板
