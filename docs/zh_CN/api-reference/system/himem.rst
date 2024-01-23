Himem
=====

:link_to_translation:`en:[English]`

概述
--------

对于小于等于 4 MiB 大小的外部内存，MMU 配置为使用“统一映射”，即每个 CPU 地址一对一地映射到外部 SPI RAM 地址，从而透明地访问外部内存。但是，由于外部内存的地址空间只有 4 MiB，因此只有小于等于 4 MiB 大小的 SPI RAM 芯片才能完全透明地使用。

ESP32 仍可使用大于等于 4 MiB 大小的 SPI RAM 芯片。不过，这些芯片上的内存需要通过存储体切换方案 (bank switching) 来访问。ESP-IDF 提供了 **Himem API** 来控制此类存储体切换。具体而言，Himem API 允许在运行时切换地址映射，将特定的 32 K 存储体映射到 4 MiB 地址空间中，从而访问超过 4 MiB 的外部内存。

使用注意事项
--------------

使用 Himem API 前，必须在 menuconfig 中启用 :ref:`CONFIG_SPIRAM_BANKSWITCH_ENABLE`，并在 :ref:`CONFIG_SPIRAM_BANKSWITCH_RESERVE` 中设置为此预留的储存体数量。这会减少由 ``malloc()`` 等函数分配的外部内存量，但允许使用 Himem API 将任何剩余内存映射到预留的存储体中。

Himem API 可以看作是存储体切换方案的一个抽象。具体而言，该 API 允许声明一个或多个地址空间存储体（在 API 中称为“regions”），以及一个或多个需映射到此范围的内存存储体。

示例
-------

ESP-IDF 中提供了对大内存范围进行简单内存测试的示例，请前往 :example:`system/himem` 查看。


API 参考
-------------

.. include-build-file:: inc/himem.inc
