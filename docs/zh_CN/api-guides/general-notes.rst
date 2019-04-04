ESP-IDF 编程注意事项
======================
:link_to_translation:`en:[English]`

应用程序的启动流程
------------------

本文将会介绍 ESP32 从上电到运行 ``app_main``
函数中间所经历的步骤（即启动流程）。

宏观上，该启动流程可以分为如下 3 个步骤：

1. 一级引导程序被固化在了 ESP32 内部的 ROM 中，它会从 Flash 的
   ``0x1000`` 偏移地址处加载二级引导程序至 RAM(IRAM & DRAM) 中。

2. 二级引导程序从 Flash 中加载分区表和主程序镜像至内存中，主程序中包含了
   RAM 段和通过 Flash 高速缓存映射的只读段。

3. 主程序运行，这时第二个 CPU 和 RTOS 的调度器可以开始运行。

下面会对上述过程进行更为详细的阐述。

一级引导程序
~~~~~~~~~~~~

SoC 复位后，PRO CPU 会立即开始运行，执行复位向量代码，而 APP CPU
仍然保持复位状态。在启动过程中，PRO CPU 会执行所有的初始化操作。APP CPU
的复位状态会在应用程序启动代码的 ``call_start_cpu0``
函数中失效。复位向量代码位于 ESP32 芯片掩膜 ROM 的 ``0x40000400``
地址处，该地址不能被修改。

复位向量调用的启动代码会根据 ``GPIO_STRAP_REG`` 寄存器的值来确定 ESP32
的工作模式，该寄存器保存着复位后 bootstrap
引脚的电平状态。根据不同的复位原因，程序会执行不同的操作：

1. 从深度睡眠模式复位：如果 ``RTC_CNTL_STORE6_REG`` 寄存器的值非零，并且
   ``RTC_CNTL_STORE7_REG`` 寄存器中的 RTC 内存的 CRC
   校验值有效，那么程序会使用 ``RTC_CNTL_STORE6_REG``
   寄存器的值作为入口地址，并立即跳转到该地址运行。如果
   ``RTC_CNTL_STORE6_REG`` 的值为零，或者 ``RTC_CNTL_STORE7_REG`` 中的
   CRC 校验值无效，又或者跳转到 ``RTC_CNTL_STORE6_REG``
   地址处运行的程序返回，那么将会执行上电复位的相关操作。 **注意** ：如果想在这里运行自定义的代码，可以参考
   :doc:`深度睡眠 <deep-sleep-stub>` 文档里面介绍的方法。

2. 上电复位、软件 SoC 复位、看门狗 SoC 复位：检查 ``GPIO_STRAP_REG``
   寄存器，判断是否 UART 或 SDIO 请求进入下载模式。如果是，则配置好 UART
   或者 SDIO，然后等待下载代码。否则程序将会执行软件 CPU
   复位的相关操作。

3. 软件 CPU 复位、看门狗 CPU 复位：根据 EFUSE 中的值配置 SPI
   Flash，然后尝试从 Flash
   中加载代码，这部分的内存将会在后面一小节详细介绍。如果从 Flash
   中加载代码失败，就会将 BASIC 解析器加压缩到 RAM
   中启动。需要注意的是，此时 RTC
   看门狗还在使能状态，如果在几百毫秒内没有任何输入事件，那么看门狗会再次复位
   SoC，重复整个过程。如果解析器收到了来自 UART
   的输入，程序会关闭看门狗。

应用程序的二进制镜像会从 Flash 的 ``0x1000`` 地址处加载。Flash 的第一个
4kB
扇区用于存储安全引导程序和应用程序镜像的签名。有关详细信息，请查看安全启动文档。

.. TODO: describe application binary image format, describe optional flash configuration commands.

二级引导程序
~~~~~~~~~~~~

在 ESP-IDF 中，存放在 Flash 的 ``0x1000``
偏移地址处的二进制镜像就是二级引导程序。二级引导程序的源码可以在 ESP-IDF
的 components/bootloader 目录下找到。请注意，对于 ESP32
芯片来说，这并不是唯一的安排程序镜像的方式。事实上用户完全可以把一个功能齐全的应用程序烧写到
Flash 的 ``0x1000`` 偏移地址处运行，但这超出本文档的范围。ESP-IDF
使用二级引导程序可以增加 Flash 分区的灵活性（使用分区表），并且方便实现
Flash 加密，安全引导和空中升级（OTA）等功能。

当一级引导程序校验并加载完二级引导程序后，它会从二进制镜像的头部找到二级引导程序的入口点，并跳转过去运行。

二级引导程序从 Flash 的 ``0x8000``
偏移地址处读取分区表。详细信息请参阅分区表文档
:doc:`分区表 <partition-tables>` 。二级引导程序会寻找出厂分区和 OTA
分区，然后根据 OTA *信息* 分区的数据决引导哪个分区。

对于选定的分区，二级引导程序将映射到 IRAM 和 DRAM
的数据和代码段复制到它们的加载地址处。对于一些加载地址位于 DROM 和 IROM
区域的段，会通过配置 Flash MMU
为其提供正确的映射。请注意，二级引导程序会为 PRO CPU 和 APP CPU 都配置
Flash MMU，但它只使能了 PRO CPU 的 Flash
MMU。这么做的原因在于二级引导程序的代码被加载到了 APP CPU
的高速缓存使用的内存区域，因此使能 APP CPU
高速缓存的任务就交给了应用程序。一旦代码加载完毕并且设置好 Flash
MMU，二级引导程序会从应用程序二进制镜像文件的头部寻找入口地址，然后跳转到该地址处运行。

目前还不支持添加钩子函数到二级引导程序中以自定义应用程序分区选择的逻辑，但是可以通过别的途径实现这个需求，比如根据某个
GPIO 的不同状态来引导不同的应用程序镜像。此类自定义的功能将在未来添加到
ESP-IDF 中。目前，可以通过将 bootloader
组件复制到应用程序目录并在那里进行必要的更改来自定义引导程序。在这种情况下，ESP-IDF
的编译系统将编译应用程序目录中的组件而不是 ESP-IDF 组件目录。

应用程序启动阶段
~~~~~~~~~~~~~~~~

ESP-IDF 应用程序的入口是 ``components/esp32/cpu_start.c`` 文件中的
``call_start_cpu0``
函数，该函数主要完成了两件事，一是启用堆分配器，二是使 APP CPU
跳转到其入口点—— ``call_start_cpu1`` 函数。PRO CPU 上的代码会给 APP
CPU 设置好入口地址，解除其复位状态，然后等待 APP CPU
上运行的代码设置一个全局标志，以表明 APP CPU 已经正常启动。 完成后，PRO
CPU 跳转到 ``start_cpu0`` 函数，APP CPU 跳转到 ``start_cpu1`` 函数。

``start_cpu0`` 和 ``start_cpu1``
这两个函数都是弱类型的，这意味着如果某些特定的应用程序需要修改初始化顺序，就可以通过重写这两个函数来实现。 ``start_cpu0``
默认的实现方式是初始化用户在 ``menuconfig``
中选择的组件，具体实现步骤可以阅读 ``components/esp32/cpu_start.c``
文件中的源码。请注意，此阶段会调用应用程序中存在的 C++
全局构造函数。一旦所有必要的组件都初始化好，就会创建 *main
task* ，并启动 FreeRTOS 的调度器。

当 PRO CPU 在 ``start_cpu0`` 函数中进行初始化的时候，APP CPU 在
``start_cpu1`` 函数中自旋，等待 PRO CPU 上的调度器启动。一旦 PRO CPU
上的调度器启动后，APP CPU 上的代码也会启动调度器。

主任务是指运行 ``app_main`` 函数的任务，主任务的堆栈大小和优先级可以在
``menuconfig``
中进行配置。应用程序可以用此任务来完成用户程序相关的初始化设置，比如启动其他的任务。应用程序还可以将主任务用于事件循环和其他通用活动。如果
``app_main`` 函数返回，那么主任务将会被删除。

.. _memory-layout:

应用程序的内存布局
------------------

ESP32 芯片具有灵活的内存映射功能，本小节将介绍 ESP-IDF
默认使用这些功能的方式。

ESP-IDF 应用程序的代码可以放在以下内存区域之一。

IRAM（指令 RAM）
~~~~~~~~~~~~~~~~

ESP-IDF 将内部 SRAM0 区域（在技术参考手册中有定义）的一部分分配为指令
RAM。除了开始的 64kB 用作 PRO CPU 和 APP CPU
的高速缓存外，剩余内存区域（从 ``0x40080000`` 至
``0x400A0000`` ）被用来存储应用程序中部分需要在RAM中运行的代码。

一些 ESP-IDF 的组件和 WiFi
协议栈的部分代码通过链接脚本文件被存放到了这块内存区域。

如果一些应用程序的代码需要放在 IRAM 中运行，可以使用 ``IRAM_ATTR``
宏定义进行声明。

.. code:: c

   #include "esp_attr.h"

   void IRAM_ATTR gpio_isr_handler(void* arg)
   {
       // ...      
   }

下面列举了应用程序中可能或者应该放入 IRAM 中运行例子。

-  当注册中断处理程序的时候设置了
   ``ESP_INTR_FLAG_IRAM`` ，那么中断处理程序就必须要放在 IRAM
   中运行。这种情况下，ISR 只能调用存放在 IRAM 或者 ROM
   中的函数。 *注意* ：目前所有 FreeRTOS 的 API 都已经存放到了 IRAM
   中，所以在中断中调用 FreeRTOS 的中断专属 API 是安全的。如果将 ISR
   放在 IRAM 中运行，那么必须使用宏定义 ``DRAM_ATTR`` 将该 ISR
   用到所有常量数据和调用的函数（包括但不限于 ``const char`` 数组）放入
   DRAM 中。

-  可以将一些时间关键的代码放在 IRAM 中，这样可以缩减从 Flash
   加载代码所消耗的时间。ESP32 是通过 32kB 的高速缓存来从外部 Flash
   中读取代码和数据的，将函数放在 IRAM
   中运行可以减少由高速缓存未命中引起的时间延迟。

IROM（代码从 Flash 中运行）
~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果一个函数没有被显式地声明放在 IRAM 或者 RTC 内存中，则将其置于 Flash
中。Flash 技术参考手册中介绍了 Flash MMU 允许代码从 Flash
执行的机制。ESP-IDF 将从 Flash 中执行的代码放在
``0x400D0000 — 0x40400000`` 区域的开始，在启动阶段，二级引导程序会初始化
Flash MMU，将代码在 Flash
中的位置映射到这个区域的开头。对这个区域的访问会被透明地缓存到
``0x40070000 — 0x40080000`` 范围内的两个 32kB 的块中。

请注意，使用 Window ABI ``CALLx`` 指令可能无法访问
``0x40000000 — 0x40400000``
区域以外的代码，所以要特别留意应用程序是否使用了
``0x40400000 — 0x40800000`` 或者 ``0x40800000 — 0x40C00000``
区域，ESP-IDF 默认不会使用这两个区域。

RTC 快速内存
~~~~~~~~~~~~

从深度睡眠模式唤醒后必须要运行的代码要放在 RTC
内存中，更多信息请查阅文档 :doc:`深度睡眠 <deep-sleep-stub>`
。

DRAM（数据 RAM）
~~~~~~~~~~~~~~~~

链接器将非常量静态数据和零初始化数据放入 ``0x3FFB0000 — 0x3FFF0000`` 这
256kB 的区域。注意，如果使用蓝牙堆栈，此区域会减少
64kB（通过将起始地址移至 ``0x3FFC0000`` ）。如果使用了内存跟踪的功能，该区域的长度还要减少
16kB 或者 32kB。放置静态数据后，留在此区域中的剩余空间都用作运行时堆。

常量数据也可以放在 DRAM 中，例如，用在 ISR 中的常量数据（参见上面 IRAM
部分的介绍），为此需要使用 ``DRAM_ATTR`` 宏来声明。

.. code:: c

   DRAM_ATTR const char[] format_string = "%p %x";
   char buffer[64];
   sprintf(buffer, format_string, ptr, val);

毋庸置疑，不建议在 ISR 中使用 ``printf``
和其余输出函数。出于调试的目的，可以在 ISR 中使用 ``ESP_EARLY_LOGx``
来输出日志，不过要确保将 ``TAG`` 和格式字符串都放在了 ``DRAM`` 中。

宏 ``__NOINIT_ATTR`` 可以用来声明将数据放在 ``.noinit``
段中，放在此段中的数据不会在启动时被初始化，并且在软件重启后会保留原来的值。

例子：

.. code:: c

   __NOINIT_ATTR uint32_t noinit_data;

DROM（数据存储在 Flash 中）
~~~~~~~~~~~~~~~~~~~~~~~~~~~

默认情况下，链接器将常量数据放入一个 4MB 区域
(``0x3F400000 — 0x3F800000``) ，该区域用于通过 Flash MMU
和高速缓存来访问外部
Flash。一种特例情况是，字面量会被编译器嵌入到应用程序代码中。

RTC 慢速内存
~~~~~~~~~~~~

从 RTC
内存运行的代码（例如深度睡眠模块的代码）使用的全局和静态变量必须要放在
RTC 慢速内存中。更多详细说明请查看文档
:doc:`深度睡眠 <deep-sleep-stub>` 。

宏 ``RTC_NOINIT_ATTR`` 用来声明将数据放入 RTC
慢速内存中，该数据在深度睡眠唤醒后将保持不变。

例子：

.. code:: c

   RTC_NOINIT_ATTR uint32_t rtc_noinit_data;

DMA 能力要求
------------

大多数的 DMA 控制器（比如 SPI，SDMMC 等）都要求发送/接收缓冲区放在 DRAM
中，并且按字对齐。我们建议将 DMA 缓冲区放在静态变量中而不是堆栈中。使用
``DMA_ATTR`` 宏可以声明该全局/本地的静态变量具备 DMA 能力，例如：

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

1. 如果堆栈在 pSRAM 中，切勿尝试这么做，因为堆栈在 pSRAM 中的话就要按照
   :doc:`片外SRAM <external-ram>` 文档介绍的步骤来操作（至少要在
   ``menuconfig`` 中使能
   ``SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY`` ），所以请确保你的任务不在
   pSRAM 中。

2. 在函数中使用 ``WORD_ALIGNED_ATTR``
   宏来修饰变量，将其放在适当的位置上，比如：

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
