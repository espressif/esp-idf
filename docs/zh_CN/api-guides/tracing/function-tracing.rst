.. _app_trace-function-tracing:

编译器插桩的函数跟踪
====================

:link_to_translation:`en:[English]`

函数跟踪可自动记录每次函数进入和退出，无需手动插入跟踪点。当源文件使用 GCC 标志 ``-finstrument-functions`` 编译时，编译器会在每个函数的开始和结束处插入调用。这些调用进入由 :doc:`esp_trace <index>` 组件提供的钩子，钩子再将事件转发给当前活动的跟踪编码器（例如 SystemView）。事件携带原始的函数地址和调用点地址，SystemView 将其记录为原始地址。将地址解析为函数名的工作单独针对 ELF 文件完成（例如使用 ``addr2line`` 或自定义工具）。

启用函数跟踪
------------

在 ``Component config`` > ``ESP Trace Configuration`` > ``Function Tracing`` 下启用。以下选项控制函数跟踪：

- :ref:`CONFIG_ESP_TRACE_FUNCTION_TRACE` - 构建函数跟踪钩子和运行时。
- :ref:`CONFIG_ESP_TRACE_FUNCTION_TRACE_AUTO_START` - 启用时（默认），记录跟随编码器的状态，并在主机启动会话后立即开始。禁用时，在应用程序调用 :cpp:func:`esp_trace_function_trace_start` 之前不会发出任何事件；仅有活动的主机会话是不够的。

启用该选项会将钩子和运行时编译进构建，但不会为任何代码添加 ``-finstrument-functions``，因此单独启用不会产生任何事件。要跟踪某个组件或文件，请在其自身的 ``CMakeLists.txt`` 中为需要跟踪的源文件添加该标志：

.. code-block:: cmake

    if(CONFIG_ESP_TRACE_FUNCTION_TRACE)
        target_compile_options(${COMPONENT_LIB} PRIVATE -finstrument-functions)
    endif()

使用 GCC 的 ``-finstrument-functions-exclude-file-list`` 和 ``-finstrument-functions-exclude-function-list`` 标志可跳过特定文件或函数。请将插桩范围限定在你自己的组件内，且不要对在关闭 flash 缓存的情况下运行的代码（IRAM 中断服务程序、SPI flash 操作）进行插桩。

解码函数跟踪事件
----------------

在采集到 SystemView 捕获数据后（参见 :doc:`sysview`），使用处理脚本解码函数跟踪事件：

.. code-block:: bash

    $IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -i func -b </path/to/program.elf> -t {IDF_TARGET_TOOLCHAIN_PREFIX}- file:///path/to/trace.svdat

``-i func`` 选项用于选择函数跟踪事件流。脚本使用 ELF 文件和工具链前缀解析地址，然后打印按函数分类的报告，列出每个被跟踪函数的地址、进入和退出次数以及源代码位置。

应用示例
--------

- :example:`system/function_tracing` 演示编译器插桩的函数进入或退出跟踪，以及如何控制对哪些代码进行插桩。
