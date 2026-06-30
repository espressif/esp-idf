.. _app_trace-gcov-source-code-coverage:

Gcov（源代码覆盖率）
====================

:link_to_translation:`en:[English]`

Gcov 是一种源代码覆盖率分析工具。在 ESP-IDF 中，目标设备上生成的覆盖率数据通过 apptrace（JTAG 或 UART）转储到主机，在主机端被转换为标准的 ``.gcda`` 文件，并使用常规的主机端工具进行处理。

生成覆盖率报告还需要 ``.gcno`` 注释文件，编译器会在构建时为每个使用 ``--coverage`` 编译的源文件生成该文件。主机端工具将运行时的 ``.gcda`` 计数与 ``.gcno`` 文件以及原始源代码相结合，从而生成覆盖率报告。

Gcov 使用跟踪基础设施进行主机数据传输，但尚未完全遵循 :doc:`esp_trace <index>` 的编码器/传输模型。特别是，它固定使用 apptrace 传输（JTAG 或 UART），不支持选择自定义传输。

覆盖率功能由托管组件 `espressif/esp_gcov <https://components.espressif.com/components/espressif/esp_gcov>`_ 提供。在项目的 ``idf_component.yml`` 中添加该组件：

.. code-block:: yaml

    dependencies:
      espressif/esp_gcov: ^1

覆盖率数据既可以在应用程序中的硬编码位置转储（通过 apptrace 经 JTAG 或 UART），也可以通过 OpenOCD ``esp gcov`` 命令从主机端按需转储（仅限 JTAG）。完整的设置、配置选项以及命令用法，请参阅上面链接的组件 README。

应用示例
--------

- :example:`system/gcov` 演示如何为项目添加代码覆盖率，并通过 JTAG 收集覆盖率数据。
