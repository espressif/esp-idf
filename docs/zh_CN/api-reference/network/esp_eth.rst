以太网
=========

:link_to_translation:`en:[English]`

.. -------------------------------- Overview -----------------------------------

概述
--------

ESP-IDF 提供一系列功能强大且兼具一致性的 API，为内部以太网 MAC (EMAC) 控制器和外部 SPI-Ethernet 模块提供支持。

本编程指南分为以下几个部分：

    1. :ref:`basic-ethernet-concepts`
    2. :ref:`driver-configuration-and-installation`
    3. :ref:`connect-driver-to-stack`
    4. :ref:`misc-operation-of-driver`

.. --------------------------- Basic Ethernet Concepts ------------------------------

.. _basic-ethernet-concepts:

以太网基本概念
-----------------------

以太网是一种异步的带冲突检测的载波侦听多路访问 (CSMA/CD) 协议/接口。通常来说，以太网不太适用于低功率应用。然而，得益于其广泛的部署、高效的网络连接、高数据率以及范围不限的可扩展性，几乎所有的有线通信都可以通过以太网进行。

符合 IEEE 802.3 标准的正常以太网帧的长度在 64 至 1518 字节之间，由五个或六个不同的字段组成：目的地 MAC 地址 (DA)、源 MAC 地址 (SA)、类型/长度字段、数据有效载荷字段、可选的填充字段和帧校验序列字段 (CRC)。此外，在以太网上传输时，以太网数据包的开头需附加 7 字节的前导码和 1 字节的帧起始符 (SOF)。

因此，双绞线上的通信如图所示：

.. rackdiag:: ../../../_static/diagrams/ethernet/data_frame_format.diag
    :caption: 以太网数据帧格式
    :align: center

前导码和帧起始符
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

前导码包含 7 字节的 ``55H``，作用是使接收器在实际帧到达之前锁定数据流。

帧前界定符 (SFD) 为二进制序列 ``10101011`` （物理介质层可见）。有时它也被视作前导码的一部分。

在传输和接收数据时，协议将自动从数据包中生成/移除前导码和帧起始符。

目的地址 (DA)
^^^^^^^^^^^^^^^^^^^

目的地址字段包含一个 6 字节长的设备 MAC 地址，数据包将发送到该地址。如果 MAC 地址第一个字节中的最低有效位是 1，则该地址为组播地址。例如，01-00-00-F0-00 和 33-45-67-89-AB-CD 是组播地址，而 00-00-00-F0-00 和 32-45-67-89-AB-CD 不是。

带有组播地址的数据包将到达选定的一组以太网节点，并发挥重要作用。如果目的地址字段是保留的多播地址，即 FF-FF-FF-FF-FF-FF，则该数据包是一个广播数据包，指向共享网络中的每个对象。如果 MAC 地址的第一个字节中的最低有效位为 0，则该地址为单播地址，仅供寻址节点使用。

通常，EMAC 控制器会集成接收过滤器，用于丢弃或接收带有组播、广播和/或单播目的地址的数据包。传输数据包时，由主机控制器将所需的目标地址写入传输缓冲区。

源地址 (SA)
^^^^^^^^^^^^^^

源地址字段包含一个 6 字节长的节点 MAC 地址，以太网数据包通过该节点创建。以太网的用户需为所使用的任意控制器生成唯一的 MAC 地址。MAC 地址由两部分组成：前三个字节称为组织唯一标识符 (OUI)，由 IEEE 分配；后三个字节是地址字节，由购买 OUI 的公司配置。有关 ESP-IDF 中使用的 MAC 地址的详细信息，请参见 :ref:`MAC 地址分配 <MAC-Address-Allocation>`。

传输数据包时，由主机控制器将分配的源 MAC 地址写入传输缓冲区。

类型/长度
^^^^^^^^^^^^^

类型/长度字段长度为 2 字节。如果其值 <= 1500（十进制），则该字段为长度字段，指定在数据字段后的非填充数据量；如果其值 >= 1536，则该字段值表示后续数据包所属的协议。以下为该字段的常见值：

  * IPv4 = 0800H
  * IPv6 = 86DDH
  * ARP = 0806H

使用专有网络的用户可以将此字段配置为长度字段。然而，对于使用互联网协议 (IP) 或地址解析协议 (ARP) 等协议的应用程序，在传输数据包时，应将此字段配置为协议规范定义的适当类型。

数据有效载荷
^^^^^^^^^^^^^

数据有效载荷字段是一个可变长度的字段，长度从 0 到 1500 字节不等。更大的数据包会因违反以太网标准而被大多数以太网节点丢弃。

数据有效载荷字段包含客户端数据，如 IP 数据报。

填充及帧校验序列 (FCS)
^^^^^^^^^^^^^^^^^^^^^^^

填充字段是一个可变长度的字段。数据有效载荷较小时，将添加填充字段以满足 IEEE 802.3 规范的要求。

以太网数据包的 DA、SA、类型、数据有效载荷和填充字段共计必须不小于 60 字节。加上所需的 4 字节 FCS 字段，数据包的长度必须不小于 64 字节。如果数据有效载荷字段小于 46 字节，则需要加上一个填充字段。

帧校验序列字段 (FCS) 长度为 4 字节，其中包含一个行业标准的 32 位 CRC，该 CRC 是根据 DA、SA、类型、数据有效载荷和填充字段的数据计算的。鉴于计算 CRC 的复杂性，硬件通常会自动生成一个有效的 CRC 进行传输。否则，需由主机控制器生成 CRC 并将其写入传输缓冲区。

通常情况下，主机控制器无需关注填充字段和 CRC 字段，因为这两部分可以在传输或接收时由硬件 EMAC 自动生成或验证。然而，当数据包到达时，填充字段和 CRC 字段将被写入接收缓冲区。因此，如果需要的话，主机控制器也可以对它们进行评估。

.. note::
    除了上述的基本数据帧，在 10/100 Mbps 以太网中还有两种常见的帧类型：控制帧和 VLAN 标记帧。ESP-IDF 不支持这两种帧类型。

.. ------------------------------ Driver Operation --------------------------------

.. _driver-configuration-and-installation:

配置 MAC 和 PHY
---------------------

以太网驱动器由两部分组成：MAC 和 PHY。

.. TODO remove esp32p4 (IDF-9057)

.. only:: SOC_EMAC_SUPPORTED and not esp32p4

    MAC 和 PHY 之间的通信可以通过多种方式进行，如： **MII** （媒体独立接口）、 **RMII** （简化媒体独立接口）等。

    .. figure:: ../../../_static/rmii-interface.png
        :scale: 80 %
        :alt: 以太网 RMII 接口
        :figclass: align-center

        以太网 RMII 接口

    MII 和 RMII 的一个明显区别在于其所需的信号数。MII 通常需要多达 18 个信号，RMII 接口则仅需要 9 个信号。

    在 RMII 模式下，接收器和发射器信号的参考时钟为 ``REF_CLK``。 **在访问 PHY 和 MAC 时，REF_CLK 必须保持稳定**。一般来说，根据设计中 PHY 设备的特征，可通过以下三种方式生成 ``REF_CLK``：

    * 一些 PHY 芯片可以从其外部连接的 25 MHz 晶体振荡器中获取 ``REF_CLK`` （如图中的选项 **a** 所示）。对于此类芯片，请在 :ref:`CONFIG_ETH_RMII_CLK_MODE` 中选择 ``CONFIG_ETH_RMII_CLK_INPUT``。

    * 一些 PHY 芯片使用可以作为 MAC 端 ``REF_CLK`` 的外接 50 MHz 晶体振荡器或其他时钟源（如图中的选项 **b** 所示）。对于此类芯片，请同样在 :ref:`CONFIG_ETH_RMII_CLK_MODE` 中选择 ``CONFIG_ETH_RMII_CLK_INPUT``。

    * 一些 EMAC 控制器可以使用其内部的高精度 PLL 生成 ``REF_CLK`` （如图中的选项 **c** 所示）。此种情况下，请在 :ref:`CONFIG_ETH_RMII_CLK_MODE` 中选择 ``CONFIG_ETH_RMII_CLK_OUTPUT``。

    .. note::
        如上所述，``REF_CLK`` 默认通过项目配置进行配置。然而，通过设置 :cpp:member:`eth_esp32_emac_config_t::interface` 和 :cpp:member:`eth_esp32_emac_config_t::clock_config`，也可以实现在用户应用代码中覆盖该时钟。更多细节，请参见 :cpp:enum:`emac_rmii_clock_mode_t` 和 :cpp:enum:`emac_rmii_clock_gpio_t`。

    .. warning::
        如果配置 RMII 时钟模式为 ``CONFIG_ETH_RMII_CLK_OUTPUT``，那么就可以使用  ``GPIO0`` 输出 ``REF_CLK`` 信号。更多细节，请参见 :ref:`CONFIG_ETH_RMII_CLK_OUTPUT_GPIO0`。

        值得一提的是，如果设计中并未使用 PSRAM，则 GPIO16 和 GPIO17 也可以用来输出参考时钟。更多细节，请参见 :ref:`CONFIG_ETH_RMII_CLK_OUT_GPIO`。

        如果配置 RMII 时钟模式为 ``CONFIG_ETH_RMII_CLK_INPUT``，那么有且只有 ``GPIO0`` 可以用来输入 ``REF_CLK`` 信号。请注意， ``GPIO0`` 同时也是 ESP32 上一个重要的 strapping GPIO 管脚。如果 GPIO0 在上电时采样为低电平，ESP32 将进入下载模式，需进行手动复位重启系统。解决这个问题的方法是，在硬件中默认禁用 ``REF_CLK``，从而避免 strapping 管脚在启动阶段受到其他信号的干扰。随后，再在以太网驱动安装阶段重新启用 ``REF_CLK``。

        可以通过以下方法禁用 ``REF_CLK`` 信号：

        * 禁用或关闭晶体振荡器的电源（对应图中的选项 **b**）。

        * 强制复位 PHY 设备（对应图中的选项 **a**）。 **此种方法并不适用于所有 PHY 设备**，即便处于复位状态，某些 PHY 设备仍会向 GPIO0 输出信号。

    **无论选择哪种 RMII 时钟模式，都请确保硬件设计中 REF_CLK 的信号完整性！** 信号线越短越好，并请保持信号线与 RF 设备和电感器元件的距离。

    .. note::
        ESP-IDF 只支持 RMII 接口（即在 Kconfig 选项 :ref:`CONFIG_ETH_PHY_INTERFACE` 中始终选择 ``CONFIG_ETH_PHY_INTERFACE_RMII``）。

        在数据平面使用的信号通过 MUX 连接至特定的 GPIO，这些信号无法配置至其他 GPIO。在控制平面使用的信号则可以通过 Matrix 矩阵路由到任何空闲 GPIO。相关的硬件设计示例，请参考 :doc:`ESP32-Ethernet-Kit <../../hw-reference/esp32/get-started-ethernet-kit>`。

根据以太网板设计，需要分别为 MAC 和 PHY 配置必要的参数，通过两者完成驱动程序的安装。

MAC 的相关配置可以在 :cpp:class:`eth_mac_config_t` 中找到，具体包括：

.. list::

    * :cpp:member:`eth_mac_config_t::sw_reset_timeout_ms`：软件复位超时值，单位为毫秒。通常，MAC 复位应在 100 ms 内完成。

    * :cpp:member:`eth_mac_config_t::rx_task_stack_size` 和 :cpp:member:`eth_mac_config_t::rx_task_prio`：MAC 驱动会创建一个专门的任务来处理传入的数据包，这两个参数用于设置该任务的堆栈大小和优先级。

    * :cpp:member:`eth_mac_config_t::flags`：指定 MAC 驱动应支持的额外功能，尤其适用于某些特殊情况。这个字段的值支持与以 ``ETH_MAC_FLAG_`` 为前缀的宏进行 OR 运算。例如，如果 MAC 驱动应在禁用缓存后开始工作，那么则需要用 :c:macro:`ETH_MAC_FLAG_WORK_WITH_CACHE_DISABLE` 配置这个字段。

    :SOC_EMAC_SUPPORTED: * :cpp:member:`eth_esp32_emac_config_t::smi_mdc_gpio_num` 和 :cpp:member:`eth_esp32_emac_config_t::smi_mdio_gpio_num`：连接 SMI 信号的 GPIO 编号。

    :SOC_EMAC_SUPPORTED: * :cpp:member:`eth_esp32_emac_config_t::interface`：配置到 PHY (MII/RMII) 的 MAC 数据接口。

    :SOC_EMAC_SUPPORTED: * :cpp:member:`eth_esp32_emac_config_t::clock_config`：配置 EMAC 接口时钟（RMII 模式下的 ``REF_CLK`` 模式以及 GPIO 编号）。

PHY 的相关配置可以在 :cpp:class:`eth_phy_config_t` 中找到，具体包括：

.. list::

    * :cpp:member:`eth_phy_config_t::phy_addr`：同一条 SMI 总线上可以存在多个 PHY 设备，所以有必要为各个 PHY 设备分配唯一地址。通常，这个地址是在硬件设计期间，通过拉高/拉低一些 PHY strapping 管脚来配置的。根据不同的以太网开发板，可配置值为 ``0`` 到 ``15``。需注意，如果 SMI 总线上仅有一个 PHY 设备，将该值配置为 ``-1``，即可使驱动程序自动检测 PHY 地址。

    * :cpp:member:`eth_phy_config_t::reset_timeout_ms`：复位超时值，单位为毫秒。通常，PHY 复位应在 100 ms 内完成。

    * :cpp:member:`eth_phy_config_t::autonego_timeout_ms`：自动协商超时值，单位为毫秒。以太网驱动程序会自动与对等的以太网节点进行协商，以确定双工和速度模式。此值通常取决于电路板上 PHY 设备的性能。

    * :cpp:member:`eth_phy_config_t::reset_gpio_num`：如果开发板同时将 PHY 复位管脚连接至了任意 GPIO 管脚，请使用该字段进行配置。否则，配置为 ``-1``。

ESP-IDF 在宏 :c:macro:`ETH_MAC_DEFAULT_CONFIG` 和 :c:macro:`ETH_PHY_DEFAULT_CONFIG` 中为 MAC 和 PHY 提供了默认配置。


创建 MAC 和 PHY 实例
---------------------------

以太网驱动是以面向对象的方式实现的。对 MAC 和 PHY 的任何操作都应基于实例。

.. only:: SOC_EMAC_SUPPORTED

    内部 EMAC + 外部 PHY
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    .. highlight:: c

    ::

        eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();                      // 应用默认的通用 MAC 配置
        eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG(); // 应用默认的供应商特定 MAC 配置
        esp32_emac_config.smi_mdc_gpio_num = CONFIG_EXAMPLE_ETH_MDC_GPIO;            // 更改用于 MDC 信号的 GPIO
        esp32_emac_config.smi_mdio_gpio_num = CONFIG_EXAMPLE_ETH_MDIO_GPIO;          // 更改用于 MDIO 信号的 GPIO
        esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config); // 创建 MAC 实例

        eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();      // 应用默认的 PHY 配置
        phy_config.phy_addr = CONFIG_EXAMPLE_ETH_PHY_ADDR;           // 根据开发板设计更改 PHY 地址
        phy_config.reset_gpio_num = CONFIG_EXAMPLE_ETH_PHY_RST_GPIO; // 更改用于 PHY 复位的 GPIO
        esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);     // 创建 PHY 实例
        // ESP-IDF 为数种以太网 PHY 芯片驱动提供官方支持
        // esp_eth_phy_t *phy = esp_eth_phy_new_rtl8201(&phy_config);
        // esp_eth_phy_t *phy = esp_eth_phy_new_lan8720(&phy_config);
        // esp_eth_phy_t *phy = esp_eth_phy_new_dp83848(&phy_config);

    可选的运行时 MAC 时钟配置
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    可以通过用户应用程序代码，选择性配置 EMAC 中的 ``REF_CLK``。

    .. highlight:: c

    ::

        eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG(); // 应用默认的供应商特定 MAC 配置

        // ...

        esp32_emac_config.interface = EMAC_DATA_INTERFACE_RMII;                      // 更改 EMAC 数据接口
        esp32_emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;               // 配置 EMAC REF_CLK 模式
        esp32_emac_config.clock_config.rmii.clock_gpio = EMAC_CLK_OUT_GPIO;          // 配置用于输入/输出 EMAC REF_CLK 的 GPIO 编号
        esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config); // 创建 MAC 实例


SPI-Ethernet 模块
^^^^^^^^^^^^^^^^^^^

.. highlight:: c

::

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();      // 应用默认的通用 MAC 配置
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();      // 应用默认的 PHY 配置
    phy_config.phy_addr = CONFIG_EXAMPLE_ETH_PHY_ADDR;           // 根据开发板设计更改 PHY 地址
    phy_config.reset_gpio_num = CONFIG_EXAMPLE_ETH_PHY_RST_GPIO; // 更改用于 PHY 复位的 GPIO
    // 安装 GPIO 中断服务（因为 SPI-Ethernet 模块为中断驱动）
    gpio_install_isr_service(0);
    // 配置 SPI 总线
    spi_device_handle_t spi_handle = NULL;
    spi_bus_config_t buscfg = {
        .miso_io_num = CONFIG_EXAMPLE_ETH_SPI_MISO_GPIO,
        .mosi_io_num = CONFIG_EXAMPLE_ETH_SPI_MOSI_GPIO,
        .sclk_io_num = CONFIG_EXAMPLE_ETH_SPI_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(CONFIG_EXAMPLE_ETH_SPI_HOST, &buscfg, 1));
    // 配置 SPI 从机设备
    spi_device_interface_config_t spi_devcfg = {
        .mode = 0,
        .clock_speed_hz = CONFIG_EXAMPLE_ETH_SPI_CLOCK_MHZ * 1000 * 1000,
        .spics_io_num = CONFIG_EXAMPLE_ETH_SPI_CS_GPIO,
        .queue_size = 20
    };
    /* dm9051 ethernet driver is based on spi driver */
    eth_dm9051_config_t dm9051_config = ETH_DM9051_DEFAULT_CONFIG(CONFIG_EXAMPLE_ETH_SPI_HOST, &spi_devcfg);
    dm9051_config.int_gpio_num = CONFIG_EXAMPLE_ETH_SPI_INT_GPIO;
    esp_eth_mac_t *mac = esp_eth_mac_new_dm9051(&dm9051_config, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_dm9051(&phy_config);


.. note::
    * 当为 SPI-Ethernet 模块（例如 DM9051）创建 MAC 和 PHY 实例时，由于 PHY 是集成在模块中的，因此调用的实例创建函数的后缀须保持一致（例如 `esp_eth_mac_new_dm9051` 和 `esp_eth_phy_new_dm9051` 搭配使用）。

    * 针对不同的以太网模块，或是为了满足特定 PCB 上的 SPI 时序，SPI 从机设备配置（即 `spi_device_interface_config_t`）可能略有不同。具体配置请查看模块规格以及 ESP-IDF 中的示例。


安装驱动程序
--------------

安装以太网驱动程序需要结合 MAC 和 PHY 实例，并在 :cpp:class:`esp_eth_config_t` 中配置一些额外的高级选项（即不仅限于 MAC 或 PHY 的选项）：

* :cpp:member:`esp_eth_config_t::mac`：由 MAC 生成器创建的实例（例如 :cpp:func:`esp_eth_mac_new_esp32`）。

* :cpp:member:`esp_eth_config_t::phy`：由 PHY 生成器创建的实例（例如 :cpp:func:`esp_eth_phy_new_ip101`）。

* :cpp:member:`esp_eth_config_t::check_link_period_ms`：以太网驱动程序会启用操作系统定时器来定期检查链接状态。该字段用于设置间隔时间，单位为毫秒。

* :cpp:member:`esp_eth_config_t::stack_input`：在大多数的以太网物联网应用中，驱动器接收的以太网帧会被传递到上层（如 TCP/IP 栈）。经配置，该字段为负责处理传入帧的函数。可以在安装驱动程序后，通过函数 :cpp:func:`esp_eth_update_input_path` 更新该字段。该字段支持在运行过程中进行更新。

* :cpp:member:`esp_eth_config_t::on_lowlevel_init_done` 和 :cpp:member:`esp_eth_config_t::on_lowlevel_deinit_done`：这两个字段用于指定钩子函数，当去初始化或初始化低级别硬件时，会调用钩子函数。

ESP-IDF 在宏 :c:macro:`ETH_DEFAULT_CONFIG` 中为安装驱动程序提供了一个默认配置。

.. highlight:: c

::

    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); // 应用默认驱动程序配置
    esp_eth_handle_t eth_handle = NULL; // 驱动程序安装完毕后，将得到驱动程序的句柄
    esp_eth_driver_install(&config, &eth_handle); // 安装驱动程序

以太网驱动程序包含事件驱动模型，该模型会向用户空间发送有用及重要的事件。安装以太网驱动程序之前，需要首先初始化事件循环。有关事件驱动编程的更多信息，请参考 :doc:`事件循环库 <../system/esp_event>`。

.. highlight:: c

::

    /** 以太网事件的事件处理程序 */
    static void eth_event_handler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data)
    {
        uint8_t mac_addr[6] = {0};
        /* 可从事件数据中获得以太网驱动句柄 */
        esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

        switch (event_id) {
        case ETHERNET_EVENT_CONNECTED:
            esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
            ESP_LOGI(TAG, "Ethernet Link Up");
            ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                        mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
            break;
        case ETHERNET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Ethernet Link Down");
            break;
        case ETHERNET_EVENT_START:
            ESP_LOGI(TAG, "Ethernet Started");
            break;
        case ETHERNET_EVENT_STOP:
            ESP_LOGI(TAG, "Ethernet Stopped");
            break;
        default:
            break;
        }
    }

    esp_event_loop_create_default(); // 创建一个在后台运行的默认事件循环
    esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL); // 注册以太网事件处理程序（用于在发生 link up/down 等事件时，处理特定的用户相关内容）

启动以太网驱动程序
---------------------

安装驱动程序后，可以立即启动以太网。

.. highlight:: c

::

    esp_eth_start(eth_handle); // 启动以太网驱动程序状态机

.. _connect-driver-to-stack:

连接驱动程序至 TCP/IP 协议栈
------------------------------

现在，以太网驱动程序已经完成安装。但对应 OSI（开放式系统互连模型）来看，目前阶段仍然属于第二层（即数据链路层）。这意味着可以检测到 link up/down 事件，获得用户空间的 MAC 地址，但无法获得 IP 地址，当然也无法发送 HTTP 请求。ESP-IDF 中使用的 TCP/IP 协议栈是 LwIP，关于 LwIP 的更多信息，请参考 :doc:`LwIP <../../api-guides/lwip>`。

要将以太网驱动程序连接至 TCP/IP 协议栈，需要以下三步：

1. 为以太网驱动程序创建网络接口
2. 将网络接口连接到以太网驱动程序
3. 注册 IP 事件处理程序

有关网络接口的更多信息，请参考 :doc:`Network Interface <esp_netif>`。

.. highlight:: c

::

    /** IP_EVENT_ETH_GOT_IP 的事件处理程序 */
    static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                     int32_t event_id, void *event_data)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        const esp_netif_ip_info_t *ip_info = &event->ip_info;

        ESP_LOGI(TAG, "Ethernet Got IP Address");
        ESP_LOGI(TAG, "~~~~~~~~~~~");
        ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
        ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
        ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
        ESP_LOGI(TAG, "~~~~~~~~~~~");
    }

    esp_netif_init()); // 初始化 TCP/IP 网络接口（在应用程序中应仅调用一次）
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH(); // 应用以太网的默认网络接口配置
    esp_netif_t *eth_netif = esp_netif_new(&cfg); // 为以太网驱动程序创建网络接口

    esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)); // 将以太网驱动程序连接至 TCP/IP 协议栈
    esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL); // 注册用户定义的 IP 事件处理程序
    esp_eth_start(eth_handle); // 启动以太网驱动程序状态机

.. warning::
    推荐在完成整个以太网驱动和网络接口的初始化后，再注册用户定义的以太网/IP 事件处理程序，也就是把注册事件处理程序作为启动以太网驱动程序的最后一步。这样可以确保以太网驱动程序或网络接口将首先执行以太网/IP 事件，从而保证在执行用户定义的处理程序时，系统处于预期状态。

.. _misc-operation-of-driver:

以太网驱动程序的杂项控制
-------------------------------

以下功能只支持在安装以太网驱动程序后调用。

* 关闭以太网驱动程序：:cpp:func:`esp_eth_stop`
* 更新以太网数据输入路径：:cpp:func:`esp_eth_update_input_path`
* 获取/设置以太网驱动程序杂项内容：:cpp:func:`esp_eth_ioctl`

.. highlight:: c

::

    /* 获取 MAC 地址 */
    uint8_t mac_addr[6];
    memset(mac_addr, 0, sizeof(mac_addr));
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
    ESP_LOGI(TAG, "Ethernet MAC Address: %02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    /* 获取 PHY 地址 */
    int phy_addr = -1;
    esp_eth_ioctl(eth_handle, ETH_CMD_G_PHY_ADDR, &phy_addr);
    ESP_LOGI(TAG, "Ethernet PHY Address: %d", phy_addr);

.. _flow-control:

数据流量控制
------------

受 RAM 大小限制，在网络拥堵时，MCU 上的以太网通常仅能处理有限数量的帧。发送站的数据传输速度可能快于对等端的接收能力。以太网数据流量控制机制允许接收节点向发送方发出信号，要求暂停传输，直到接收方跟上。这项功能是通过暂停帧实现的，该帧定义在 IEEE 802.3x 中。

暂停帧是一种特殊的以太网帧，用于携带暂停命令，其 EtherType 字段为 ``0x8808``，控制操作码为 ``0x0001``。只有配置为全双工操作的节点组可以发送暂停帧。当节点组希望暂停链路的另一端时，它会发送一个暂停帧到 48 位的保留组播地址 ``01-80-C2-00-00-01``。暂停帧中也包括请求暂停的时间段，以两字节的整数形式发送，值的范围从 ``0`` 到 ``65535``。

安装以太网驱动程序后，数据流量控制功能默认禁用，可以通过以下方式启用此功能：

.. highlight:: c

::

    bool flow_ctrl_enable = true;
    esp_eth_ioctl(eth_handle, ETH_CMD_S_FLOW_CTRL, &flow_ctrl_enable);

需注意，暂停帧是在自动协商期间由 PHY 向对等端公布的。只有当链路的两边都支持暂停帧时，以太网驱动程序才会发送暂停帧。

.. -------------------------------- Examples -----------------------------------

应用示例
--------------------

  * 以太网基本示例：:example:`ethernet/basic`
  * 以太网 iperf 示例：:example:`ethernet/iperf`
  * 以太网到 Wi-Fi AP“路由器”：:example:`network/eth2ap`
  * Wi-Fi station 到以太网 “网桥”：:example:`network/sta2eth`
  * 大多数协议示例也适用于以太网：:example:`protocols`

.. ------------------------------ Advanced Topics -------------------------------

.. _advanced-topics:

进阶操作
---------------

自定义 PHY 驱动程序
^^^^^^^^^^^^^^^^^^^^^^^^^

目前市面上已有多家 PHY 制造商提供了大量的芯片组合。ESP-IDF 现已支持数种 PHY 芯片，但是由于价格、功能、库存等原因，有时用户还是无法找到一款能满足其实际需求的芯片。

好在 IEEE 802.3 在其 22.2.4 管理功能部分对 EMAC 和 PHY 之间的管理接口进行了标准化。该部分定义了所谓的 ”MII 管理接口”规范，用于控制 PHY 和收集 PHY 的状态，还定义了一组管理寄存器来控制芯片行为、链接属性、自动协商配置等。在 ESP-IDF 中，这项基本的管理功能是由 :component_file:`esp_eth/src/esp_eth_phy_802_3.c` 实现的，这也大大降低了创建新的自定义 PHY 芯片驱动的难度。

.. note::
    由于一些 PHY 芯片可能不符合 IEEE 802.3 第 22.2.4 节的规定，所以请首先查看 PHY 数据手册。不过，就算芯片不符合规定，依旧可以创建自定义 PHY 驱动程序，只是由于需要自行定义所有的 PHY 管理功能，这个过程将变得较为复杂。

ESP-IDF 以太网驱动程序所需的大部分 PHY 管理功能都已涵盖在 :component_file:`esp_eth/src/esp_eth_phy_802_3.c` 中。不过对于以下几项，可能仍需针对不同芯片开发具体的管理功能：

    * 链接状态。此项总是由使用的具体芯片决定
    * 芯片初始化。即使不存在严格的限制，也应进行自定义，以确保使用的是符合预期的芯片
    * 芯片的具体功能配置

**创建自定义 PHY 驱动程序的步骤：**

1. 请根据 PHY 数据手册，定义针对供应商的特定注册表布局。示例请参见 :component_file:`esp_eth/src/esp_eth_phy_ip101.c`。
2. 准备衍生的 PHY 管理对象信息结构，该结构：

    * 必须至少包含 IEEE 802.3 :cpp:class:`phy_802_3_t` 父对象
    * 可选包含支持非 IEEE 802.3 或自定义功能所需的额外变量。示例请参见 :component_file:`esp_eth/src/esp_eth_phy_ksz80xx.c`。

3. 定义针对芯片的特定管理回调功能。
4. 初始化 IEEE 802.3 父对象并重新分配针对芯片的特定管理回调功能。

实现新的自定义 PHY 驱动程序后，你可以通过 `ESP-IDF 组件管理中心 <https://components.espressif.com/>`_ 将驱动分享给其他用户。

.. ---------------------------- API Reference ----------------------------------

API 参考
-------------

.. include-build-file:: inc/esp_eth.inc
.. include-build-file:: inc/esp_eth_driver.inc
.. include-build-file:: inc/esp_eth_com.inc
.. include-build-file:: inc/esp_eth_mac.inc
.. include-build-file:: inc/esp_eth_phy.inc
.. include-build-file:: inc/esp_eth_phy_802_3.inc
.. include-build-file:: inc/esp_eth_netif_glue.inc
