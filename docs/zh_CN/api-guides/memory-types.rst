.. _memory-layout:

应用程序的内存布局
------------------

{IDF_TARGET_NAME} 芯片具有灵活的内存映射功能，本小节将介绍 ESP-IDF 默认使用这些功能的方式。

ESP-IDF 应用程序的代码可以放在以下内存区域之一。

.. _iram:

IRAM（指令 RAM）
~~~~~~~~~~~~~~~~

ESP-IDF 将内部 SRAM0 区域（在技术参考手册中有定义）的一部分分配为指令 RAM。除了开始的 64kB 用作 PRO CPU 和 APP CPU 的高速缓存外，剩余内存区域（从 ``0x40080000`` 至 ``0x400A0000`` ）被用来存储应用程序中部分需要在 RAM 中运行的代码。

一些 ESP-IDF 的组件和 WiFi 协议栈的部分代码通过链接脚本文件被存放到了这块内存区域。

如果一些应用程序的代码需要放在 IRAM 中运行，可以使用 ``IRAM_ATTR`` 宏定义进行声明。

.. code:: c

   #include "esp_attr.h"

   void IRAM_ATTR gpio_isr_handler(void* arg)
   {
       // ...
   }

下面列举了应用程序中可能或者应该放入 IRAM 中运行例子。

-  当注册中断处理程序的时候设置了 ``ESP_INTR_FLAG_IRAM`` ，那么中断处理程序就必须要放在 IRAM 中运行。这种情况下，ISR 只能调用存放在 IRAM 或者 ROM 中的函数。 *注意* ：目前所有 FreeRTOS 的 API 都已经存放到了 IRAM 中，所以在中断中调用 FreeRTOS 的中断专属 API 是安全的。如果将 ISR 放在 IRAM 中运行，那么必须使用宏定义 ``DRAM_ATTR`` 将该 ISR 用到所有常量数据和调用的函数（包括但不限于 ``const char`` 数组）放入 DRAM 中。

-  可以将一些时间关键的代码放在 IRAM 中，这样可以缩减从 Flash 加载代码所消耗的时间。{IDF_TARGET_NAME} 是通过 32kB 的高速缓存来从外部 Flash 中读取代码和数据的，将函数放在 IRAM 中运行可以减少由高速缓存未命中引起的时间延迟。


.. _irom:

IROM（代码从 Flash 中运行）
~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果一个函数没有被显式地声明放在 IRAM 或者 RTC 内存中，则将其置于 Flash 中。Flash 技术参考手册中介绍了 Flash MMU 允许代码从 Flash 执行的机制。ESP-IDF 将从 Flash 中执行的代码放在 ``0x400D0000 — 0x40400000`` 区域的开始，在启动阶段，二级引导程序会初始化 Flash MMU，将代码在 Flash 中的位置映射到这个区域的开头。对这个区域的访问会被透明地缓存到 ``0x40070000 — 0x40080000`` 范围内的两个 32kB 的块中。

请注意，使用 Window ABI ``CALLx`` 指令可能无法访问 ``0x40000000 — 0x40400000`` 区域以外的代码，所以要特别留意应用程序是否使用了 ``0x40400000 — 0x40800000`` 或者 ``0x40800000 — 0x40C00000`` 区域，ESP-IDF 默认不会使用这两个区域。

RTC 快速内存
~~~~~~~~~~~~

从深度睡眠模式唤醒后必须要运行的代码要放在 RTC 内存中，更多信息请查阅文档 :doc:`深度睡眠 <deep-sleep-stub>`。

.. _dram:

DRAM（数据 RAM）
~~~~~~~~~~~~~~~~

链接器将非常量静态数据和零初始化数据放入 ``0x3FFB0000 — 0x3FFF0000`` 这 256kB 的区域。注意，如果使用蓝牙堆栈，此区域会减少 64kB（通过将起始地址移至 ``0x3FFC0000`` ）。如果使用了内存跟踪的功能，该区域的长度还要减少 16kB 或者 32kB。放置静态数据后，留在此区域中的剩余空间都用作运行时堆。

常量数据也可以放在 DRAM 中，例如，用在 ISR 中的常量数据（参见上面 IRAM 部分的介绍），为此需要使用 ``DRAM_ATTR`` 宏来声明。

.. code:: c

   DRAM_ATTR const char[] format_string = "%p %x";
   char buffer[64];
   sprintf(buffer, format_string, ptr, val);

毋庸置疑，不建议在 ISR 中使用 ``printf`` 和其余输出函数。出于调试的目的，可以在 ISR 中使用 ``ESP_EARLY_LOGx`` 来输出日志，不过要确保将 ``TAG`` 和格式字符串都放在了 ``DRAM`` 中。

宏 ``__NOINIT_ATTR`` 可以用来声明将数据放在 ``.noinit`` 段中，放在此段中的数据不会在启动时被初始化，并且在软件重启后会保留原来的值。

例子：

.. code:: c

   __NOINIT_ATTR uint32_t noinit_data;

.. _drom:

DROM（数据存储在 Flash 中）
~~~~~~~~~~~~~~~~~~~~~~~~~~~

默认情况下，链接器将常量数据放入一个 4MB 区域 (``0x3F400000 — 0x3F800000``) ，该区域用于通过 Flash MMU 和高速缓存来访问外部 Flash。一种特例情况是，字面量会被编译器嵌入到应用程序代码中。

RTC 慢速内存
~~~~~~~~~~~~

从 RTC 内存运行的代码（例如深度睡眠模块的代码）使用的全局和静态变量必须要放在 RTC 慢速内存中。更多详细说明请查看文档 :doc:`深度睡眠 <deep-sleep-stub>` 。

宏 ``RTC_NOINIT_ATTR`` 用来声明将数据放入 RTC 慢速内存中，该数据在深度睡眠唤醒后将保持不变。

例子：

.. code:: c

   RTC_NOINIT_ATTR uint32_t rtc_noinit_data;

DMA 能力要求
------------

大多数的 DMA 控制器（比如 SPI，SDMMC 等）都要求发送/接收缓冲区放在 DRAM 中，并且按字对齐。我们建议将 DMA 缓冲区放在静态变量中而不是堆栈中。使用 ``DMA_ATTR`` 宏可以声明该全局/本地的静态变量具备 DMA 能力，例如：

.. code:: c

   DMA_ATTR uint8_t buffer[]="I want to send something";

   void app_main()
   {
       // 初始化代码...
       spi_transaction_t temp = {
           .tx_buffer = buffer,
           .length = 8*sizeof(buffer),
       };
       spi_device_transmit( spi, &temp );
       // 其他程序
   }

或者：

.. code:: c

   void app_main()
   {
       DMA_ATTR static uint8_t buffer[]="I want to send something";
       // 初始化代码...
       spi_transaction_t temp = {
           .tx_buffer = buffer,
           .length = 8*sizeof(buffer),
       };
       spi_device_transmit( spi, &temp );
       // 其他程序
   }

在堆栈中放置 DMA 缓冲区仍然是允许的，但是你必须记住：

.. list::

   :SOC_SPIRAM_SUPPORTED:- 如果堆栈在 pSRAM 中，切勿尝试这么做，因为堆栈在 pSRAM 中的话就要按照 :doc:`片外SRAM <external-ram>` 文档介绍的步骤来操作（至少要在 ``menuconfig`` 中使能 ``SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY`` ），所以请确保你的任务不在 PSRAM 中。

   - 在函数中使用 ``WORD_ALIGNED_ATTR`` 宏来修饰变量，将其放在适当的位置上，比如：

      .. code:: c

         void app_main()
         {
            uint8_t stuff;
            WORD_ALIGNED_ATTR uint8_t buffer[]="I want to send something";   //否则buffer数组会被存储在stuff变量的后面
            // 初始化代码...
            spi_transaction_t temp = {
               .tx_buffer = buffer,
               .length = 8*sizeof(buffer),
            };
            spi_device_transmit( spi, &temp );
            // 其他程序
         }
