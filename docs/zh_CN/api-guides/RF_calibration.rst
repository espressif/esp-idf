RF 校准
==============

:link_to_translation:`en:[English]`

{IDF_TARGET_NAME} 在 RF 初始化过程中支持以下三种 RF 校准：

1. 部分校准

2. 全面校准

3. 不校准

部分校准
-------------------

在 RF 初始化期间，默认使用部分校准，该方法基于存储在 NVS 中的完全校准数据实现。要使用此校准方法，请前往 ``menuconfig`` 并启用 :ref:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE`。

全面校准
----------------

在以下几种情况下，使用全面校准：

1. NVS 不存在。

2. 存储校准数据的 NVS 分区已擦除。

3. 硬件 MAC 地址变更。

4. PHY 库版本变更。

5. 从 NVS 分区加载的 RF 校准数据损坏。

全面校准耗时比部分校准长约 100 ms。当启动时长的重要性不高时，建议使用全面校准。要切换到全面校准，请前往 ``menuconfig`` 并禁用 :ref:`CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE`。如果使用了默认的部分校准，作为最后补救措施，可以使用以下两种方法添加函数，触发全面校准：

1. 如果不介意擦除 NVS 分区中的所有数据，那么擦除 NVS 分区是最简单的方法。

2. 在初始化 Wi-Fi 和 Bluetooth®/Bluetooth Low Energy 前，根据某些条件（例如，在某些诊断模式下提供的选项）调用 API :cpp:func:`esp_phy_erase_cal_data_in_nvs`，此时仅会擦除 NVS 分区的 PHY 命名空间。

不校准
---------------

设备从 Deep-sleep 模式中唤醒时，可以不校准。

PHY 初始化数据
-----------------------

PHY 初始化数据用于 RF 校准，通过以下两种方法，可以获取 PHY 初始化数据。

一是使用默认初始化数据，该数据位于头文件 :idf_file:`components/esp_phy/{IDF_TARGET_PATH_NAME}/include/phy_init_data.h` 中。默认初始化数据会在编译后嵌入到应用程序的二进制文件，然后存储在只读存储器 (DROM) 中。要使用默认的初始化数据，请前往 ``menuconfig`` 并禁用 :ref:`CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION`。

二是将初始化数据存储在 PHY 数据分区中。默认的分区表中已经包含了 PHY 数据分区，但是使用自定义分区表时，请确保在自定义分区表中包含一个 PHY 数据分区（类型：``data``，子类型：``phy``）。无论使用自定义分区表还是默认分区表，只要初始化数据存储在分区中，就需要将其烧录到相应的分区中，否则运行时可能会出现错误。如果想要使用存储在分区中的初始化数据，请前往 ``menuconfig`` 并启用选项 :ref:`CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION`。

API 参考
-------------

.. include-build-file:: inc/esp_phy_init.inc
.. include-build-file:: inc/esp_phy_cert_test.inc
