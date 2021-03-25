片外 RAM
************************

:link_to_translation:`en:[English]`

.. toctree::
   :maxdepth: 1

简介
============

{IDF_TARGET_NAME} 提供了好几百 KB 的片上 RAM，可以满足大部分需求。但有些场景可能需要更多 RAM，因此 {IDF_TARGET_NAME} 另外提供了高达 4 MB 的片外 SPI RAM 存储器以供用户使用。片外 RAM 被添加到内存映射中，在某些范围内与片上 RAM 使用方式相同。


硬件
========

{IDF_TARGET_NAME} 支持与 SPI Flash 芯片并联的 SPI PSRAM。虽然 {IDF_TARGET_NAME} 支持多种类型的 RAM 芯片，但 ESP-IDF 当前仅支持乐鑫品牌的 PSRAM 芯片，如 ESP-PSRAM32、ESP-PSRAM64 等。

.. note:: PSRAM 芯片的工作电压分为 1.8 V 和 3.3 V。其工作电压必须与 flash 的工作电压匹配。请查询您 PSRAM 芯片以及 {IDF_TARGET_NAME} 的技术规格书获取准确的工作电压。对于 1.8 V 的 PSRAM 芯片，请确保在启动时将 MTDI 管脚设置为高电平，或者将 {IDF_TARGET_NAME} 中的 eFuses 设置为始终使用 1.8 V 的 VDD_SIO 电平，否则有可能会损坏 PSRAM 和/或 flash 芯片。

.. note:: 乐鑫同时提供模组和系统级封装芯片，集成了兼容的 PSRAM 和 flash，可直接用于终端产品 PCB 中。如需了解更多信息，请前往乐鑫官网。

有关将 SoC 或模组管脚连接到片外 PSRAM 芯片的具体细节，请查阅 SoC 或模组技术规格书。

.. _external_ram_config:


配置片外 RAM
========================

ESP-IDF 完全支持将外部存储器集成到您的应用程序中。在启动并完成片外 RAM 初始化后，可以将 ESP-IDF 配置为以多种方式处理片外 RAM：

.. list::

    * :ref:`external_ram_config_memory_map`
    * :ref:`external_ram_config_capability_allocator`
    * :ref:`external_ram_config_malloc` (default)
    :esp32: * :ref:`external_ram_config_bss`

.. _external_ram_config_memory_map:


集成片外 RAM 到 {IDF_TARGET_NAME} 内存映射
-------------------------------------------

在 :ref:`CONFIG_SPIRAM_USE` 中选择 "Integrate RAM into memory map（集成片外 RAM 到 {IDF_TARGET_NAME} 内存映射）" 选项。

这是集成片外 RAM 最基础的设置选项，大多数用户需要用到其他更高级的选项。

ESP-IDF 启动过程中，片外 RAM 被映射到以 0x3F800000 起始的数据地址空间（字节可寻址），空间大小正好为 SPI RAM 的大小 (4 MB)。

应用程序可以通过创建指向该区域的指针手动将数据放入片外存储器，同时应用程序全权负责管理片外 SPI RAM，包括协调 Buffer 的使用、防止发生损坏等。

.. _external_ram_config_capability_allocator:


添加片外 RAM 到堆内存分配器
----------------------------

在 :ref:`CONFIG_SPIRAM_USE` 中选择 "Make RAM allocatable using heap_caps_malloc(..., MALLOC_CAP_SPIRAM)" 选项。

启用上述选项后，片外 RAM 被映射到地址 0x3F800000，并将这个区域添加到 :doc:`堆内存分配器 </api-reference/system/mem_alloc>` 里携带 ``MALLOC_CAP_SPIRAM`` 的标志

程序如果想从片外存储器分配存储空间，则需要调用 ``heap_caps_malloc(size, MALLOC_CAP_SPIRAM)``，之后可以调用 ``free()`` 函数释放这部分存储空间。

.. _external_ram_config_malloc:


调用 malloc() 分配片外 RAM
---------------------------------

在 :ref:`CONFIG_SPIRAM_USE` 中选择 "Make RAM allocatable using malloc() as well" 选项，该选项为默认选项。

启用此选项后，片外存储器将被添加到内存分配程序（与上一选项相同），同时也将被添加到由标准 ``malloc()`` 函数返回的 RAM 中。

这允许应用程序使用片外 RAM，无需重写代码就能使用 ``heap_caps_malloc(..., MALLOC_CAP_SPIRAM)``。

如果某次内存分配偏向于片外存储器，您也可以使用 :ref:`CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL` 设置分配空间的大小阈值，控制分配结果：

- 如果分配的空间小于阈值，分配程序将首先选择内部存储器。
- 如果分配的空间等于或大于阈值，分配程序将首先选择外部存储器。

如果优先考虑的内部或外部存储器中没有可用的存储块，分配程序则会选择其他类型存储。

由于有些 Buffer 仅可在内部存储器中分配，因此需要使用第二个配置项 :ref:`CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL` 定义一个内部存储池，仅限显式的内部存储器分配使用（例如用于 DMA 的存储器）。常规 ``malloc()`` 将不会从该池中分配，但可以使用 :ref:`MALLOC_CAP_DMA <dma-capable-memory>` 和 ``MALLOC_CAP_INTERNAL`` 旗标从该池中分配存储器。

.. only:: esp32

   .. _external_ram_config_bss:

   允许 .bss 段放入片外存储器
   -----------------------------

   通过检查 :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY` 启用该选项，此选项配置与上面三个选项互不影响。

   启用该选项后，从 0x3F800000 起始的地址空间将用于存储来自 lwip、net80211、libpp 和 bluedroid ESP-IDF 库中零初始化的数据（BSS 段）。

   ``EXT_RAM_ATTR`` 宏应用于任何静态声明（未初始化为非零值）之后，可以将附加数据从内部 BSS 段移到片外 RAM。

   也可以使用链接器片段方案 ``extram_bss`` 将组件或库的 BSS 段放到片外 RAM 中。

   启用此选项可以减少 BSS 段占用的内部静态存储。

   剩余的片外 RAM 也可以通过上述方法添加到堆分配器中。


片外 RAM 使用限制
===================

使用片外 RAM 有下面一些限制：

 * Flash cache 禁用时（比如，正在写入 flash），片外 RAM 将无法访问；同样，对片外 RAM 的读写操作也将导致 cache 访问异常。出于这个原因，ESP-IDF 不会在片外 RAM 中分配任务堆栈（详见下文）。

 * 片外 RAM 不能用于储存 DMA 事物描述符，也不能用作 DMA 读写操作的缓冲区 (Buffer)。与 DMA 搭配使用的 Buffer 必须先使用 ``heap_caps_malloc(size, MALLOC_CAP_DMA)`` 进行分配，之后可以调用标准 ``free()`` 回调释放 Buffer。

 * 片外 RAM 与片外 flash 使用相同的 cache 区域，这意味着频繁在片外 RAM 访问的变量可以像在片上 RAM 中一样快速读取和修改。但访问大块数据时（大于 32 KB），cache 空间可能会不足，访问速度将回落到片外 RAM 访问速度。此外，访问大块数据可以挤出 flash cache，可能会降低代码执行速度。

 * 片外 RAM 不可用作任务堆栈存储器。因此 :cpp:func:`xTaskCreate` 及类似函数将始终为堆栈和任务 TCB 分配片上储存器，而 :cpp:func:`xTaskCreateStatic` 类型的函数将检查传递的 Buffer 是否属于片上存储器。

默认情况下，片外 RAM 初始化失败将终止 ESP-IDF 启动。如果想禁用此功能，可启用 :ref:`CONFIG_SPIRAM_IGNORE_NOTFOUND` 配置选项。

.. only:: esp32

   如果启用 :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY`，:ref:`CONFIG_SPIRAM_IGNORE_NOTFOUND` 选项将不能使用，这是因为在链接时，链接器已经向片外 RAM 分配符号。


.. only:: esp32

   .. include:: inc/external-ram-esp32-notes.rst

.. _ESP32 ECO: https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_cn.pdf
.. _ESP32 ECO V3 User Guide: https://www.espressif.com/sites/default/files/documentation/ESP32_ECO_V3_User_Guide__CN.pdf

