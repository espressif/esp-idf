以太网
=========

{IDF_TARGET_SOC_DMA_DESC_SIZE:default="", esp32="32 字节", esp32p4=" 32 字节（由于需要适当的内存对齐，实际占用 64 字节）"}
{IDF_TARGET_SOC_REF_CLK_IN_GPIO:default="", esp32="GPIO0", esp32p4="GPIO32，GPIO44 和 GPIO50"}
{IDF_TARGET_SOC_REF_CLK_OUT_GPIO:default="", esp32="GPIO0，GPIO16 和 GPIO17", esp32p4="GPIO23 和 GPIO39"}
{IDF_TARGET_SOC_RMII_TX_EN:default="", esp32="GPIO21", esp32p4="GPIO33，GPIO40 和 GPIO49"}
{IDF_TARGET_SOC_RMII_TXD0:default="", esp32="GPIO19", esp32p4="GPIO34 和 GPIO41"}
{IDF_TARGET_SOC_RMII_TXD1:default="", esp32="GPIO22", esp32p4="GPIO35 和 GPIO42"}
{IDF_TARGET_SOC_RMII_CRS_DV:default="", esp32="GPIO27", esp32p4="GPIO28，GPIO45 和 GPIO51"}
{IDF_TARGET_SOC_RMII_RXD0:default="", esp32="GPIO25", esp32p4="GPIO29，GPIO46 和 GPIO52"}
{IDF_TARGET_SOC_RMII_RXD1:default="", esp32="GPIO26", esp32p4="GPIO30，GPIO47 和 GPIO53"}


:link_to_translation:`en:[English]`

.. -------------------------------- Overview -----------------------------------

概述
--------

.. only:: SOC_EMAC_SUPPORTED

    ESP-IDF 提供一系列灵活度高且兼具一致性的 API，为内部以太网 MAC (EMAC) 控制器和外部 SPI-Ethernet 模块提供支持。

.. only:: not SOC_EMAC_SUPPORTED

    ESP-IDF 提供一系列灵活度高且兼具一致性的 API，为外部 SPI-Ethernet 模块提供支持。

本编程指南分为以下几个部分：

    1. :ref:`basic-ethernet-concepts`
    2. :ref:`driver-configuration-and-installation`
    3. :ref:`connect-driver-to-stack`
    4. :ref:`misc-operation-of-driver`

.. --------------------------- Basic Ethernet Concepts ------------------------------

.. _basic-ethernet-concepts:

以太网基本概念
-----------------------

以太网是一种异步的带冲突检测的载波侦听多路访问 (CSMA/CD) 协议/接口。通常来说，以太网不太适用于低功耗应用。然而，得益于其广泛的部署、高效的网络连接、高数据率以及范围不限的可扩展性，几乎所有的有线通信都可以通过以太网进行。

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

.. only:: SOC_EMAC_SUPPORTED

    MAC 和 PHY 之间的通信可以通过多种方式进行，如： **MII** （媒体独立接口）、 **RMII** （简化媒体独立接口）等。

    .. figure:: ../../../_static/rmii-interface.png
        :scale: 80 %
        :alt: 以太网 RMII 接口
        :figclass: align-center

        以太网 RMII 接口

    MII 和 RMII 的一个明显区别在于其所需的信号数。MII 通常需要多达 18 个信号，RMII 接口则仅需要 9 个信号。

    .. note::
        ESP-IDF 只支持 RMII 接口，所以请将 :cpp:member:`eth_esp32_emac_config_t::interface` 设置为 :cpp:enumerator:`eth_data_interface_t::EMAC_DATA_INTERFACE_RMII`。

    在 RMII 模式下，接收器和发射器信号的参考时钟为 ``REF_CLK``。 **在访问 PHY 和 MAC 时，REF_CLK 必须保持稳定**。一般来说，根据设计中 PHY 设备的特征，可通过以下三种方式生成 ``REF_CLK``：

    * 一些 PHY 芯片可以从其外部连接的 25 MHz 晶体振荡器中衍生出 ``REF_CLK`` （如图中的选项 **a** 所示）。对于此类芯片，请将 :cpp:member:`eth_esp32_emac_config_t::clock_config` 中的 :cpp:member:`eth_mac_clock_config_t::clock_mode` 设置为 :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_EXT_IN`。

    * 一些 PHY 芯片使用外接的 50 MHz 晶体振荡器或其他时钟源作为 MAC 端的 ``REF_CLK`` （如图中的选项 **b** 所示）。对于此类芯片，请同样将 :cpp:member:`eth_esp32_emac_config_t::clock_config` 中的 :cpp:member:`eth_mac_clock_config_t::clock_mode` 设置为 :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_EXT_IN`。

    * 一些 EMAC 控制器可以使用其内部的高精度 PLL 生成 ``REF_CLK`` （如图中的选项 **c** 所示）。此种情况下，请将 :cpp:member:`eth_esp32_emac_config_t::clock_config` 中的 :cpp:member:`eth_mac_clock_config_t::clock_mode` 设置为 :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_OUT`。

    .. only:: esp32

        .. warning::
            如果将 RMII 时钟模式配置为 :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_OUT`，则会使用内部音频 PLL (APLL) 时钟作为 50 MHz 时钟源。因此，请确保该配置不会与 I2S 总线的配置发生冲突。

            当选择内部时钟时，可以使用 ``GPIO0`` 输出 ``REF_CLK`` 信号。然而，在这种情况下时钟是直接输出到 GPIO 的，因此与 EMAC 外设并没有直接关联。有时这种配置可能无法很好地兼容所使用的 PHY 芯片。如果设计中未使用 PSRAM，则还可以使用 GPIO16 和 GPIO17 来输出参考时钟信号。其时钟源相同（均来自 APLL），但这些信号是通过 EMAC 外设引出的。

            如果配置 RMII 时钟模式为 :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_EXT_IN`，那么只能选择 ``GPIO0`` 输入 ``REF_CLK`` 信号。请注意， ``GPIO0`` 同时也是 ESP32 上一个重要的 strapping GPIO 管脚。如果上电时 GPIO0 为低电平，则 ESP32 将进入下载模式，需进行手动复位重启系统。解决这个问题的方法是，在硬件中默认禁用 ``REF_CLK``，从而避免 strapping 管脚在启动阶段受到其他信号的干扰。随后，再在以太网驱动安装阶段重新启用 ``REF_CLK``。

            可以通过以下方法禁用 ``REF_CLK`` 信号：

            * 禁用或关闭晶体振荡器的电源（对应图中的选项 **b**）。

            * 强制复位 PHY 设备（对应图中的选项 **a**）。**此种方法并不适用于所有 PHY 设备** （即便处于复位状态，某些 PHY 设备仍会向 GPIO0 输出信号）。

        .. warning::

            如希望 **以太网与 Wi-Fi 一起工作**，不要选择 ESP32 作为 ``REF_CLK`` 的源，因为这会导致 ``REF_CLK`` 不稳定。可以选择禁用 Wi-Fi，或使用 PHY 或外部振荡器作为 ``REF_CLK`` 的源。

    .. only:: not esp32

        .. note::
            如果 RMII 时钟模式配置为 :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_OUT`，则可以通过 IO_MUX 将 {IDF_TARGET_SOC_REF_CLK_OUT_GPIO} 选择为 ``REF_CLK`` 信号的输出管脚。

            如果 RMII 时钟模式配置为 :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_EXT_IN`，则可以通过 IO_MUX 将 {IDF_TARGET_SOC_REF_CLK_IN_GPIO} 选择为 ``REF_CLK`` 信号的输入管脚。

    .. only:: not SOC_EMAC_RMII_CLK_OUT_INTERNAL_LOOPBACK

        .. warning::
            如果 RMII 时钟模式配置为 :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_OUT`，则必须从外部将 ``REF_CLK`` 输出信号回环到 EMAC。请将 :cpp:member:`eth_esp32_emac_config_t::clock_config_out_in` 中的 :cpp:member:`eth_mac_clock_config_t::clock_mode` 配置为 :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_EXT_IN`，并选择与 ``REF_CLK`` 输入 GPIO ({IDF_TARGET_SOC_REF_CLK_IN_GPIO}) 相关联的 GPIO 编号。

            .. only:: esp32p4

                .. figure:: ../../../_static/rmii_ref_clk_esp32p4.png
                    :scale: 95 %
                    :alt: RMII REF_CKL Output Loopback
                    :figclass: align-center

                    RMII REF_CKL 输出回环

    **无论选择哪种 RMII 时钟模式，都请确保硬件设计中 REF_CLK 的信号完整性！** 信号线越短越好，并远离 RF 设备和电感。

    .. only:: not SOC_EMAC_USE_MULTI_IO_MUX

        .. note::
            数据平面中使用的信号通过 IO_MUX 连接至特定的 GPIO，这些信号无法配置到其他 GPIO 上。控制平面中使用的信号可以通过矩阵路由到任何空闲的 GPIO 上。相关硬件设计示例，请参阅 `ESP32-Ethernet-Kit <https://docs.espressif.com/projects/esp-dev-kits/zh_CN/latest/esp32/esp32-ethernet-kit/index.html>`_。

    .. only:: SOC_EMAC_USE_MULTI_IO_MUX

        .. note::
            数据平面中使用的信号可以通过 IO_MUX 配置为 RMII 预定义的 GPIO，请参阅下表。数据平面 GPIO 配置由驱动程序根据 :cpp:member:`eth_esp32_emac_config_t::emac_dataif_gpio` 的内容执行。控制平面中使用的信号可以通过 GPIO 矩阵路由到任何空闲的 GPIO。

            .. list-table:: {IDF_TARGET_NAME} RMII 数据平面 GPIO
                :header-rows: 1
                :widths: 50 50
                :align: center

                * - 管脚名
                  - GPIO 编号

                * - TX_EN
                  - {IDF_TARGET_SOC_RMII_TX_EN}

                * - TXD0
                  - {IDF_TARGET_SOC_RMII_TXD0}

                * - TXD1
                  - {IDF_TARGET_SOC_RMII_TXD1}

                * - CRS_DV
                  - {IDF_TARGET_SOC_RMII_CRS_DV}

                * - RXD0
                  - {IDF_TARGET_SOC_RMII_RXD0}

                * - RXD1
                  - {IDF_TARGET_SOC_RMII_RXD1}

根据以太网板设计，需要分别为 MAC 和 PHY 配置必要的参数，通过两者完成驱动程序的安装。

MAC 的相关配置可以在 :cpp:class:`eth_mac_config_t` 中找到，具体包括：

.. list::

    * :cpp:member:`eth_mac_config_t::sw_reset_timeout_ms`：软件复位超时值，单位为毫秒。通常，MAC 复位应在 100 ms 内完成。

    * :cpp:member:`eth_mac_config_t::rx_task_stack_size` 和 :cpp:member:`eth_mac_config_t::rx_task_prio`：MAC 驱动会创建一个专门的任务来处理传入的数据包，这两个参数用于设置该任务的堆栈大小和优先级。

    * :cpp:member:`eth_mac_config_t::flags`：指定 MAC 驱动应支持的额外功能，尤其适用于某些特殊情况。这个字段的值支持与以 ``ETH_MAC_FLAG_`` 为前缀的宏进行 OR 运算。例如，如果要求 MAC 驱动程序在 cache 禁用时仍能正常工作，那么则需要用 :c:macro:`ETH_MAC_FLAG_WORK_WITH_CACHE_DISABLE` 配置这个字段。

.. only:: SOC_EMAC_SUPPORTED

    :cpp:class:`eth_esp32_emac_config_t` 描述了 **内部 MAC 模块** 的特定配置，其中包括：

    .. list::

        * :cpp:member:`eth_esp32_emac_config_t::smi_mdc_gpio_num` 和 :cpp:member:`eth_esp32_emac_config_t::smi_mdio_gpio_num`：连接 SMI 信号的 GPIO 编号。

        * :cpp:member:`eth_esp32_emac_config_t::interface`：配置到 PHY (MII/RMII) 的 MAC 数据接口。

        * :cpp:member:`eth_esp32_emac_config_t::clock_config`：配置 EMAC 接口时钟（RMII 模式下的 ``REF_CLK`` 模式以及 GPIO 编号）。

        * :cpp:member:`eth_esp32_emac_config_t::intr_priority`： 设置 MAC 中断的优先级。如果设置为 ``0`` 或负值，则驱动程序将分配一个具有默认优先级的中断。否则，驱动程序将使用给定的优先级。请注意，可以设置 *低*、 *中* 中断优先级（1 到 3），因为这可以在 C 中处理。

        :SOC_EMAC_USE_MULTI_IO_MUX: * :cpp:member:`eth_esp32_emac_config_t::emac_dataif_gpio`：EMAC MII/RMII 数据平面 GPIO 编号配置。

        :not SOC_EMAC_RMII_CLK_OUT_INTERNAL_LOOPBACK: * :cpp:member:`eth_esp32_emac_config_t::clock_config_out_in`：当 ``REF_CLK`` 信号在内部生成并从外部回环到 EMAC 时，配置 EMAC 输入接口时钟。必须始终将 EMAC 的模式配置为 :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_EXT_IN`。此选项仅在 :cpp:member:`eth_esp32_emac_config_t::clock_config` 的配置设置为 :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_OUT` 时有效。

    使用内部 MAC 时的内存注意事项
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    内部 MAC 子系统通过 DMA 使用描述符链表在 CPU 域之间传输数据。描述符有发送和接收两种类型，分别保存接收或已发送帧的状态信息，或提供传输控制。每个描述符还包含指向当前数据缓冲区和下一个描述符的指针。因此，在支持 DMA 的内存中，单个 EMAC DMA 描述符的大小为 {IDF_TARGET_SOC_DMA_DESC_SIZE}。

    默认配置应涵盖大多数用例。然而，某些情况可能需要对以太网 DMA 内存的使用进行配置，以满足特定需求。典型问题可能发生于下列情形：

    .. list::

        * **网络流量由短且频繁的帧主导时**：如果你的网络流量主要由短且频繁发送（或接收）的帧组成，可能会遇到吞吐量低于预期（尽管额定为 100 Mbps），以及接收过程中丢帧等问题。在发送时，套接字发送 API 可能会返回 ``errno`` 为 ``ENOMEM``，并显示 `TX 缓冲区大小不足`（如果启用了调试日志级别）。这些问题的主要原因是，默认的内存配置针对较大帧进行了优化。默认情况下 :ref:`CONFIG_ETH_DMA_BUFFER_SIZE` 设置为 512 字节，以确保 *数据缓冲区* 与 *描述符* 大小的开销比。要解决此问题，可以增加缓冲区数量， :ref:`CONFIG_ETH_DMA_RX_BUFFER_NUM` 或 :ref:`CONFIG_ETH_DMA_TX_BUFFER_NUM`。此外，还可以减小 :ref:`CONFIG_ETH_DMA_BUFFER_SIZE`，使其与网络中典型帧的大小相匹配，从而合理控制以太网驱动的内存占用。

        * **高吞吐量导致缓冲区耗尽时**：如果套接字发送 API 间歇性返回 ``errno`` 为 ``ENOMEM``，并显示 `TX 缓冲区大小不足`（如果启用了调试日志级别），且吞吐量接近额定的 100 Mbps，这通常表明接近硬件限制。在这种情况下，硬件无法跟上传输请求。解决方案是，增加 :ref:`CONFIG_ETH_DMA_TX_BUFFER_NUM`，以缓存更多的帧，并缓解传输请求的短时峰值。然而，如果请求的流量持续超过额定吞吐量，此方法将失效，需通过应用层通过软件限制带宽。

PHY 的相关配置可以在 :cpp:class:`eth_phy_config_t` 中找到，具体包括：

.. list::

    * :cpp:member:`eth_phy_config_t::phy_addr`：同一条 SMI 总线上可以存在多个 PHY 设备，所以有必要为各个 PHY 设备分配唯一地址。通常，这个地址是在硬件设计期间，通过拉高/拉低一些 PHY strapping 管脚来配置的。根据不同的以太网开发板，可配置值为 ``0`` 到 ``15``。需注意，如果 SMI 总线上仅有一个 PHY 设备，将该值配置为 ``-1``，即可使驱动程序自动检测 PHY 地址。

    * :cpp:member:`eth_phy_config_t::reset_timeout_ms`：复位超时值，单位为毫秒。通常，PHY 复位应在 100 ms 内完成。

    * :cpp:member:`eth_phy_config_t::autonego_timeout_ms`：自动协商超时值，单位为毫秒。以太网驱动程序会与链路另一端的设备进行自协商，以确定连接的最佳双工模式和速率。此值通常取决于电路板上 PHY 设备的性能。

    * :cpp:member:`eth_phy_config_t::reset_gpio_num`：如果开发板同时将 PHY 复位管脚连接至了任意 GPIO 管脚，请使用该字段进行配置。否则，配置为 ``-1``。

    * :cpp:member:`eth_phy_config_t::hw_reset_assert_time_us`：PHY 复位引脚被置为有效状态的时间（以微秒为单位）。将该值配置为 ``0``，即可使用芯片默认的复位时长。

    * :cpp:member:`eth_phy_config_t::post_hw_reset_delay_ms`：PHY 硬件复位完成后的等待时间（以毫秒为单位）。将该值配置为 ``0``，即可使用芯片默认的等待时长，配置为 ``-1``，表示执行 PHY 硬件复位后不等待。

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
        esp32_emac_config.smi_gpio.mdc_num = CONFIG_ETHERNET_MDC_GPIO;               // 更改用于 MDC 信号的 GPIO
        esp32_emac_config.smi_gpio.mdio_num = CONFIG_ETHERNET_MDIO_GPIO;             // 更改用于 MDIO 信号的 GPIO
        esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config); // 创建 MAC 实例

        eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();      // 应用默认的 PHY 配置
        phy_config.phy_addr = CONFIG_ETHERNET_PHY_ADDR;              // 根据开发板设计更改 PHY 地址
        phy_config.reset_gpio_num = CONFIG_ETHERNET_PHY_RST_GPIO;    // 更改用于 PHY 复位的 GPIO
        esp_eth_phy_t *phy = esp_eth_phy_new_generic(&phy_config);     // 创建通用 PHY 实例

    .. note::
        使用 :cpp:func:`esp_eth_phy_new_generic` 创建新的 PHY 实例时，可以使用任何符合 IEEE 802.3 标准的以太网 PHY 芯片。然而，尽管 PHY 芯片符合 IEEE 802.3 标准，能提供基本功能，但某些特定的功能可能无法完全实现。例如，某些以太网 PHY 芯片可能需要配置特定的速度模式才能启用环回功能。遇到这种情况，需要配置 PHY 驱动程序以满足特定芯片需求，请使用 ESP-IDF 官方支持的 PHY 芯片驱动程序，或参阅 :ref:`Custom PHY Driver <custom-phy-driver>` 小节以创建新的自定义驱动程序。

    .. tip::
        乐鑫为多种特定的以太网 PHY 芯片提供了驱动程序，这些驱动程序可在 `esp-eth-drivers <https://github.com/espressif/esp-eth-drivers>`_ 仓库中获取。驱动以组件形式分发，并可在 `ESP 组件库 <https://components.espressif.com/>`_ 上获取。

    可选的运行时 MAC 时钟配置
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    可以通过用户应用程序代码，选择性配置 EMAC 中的 ``REF_CLK``。

    .. highlight:: c

    ::

        eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG(); // 应用默认的供应商特定 MAC 配置

        // ...

        esp32_emac_config.interface = EMAC_DATA_INTERFACE_RMII;                      // 更改 EMAC 数据接口
        esp32_emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;               // 配置 EMAC REF_CLK 模式
        esp32_emac_config.clock_config.rmii.clock_gpio = 17;                         // 配置用于输入/输出 EMAC REF_CLK 的 GPIO 编号
        esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config); // 创建 MAC 实例


SPI-Ethernet 模块
^^^^^^^^^^^^^^^^^^^

.. highlight:: c

::

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();      // 应用默认的通用 MAC 配置
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();      // 应用默认的 PHY 配置
    phy_config.phy_addr = CONFIG_ETHERNET_PHY_ADDR;              // 根据开发板设计更改 PHY 地址
    phy_config.reset_gpio_num = CONFIG_ETHERNET_PHY_RST_GPIO;    // 更改用于 PHY 复位的 GPIO
    // 安装 GPIO 中断服务（因为 SPI-Ethernet 模块为中断驱动）
    gpio_install_isr_service(0);
    // 配置 SPI 总线
    spi_device_handle_t spi_handle = NULL;
    spi_bus_config_t buscfg = {
        .miso_io_num = CONFIG_ETHERNET_SPI_MISO_GPIO,
        .mosi_io_num = CONFIG_ETHERNET_SPI_MOSI_GPIO,
        .sclk_io_num = CONFIG_ETHERNET_SPI_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(CONFIG_ETHERNET_SPI_HOST, &buscfg, 1));
    // 配置 SPI 从机设备
    spi_device_interface_config_t spi_devcfg = {
        .mode = 0,
        .clock_speed_hz = CONFIG_ETHERNET_SPI_CLOCK_MHZ * 1000 * 1000,
        .spics_io_num = CONFIG_ETHERNET_SPI_CS_GPIO,
        .queue_size = 20
    };
    /* dm9051 ethernet driver is based on spi driver */
    eth_dm9051_config_t dm9051_config = ETH_DM9051_DEFAULT_CONFIG(CONFIG_ETHERNET_SPI_HOST, &spi_devcfg);
    dm9051_config.int_gpio_num = CONFIG_ETHERNET_SPI_INT_GPIO;
    esp_eth_mac_t *mac = esp_eth_mac_new_dm9051(&dm9051_config, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_dm9051(&phy_config);


.. note::
    * 当为 SPI-Ethernet 模块（例如 DM9051）创建 MAC 和 PHY 实例时，由于 PHY 是集成在模块中的，因此调用的实例创建函数的后缀须保持一致（例如 `esp_eth_mac_new_dm9051` 和 `esp_eth_phy_new_dm9051` 搭配使用）。

    * 针对不同的以太网模块，或是为了满足特定 PCB 上的 SPI 时序，SPI 从机设备配置（即 `spi_device_interface_config_t`）可能略有不同。具体配置请查看模块规格以及 ESP-IDF 中的示例。

.. tip::
    乐鑫为多种 SPI-Ethernet 模块提供了驱动程序，这些驱动程序可在 `esp-eth-drivers <https://github.com/espressif/esp-eth-drivers>`_ 仓库中获取。驱动以组件形式分发，并可在 `ESP 组件库 <https://components.espressif.com/>`_ 上获取。

安装驱动程序
--------------

安装以太网驱动程序需要结合 MAC 和 PHY 实例，并在 :cpp:class:`esp_eth_config_t` 中配置一些额外的高级选项（即不仅限于 MAC 或 PHY 的选项）：

* :cpp:member:`esp_eth_config_t::mac`：由 MAC 生成器创建的实例（例如 :cpp:func:`esp_eth_mac_new_esp32`）。

* :cpp:member:`esp_eth_config_t::phy`：由 PHY 生成器创建的实例（例如 :cpp:func:`esp_eth_phy_new_generic`）。

* :cpp:member:`esp_eth_config_t::check_link_period_ms`：以太网驱动程序会启用操作系统定时器来定期检查链接状态。该字段用于设置间隔时间，单位为毫秒。

* :cpp:member:`esp_eth_config_t::stack_input` 或 :cpp:member:`esp_eth_config_t::stack_input_info`：在大多数的以太网物联网应用中，驱动器接收的以太网帧会被传递到上层（如 TCP/IP 栈）。经配置，该字段为负责处理传入帧的函数。可以在安装驱动程序后，通过函数 :cpp:func:`esp_eth_update_input_path` 更新该字段。该字段支持在运行过程中进行更新。

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

.. _time-stamping:

.. only:: SOC_EMAC_IEEE1588V2_SUPPORTED

    EMAC 硬件时间戳
    -----------------

    EMAC 时间戳功能可以精确记录以太网帧的发送和接收时间。硬件时间戳对于精确时间协议 (PTP) 等应用至关重要，因为它减少了依赖软件时间戳可能出现的抖动和不准确性。将时间戳直接嵌入硬件，避免了由软件层或处理开销引入的延迟，从而实现纳秒级精度。

    .. 警告::

        请注意：时间戳相关的 API 当前属于 **“实验特性”**，未来版本中可能会有所更改。

    以下内容介绍如何在 EMAC 中启用时间戳、获取和设置时间。

    .. highlight:: c

    ::

        esp_eth_mac_t *mac;
        esp_eth_get_mac_instance(eth_hndl, &mac);

        // 启用硬件时间戳
        eth_mac_ptp_config_t ptp_cfg = ETH_MAC_ESP_PTP_DEFAULT_CONFIG();
        esp_eth_mac_ptp_enable(mac, &ptp_cfg);

        // 获取当前 EMAC 时间
        eth_mac_time_t ptp_time;
        esp_eth_mac_get_ptp_time(mac, &ptp_time);

        // 设置 EMAC 时间
        ptp_time = {
            .seconds = 42,
            .nanoseconds = 0
        };
        esp_eth_mac_set_ptp_time(mac, &ptp_time);

    PTP 模块可以按如下方式配置：

    .. list::
        * :cpp:member:`eth_mac_ptp_config_t::clk_src`：PTP 时钟源。从 :cpp:type:`soc_periph_emac_ptp_clk_src_t` 枚举中选择一个时钟源。

        * :cpp:member:`eth_mac_ptp_config_t::clk_src_period_ns`：PTP 时钟源的周期（以纳秒为单位）。例如，如果时钟源为 40 MHz，则周期为 25 ns。

        * :cpp:member:`eth_mac_ptp_config_t::required_accuracy_ns`：PTP 的所需精度（以纳秒为单位）。所需精度必须低于 PTP 时钟源的周期。例如，如果时钟源为 40 MHz（25 ns 周期），则所需精度为 40 ns。

        * :cpp:member:`eth_mac_ptp_config_t::roll_type`：亚秒寄存器的翻转模式（数字或二进制）。推荐使用二进制翻转模式，因为它能提供更精确的时间同步。

    接收帧的时间戳可以通过注册的 :cpp:member:`esp_eth_config_t::stack_input_info` 函数的最后一个参数进行访问，传输帧的时间戳可以通过注册的 :cpp:func:`esp_eth_transmit_ctrl_vargs` 函数的 ``ctrl`` 参数进行访问。然而，对于用户获取时间戳信息，更简便的方式是利用 L2 TAP :ref:`扩展缓冲区 <esp_netif_l2tap_ext_buff>` 机制。

    您可以通过注册回调函数和设置事件触发的目标时间，在精确的时间点调度事件。请注意，回调函数将在中断服务程序 (ISR) 上下文中调用，因此应尽量简洁。

    .. highlight:: c

    ::

        // 注册回调函数
        esp_eth_mac_set_target_time_cb(mac, ts_callback);

        // 设置事件的触发时间
        eth_mac_time_t mac_target_time = {
            .seconds = 42,
            .nanoseconds = 0
        };
        esp_eth_mac_set_target_time(mac, &mac_target_time);

    此外，PTP 同步的时间可以通过 GPIO 上的 PPS (Pulse-Per-Second) 信号输出。这提供了一个精确的硬件时间参考，可用于同步外部设备、对齐独立时钟域或驱动 ESP32 芯片系列之外的时间关键型进程。顾名思义，PPS 信号默认每秒产生一次脉冲。但是，可以通过 :cpp:func:`esp_eth_mac_set_pps_out_freq` 函数设置 PPS0 输出频率来调整频率。该命令接受 0-16384 范围内的整数值，其中 0 = 1 PPS（单次脉冲），其他值产生方波时钟信号。时钟频率必须是 2 的幂且小于或等于 16384 Hz。请注意，由于数字翻转模式中位的非线性切换，实际频率是一个平均值（在一秒钟的总周期内占空比不同于 50%）。此行为不适用于二进制翻转模式，因此推荐使用二进制翻转模式。可以使用 :cpp:func:`esp_eth_mac_set_pps_out_gpio` 函数配置 PPS 信号在 GPIO 上输出。

    .. only:: esp32p4

        .. note::
            GPIO 管脚上的 PPS 信号输出从 ESP32-P4 芯片版本 3 开始可用。

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

需注意，暂停帧是在自动协商期间由 PHY 向对等端公布的。只有当链路两端都支持暂停帧时，以太网驱动程序才会发送暂停帧。

.. -------------------------------- Examples -----------------------------------

应用示例
--------------------

  * :example:`ethernet/basic` 演示了如何使用以太网驱动程序，包括驱动程序的安装、将其连接到 ``esp_netif``、发送 DHCP 请求以及获取可 ping 的 IP 地址。

  * :example:`ethernet/iperf` 演示了如何使用以太网功能，使用 iPerf 测量吞吐量/带宽。

  * :example:`ethernet/ptp` 演示了如何在以太网上使用精确时间协议 (PTP) 同步时间。

  * :example:`network/vlan_support` 演示了如何在以太网上创建虚拟网络接口，包括 VLAN 和非 VLAN 接口。

  * :example:`network/sta2eth` 演示了如何使用 Wi-Fi station 和有线接口（如以太网或 USB）创建 1 对 1 的桥接。

  * :example:`network/simple_sniffer` 演示了如何在嗅探模式下使用 Wi-Fi 和以太网来捕获数据包，并将其保存为 PCAP 格式。

  * :example:`network/eth2ap` 演示了如何实现一个桥接器，在以太网端口和 Wi-Fi AP 接口之间转发数据包。该示例使用 {IDF_TARGET_NAME} 创建以太网和 Wi-Fi 之间的 1 对多连接，而无需初始化 TCP/IP 栈。

  * :example:`network/bridge` 演示了如何使用 LwIP IEEE 802.1D 桥接器根据 MAC 地址在多个网络段之间转发以太网帧。

  * 大多数协议示例也适用于以太网：:example:`protocols`

.. ------------------------------ Advanced Topics -------------------------------

.. _advanced-topics:

进阶操作
---------------

.. _custom-phy-driver:

自定义 PHY 驱动程序
^^^^^^^^^^^^^^^^^^^^^^^^^

市面上有多家 PHY 芯片制造商提供各种类型的芯片。ESP-IDF 现已支持 ``通用 PHY`` 和数种特定的 PHY 芯片，但是由于价格、功能、库存等原因，有时用户还是无法找到一款能满足其实际需求的芯片。

好在 IEEE 802.3 在其 22.2.4 管理功能部分对 EMAC 和 PHY 之间的管理接口进行了标准化。该部分定义了所谓的 ”MII 管理接口”规范，用于控制 PHY 和收集 PHY 的状态，还定义了一组管理寄存器来控制芯片行为、链接属性、自动协商配置等。在 ESP-IDF 中，这项基本的管理功能是由 :component_file:`esp_eth/src/phy/esp_eth_phy_802_3.c` 实现的，这也大大降低了创建新的自定义 PHY 芯片驱动的难度。

.. note::
    由于一些 PHY 芯片可能不符合 IEEE 802.3 第 22.2.4 节的规定，所以请首先查看 PHY 数据手册。不过，就算芯片不符合规定，依旧可以创建自定义 PHY 驱动程序，只是由于需要自行定义所有的 PHY 管理功能，这个过程将变得较为复杂。

ESP-IDF 以太网驱动程序所需的大部分 PHY 管理功能都已涵盖在 :component_file:`esp_eth/src/phy/esp_eth_phy_802_3.c` 中。不过对于以下几项，可能仍需针对不同芯片开发具体的管理功能：

    * 链接状态。此项总是由使用的具体芯片决定
    * 芯片初始化。即使不存在严格的限制，也应进行自定义，以确保使用的是符合预期的芯片
    * 芯片的具体功能配置

**创建自定义 PHY 驱动程序的步骤：**

1. 请根据 PHY 数据手册，定义厂家私有的寄存器。
2. 准备衍生的 PHY 管理对象信息结构，该结构：

    * 必须至少包含 IEEE 802.3 :cpp:class:`phy_802_3_t` 父对象
    * 可选择包含额外的变量，以支持非 IEEE 802.3 或定制功能。

3. 定义针对芯片的特定管理回调功能。
4. 初始化 IEEE 802.3 父对象并重新分配针对芯片的特定管理回调功能。

实现新的自定义 PHY 驱动程序后，你可以通过 `乐鑫组件注册表 <https://components.espressif.com/>`_ 将驱动分享给其他用户。

.. ---------------------------- API Reference ----------------------------------

API 参考
-------------

.. include-build-file:: inc/eth_types.inc
.. include-build-file:: inc/esp_eth.inc
.. include-build-file:: inc/esp_eth_driver.inc
.. include-build-file:: inc/esp_eth_com.inc
.. include-build-file:: inc/esp_eth_mac.inc
.. include-build-file:: inc/esp_eth_mac_esp.inc
.. include-build-file:: inc/esp_eth_mac_spi.inc
.. include-build-file:: inc/esp_eth_phy.inc
.. include-build-file:: inc/esp_eth_phy_802_3.inc
.. include-build-file:: inc/esp_eth_netif_glue.inc
