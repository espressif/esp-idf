应用程序的启动流程
===================
:link_to_translation:`en:[English]`

本文将会介绍 {IDF_TARGET_NAME} 从上电到运行 ``app_main``
函数中间所经历的步骤（即启动流程）。

宏观上，该启动流程可以分为如下 3 个步骤：

1. 一级引导程序被固化在了 {IDF_TARGET_NAME} 内部的 ROM 中，它会从 Flash 的
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
函数中失效。复位向量代码位于 {IDF_TARGET_NAME} 芯片掩膜 ROM 的 ``0x40000400``
地址处，该地址不能被修改。

复位向量调用的启动代码会根据 ``GPIO_STRAP_REG`` 寄存器的值来确定 {IDF_TARGET_NAME}
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
的 components/bootloader 目录下找到。请注意，对于 {IDF_TARGET_NAME}
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

ESP-IDF 应用程序的入口是 ``components/{IDF_TARGET_PATH_NAME}/cpu_start.c`` 文件中的
``call_start_cpu0``
函数，该函数主要完成了两件事，一是启用堆分配器，二是使 APP CPU
跳转到其入口点—— ``call_start_cpu1`` 函数。PRO CPU 上的代码会给 APP
CPU 设置好入口地址，解除其复位状态，然后等待 APP CPU
上运行的代码设置一个全局标志，以表明 APP CPU 已经正常启动。 完成后，PRO
CPU 跳转到 ``start_cpu0`` 函数，APP CPU 跳转到 ``start_cpu1`` 函数。

``start_cpu0`` 和 ``start_cpu1``
这两个函数都是弱类型的，这意味着如果某些特定的应用程序需要修改初始化顺序，就可以通过重写这两个函数来实现。 ``start_cpu0``
默认的实现方式是初始化用户在 ``menuconfig``
中选择的组件，具体实现步骤可以阅读 ``components/{IDF_TARGET_PATH_NAME}/cpu_start.c``
文件中的源码。请注意，此阶段会调用应用程序中存在的 C++
全局构造函数。一旦所有必要的组件都初始化好，就会创建 *main
task* ，并启动 FreeRTOS 的调度器。

当 PRO CPU 在 ``start_cpu0`` 函数中进行初始化的时候，APP CPU 在
``start_cpu1`` 函数中自旋，等待 PRO CPU 上的调度器启动。一旦 PRO CPU
上的调度器启动后，APP CPU 上的代码也会启动调度器。

.. _app-main-task:

Running the main task
---------------------

主任务是指运行 ``app_main`` 函数的任务，主任务的堆栈大小和优先级可以在
``menuconfig``
中进行配置。应用程序可以用此任务来完成用户程序相关的初始化设置，比如启动其他的任务。应用程序还可以将主任务用于事件循环和其他通用活动。如果
``app_main`` 函数返回，那么主任务将会被删除。
