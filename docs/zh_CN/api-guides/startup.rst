应用程序的启动流程
===================

:link_to_translation:`en:[English]`

本文将会介绍 {IDF_TARGET_NAME} 从上电到运行 ``app_main`` 函数中间所经历的步骤（即启动流程）。

宏观上，该启动流程可以分为如下 3 个步骤：

1. :ref:`first-stage-bootloader` 被固化在了 {IDF_TARGET_NAME} 内部的 ROM 中，它会从 flash 的  {IDF_TARGET_CONFIG_BOOTLOADER_OFFSET_IN_FLASH} 偏移地址处加载二级引导程序至 RAM (IRAM & DRAM) 中。

2. :ref:`second-stage-bootloader` 从 flash 中加载分区表和主程序镜像至内存中，主程序中包含了 RAM 段和通过 flash 高速缓存映射的只读段。

3. :ref:`application-startup` 运行，这时第二个 CPU 和 RTOS 的调度器启动。

下面会对上述过程进行更为详细的阐述。

.. _first-stage-bootloader:

一级引导程序
~~~~~~~~~~~~

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

   SoC 复位后，PRO CPU 会立即开始运行，执行复位向量代码，而 APP CPU 仍然保持复位状态。在启动过程中，PRO CPU 会执行所有的初始化操作。APP CPU 的复位状态会在应用程序启动代码的 ``call_start_cpu0`` 函数中失效。复位向量代码位于 {IDF_TARGET_NAME} 芯片掩膜 ROM 处，且不能被修改。

.. only:: not SOC_HP_CPU_HAS_MULTIPLE_CORES

   SoC 复位后，CPU 会立即开始运行，执行所有的初始化操作。复位向量代码位于 {IDF_TARGET_NAME} 芯片掩膜 ROM 处，且不能被修改。

复位向量调用的启动代码会根据 ``GPIO_STRAP_REG`` 寄存器的值来确定 {IDF_TARGET_NAME} 的启动模式，该寄存器保存着复位后 bootstrap 引脚的电平状态。根据不同的复位原因，程序会执行如下操作：

.. list::

   :ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB: #. 从深度睡眠模式复位：如果 ``RTC_CNTL_STORE6_REG`` 寄存器的值非零，且 ``RTC_CNTL_STORE7_REG`` 寄存器中的 RTC 内存的 CRC 校验值有效，那么程序会使用 ``RTC_CNTL_STORE6_REG`` 寄存器的值作为入口地址，并立即跳转到该地址运行。如果  ``RTC_CNTL_STORE6_REG`` 的值为零，或 ``RTC_CNTL_STORE7_REG`` 中的 CRC 校验值无效，又或通过 ``RTC_CNTL_STORE6_REG`` 调用的代码返回，那么则像上电复位一样继续启动。 **注意**：如果想在这里运行自定义的代码，可以参考 :doc:`深度睡眠 <deep-sleep-stub>` 文档里面介绍的深度睡眠存根机制方法。

   #. 上电复位、软件 SoC 复位、看门狗 SoC 复位：检查 ``GPIO_STRAP_REG`` 寄存器，判断是否请求自定义启动模式，如 UART 下载模式。如果是，ROM 会执行此自定义加载模式，否则会像软件 CPU 复位一样继续启动。请参考 {IDF_TARGET_NAME} 技术规格书了解 SoC 启动模式以及具体执行过程。

   #. 软件 CPU 复位、看门狗 CPU 复位：根据 EFUSE 中的值配置 SPI flash，然后尝试从 flash 中加载代码，这部分将会在后面一小节详细介绍。

.. note::

   正常启动模式下会使能 RTC 看门狗，因此，如果进程中断或停止，看门狗将自动重置 SOC 并重复启动过程。如果 strapping GPIOs 已更改，则可能导致 SoC 陷入新的启动模式。

.. only:: esp32

    二级引导程序二进制镜像会从 flash 的 {IDF_TARGET_CONFIG_BOOTLOADER_OFFSET_IN_FLASH} 偏移地址处加载。如果正在使用 :doc:`/security/secure-boot-v1`，则 flash 的第一个 4 kB 扇区用于存储安全启动 IV 以及引导程序镜像的摘要，否则不使用该扇区。

.. only:: esp32s2

    二级引导程序二进制镜像会从 flash 的 {IDF_TARGET_CONFIG_BOOTLOADER_OFFSET_IN_FLASH} 偏移地址处加载。该地址前面的 flash 4 kB 扇区未使用。

.. only:: SOC_KEY_MANAGER_SUPPORTED

    二级引导程序二进制镜像会从 flash 的 {IDF_TARGET_CONFIG_BOOTLOADER_OFFSET_IN_FLASH} 偏移地址处加载。该地址前面的 flash 8 kB 扇区将为密钥管理器保留，用于与 flash 加密 (AES-XTS) 相关的操作。

 .. only:: not (esp32 or esp32s2 or SOC_KEY_MANAGER_SUPPORTED)

    二级引导程序二进制镜像会从 flash 的 {IDF_TARGET_CONFIG_BOOTLOADER_OFFSET_IN_FLASH} 偏移地址处加载。

.. TODO: describe application binary image format, describe optional flash configuration commands.

.. _second-stage-bootloader:

二级引导程序
~~~~~~~~~~~~

在 ESP-IDF 中，存放在 flash 的 {IDF_TARGET_CONFIG_BOOTLOADER_OFFSET_IN_FLASH} 偏移地址处的二进制镜像就是二级引导程序。二级引导程序的源码可以在 ESP-IDF 的 :idf:`components/bootloader` 目录下找到。ESP-IDF 使用二级引导程序可以增加 flash 分区的灵活性（使用分区表），并且方便实现 flash 加密，安全引导和空中升级 (OTA) 等功能。

当一级引导程序校验并加载完二级引导程序后，它会从二进制镜像的头部找到二级引导程序的入口点，并跳转过去运行。

二级引导程序默认从 flash 的 {IDF_TARGET_CONFIG_PARTITION_TABLE_OFFSET} 偏移地址处（:ref:`可配置的值 <CONFIG_PARTITION_TABLE_OFFSET>`）读取分区表。请参考 :doc:`分区表 <partition-tables>` 获取详细信息。引导程序会寻找工厂分区和 OTA 应用程序分区。如果在分区表中找到了 OTA 应用程序分区，引导程序将查询 ``otadata`` 分区以确定应引导哪个分区。更多信息请参考 :doc:`/api-reference/system/ota`。

关于 ESP-IDF 引导程序可用的配置选项，请参考 :doc:`bootloader`。

对于选定的分区，二级引导程序将从 flash 逐段读取二进制镜像：

- 对于在内部 :ref:`iram` 或 :ref:`dram` 中具有加载地址的段，将把数据从 flash 复制到它们的加载地址处。
- 对于一些加载地址位于 :ref:`drom` 或 :ref:`irom` 区域的段，通过配置 flash MMU，可为从 flash 到加载地址提供正确的映射。

.. only:: esp32

    请注意，二级引导程序同时为 PRO CPU 和 APP CPU 配置 flash MMU，但仅使能 PRO CPU 的 flash MMU。原因是二级引导程序代码已加载到 APP CPU 的高速缓存使用的内存区域中。因此使能 APP CPU 高速缓存的任务就交给了应用程序。

一旦处理完所有段（即加载了代码并设置了 flash MMU），二级引导程序将验证应用程序的完整性，并从二进制镜像文件的头部寻找入口地址，然后跳转到该地址处运行。

.. _application-startup:

应用程序启动阶段
~~~~~~~~~~~~~~~~

应用程序启动包含了从应用程序开始执行到 ``app_main`` 函数在主任务内部运行前的所有过程。可分为三个阶段：

- 硬件和基本 C 语言运行环境的端口初始化。
- 软件服务和 FreeRTOS 的系统初始化。
- 运行主任务并调用 ``app_main``。

.. note::

   通常不需要了解 ESP-IDF 应用程序初始化的所有阶段。如果需要仅从应用程序开发人员的角度了解初始化，请跳至 :ref:`app-main-task`。

端口初始化
------------------

ESP-IDF 应用程序的入口是 :idf_file:`components/esp_system/port/cpu_start.c` 文件中的 ``call_start_cpu0`` 函数。这个函数由二级引导加载程序执行，并且从不返回。

该端口层的初始化功能会初始化基本的 C 运行环境 ("CRT"），并对 SoC 的内部硬件进行了初始配置。

.. list::

   - 为应用程序重新配置 CPU 异常（允许应用程序中断处理程序运行，并使用为应用程序配置的选项来处理 :doc:`fatal-errors`，而不是使用 ROM 提供的简易版错误处理程序处理。
   - 如果没有设置选项 :ref:`CONFIG_BOOTLOADER_WDT_ENABLE`，则不使能 RTC 看门狗定时器。
   - 初始化内部存储器（数据和 bss）。
   - 完成 MMU 高速缓存配置。
   :SOC_SPIRAM_SUPPORTED: - 如果配置了 PSRAM，则使能 PSRAM。
   - 将 CPU 时钟设置为项目配置的频率。
   :SOC_MEMPROT_SUPPORTED: - 如果配置了内存保护，则初始化内存保护。
   :esp32: - 根据应用程序头部设置重新配置主 SPI flash，这是为了与 ESP-IDF V4.0 之前的引导程序版本兼容，请参考 :ref:`bootloader-compatibility`。
   :SOC_HP_CPU_HAS_MULTIPLE_CORES: - 如果应用程序被配置为在多个内核上运行，则启动另一个内核并等待其初始化（在类似的“端口层”初始化函数 ``call_start_cpu1`` 内）。

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

   ``call_start_cpu0`` 完成运行后，将调用在 :idf_file:`components/esp_system/startup.c` 中找到的“系统层”初始化函数 ``start_cpu0``。其他内核也将完成端口层的初始化，并调用同一文件中的 ``start_other_cores``。

.. only:: not SOC_HP_CPU_HAS_MULTIPLE_CORES

   ``call_start_cpu0`` 完成运行后，将调用在 :idf_file:`components/esp_system/startup.c` 中找到的“系统层”初始化函数 ``start_cpu0``。

系统初始化
---------------------

主要的系统初始化函数是 ``start_cpu0``。默认情况下，这个函数与 ``start_cpu0_default`` 函数弱链接。这意味着可以覆盖这个函数，增加一些额外的初始化步骤。

主要的系统初始化阶段包括：

.. list::

   - 如果默认的日志级别允许，则记录该应用程序的相关信息（项目名称、:ref:`app-version` 等）。
   - 初始化堆分配器（在这之前，所有分配必须是静态的或在堆栈上）。
   - 初始化 newlib 组件的系统调用和时间函数。
   - 配置断电检测器。
   - 根据 :ref:`串行控制台配置 <CONFIG_ESP_CONSOLE_UART>` 设置 libc stdin、stdout、和 stderr。
   :esp32: - 执行与安全有关的检查，包括为该配置烧录 efuse（包括 :ref:`禁用 ESP32 V3 的 ROM 下载模式 <CONFIG_SECURE_UART_ROM_DL_MODE>`、:ref:`CONFIG_ESP32_DISABLE_BASIC_ROM_CONSOLE`）。
   :not esp32: - 执行与安全有关的检查，包括为该配置烧录 efuse（包括 :ref:`永久限制 ROM 下载模式 <CONFIG_SECURE_UART_ROM_DL_MODE>`)。
   - 初始化 SPI flash API 支持。
   - 调用全局 C++ 构造函数和任何标有 ``__attribute__((constructor))`` 的 C 函数。

二级系统初始化允许单个组件被初始化。如果一个组件有一个用 ``ESP_SYSTEM_INIT_FN`` 宏注释的初始化函数，它将作为二级初始化的一部分被调用。

.. _app-main-task:

运行主任务
---------------------

在所有其他组件都初始化后，主任务会被创建，FreeRTOS 调度器开始运行。

做完一些初始化任务后（需要启动调度器），主任务在固件中运行应用程序提供的函数 ``app_main``。

运行 ``app_main`` 的主任务有一个固定的 RTOS 优先级（比最小值高）和一个 :ref:`可配置的堆栈大小<CONFIG_ESP_MAIN_TASK_STACK_SIZE>`。

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

   主任务的内核亲和性也是可以配置的，请参考 :ref:`CONFIG_ESP_MAIN_TASK_AFFINITY`。

与普通的 FreeRTOS 任务（或嵌入式 C 的 ``main`` 函数）不同，``app_main`` 任务可以返回。如果 ``app_main`` 函数返回，那么主任务将会被删除。系统将继续运行其他的 RTOS 任务。因此可以将 ``app_main`` 实现为一个创建其他应用任务然后返回的函数，或主应用任务本身。

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

    APP CPU 的内核启动流程
    ------------------------------------

    APP CPU 的启动流程类似但更简单：

    当运行系统初始化时，PRO CPU 上的代码会给 APP CPU 设置好入口地址，解除其复位状态，然后等待 APP CPU 上运行的代码设置一个全局标志，以表明 APP CPU 已经正常启动。 完成后，APP CPU 跳转到 :idf_file:`components/esp_system/port/cpu_start.c` 中的 ``call_start_cpu1`` 函数。

    当 ``start_cpu0`` 函数对 PRO CPU 进行初始化的时候，APP CPU 运行 ``start_cpu_other_cores`` 函数。与 ``start_cpu0`` 函数类似，``start_cpu_other_cores`` 函数是弱链接的，默认为 ``start_cpu_other_cores_default`` 函数，但可以由应用程序替换为不同的函数。

    ``start_cpu_other_cores_default`` 函数做了一些与内核相关的系统初始化，然后等待 PRO CPU 启动 FreeRTOS 的调度器，启动完成后，它会执行 ``esp_startup_start_app_other_cores`` 函数，这是另一个默认为 ``esp_startup_start_app_other_cores_default`` 的弱链接函数。

    默认情况下，``esp_startup_start_app_other_cores_default`` 只会自旋，直到 PRO CPU 上的调度器触发中断，以启动 APP CPU 上的 RTOS 调度器。
