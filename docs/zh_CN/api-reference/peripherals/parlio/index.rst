并行 IO
=======

:link_to_translation:`en:[English]`

简介
----

[`并行 IO 外设 <{IDF_TARGET_TRM_CN_URL}#parlio>`__] 是一个通用的并行接口，可用于连接外部设备，如 LED 矩阵、LCD 显示器、打印机和摄像头。该外设具有独立的 TX 和 RX 单元。每个单元最多可以具有 8 或 16 个数据信号以及 1 或 2 个时钟信号。

并行 IO 外设的 TX 和 RX 驱动程序有各自独立的设计，可分别通过包含头文件 ``driver/parlio_tx.h`` 或 ``driver/parlio_rx.h`` 来使用。

.. toctree::
    :maxdepth: 1

    parlio_tx
    parlio_rx

API 参考
-------------

.. include-build-file:: inc/components/esp_driver_parlio/include/driver/parlio_types.inc
.. include-build-file:: inc/components/hal/include/hal/parlio_types.inc
