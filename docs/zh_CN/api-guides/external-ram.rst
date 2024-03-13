片外 RAM
************************

:link_to_translation:`en:[English]`

.. toctree::
   :maxdepth: 1

简介
============
{IDF_TARGET_PSRAM_SIZE:default="Value not updated", esp32="4 MB", esp32s2="10.5 MB", esp32s3="16 MB"}

{IDF_TARGET_NAME} 提供了好几百 KB 的片上 RAM，可以满足大部分需求。但有些场景可能需要更多 RAM，因此 {IDF_TARGET_NAME} 另外提供了高达 {IDF_TARGET_PSRAM_SIZE} 的片外 SPI RAM 存储器供用户使用。片外 RAM 已经集成到内存映射中，在某些范围内与片上 RAM 使用方式相同。


硬件
========

{IDF_TARGET_NAME} 支持与 SPI flash 芯片并联的 SPI PSRAM（伪静态随机存储器）。虽然 {IDF_TARGET_NAME} 支持多种类型的 RAM 芯片，但 ESP-IDF 当前仅支持乐鑫品牌的 PSRAM 芯片，如 ESP-PSRAM32、ESP-PSRAM64 等。

.. note::

    PSRAM 芯片的工作电压分为 1.8 V 和 3.3 V。其工作电压必须与 flash 的工作电压匹配。请查询您 PSRAM 芯片以及 {IDF_TARGET_NAME} 的技术规格书获取准确的工作电压。对于 1.8 V 的 PSRAM 芯片，请确保在启动时将 MTDI 管脚设置为高电平，或者将 {IDF_TARGET_NAME} 中的 eFuses 设置为始终使用 1.8 V 的 VDD_SIO 电平，否则有可能会损坏 PSRAM 和/或 flash 芯片。

.. note::

    乐鑫同时提供模组和系统级封装芯片，集成了兼容的 PSRAM 和 flash，可直接用于终端产品 PCB 中。如需了解更多信息，请前往乐鑫官网。

有关将 SoC 或模组管脚连接到片外 PSRAM 芯片的具体细节，请查阅 SoC 或模组技术规格书。

.. _external_ram_config:


配置片外 RAM
========================

ESP-IDF 完全支持将片外 RAM 集成到您的应用程序中。在启动并完成片外 RAM 初始化后，可以将 ESP-IDF 配置为用多种方式处理片外 RAM：

.. list::

    * :ref:`external_ram_config_memory_map`
    * :ref:`external_ram_config_capability_allocator`
    * :ref:`external_ram_config_malloc` (default)
    * :ref:`external_ram_config_bss`
    :esp32: * :ref:`external_ram_config_noinit`
    :esp32s2 or esp32s3: * :ref:`external_ram_config_instructions`
    :esp32s2 or esp32s3: * :ref:`external_ram_config_rodata`

.. _external_ram_config_memory_map:


集成片外 RAM 到 {IDF_TARGET_NAME} 内存映射
-------------------------------------------

{IDF_TARGET_PSRAM_ADDR_START:default="Value not updated", esp32="0x3F800000", esp32s2="0x3F500000", esp32s3="0x3D000000"}

在 :ref:`CONFIG_SPIRAM_USE` 中选择 "Integrate RAM into memory map（集成片外 RAM 到 {IDF_TARGET_NAME} 内存映射）" 选项。

这是集成片外 RAM 最基础的设置选项，大多数用户需要用到其他更高级的选项。

ESP-IDF 启动过程中，片外 RAM 被映射到以 {IDF_TARGET_PSRAM_ADDR_START} 起始的数据地址空间（字节可寻址），空间大小正好为 SPI RAM 的大小 ({IDF_TARGET_PSRAM_SIZE})。

应用程序可以通过创建指向该区域的指针手动将数据放入片外存储器，同时应用程序全权负责管理片外 SPI RAM，包括协调 Buffer 的使用、防止发生损坏等。

.. _external_ram_config_capability_allocator:


添加片外 RAM 到堆内存分配器
----------------------------

在 :ref:`CONFIG_SPIRAM_USE` 中选择 "Make RAM allocatable using heap_caps_malloc(..., MALLOC_CAP_SPIRAM)" 选项。

启用上述选项后，片外 RAM 被映射到地址 {IDF_TARGET_PSRAM_ADDR_START}，并将这个区域添加到携带 ``MALLOC_CAP_SPIRAM`` 标志的 :doc:`堆内存分配器 </api-reference/system/mem_alloc>` 。

程序如果想从片外存储器分配存储空间，则需要调用 ``heap_caps_malloc(size, MALLOC_CAP_SPIRAM)``，之后可以调用 ``free()`` 函数释放这部分存储空间。

.. _external_ram_config_malloc:


调用 malloc() 分配片外 RAM
---------------------------------

在 :ref:`CONFIG_SPIRAM_USE` 中选择 "Make RAM allocatable using malloc() as well" 选项，该选项为默认选项。

启用此选项后，片外存储器将被添加到内存分配程序（与上一选项相同），同时也将被添加到由标准 ``malloc()`` 函数返回的 RAM 中。

应用程序因此可以使用片外 RAM，无需重写代码就能使用 ``heap_caps_malloc(..., MALLOC_CAP_SPIRAM)``。

如果某次内存分配偏向于片外存储器，您也可以使用 :ref:`CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL` 设置分配空间的大小阈值，控制分配结果：

- 如果分配的空间小于或等于阈值，分配程序将首先选择内部存储器。
- 如果分配的空间大于阈值，分配程序将首先选择外部存储器。

如果优先考虑的内部或外部存储器中没有可用的存储块，分配程序则会选择其他类型存储。

由于有些内存缓冲器仅可在内部存储器中分配，因此需要使用第二个配置项 :ref:`CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL` 定义一个内部内存池，仅限显式的内部存储器分配使用（例如用于 DMA 的存储器）。常规 ``malloc()`` 将不会从该池中分配，但可以使用 :ref:`MALLOC_CAP_DMA <dma-capable-memory>` 和 ``MALLOC_CAP_INTERNAL`` 标志从该池中分配存储器。

.. _external_ram_config_bss:

允许 .bss 段放入片外存储器
-----------------------------------

通过勾选 :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY` 启用该选项。

启用该选项后，PSRAM 被映射到的数据虚拟地址空间将用于存储来自 lwip、net80211、libpp, wpa_supplicant 和 bluedroid ESP-IDF 库中零初始化的数据（BSS 段）。

通过将宏 ``EXT_RAM_BSS_ATTR`` 应用于任何静态声明（未初始化为非零值），可以将附加数据从内部 BSS 段移到片外 RAM。

也可以使用链接器片段方案 ``extram_bss`` 将组件或库的 BSS 段放到片外 RAM 中。

启用此选项可以减少 BSS 段占用的内部静态存储。

剩余的片外 RAM 也可以通过上述方法添加到堆分配器中。

.. only:: esp32

    .. _external_ram_config_noinit:

    允许 .noinit 段放入片外存储器
    -------------------------------------

    通过勾选 :ref:`CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY` 启用该选项。启用该选项后，外部 RAM 中提供的地址空间区域将用于存储未初始化的数据。即使在启动或重新启动期间，放置在该段中的值也不会被初始化或修改。

    通过应用 ``EXT_RAM_NOINIT_ATTR`` 宏，可以将数据从内部 NOINIT 段移到片外 RAM。剩余的片外 RAM 也可以通过上述方法添加到堆分配器中，具体请参考 :ref:`external_ram_config_capability_allocator`。

.. only:: SOC_SPIRAM_XIP_SUPPORTED

    .. _external_ram_config_instructions:

    将 flash 中的指令移至 PSRAM
    -----------------------------------

    启用 :ref:`CONFIG_SPIRAM_FETCH_INSTRUCTIONS` 选项后，flash 中 ``.text`` 部分的数据（用于指令）将被放入 PSRAM。

    启用 :ref:`CONFIG_SPIRAM_FETCH_INSTRUCTIONS` 选项后：

    - flash ``.text`` 部分中的指令将在系统启动时移至 PSRAM。

    - 上述指令对应的虚拟内存范围也将重新映射至 PSRAM。

    如果同时启用 :ref:`CONFIG_SPIRAM_RODATA`，SPI1 flash 操作期间不会禁用 cache。ISR、ISR 回调和相关数据无需放在内部 RAM 中，因此可以优化内部 RAM 的使用。

    .. _external_ram_config_rodata:

    将 flash 中的只读数据移至 PSRAM
    ---------------------------------------

    启用 :ref:`CONFIG_SPIRAM_RODATA` 选项后，flash 中 ``.rodata`` 部分的数据（用于只读数据）将被放入 PSRAM。

    启用 :ref:`CONFIG_SPIRAM_RODATA` 选项后：

    - flash ``.rodata`` 部分中的指令将在系统启动时移至 PSRAM。

    - 上述只读数据对应的虚拟内存范围也将重新映射至 PSRAM。

    如果同时启用 :ref:`CONFIG_SPIRAM_FETCH_INSTRUCTIONS`，SPI1 flash 操作期间不会禁用 cache。ISR、ISR 回调和相关数据无需放在内部 RAM 中，因此可以优化内部 RAM 的使用。

片外 RAM 使用限制
===================

使用片外 RAM 有下面一些限制：

 * flash cache 禁用时（比如，正在写入 flash），片外 RAM 将无法访问；同样，对片外 RAM 的读写操作也将导致 cache 访问异常。出于这个原因，ESP-IDF 不会在片外 RAM 中分配任务堆栈（详见下文）。

.. only:: SOC_PSRAM_DMA_CAPABLE and not esp32s3

    * 片外 RAM 不能用于储存 DMA 事务描述符，也不能用作 DMA 读写操作的缓冲区 (Buffer)。因此，当片外 RAM 启用时，与 DMA 搭配使用的 Buffer 必须先使用 ``heap_caps_malloc(size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL)`` 进行分配，之后可以调用标准 ``free()`` 回调释放 Buffer。注意，尽管 {IDF_TARGET_NAME} 中已有硬件支持 DMA 与片外 RAM，但在 ESP-IDF 中，尚未提供软件支持。

.. only:: esp32s3

    注意，尽管 {IDF_TARGET_NAME} 中已有硬件支持 DMA 与片外 RAM，但仍有以下限制：

    - DMA 访问外部 RAM 的带宽十分有限，尤其是当 CPU 同时访问外部 RAM 时。
    - 将八线 PSRAM 的 :ref:`CONFIG_SPIRAM_SPEED` 配置为 120 MHz 可提高带宽，但使用此选项仍有一定限制，更多信息请参见 :ref:`所有支持的 PSRAM 模式和速度 <flash-psram-combination>`。

* 片外 RAM 与片外 flash 使用相同的 cache 区域，这意味着频繁在片外 RAM 访问的变量可以像在片上 RAM 中一样快速读取和修改。但访问大块数据时（大于 32 KB），cache 空间可能会不足，访问速度将回落到片外 RAM 访问速度。此外，访问大块数据会挤出 flash cache，可能降低代码执行速度。

 * 一般来说，片外 RAM 不会用作任务堆栈存储器。:cpp:func:`xTaskCreate` 及类似函数始终会为堆栈和任务 TCB 分配片上储存器。

可以使用 :ref:`CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY` 选项将任务堆栈放入片外存储器。这时，必须使用 :cpp:func:`xTaskCreateStatic` 指定从片外存储器分配的任务堆栈缓冲区，否则任务堆栈将会从片上存储器分配。


初始化失败
=====================

默认情况下，片外 RAM 初始化失败将终止 ESP-IDF 启动。如果想禁用此功能，可启用 :ref:`CONFIG_SPIRAM_IGNORE_NOTFOUND` 配置选项。

 .. only:: esp32 or esp32s2

    如果启用 :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY`，忽略失败的选项将无法使用，这是因为在链接时，链接器已经向片外存储器分配标志符。


.. only:: not esp32

    加密
    ==========

    可以为存储在外部 RAM 中的数据启用自动加密功能。启用该功能后，通过缓存读写的任何数据将被外部存储器加密硬件自动加密/解密。

    只要启用了 flash 加密功能，就会启用这个功能。关于如何启用 flash 加密以及其工作原理，请参考 :doc:`Flash 加密 </security/flash-encryption>`。


.. only:: esp32

    .. include:: inc/external-ram-esp32-notes.rst

.. _ESP32 系列芯⽚勘误表: https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_cn.pdf
.. _ESP32 芯⽚版本 v3.0 使⽤指南: https://www.espressif.com/sites/default/files/documentation/ESP32_ECO_V3_User_Guide__CN.pdf
