系统
====

:link_to_translation:`en:[English]`

电源管理
--------

* ``esp_sleep_enable_ext1_wakeup_with_level_mask`` 已被弃用，请用 :cpp:func:`esp_sleep_enable_ext1_wakeup_io` 以及 :cpp:func:`esp_sleep_disable_ext1_wakeup_io`。

单元测试
--------

旧版 Unity 框架版本允许省略 ``TEST_ASSERT_*`` 宏语句末尾的分号，但 ESP-IDF v5.3 所使用的新版本 Unity 不再允许这一操作。

例如，以下代码：

.. code-block:: c

    TEST_ASSERT(some_func() == ESP_OK)

现在将导致编译错误。要修复此问题，请如下所示，在语句末尾添加分号：

    TEST_ASSERT(some_func() == ESP_OK);

分区表
------

分区表生成工具已修复，确保类型为 ``app`` 的分区大小与 flash 扇区（最小擦除单元）大小对齐（请参阅 :ref:`partition-offset-and-size`）。如果分区大小不对齐，分区表生成工具将报错。此修复确保在文件大小接近或等于分区大小的情况下，OTA 更新能够正常进行（擦除操作不会超出分区大小）。

如果 ``app`` 分区大小不是 4 KB 的倍数，请注意，在迁移到 ESP-IDF v5.3 时，必须将此分区的大小与 4 KB（或 4 KB 的倍数）对齐，以确保能成功构建。这不会影响现有设备的分区表，但能确保生成的固件大小保持在可以 OTA 更新的大小范围内。
