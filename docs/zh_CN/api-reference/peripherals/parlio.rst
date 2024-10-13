并行 IO
=======

:link_to_translation:`en:[English]`

简介
----

并行 IO 外设是一个通用的并行接口，可用于连接外部设备，如 LED 矩阵、LCD 显示器、打印机和摄像头。该外设具有独立的 TX 和 RX 单元。每个单元最多可以具有 8 或 16 个数据信号以及 1 或 2 个时钟信号。[1]_

并行 IO 外设的 TX 和 RX 驱动程序有各自独立的设计，可分别通过包含头文件 ``driver/parlio_tx.h`` 或 ``driver/parlio_rx.h`` 来使用。

应用示例
--------

* :example:`peripherals/parlio/parlio_rx/logic_analyzer` 演示了如何使用并行 IO RX 外设来实现逻辑分析仪。该分析仪可以以高频率并行采样多个 GPIO 上的数据，还可以探测内部或外部信号，并通过 flash 或 TCP 流输出原始数据。
* :example:`peripherals/parlio/parlio_tx/simple_rgb_led_matrix` 演示了如何使用 {IDF_TARGET_NAME} 的并行 IO TX 单元驱动支持 HUB75 接口的 RGB LED 矩阵板，并使用 LVGL 库来展示简单的 UI 元素。


API 参考
-------------

.. include-build-file:: inc/parlio_tx.inc
.. include-build-file:: inc/parlio_rx.inc
.. include-build-file:: inc/components/esp_driver_parlio/include/driver/parlio_types.inc
.. include-build-file:: inc/components/hal/include/hal/parlio_types.inc

.. [1]
  不同的 ESP 系列芯片可能具有不同数量的并行 IO TX/RX 实例，数据总线的最大宽度也会有所不同。更多信息，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **并行 IO 控制器 (PARLIO) 章节** [`PDF <{IDF_TARGET_TRM_EN_URL}#parlio>`__]。驱动程序允许你创建更多的驱动程序对象，但当所有可用的硬件资源都被用完时，将会显示返回错误。在进行资源分配时，请时刻注意返回值 (e.g., :cpp:func:`parlio_new_tx_unit`)。
