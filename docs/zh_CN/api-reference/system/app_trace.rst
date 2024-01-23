应用级追踪
=========================

:link_to_translation:`en:[English]`

概述
--------

ESP-IDF 支持用于程序行为分析的 **应用级追踪** 功能。在 menuconfig 中启用该功能后，即可通过 JTAG 接口在主机和 {IDF_TARGET_NAME} 之间传输任意数据，最小化程序的执行开销。

在程序运行时，开发人员可使用该库向主机发送应用程序的特定执行状态，并接收来自应用程序的命令或其他信息。该库的主要用例包括：

1. 收集特定应用程序的数据，参见 :ref:`app_trace-application-specific-tracing`。
2. 向主机发送轻量级日志，参见 :ref:`app_trace-logging-to-host`。
3. 系统行为分析，参见 :ref:`app_trace-system-behaviour-analysis-with-segger-systemview`。

API 参考
-------------

.. include-build-file:: inc/esp_app_trace.inc
.. include-build-file:: inc/esp_sysview_trace.inc
