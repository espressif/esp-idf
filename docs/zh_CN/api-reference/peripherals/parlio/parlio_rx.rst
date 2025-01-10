并行 IO RX 驱动程序
=====================

:link_to_translation:`en:[English]`

简介
----

并行 IO RX 单元通常支持以下场景：

- 用于高速数据采集，如摄像头、传感器数据读取
- 作为从机与其他主机进行高速并行通信

应用示例
--------

* :example:`peripherals/parlio/parlio_rx/logic_analyzer` 演示了如何使用并行 IO RX 外设来实现逻辑分析仪。该分析仪可以以高频率并行采样多个 GPIO 上的数据，还可以探测内部或外部信号，并将原始采样数据保存至 Flash 中或者输出到 TCP 流。

API 参考
-------------

.. include-build-file:: inc/parlio_rx.inc
