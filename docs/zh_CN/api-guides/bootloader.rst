引导加载程序 (Bootloader)
==========================

:link_to_translation:`en:[English]`

ESP-IDF 软件引导加载程序 (Bootloader) 主要执行以下任务：

1. 内部模块的最小化初始配置；
2. 如果配置了 :doc:`/security/flash-encryption` 和/或 :doc:`Secure </security/secure-boot-v2>`，则对其进行初始化。
3. 根据分区表和 ota_data（如果存在）选择需要引导的应用程序 (app) 分区；
4. 将此应用程序镜像加载到 RAM（IRAM 和 DRAM）中，最后把控制权转交给此应用程序。

引导加载程序位于 flash 的 {IDF_TARGET_CONFIG_BOOTLOADER_OFFSET_IN_FLASH} 偏移地址处。

关于启动过程以及 ESP-IDF 引导加载程序的更多信息，请参考 :doc:`startup`。

.. _bootloader-compatibility:

引导加载程序兼容性
-------------------

建议使用最新发布的 :doc:`ESP-IDF 版本 </versions>`。OTA（空中升级）更新可以在现场烧录新的应用程序，但不能烧录一个新的引导加载程序。因此，引导加载程序支持引导从 ESP-IDF 新版本中构建的应用程序。

但不支持引导从 ESP-IDF 旧版本中构建的程序。如果现有产品可能需要将应用程序降级到旧版本，那么在手动更新 ESP-IDF 时，请继续使用旧版本 ESP-IDF 引导加载程序的二进制文件。

.. note::

   如果在生产中测试现有产品的 OTA 更新，请确保测试中使用的 ESP-IDF 引导加载程序二进制文件与生产中部署的相同。

.. only:: esp32

   ESP-IDF V2.1 之前的版本
   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

   与新版本相比，ESP-IDF V2.1 之前的版本构建的引导加载程序对硬件的配置更少。使用这些早期 ESP-IDF 版本的引导加载程序并构建新应用程序时，请启用配置选项 :ref:`CONFIG_APP_COMPATIBLE_PRE_V2_1_BOOTLOADERS`。

   ESP-IDF V3.1 之前的版本
   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

   ESP-IDF V3.1 之前的版本构建的引导加载程序不支持分区表二进制文件中的 MD5 校验。使用这些 ESP-IDF 版本的引导加载程序并构建新应用程序时，请启用配置选项 :ref:`CONFIG_APP_COMPATIBLE_PRE_V3_1_BOOTLOADERS`。

   ESP-IDF V5.1 之前的版本
   ^^^^^^^^^^^^^^^^^^^^^^^^^^

   ESP-IDF V5.1 之前的版本构建的引导加载程序不支持 :ref:`CONFIG_ESP_SYSTEM_ESP32_SRAM1_REGION_AS_IRAM`。使用这些 ESP-IDF 版本的引导加载程序并构建新应用程序时，不应使用该选项。


配置 SPI flash
^^^^^^^^^^^^^^^^^^^^^^^

每个 ESP-IDF 应用程序或引导加载程序的二进制文件中都包含一个文件头，其中内置了 :ref:`CONFIG_ESPTOOLPY_FLASHMODE`、:ref:`CONFIG_ESPTOOLPY_FLASHFREQ`、和 :ref:`CONFIG_ESPTOOLPY_FLASHSIZE`。这些是用于在启动时配置 SPI flash。

ROM 中的 :ref:`first-stage-bootloader` 从 flash 中读取 :ref:`second-stage-bootloader` 文件头中的配置信息，并使用这些信息来加载剩余的 :ref:`second-stage-bootloader`。然而，此时系统的时钟速度低于其被配置的速度，并且在这个阶段，只支持部分 flash 模式。因此，当 :ref:`second-stage-bootloader` 运行时，它会从当前应用程序的二进制文件头中读取数据（而不是从引导加载程序的文件头中读取数据），并使用这些数据重新配置 flash。这样的配置流程可让 OTA 更新去更改当前使用的 SPI flash 的配置。

.. only:: esp32

   ESP-IDF V4.0 版本之前的引导加载程序使用其自身的文件头来配置 SPI flash，这意味着无法在 OTA 更新时更改 SPI flash 配置。为了与旧引导加载程序兼容，应用程序在其启动期间使用应用程序文件头中的配置信息重新初始化 flash 配置。

日志级别
---------

引导加载程序日志的级别默认为 "Info"。通过设置 :ref:`CONFIG_BOOTLOADER_LOG_LEVEL` 选项，可以增加或减少这个等级。这个日志级别与应用程序中使用的日志级别是分开的（见 :doc:`/api-reference/system/log`）。

降低引导加载程序日志的详细程度可以稍微缩短整个项目的启动时间。

恢复出厂设置
------------

在更新出现问题时，最好能有一种方法让设备回到已知的正常状态，这时可选择恢复出厂设置。

要回到原始出厂设置并清除所有用户设置，请在引导加载程序中配置 :ref:`CONFIG_BOOTLOADER_FACTORY_RESET`。

以下两种方式可以将设备恢复出厂设置。

- 清除一个或多个数据分区。:ref:`CONFIG_BOOTLOADER_DATA_FACTORY_RESET` 选项允许用户选择哪些数据分区在恢复出厂设置时需要被擦除。

  用户可以使用以逗号分隔的列表形式指定分区的名称，为了提高可读性，可以选择添加空格（如：``nvs, phy_init, nvs_custom``）。

  请确保选项里指定的分区名称和分区表中的名称相同。此处不能指定 “app” 类型的分区。

- 从“工厂”应用分区启动。当启用 :ref:`CONFIG_BOOTLOADER_OTA_DATA_ERASE` 选项，恢复出厂设置后，设备将从默认的“工厂”应用分区启动（如果分区表中没有“工厂”应用分区，则从默认的 OTA 应用分区启动）。这个恢复过程是通过擦除 OTA 数据分区来完成的，OTA 数据分区中保存了当前选择的 OTA 分区槽。“工厂”应用分区槽（如果存在）永远不会通过 OTA 更新，因此重置为从“工厂”应用分区启动则意味着让固件应用程序恢复正常状态。

这两个配置选项都可以独立启用。

此外，以下配置选项用于配置触发恢复出厂设置的条件：

- :ref:`CONFIG_BOOTLOADER_NUM_PIN_FACTORY_RESET`- 输入管脚 (GPIO) 的编号，该管脚用于触发恢复出厂设置。必须在重置时将此管脚拉低或拉高（可配置）才能触发出厂重置事件。

- :ref:`CONFIG_BOOTLOADER_HOLD_TIME_GPIO`- 管脚电平保持时间（默认为 5 秒）。设备重置后，管脚电平必须保持该设定的时间，才能执行恢复出厂设置或引导测试分区（如适用）。

- :ref:`CONFIG_BOOTLOADER_FACTORY_RESET_PIN_LEVEL` - 设置管脚电平高低。设备重置后，根据此设置将管脚拉高或拉低，才能触发出厂重置事件。如果管脚具有内部上拉，则上拉会在管脚采样前生效。有关管脚内部上拉的详细信息，请参考 {IDF_TARGET_NAME} 的技术规格书。

.. only:: SOC_RTC_FAST_MEM_SUPPORTED

    如果应用程序需要知道设备是否触发了出厂重置，可以通过调用 :cpp:func:`bootloader_common_get_rtc_retain_mem_factory_reset_state` 函数来确定：

    - 如果读取到设备出厂重置状态为 true，会返回状态 true，说明设备已经触发出厂重置。此后会重置状态为 false，以便后续的出厂重置触发判断。
    - 如果读取到设备出厂重置状态为 false，会返回状态 false，说明设备并未触发出厂重置，或者保存此状态的内存区域已失效。

    同时需要注意该功能需要占用部分 RTC FAST 内存（占用的内存与 :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` 大小相同）。

.. only:: not SOC_RTC_FAST_MEM_SUPPORTED

    有时应用程序需要知道设备是否触发了出厂重置，但 {IDF_TARGET_NAME} 没有 RTC FAST 内存，因此没有相应的 API 可用于监测。然而也有方法实现出厂重置监测，比如，设置一个在出厂重置时会被引导加载程序擦除的 NVS 分区（需将此分区添加到 :ref:`CONFIG_BOOTLOADER_DATA_FACTORY_RESET` 中）。在这个 NVS 分区中保存一个令牌数据 "factory_reset_state"，让该令牌在应用程序中自增。"factory_reset_state" 为 0 时则表明触发了出厂重置。

.. _bootloader_boot_from_test_firmware:

从测试固件启动
-------------------

用户可以编写特殊固件用于生产环境中测试，并在需要的时候运行。此时需要在项目分区表中专门申请一块分区用于保存该测试固件，其类型为 app，子类型为 test（详情请参考 :doc:`/api-guides/partition-tables`）。

实现该测试应用固件需要为测试应用创建一个完全独立的 ESP-IDF 项目（ESP-IDF 中的每个项目仅构建一个应用）。该测试应用可以独立于主项目进行开发和测试，然后在生成测试时作为一个预编译 .bin 文件集成到主项目的测试应用程序分区的地址。

要在主项目的引导加载程序中支持这个功能，请设置 :ref:`CONFIG_BOOTLOADER_APP_TEST` 并配置以下三个选项：

- :ref:`CONFIG_BOOTLOADER_NUM_PIN_APP_TEST` - 设置启动 TEST 分区的管脚编号，该管脚将被配置为输入并启用内部上拉。要触发测试应用，必须在重置时将此管脚拉低或拉高（可配置）。

   释放管脚输入并重启设备后，将重新启用默认的启动顺序，即启动工厂分区或任意 OTA 应用分区槽。

- :ref:`CONFIG_BOOTLOADER_HOLD_TIME_GPIO` - 设置 GPIO 电平保持的时间，默认为 5 秒。设备重置后，管脚电平必须保持该设定的时间，才能执行恢复出厂设置或引导测试分区（如适用）。

- :ref:`CONFIG_BOOTLOADER_APP_TEST_PIN_LEVEL` - 配置应在 GPIO 的高电平还是低电平上触发测试分区启动。若 GPIO 有内部上拉，则该功能在采样管脚前就会被启用。关于管脚内部上拉的详细信息，请参考 {IDF_TARGET_NAME} 数据规格书。

回滚
--------

回滚和反回滚功能也必须在引导程序中配置。

请参考 :doc:`OTA API 参考文档 </api-reference/system/ota>` 中的 :ref:`app_rollback` 和 :ref:`anti-rollback` 章节。

.. _bootloader-watchdog:

看门狗
----------

芯片配备两组看门狗定时器：主系统看门狗定时器 (MWDT_WDT) 和 RTC 看门狗定时器 (RTC_WDT)。芯片上电时，两组看门狗定时器都会被启用，但在引导加载程序中，两组看门狗定时器都会被禁用。设置 :ref:`CONFIG_BOOTLOADER_WDT_ENABLE` （默认设置）可以重新启用 RTC 看门狗定时器，用于跟踪从启用引导加载程序到调用用户主函数的时间。此期间内 RTC 看门狗定时器始终可用，并且如果在 9 秒内没有应用程序成功启动，则 RTC 看门狗定时器会自动重置芯片。这一功能可以有效防止启动过程中由于电源不稳定而导致的死机。

- 可以通过设置 :ref:`CONFIG_BOOTLOADER_WDT_TIME_MS` 并重新编译引导加载程序来调整超时时间。
- 通过禁用 :ref:`CONFIG_BOOTLOADER_WDT_ENABLE` 设置并重新编译引导加载程序，可以在引导加载程序中禁用 RTC 看门狗，但并不建议这样做。
- 请参阅 :ref:`app-hardware-watchdog-timers`，了解如何在应用程序中使用 RTC_WDT。

.. _bootloader-size:

引导加载程序大小
---------------------

{IDF_TARGET_MAX_BOOTLOADER_SIZE:default = "64 KB (0x10000 bytes)", esp32 = "48 KB (0xC000 bytes)"}
{IDF_TARGET_MAX_PARTITION_TABLE_OFFSET:default = "0x12000", esp32 = "0xE000"}
.. Above is calculated as 0x1000 at start of flash + IDF_TARGET_MAX_BOOTLOADER_SIZE + 0x1000 signature sector

当需要启用额外的引导加载程序功能，包括 :doc:`/security/flash-encryption` 或安全启动，尤其是设置高级别 :ref:`CONFIG_BOOTLOADER_LOG_LEVEL` 时，监控引导加载程序 .bin 文件的大小变得非常重要。

当使用默认的 :ref:`CONFIG_PARTITION_TABLE_OFFSET` 值 0x8000 时，二进制文件最大可为 {IDF_TARGET_CONFIG_PARTITION_TABLE_OFFSET} 字节。

如果引导加载程序二进制文件过大，则引导加载程序会构建将失败并显示 "Bootloader binary size [..] is too large for partition table offset" 的错误。如果此二进制文件已经被烧录，那么 {IDF_TARGET_NAME} 将无法启动 - 日志中将记录无效分区表或无效引导加载程序校验和的错误。

可以使用如下方法解决此问题：

- 将 :ref:`bootloader 编译器优化 <CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION>` 重新设置回默认值“Size”。
- 降低 :ref:`引导加载程序日志级别 <CONFIG_BOOTLOADER_LOG_LEVEL>`。将日志级别设置为 Warning, Error 或 None 都会显著减少最终二进制文件的大小（但也可能会让调试变得更加困难）。
- 将 :ref:`CONFIG_PARTITION_TABLE_OFFSET` 设置为高于 0x8000 的值，以便稍后将分区表放置在 flash 中，这样可以增加引导加载程序的可用空间。如果 :doc:`分区表 </api-guides/partition-tables>` 的 CSV 文件包含明确的分区偏移量，则需要修改这些偏移量，从而保证没有分区的偏移量低于 ``CONFIG_PARTITION_TABLE_OFFSET + 0x1000``。（这包括随 ESP-IDF 提供的默认分区 CSV 文件）

当启用 Secure Boot V2 时，由于引导加载程序最先加载到固定大小的缓冲区中进行验证，对二进制文件大小的绝对限制为 {IDF_TARGET_MAX_BOOTLOADER_SIZE}（不包括 4 KB 签名）。

.. only:: SOC_RTC_FAST_MEM_SUPPORTED

    从深度睡眠中快速启动
    ----------------------

    引导加载程序有 :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` 选项，可以减少从深度睡眠中唤醒的时间（有利于降低功耗）。当 :ref:`CONFIG_SECURE_BOOT` 选项禁用时，该选项可用。由于无需镜像校验，唤醒时间减少。在第一次启动时，引导加载程序将启动的应用程序的地址存储在 RTC FAST 存储器中。而在唤醒过程中，这个地址用于启动而无需任何检查，从而实现了快速加载。

自定义引导加载程序
----------------------

用户可以扩展或修改当前的引导加载程序，具体有两种方法：使用钩子实现或重写覆盖当前程序。这两种方法在 ESP-IDF 示例的 :example:`custom_bootloader` 文件夹中都有呈现。

* `bootloader_hooks` 介绍了如何将钩子与引导加载程序初始化连接。
* `bootloader_override` 介绍了如何覆盖引导加载程序的实现。

在引导加载程序的代码中，用户不能使用其他组件提供的驱动和函数，如果确实需要，请将该功能的实现部分放在项目的 `bootloader_components` 目录中（注意，这会增加引导加载程序的大小）。

如果引导加载程序过大，则可能与内存中的分区表重叠，分区表默认烧录在偏移量 0x8000 处。增加 :ref:`分区表偏移量 <CONFIG_PARTITION_TABLE_OFFSET>` ，将分区表放在 flash 中靠后的区域，这样可以增加引导程序的可用空间。
