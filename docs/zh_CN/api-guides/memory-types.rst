.. _memory-layout:

存储器类型
------------------

:link_to_translation:`en:[English]`

{IDF_TARGET_NAME} 芯片具有不同类型的存储器和灵活的存储器映射特性，本小节将介绍 ESP-IDF 默认如何使用这些功能。

ESP-IDF 区分了指令总线（IRAM、IROM、RTC FAST memory）和数据总线 (DRAM、DROM)。指令存储器是可执行的，只能通过 4 字节对齐字读取或写入。数据存储器不可执行，可以通过单独的字节操作访问。有关总线的更多信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *系统和存储器* [`PDF <{IDF_TARGET_TRM_CN_URL}#sysmem>`__]。

.. _dram:

DRAM（数据 RAM）
^^^^^^^^^^^^^^^^^^^^^^^^

非常量静态数据（.data 段）和零初始化数据（.bss 段）由链接器放入内部 SRAM 作为数据存储。此区域中的剩余空间可在程序运行时用作堆。

.. only:: SOC_SPIRAM_SUPPORTED

   通过应用 ``EXT_RAM_BSS_ATTR`` 宏，零初始化数据也可以放入外部 RAM。使用这个宏需要启用 :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY`。详情请见 :ref:`external_ram_config_bss`。

.. only:: esp32

   如果使用蓝牙堆栈，内部 DRAM 区域的可用大小将减少 64 KB（由于起始地址移动到 ``0x3FFC0000``）。如果使用内存跟踪功能，该区域的长度还会减少 16 KB 或 32 KB。由于 ROM 引起的一些内存碎片问题，不可能将所有可用的 DRAM 用于静态分配，但是剩余的 DRAM 在运行时仍可用作堆。

   .. note::

    ESP32 上有 520 KB 的可用 SRAM（320 KB 的 DRAM 和 200 KB 的 IRAM）。 但是，由于技术限制，用于静态分配的 DRAM 最多可为 160 KB。 剩余的 160 KB（DRAM 总共 320 KB）只能在运行时分配为堆。

.. only:: not esp32

   .. note::

    静态分配的 DRAM 的最大值也会因编译应用程序的 :ref:`iram` 大小而减小。运行时可用的堆内存会因应用程序的总静态 IRAM 和 DRAM 使用而减少。

常量数据也可能被放入 DRAM，例如当它被用于 non-flash-safe ISR 时（具体请参考 :ref:`how-to-place-code-in-iram`）。


"noinit" DRAM
=============

可以将 ``__NOINIT_ATTR`` 宏用作属性，从而将数据放入 ``.noinit`` 部分。放入该部分的值在启动时不会被初始化，在软件重启后也会保持值不变。

.. only:: esp32

   通过使用 ``EXT_RAM_NOINIT_ATTR`` 宏，noinit 数据也可以放入外部 RAM 中。为此，需要启用 :ref:`CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY`，可参考 :ref:`external_ram_config_noinit`。如果没有启用 :ref:`CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY`， ``EXT_RAM_NOINIT_ATTR`` 会和 ``__NOINIT_ATTR`` 一样，将数据放入内部 RAM 的 ``.noinit`` 部分。

示例::

    __NOINIT_ATTR uint32_t noinit_data;


.. _iram:

IRAM（指令 RAM）
^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

   ESP-IDF 将内部 SRAM0 的部分区域分配为指令 RAM。可在 *{IDF_TARGET_NAME} 技术参考手册* > *系统和存储器* > *内部存储器* [`PDF <{IDF_TARGET_TRM_CN_URL}#sysmem>`__] 中查看 IRAM 区域的定义。该内存中第一个 64 KB 块用于 PRO 和 APP MMU 缓存，其余部分（即从 ``0x40080000`` 到 ``0x400A0000``）用于存储需要从 RAM 运行的应用程序部分。

.. only:: esp32s2

   ESP-IDF 将内部 SRAM 的部分区域分配为指令 RAM。可在 *{IDF_TARGET_NAME} 技术参考手册* > *系统和存储器* > *内部存储器* [`PDF <{IDF_TARGET_TRM_CN_URL}#sysmem>`__] 中查看 IRAM 区域的定义。该内存中第一个块（最多 32 KB）用于 MMU 缓存，其余部分用于存储需要从 RAM 运行的应用程序部分。

.. only:: not esp32

    .. note::

         内部 SRAM 中不用于指令 RAM 的部分都会作为 :ref:`dram` 供静态数据和动态分配（堆）使用。


何时需要将代码放入 IRAM
======================================

以下情况时应将部分应用程序放入 IRAM：

- 如果在注册中断处理程序时使用了 ``ESP_INTR_FLAG_IRAM``，则中断处理程序必须要放入 IRAM。更多信息可参考 :ref:`iram-safe-interrupt-handlers`。

- 可将一些时序关键代码放入 IRAM，以减少从 flash 中加载代码造成的相关损失。{IDF_TARGET_NAME} 通过 MMU 缓存从 flash 中读取代码和数据。在某些情况下，将函数放入 IRAM 可以减少由缓存未命中造成的延迟，从而显著提高函数的性能。


.. _how-to-place-code-in-iram:

如何将代码放入 IRAM
=====================================

借助链接器脚本，一些代码会被自动放入 IRAM 区域中。

如果需要将某些特定的应用程序代码放入 IRAM，可以使用 :doc:`linker-script-generation` 功能并在组件中添加链接器脚本片段文件，在该片段文件中，可以给整个目标源文件或其中的个别函数打上 ``noflash`` 标签。更多信息可参考 :doc:`linker-script-generation`。

或者，也可以通过使用 ``IRAM_ATTR`` 宏在源代码中指定需要放入 IRAM 的代码::

    #include "esp_attr.h"

    void IRAM_ATTR gpio_isr_handler(void* arg)
    {
        // ...
    }

放入 IRAM 后可能会导致 IRAM 安全中断处理程序出现问题：

* ``IRAM_ATTR`` 函数中的字符串或常量可能没有自动放入 RAM 中，这时可以使用 ``DRAM_ATTR`` 属性进行标记，或者也可以使用链接器脚本方法将它们自动放入 RAM 中。

  .. code-block:: c

    void IRAM_ATTR gpio_isr_handler(void* arg)
    {
       const static DRAM_ATTR uint8_t INDEX_DATA[] = { 45, 33, 12, 0 };
       const static char *MSG = DRAM_STR("I am a string stored in RAM");
    }

注意，具体哪些数据需要被标记为 ``DRAM_ATTR`` 可能很难确定。如果没有被标记为 ``DRAM_ATTR``，某些变量或表达式有时会被编译器别为常量（即使它们没有被标记为 ``const``）并将其放入 flash 中。

* GCC 的优化会自动生成跳转表或 switch/case 查找表，并将这些表放在 flash 中。IDF 默认在编译所有文件时使用 ``-fno-jump-tables -fno-tree-switch-conversion`` 标志来避免这种情况。

可以为不需要放置在 IRAM 中的单个源文件重新启用跳转表优化。关于如何在编译单个源文件时添加 ``-fno-jump-tables -fno-tree-switch-conversion`` 选项，请参考 :ref:`component_build_control`。


.. _irom:

IROM（代码从 flash 中运行）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如果一个函数没有被显式地声明放在 IRAM 或者 RTC 存储器中，则它会放在 flash 中。由于 IRAM 空间有限，应用程序的大部分二进制代码都需要放入 IROM 中。

.. only:: esp32

    允许从 flash 中执行代码的 flash MMU 机制可参考 {IDF_TARGET_NAME} 技术参考手册* > *存储器管理和保护单元 (MMU, MPU)* [`PDF <{IDF_TARGET_TRM_CN_URL}#mpummu>`__]。

在 :doc:`启动 <startup>` 过程中，从 IRAM 中运行的引导加载程序配置 MMU flash 缓存，将应用程序的指令代码区域映射到指令空间。通过 MMU 访问的 flash 使用一些内部 SRAM 进行缓存，访问缓存的 flash 数据与访问其他类型的内部存储器一样快。


.. _drom:

DROM（数据存储在 flash 中）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. highlight:: c

默认情况下，链接器将常量数据放入一个映射到 MMU flash 缓存的区域中。这与 :ref:`irom` 部分相同，但此处用于只读数据而不是可执行代码。

唯一没有默认放入 DROM 的常量数据是被编译器嵌入到应用程序代码中的字面常量。这些被放置在周围函数的可执行指令中。

``DRAM_ATTR`` 属性可以用来强制将常量从 DROM 放入 :ref:`dram` 部分（见上文）。

.. only:: SOC_RTC_SLOW_MEM_SUPPORTED

    RTC Slow memory（RTC 慢速存储器）
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    从 RTC 存储器运行的代码中使用的全局和静态变量必须放入 RTC Slow memory 中。例如 :doc:`深度睡眠 <deep-sleep-stub>` 变量可以放在 RTC Slow memory 中，而不是 RTC FAST memory，或者也可以放入由 :doc:`/api-reference/system/ulp` 访问的代码和变量。

    ``RTC_NOINIT_ATTR`` 属性宏可以用来将数据放入 RTC Slow memory。放入此类型存储器的值从深度睡眠模式中醒来后会保持值不变。

    示例::

            RTC_NOINIT_ATTR uint32_t rtc_noinit_data;


.. only:: SOC_RTC_FAST_MEM_SUPPORTED

    RTC FAST memory（RTC 快速存储器）
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    .. only:: esp32c6 or esp32h2

        .. note::

            对于 {IDF_TARGET_NAME}，RTC 存储器已被重新重命名为 LP（低功耗）存储器。在与 {IDF_TARGET_NAME} 相关的 IDF 代码、文档以及技术参考手册中，可能会出现这两个术语混用的情况。


    RTC FAST memory 的同一区域既可以作为指令存储器也可以作为数据存储器进行访问。从深度睡眠模式唤醒后必须要运行的代码要放在 RTC 存储器中，更多信息请查阅文档 :doc:`深度睡眠 <deep-sleep-stub>`。

    .. only:: esp32

        在单核模式下 (:ref:`CONFIG_FREERTOS_UNICORE`)，除非禁用 :ref:`CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP` 选项，否则剩余的 RTC FAST memory 会被添加到堆中。该部分内存可以和 :ref:`DRAM` 互换使用，但是访问速度稍慢，且不具备 DMA 功能。

        :ref:`CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP` 选项在双核模式下不可用，因为 {IDF_TARGET_NAME} 的 RTC FAST memory 只能由 PRO CPU 访问。

    .. only:: not esp32

        除非禁用 :ref:`CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP` 选项，否则剩余的 RTC FAST memory 会被添加到堆中。该部分内存可以和 :ref:`DRAM` 互换使用，但是访问速度稍慢一点。


.. only:: SOC_MEM_TCM_SUPPORTED

    紧密耦合内存 (TCM)
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    TCM 是靠近 CPU 放置的内存，支持在 CPU 频率下直接访问，无需通过 cache。虽然在一般情况下，TCM 的效率或速度相较 cache 偏低，但是访问 TCM 所需的时间是可以预测且始终一致的。具有稳定的访问速度对于时间关键型例程来说十分重要，因此 TCM 对于此类例程而言非常有用。


具备 DMA 功能
^^^^^^^^^^^^^^^^^^^

.. highlight:: c

大多数的 DMA 控制器（比如 SPI、sdmmc 等）都要求发送/接收缓冲区放在 DRAM 中，并且按字对齐。我们建议将 DMA 缓冲区放在静态变量而不是堆栈中。使用 ``DMA_ATTR`` 宏可以声明该全局/本地的静态变量具备 DMA 功能，例如::

    DMA_ATTR uint8_t buffer[]="I want to send something";

    void app_main()
    {
        // 初始化代码
        spi_transaction_t temp = {
            .tx_buffer = buffer,
            .length = 8 * sizeof(buffer),
        };
        spi_device_transmit(spi, &temp);
        // 其它程序
    }

或者::

    void app_main()
    {
        DMA_ATTR static uint8_t buffer[] = "I want to send something";
        // 初始化代码
        spi_transaction_t temp = {
            .tx_buffer = buffer,
            .length = 8 * sizeof(buffer),
        };
        spi_device_transmit(spi, &temp);
        // 其它程序
    }

也可以通过使用 :ref:`MALLOC_CAP_DMA <dma-capable-memory>` 标志来动态分配具备 DMA 能力的内存缓冲区。

在堆栈中放置 DMA 缓冲区
^^^^^^^^^^^^^^^^^^^^^^^^^^^

可以在堆栈中放置 DMA 缓冲区，但建议尽量避免。如果实在有需要的话，请注意以下几点：

.. list::

    :SOC_SPIRAM_SUPPORTED: - 如果堆栈在 PSRAM 中，则不建议将 DRAM 缓冲区放在堆栈上。如果任务堆栈在 PSRAM 中，则必须执行 :doc:`external-ram` 中描述的几个步骤。
    - 在函数中使用 ``WORD_ALIGNED_ATTR`` 宏来修饰变量，将其放在适当的位置上，比如::

        void app_main()
        {
            uint8_t stuff;
            WORD_ALIGNED_ATTR uint8_t buffer[] = "I want to send something";   //否则 buffer 会被存储在 stuff 变量后面
            // 初始化代码
            spi_transaction_t temp = {
                .tx_buffer = buffer,
                .length = 8 * sizeof(buffer),
            };
            spi_device_transmit(spi, &temp);
            // 其它程序
        }
