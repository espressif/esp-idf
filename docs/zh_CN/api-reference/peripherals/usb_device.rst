USB 设备栈
=================

:link_to_translation:`en:[English]`

{IDF_TARGET_USB_DP_GPIO_NUM:default="20"}
{IDF_TARGET_USB_DM_GPIO_NUM:default="19"}
{IDF_TARGET_USB_EP_NUM: default="6", esp32p4="15"}
{IDF_TARGET_USB_EP_NUM_INOUT:default="5", esp32p4="8"}
{IDF_TARGET_USB_EP_NUM_IN:default="1", esp32p4="7"}

概述
--------

USB 设备栈（以下简称设备栈）支持在 {IDF_TARGET_NAME} 上启用 USB 设备支持。通过使用设备栈，可以为 {IDF_TARGET_NAME} 烧录任意具有明确定义的 USB 设备功能（如键盘、鼠标、摄像头）、自定义功能（也称特定供应商类别）或上述功能的组合（也称复合设备）。

设备栈基于 TinyUSB 栈构建，但对 TinyUSB 进行了一些小的功能扩展和修改，使其更好地集成到 ESP-IDF。设备栈通过 `乐鑫组件注册表 <https://components.espressif.com/components/espressif/esp_tinyusb>`__ 作为托管组件分发。

功能列表
--------

- 支持多种设备类别 (CDC, HID, MIDI, MSC)
- 支持复合设备
- 支持特定供应商类别
- 最多支持 {IDF_TARGET_USB_EP_NUM} 个端点

    - {IDF_TARGET_USB_EP_NUM_INOUT} 个输入/输出端点
    - {IDF_TARGET_USB_EP_NUM_IN} 个输入端点

- 自供电设备的 VBUS 监测

.. Todo: Refactor USB hardware connect into a separate guide

硬件连接
--------

.. only:: esp32s2 or esp32s3

    {IDF_TARGET_NAME} 将 USB D+ 和 D- 信号分别路由到 GPIO {IDF_TARGET_USB_DP_GPIO_NUM} 和 {IDF_TARGET_USB_DM_GPIO_NUM}。为了实现 USB 设备功能，这些 GPIO 应通过某种方式连接到总线（例如，通过 Micro-B 端口、USB-C 端口或直接连接到标准-A 插头）。

.. only:: esp32p4

    {IDF_TARGET_NAME} 将 USB D+ 和 D- 信号路由到其专用管脚。为了实现 USB 设备功能，这些管脚应通过某种方式连接到总线（例如，通过 Micro-B 端口、USB-C 端口或直接连接到标准-A 插头）。

.. figure:: ../../../_static/usb-board-connection.png
    :align: center
    :alt: 将 USB GPIO 直接接连至 USB 标准-A 插头
    :figclass: align-center

.. only:: esp32s2 or esp32s3

    .. note::

        如果你使用带有两个 USB 端口的 {IDF_TARGET_NAME} 开发板，标有 "USB" 的端口已经连接到 D+ 和 D- GPIO。

.. note::

    自供电设备还必须通过电压分压器或比较器连接 VBUS，详情请参阅 :ref:`self-powered-device`。

.. only:: esp32s3

    外部 PHY 配置
    -------------

    {IDF_TARGET_NAME} 内部集成了两个 USB 控制器：USB-OTG 与 USB-Serial-JTAG。这两个控制器 **共用同一个 PHY**，因此同一时间只能有一个控制器工作。如果在 USB-Serial-JTAG 工作时（例如调试或烧录）时仍需要使用 USB 设备功能，必须使用 **外部 PHY**，因为此时内部 PHY 已被 USB-Serial-JTAG 占用。

    .. note::
        使用外部 PHY 并不是在 USB 主机或设备功能开启时同时实现调试的唯一办法。也可以通过烧录对应的 eFuse，将调试接口从 USB-Serial-JTAG 切换为传统的 JTAG 接口。具体步骤请参考 ESP-IDF 编程指南中针对你的芯片的 :doc:`JTAG 调试 <../../api-guides/jtag-debugging/index>` 章节。

    {IDF_TARGET_NAME} 支持连接外部 PHY 芯片。当需要在使用 USB-Serial-JTAG 控制器的同时提供全速 USB 设备功能时，这一点尤其重要。不同的外部 PHY 芯片可能需要不同的硬件配置，请参考各自芯片的规格书。乐鑫官方文档提供了如下的通用连接示意图供参考。如需了解更多内容，请参阅 `使用外部 PHY <https://docs.espressif.com/projects/esp-iot-solution/zh_CN/latest/usb/usb_overview/usb_phy.html#external-phy>`__。

    .. figure:: ../../../_static/usb_device/usb_fs_phy_sp5301.png
       :align: center
       :alt: usb_fs_phy_sp5301

       连接外部 PHY 芯片的典型电路图

    **已测试的外部 PHY 芯片如下：**

    - **SP5301** — {IDF_TARGET_NAME} 原生支持此芯片。原理图与布线方法请参考上文链接。
    - **STUSB03E** — 需要通过模拟开关进行信号切换，详情参考下方示例。

    .. figure:: ../../../_static/usb_device/ext_phy_schematic_stusb03e.png
       :align: center
       :alt: 使用模拟开关的外部 PHY 原理图（设备模式）

       使用 STUSB03E 与模拟开关的连接示例（设备模式）

    .. note::
        此原理图仅为简化示例，用于展示外部 PHY 的连接方式，未包含完整 {IDF_TARGET_NAME} 设计所需的所有元件和信号（如 VCC、GND、RESET 等）。
        图中包含 +5 V 电源轨（通常来自 USB VBUS）和 VCC 电源轨。VCC 电压应与芯片供电电压一致（通常为 3.3 V），并确保外部 PHY 与芯片使用同一电压域供电。如果设计自供电 USB 设备，请将外部 PHY 的 VBUSDET 信号接入 {IDF_TARGET_NAME}，以便实现对 VBUS 电压状态的监控。

    硬件配置通过将 GPIO 映射到 PHY 管脚实现。任何未使用的管脚（如 :cpp:member:`usb_phy_ext_io_conf_t::suspend_n_io_num`、:cpp:member:`usb_phy_ext_io_conf_t::fs_edge_sel_io_num`） **必须设置为 -1**。

    .. note::
        :cpp:member:`usb_phy_ext_io_conf_t::suspend_n_io_num` 管脚 **当前不受支持**，无需连接。
        :cpp:member:`usb_phy_ext_io_conf_t::fs_edge_sel_io_num` 管脚为可选管脚，仅需在低速和全速模式间切换时使用。

    ESP TinyUSB 设备栈从 2.0 版本开始支持外部 PHY。要在设备模式下使用外部 PHY，需执行以下步骤：

    1. 使用 :cpp:type:`usb_phy_config_t` 配置 GPIO 映射与 PHY。
    2. 调用 :cpp:func:`usb_new_phy()` 创建 PHY。
    3. 使用 :cpp:func:`TINYUSB_DEFAULT_CONFIG()` 初始化 :cpp:type:`tinyusb_config_t`。
    4. 将 :cpp:type:`tinyusb_config_t` 的 `phy.skip_setup` 字段设为 ``true``，从而跳过 PHY 的重新初始化，直接使用已配置的外部 PHY。

    **示例代码：**

    .. code-block:: c

        // 外部 PHY 的 GPIO 配置
        const usb_phy_ext_io_conf_t ext_io_conf = {
            .vp_io_num  = 8,
            .vm_io_num  = 5,
            .rcv_io_num = 11,
            .oen_io_num = 17,
            .vpo_io_num = 4,
            .vmo_io_num = 46,
            .suspend_n_io_num = -1,
            .fs_edge_sel_io_num = -1,
        };

        // 针对 OTG 控制器（设备模式）的外部 PHY 配置与初始化
        const usb_phy_config_t phy_config = {
            .controller = USB_PHY_CTRL_OTG,
            .target = USB_PHY_TARGET_EXT,
            .otg_mode = USB_OTG_MODE_DEVICE,
            .otg_speed = USB_PHY_SPEED_FULL,
            .ext_io_conf = &ext_io_conf
        };

        usb_phy_handle_t phy_hdl;
        ESP_ERROR_CHECK(usb_new_phy(&phy_config, &phy_hdl));

        // 使用默认配置初始化 TinyUSB（可根据需要设置事件处理函数）
        tinyusb_config_t config = TINYUSB_DEFAULT_CONFIG();
        config.phy.skip_setup = true;

        tinyusb_driver_install(&config);

    通过上述配置，USB 设备栈会直接使用 **外部 PHY**，而不会尝试配置内部 PHY。

设备栈结构
----------

设备栈以 TinyUSB 栈为基础，在此基础上，该设备栈实现了以下功能：

- 自定义 USB 描述符
- 支持串行设备
- 通过串行设备重定向标准流
- 提供用于 USB 设备 MSC 类的存储介质（SPI-Flash 和 SD 卡）
- 封装设备栈中处理 TinyUSB 服务的任务

组件依赖项
-------------

设备栈通过 `乐鑫组件注册表 <https://components.espressif.com/components/espressif/esp_tinyusb>`__ 分发，使用前，请使用以下命令将设备栈组件添加为依赖项：

.. code:: bash

  idf.py add-dependency esp_tinyusb

配置选项
^^^^^^^^^

通过 menuconfig 选项，可以对设备栈进行以下多方面配置：

- TinyUSB 日志的详细程度
- 设备栈任务相关选项
- 默认设备/字符串描述符选项
- 特定类别的选项

.. _descriptors-configuration:

配置描述符
^^^^^^^^^^^^^^^^^^^^^^^^^

结构体 :cpp:type:`tinyusb_config_t` 提供了与 USB 描述符相关的字段，应进行初始化。

无论是全速 USB 设备还是高速 USB 设备，都应初始化以下描述符：

- :cpp:member:`device_descriptor`
- :cpp:member:`string_descriptor`

全速 USB 设备应初始化以下字段，以提供相应的配置描述符：

- :cpp:member:`configuration_descriptor`

.. only:: esp32p4

    高速 USB 设备应初始化以下字段，以提供不同速度下的配置描述符：

    - :cpp:member:`fs_configuration_descriptor`
    - :cpp:member:`hs_configuration_descriptor`
    - :cpp:member:`qualifier_descriptor`

    .. note::

        为符合 USB 2.0 协议规范，需同时初始化 :cpp:member:`fs_configuration_descriptor` 和 :cpp:member:`hs_configuration_descriptor`。

调用 :cpp:func:`tinyusb_driver_install` 时，设备栈将基于上述字段中提供的描述符实现 USB 设备。

设备栈还提供了默认描述符，将 :cpp:func:`tinyusb_driver_install` 中的相应字段设置为 ``NULL`` 即可安装。默认描述符包括：

- 默认设备描述符：如需启用，将 :cpp:member:`device_descriptor` 设置为 ``NULL``。默认设备描述符将使用相应的 menuconfig 选项设置的值（如 PID、VID、bcdDevice 等）。
- 默认字符串描述符：如需启用，将 :cpp:member:`string_descriptor` 设置为 ``NULL``。默认字符串描述符将使用相应的 menuconfig 选项设置的值（如制造商、产品和序列字符串描述符选项）。
- 默认配置描述符。某些很少需要自定义配置的类别（如 CDC 和 MSC）将提供默认配置描述符。如需启用，将相应的配置描述符字段设置为 ``NULL``。

    - :cpp:member:`configuration_descriptor`：全速描述符，仅适用于全速设备
    - :cpp:member:`fs_configuration_descriptor`：全速描述符，适用于高速设备
    - :cpp:member:`hs_configuration_descriptor`：高速描述符，适用于高速设备

.. note::

    为实现向后兼容性，若设备栈支持高速，可使用 :cpp:member:`configuration_descriptor` 代替 :cpp:member:`fs_configuration_descriptor` 来设置全速配置描述符。

安装设备栈
----------

请调用 :cpp:func:`tinyusb_driver_install` 安装设备栈。结构体 :cpp:type:`tinyusb_config_t` 指定了设备栈的配置，而 :cpp:type:`tinyusb_config_t` 作为参数传递给 :cpp:func:`tinyusb_driver_install`。

.. note::

    结构体 :cpp:type:`tinyusb_config_t` 可以实现零初始化（如 ``const tinyusb_config_t tusb_cfg = { 0 };``）或部分初始化（如下所示）。对于结构体中任何初始化为 ``0`` 或 ``NULL`` 的成员，设备栈将使用其默认配置，请参阅如下示例。

.. code-block:: c

    const tinyusb_config_t partial_init = {
        .device_descriptor = NULL,  // 使用在 menuconfig 中指定的默认设备描述符
        .string_descriptor = NULL,  // 使用在 menuconfig 中指定的默认字符串描述符
        .external_phy = false,      // 使用内部 USB PHY
    #if (TUD_OPT_HIGH_SPEED)
        .fs_configuration_descriptor = NULL, // 使用在 menuconfig 中根据设置指定的默认全速配置描述符
        .hs_configuration_descriptor = NULL, // 使用在 menuconfig 中根据设置指定的默认高速配置描述符
        .qualifier_descriptor = NULL,  // 使用默认限定描述符，值取自默认设备描述符
    #else
        .configuration_descriptor = NULL,   // 使用在 menuconfig 中根据设置指定的默认配置描述符
    #endif // TUD_OPT_HIGH_SPEED

    };

.. _self-powered-device:

自供电设备
-------------------

USB 规范要求自供电设备监测 USB 的 VBUS 信号的电压水平。与总线供电设备相反，即使没有 USB 连接，自供电设备也可以正常工作。通过监测 VBUS 电压水平，自供电设备可以检测连接和断开事件。当 VBUS 电压升高到 4.75 V 以上时视为有效；当 VBUS 电压下降到 4.35 V 以下时视为无效。

在 {IDF_TARGET_NAME} 上，需要使用一个 GPIO 作为电压感测管脚，检测 VBUS 处于在规定阈值之上/之下。然而，由于 {IDF_TARGET_NAME} 管脚具有 3.3 V 容差，即使 VBUS 上升/下降到高于/低于上述规定阈值，{IDF_TARGET_NAME} 仍会显示为逻辑高电平。因此，为了检测 VBUS 是否有效，可以采用以下方法：

- 将 VBUS 连接至电压比较器芯片/电路，该芯片/电路可检测上述阈值（即 4.35 V 和 4.75 V），并向 {IDF_TARGET_NAME} 输出 3.3 V 逻辑电平，指示 VBUS 是否有效。
- 如果 VBUS 为 4.4 V，则使用电阻分压器输出 (0.75 x Vdd)（见下图）。

.. note::

    在这两种情况下，设备从 USB 主机拔出后 3 毫秒内，传感管脚上的电压必须为逻辑低电平。

.. figure:: ../../../_static/diagrams/usb/usb_vbus_voltage_monitor.png
    :align: center
    :alt: 用于 VBUS 监测的简易分压器
    :figclass: align-center

    用于 VBUS 监测的简易分压器

请在结构体 :cpp:type:`tinyusb_config_t` 中将 :cpp:member:`self_powered` 设置为 ``true``，并将 :cpp:member:`vbus_monitor_io` 设置为用于 VBUS 监测的 GPIO 管脚编号以使用此功能。

USB 串行设备 (CDC-ACM)
---------------------------

如果在 menuconfig 中启用了 CDC 选项，则可以根据 :cpp:type:`tinyusb_config_cdcacm_t` 的设置，使用 :cpp:func:`tusb_cdc_acm_init` 初始化 USB 串行设备，请参阅如下示例：

.. code-block:: c

    const tinyusb_config_cdcacm_t acm_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,
        .cdc_port = TINYUSB_CDC_ACM_0,
        .rx_unread_buf_sz = 64,
        .callback_rx = NULL,
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };
    tusb_cdc_acm_init(&acm_cfg);

可以在配置结构体中设置指向 :cpp:type:`tusb_cdcacm_callback_t` 函数的指针指定回调函数，或在初始化 USB 串行设备后，调用 :cpp:func:`tinyusb_cdcacm_register_callback` 指定回调函数。

USB 串行控制台
^^^^^^^^^^^^^^^^^^

USB 串行设备支持将所有标准输入/输出流 (stdin、stdout、stderr) 重定向到 USB。因此，调用如 ``printf()`` 等标准库输入/输出函数将导致通过 USB 而不是 UART 发送/接收数据。

建议调用 :cpp:func:`esp_tusb_init_console` 将标准输入/输出流切换到 USB，并调用 :cpp:func:`esp_tusb_deinit_console` 将其切换回 UART。

USB 大容量存储设备 (MSC)
-----------------------------

在 menuconfig 中启用 MSC ``CONFIG_TINYUSB_MSC_ENABLED`` 选项时，可以将 ESP 芯片作为 USB 大容量存储设备使用。按如下示例，可以初始化存储媒介（SPI-Flash 或 SD 卡）。

- SPI-Flash

.. code-block:: c

    static esp_err_t storage_init_spiflash(wl_handle_t *wl_handle)
    {
        ***
        esp_partition_t *data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);
        ***
        wl_mount(data_partition, wl_handle);
        ***
    }
    storage_init_spiflash(&wl_handle);

    const tinyusb_msc_spiflash_config_t config_spi = {
        .wl_handle = wl_handle
    };
    tinyusb_msc_storage_init_spiflash(&config_spi);


- SD 卡

.. code-block:: c

    static esp_err_t storage_init_sdmmc(sdmmc_card_t **card)
    {
        ***
        sdmmc_host_t host = SDMMC_HOST_DEFAULT();
        sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
        // 对于 SD 卡，设置要使用的总线宽度

        slot_config.width = 4;
        slot_config.clk = CONFIG_EXAMPLE_PIN_CLK;
        slot_config.cmd = CONFIG_EXAMPLE_PIN_CMD;
        slot_config.d0 = CONFIG_EXAMPLE_PIN_D0;
        slot_config.d1 = CONFIG_EXAMPLE_PIN_D1;
        slot_config.d2 = CONFIG_EXAMPLE_PIN_D2;
        slot_config.d3 = CONFIG_EXAMPLE_PIN_D3;
        slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

        sd_card = (sdmmc_card_t *)malloc(sizeof(sdmmc_card_t));
        (*host.init)();
        sdmmc_host_init_slot(host.slot, (const sdmmc_slot_config_t *) &slot_config);
        sdmmc_card_init(&host, sd_card);
        ***
    }
    storage_init_sdmmc(&card);

    const tinyusb_msc_sdmmc_config_t config_sdmmc = {
        .card = card
    };
    tinyusb_msc_storage_init_sdmmc(&config_sdmmc);

MSC 性能优化
^^^^^^^^^^^^^^

**single-buffer 方案**

single-buffer 方案通过使用专用 buffer 临时存储接收到的写入数据，而不是在回调中立即处理，从而提升性能。

- **可配置的 buffer 大小**： 通过 ``CONFIG_TINYUSB_MSC_BUFSIZE`` 参数设置 buffer 大小，用户可以根据实际需要，灵活调整性能与存储占用的平衡点。

该方案确保了 USB 传输的效率，同时避免因存储操作可能带来的延迟。

**USB MSC 驱动器性能**

.. only:: esp32s3

    .. list-table::
        :header-rows: 1
        :widths: 20 20 20

        * - FIFO 大小
          - 读取速度
          - 写入速度

        * - 512 B
          - 0.566 MB/s
          - 0.236 MB/s

        * - 8192 B
          - 0.925 MB/s
          - 0.928 MB/s

.. only:: esp32p4

    .. list-table::
        :header-rows: 1
        :widths: 20 20 20

        * - FIFO 大小
          - 读取速度
          - 写入速度

        * - 512 B
          - 1.174 MB/s
          - 0.238 MB/s

        * - 8192 B
          - 4.744 MB/s
          - 2.157 MB/s

        * - 32768 B
          - 5.998 MB/s
          - 4.485 MB/s

.. only:: esp32s2

    .. note::

        ESP32-S2 在 MSC 设备模式下不支持 SD 卡功能。

    **SPI flash 性能：**

    .. list-table::
        :header-rows: 1
        :widths: 20 20

        * - FIFO 大小
          - 写入速度

        * - 512 B
          - 5.59 KB/s

        * - 8192 B
          - 21.54 KB/s

性能限制：

- **内部 SPI flash 性能** 受架构限制影响。程序执行和存储访问共享同一 flash 芯片，导致写入 flash 时必须暂停程序执行，会显著影响性能。
- **内部 SPI flash 主要适用于演示场景**。在需要更高性能的实际应用中，在支持的情况下使用 **外部存储设备**，如 SD 卡或外部 SPI flash 芯片。

.. only:: esp32s3 or esp32p4

    SD 卡不受此限制影响，因此能获得更高的性能提升。

应用示例
--------------------

如需查看相关示例，请前往目录 :example:`peripherals/usb/device`。

- :example:`peripherals/usb/device/tusb_console` 演示了如何使用 TinyUSB 组件配置 {IDF_TARGET_NAME}，以通过串行设备连接获取和输出日志，适用于任何支持 USB-OTG 的乐鑫开发板。
- :example:`peripherals/usb/device/tusb_serial_device` 演示了如何使用 TinyUSB 组件将 {IDF_TARGET_NAME} 配置为 USB 串行设备，还支持配置为双串行设备。
- :example:`peripherals/usb/device/tusb_midi` 演示了如何使用 TinyUSB 组件将 {IDF_TARGET_NAME} 配置为 USB MIDI 设备，从而通过本地 USB 端口输出 MIDI 音符序列。
- :example:`peripherals/usb/device/tusb_hid` 演示了如何使用 TinyUSB 组件实现 USB 键盘和鼠标，在连接到 USB 主机时发送 “按下和释放 key a/A” 事件，并使鼠标沿方形轨迹移动。
- :example:`peripherals/usb/device/tusb_msc` 演示了如何使用 USB 功能创建一个可以被 USB 主机识别的大容量存储设备，允许访问其内部数据存储，支持 SPI Flash 和 SD MMC 卡存储介质。
- :example:`peripherals/usb/device/tusb_composite_msc_serialdevice` 演示了如何使用 TinyUSB 组件将 {IDF_TARGET_NAME} 同时配置为 USB 串行设备和 MSC 设备（存储介质为 SPI-Flash）运行。

.. only:: not esp32p4

  - :example:`peripherals/usb/device/tusb_ncm` 演示了使用 TinyUSB 组件，借助网络控制模型 (NCM) 将 Wi-Fi 数据通过 USB 传输到 Linux 或 Windows 主机。NCM 是通信设备类 (CDC) USB 设备的一个子类，专用于 Ethernet-over-USB 应用。
