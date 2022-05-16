SPI1 Flash 并发约束
=========================================

指令/数据 cache （用以执行固件）与 SPI1 外设（由像 SPI Flash 驱动一样的驱动程序控制）共享 SPI0/1 总线。因此，对 SPI1 外设的操作会对整个系统造成显著的影响。这类操作包括调用 SPI Flash API 或者其他 SPI1 总线上的驱动，任何 flash 操作（如读取、写入、擦除）或者其他用户定义的 SPI 操作，无论是对主 flash 或者其他各类的 SPI 从机。

.. only:: not esp32c3

   在 {IDF_TARGET_NAME} 上， flash 读取/写入/擦除时 cache 必须被禁用。

.. only:: esp32c3

    在 {IDF_TARGET_NAME} 上，默认启用的配置选项 :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND` 允许 flash / PSRAM 的 cache 访问和 SPI1 的操作存并发地执行。更多详情，参见 :ref:`auto_suspend` 。

    然而当该选项被禁用时，读取/写入/擦除 flash 时， cache 必须被禁用。使用驱动访问 SPI1 的相关约束参见 :ref:`impact_disabled_cache` 。这些约束会带来更多的 IRAM / DRAM 消耗。

.. _impact_disabled_cache:

当 cache 被禁用时
----------------------------

这意味着当 flash 擦写操作发生时，所有的 CPU 都只能执行 IRAM 中的代码，而且必须从 DRAM 中读取数据。如果您使用本文档中 API 函数，上述限制将自动生效且透明（无需您额外关注），但这些限制可能会影响系统中的其他任务的性能。

除 SPI0/1 以外的 SPI 总线上的其它 flash 芯片则不受这种限制。

请参阅 :ref:`应用程序内存分布 <memory-layout>`，查看 IRAM、DRAM 和 flash cache 的区别。

.. only:: not CONFIG_FREERTOS_UNICORE

    为避免意外读取 flash cache，一个 CPU 在启动 flash 写入或擦除操作时，另一个 CPU 将阻塞，并且在 flash 操作完成前，所有 CPU 上，所有的非 IRAM 安全的中断都会被禁用。

另请参阅 :ref:`esp_flash_os_func` 和 :ref:`spi_bus_lock` 。

.. _iram-safe-interrupt-handlers:

IRAM 安全中断处理程序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如果您需要在 flash 操作期间运行中断处理程序（比如低延迟操作），请在 :doc:`注册中断处理程序 </api-reference/system/intr_alloc>` 时设置 ``ESP_INTR_FLAG_IRAM``。

请确保中断处理程序访问的所有数据和函数（包括其调用的数据和函数）都存储在 IRAM 或 DRAM 中。有两种方法可供使用：

使用属性宏
""""""""""""""""""""

为函数添加 ``IRAM_ATTR`` 属性::

    #include "esp_attr.h"

    void IRAM_ATTR gpio_isr_handler(void* arg)
    {
        // ...
    }


为常量添加 ``DRAM_ATTR`` 和 ``DRAM_STR`` 属性::

    void IRAM_ATTR gpio_isr_handler(void* arg)
    {
       const static DRAM_ATTR uint8_t INDEX_DATA[] = { 45, 33, 12, 0 };
       const static char *MSG = DRAM_STR("I am a string stored in RAM");
    }

辨别哪些数据应标记为 ``DRAM_ATTR`` 可能会比较困难，除非明确标记为 ``DRAM_ATTR``，否则编译器依然可能将某些变量或表达式当做常量（即便没有 ``const`` 标记），并将其放入 flash。

使用链接脚本
""""""""""""""""""""""

参见 :doc:`/api-guides/linker-script-generation`。

如果函数或符号未被正确放入 IRAM/DRAM 中，当中断处理程序在 flash 操作期间从 flash cache 中读取数据，则会产生非法指令异常（这是因为代码未被正确放入 IRAM）或读取垃圾数据（这是因为常数未被正确放入 DRAM），而导致崩溃。

.. only:: esp32c3

   .. include:: auto_suspend.inc
