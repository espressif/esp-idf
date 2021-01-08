引导加载程序（Bootloader）
==========================
:link_to_translation:`en:[English]`

引导加载程序（Bootloader）主要执行以下任务：

1. 内部模块的最小化初始配置；
2. 根据分区表和 ota_data（如果存在）选择需要引导的应用程序（app）分区；
3. 将此应用程序映像加载到 RAM（IRAM 和 DRAM）中，最后把控制权转交给应用程序。

引导加载程序位于 Flash 的 `0x1000` 偏移地址处。

引导加载程序兼容性
-------------------

建议使用最新发布的 :doc:`ESP-IDF 版本 </versions>`。OTA（空中升级）更新可以在现场烧录新的应用程序，但不能烧录一个新的引导加载程序。因此，引导加载程序支持引导从 ESP-IDF 新版本中构建的应用程序。

但不支持引导从 ESP-IDF 旧版本中构建的程序。如果现有产品可能需要将应用程序降级到旧版本，那么在手动更新 ESP-IDF 时，请继续使用旧版本 ESP-IDF 引导加载程序的二进制文件。

恢复出厂设置
------------
用户可以编写一个基本的工作固件，然后将其加载到工厂分区（factory）中。 

接下来，通过 OTA（空中升级）更新固件，更新后的固件会被保存到某个 OTA app 分区中，OTA 数据分区也会做相应更新以指示从该分区引导应用程序。

如果你希望回滚到出厂固件并清除设置，则需要设置 :ref:`CONFIG_BOOTLOADER_FACTORY_RESET`。出厂重置机制允许将设备重置为出厂模式：

- 清除一个或多个数据分区。 
- 从工厂分区启动。 

:ref:`CONFIG_BOOTLOADER_DATA_FACTORY_RESET` - 允许用户选择在恢复出厂设置时需要擦除的数据分区。可以通过逗号来分隔多个分区的名字，并适当增加空格以便阅读（例如 "nvs, phy_init, nvs_custom, ..."）。请确保此处指定的名称和分区表中的名称相同，且不含有 “app” 类型的分区。

:ref:`CONFIG_BOOTLOADER_OTA_DATA_ERASE` - 恢复出厂模式后，设备会从工厂分区启动，OTA 数据分区会被清除。

:ref:`CONFIG_BOOTLOADER_NUM_PIN_FACTORY_RESET`- 设置用于触发出厂重置的 GPIO 编号，必须在芯片复位时将此 GPIO 拉低才能触发出厂重置事件。

:ref:`CONFIG_BOOTLOADER_HOLD_TIME_GPIO`- 设置进入重置或测试模式所需要的保持时间（默认为 5 秒）。设备复位后，GPIO 必须在这段时间内持续保持低电平，然后才会执行出厂重置或引导测试分区。

示例分区表如下::

	# Name,   Type, SubType, Offset,   Size, Flags
	# 注意，如果你增大了引导加载程序的大小，请确保更新偏移量，避免和其它分区发生重叠
	nvs,      data, nvs,     0x9000,   0x4000
	otadata,  data, ota,     0xd000,   0x2000
	phy_init, data, phy,     0xf000,   0x1000
	factory,  0,    0,       0x10000,  1M
	test,     0,    test,    ,         512K
	ota_0,    0,    ota_0,   ,         512K
	ota_1,    0,    ota_1,   ,         512K

.. _bootloader_boot_from_test_firmware:

从测试应用程序分区启动
------------------------
用户可以编写特殊固件用于生产环境中测试，然后在需要的时候运行。此时需要在分区表中专门申请一块分区用于保存该测试固件（详情请参阅 :doc:`分区表 <partition-tables>`）。如果想要触发测试固件，还需要设置 :ref:`CONFIG_BOOTLOADER_APP_TEST`。

:ref:`CONFIG_BOOTLOADER_NUM_PIN_APP_TEST` - 设置引导测试分区的 GPIO 管脚编号，该 GPIO 会被配置为输入模式，并且会使能内部上拉电阻。若想触发测试固件，该 GPIO 必须在芯片复位时拉低。设备重启时如果该 GPIO 没有被激活（即处于高电平状态），那么会加载常规配置的应用程序（可能位于工厂分区或者 OTA 分区）。 

:ref:`CONFIG_BOOTLOADER_HOLD_TIME_GPIO` - 设置进入重置或测试模式所需要的保持时间（默认为 5 秒）。设备复位后，GPIO 必须在这段时间内持续保持低电平，然后才会执行出厂重置或引导测试分区。


从深度睡眠中快速启动
----------------------
引导加载程序有 :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` 选项，可以减少唤醒时间（有利于降低消耗）。当 :ref:`CONFIG_SECURE_BOOT` 选项禁用时，该选项可用。由于无需固件校验，唤醒时间减少。在第一次启动时，引导加载程序将启动的应用程序的地址存储在 RTC FAST 存储器中。而在唤醒过程中，这个地址用于启动而无需任何检查，从而实现了快速加载。 

自定义引导程序
--------------
用户可以重写当前的引导加载程序，具体做法是，复制 `/esp-idf/components/bootloader` 文件夹到项目组件目录中，然后编辑 ``/your_project/components/bootloader/subproject/main/bootloader_start.c`` 文件。

在引导加载程序的代码中，用户不能使用其他组件提供的驱动和函数，如果确实需要，请将该功能的实现部分放在项目的 bootloader 目录中（注意，这会增加引导程序的大小）。

如果引导程序过大，则可能与内存中的分区表重叠，分区表默认烧录在 
偏移量 0x8000 处。增加 :ref:`分区表偏移量值 <CONFIG_PARTITION_TABLE_OFFSET>` ，将分区表放在 flash 中靠后的区域，这样可以增加引导程序的可用空间。

.. note:: 初次将引导程序复制到现有项目中时，由于路径发生了意外变化，项目可能无法构建。对于这种情况，请运行 ``idf.py fullclean`` （或删除项目构建目录），然后重新构建。

