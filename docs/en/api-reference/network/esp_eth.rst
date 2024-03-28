Ethernet
========

{IDF_TARGET_SOC_REF_CLK_IN_GPIO:default="", esp32="GPIO0", esp32p4="GPIO32, GPIO44 and GPIO50"}
{IDF_TARGET_SOC_REF_CLK_OUT_GPIO:default="", esp32="GPIO0, GPIO16 and GPIO17", esp32p4="GPIO23 and GPIO39"}
{IDF_TARGET_SOC_RMII_TX_EN:default="", esp32="GPIO21", esp32p4="GPIO33, GPIO40 and GPIO49"}
{IDF_TARGET_SOC_RMII_TXD0:default="", esp32="GPIO19", esp32p4="GPIO34 and GPIO41"}
{IDF_TARGET_SOC_RMII_TXD1:default="", esp32="GPIO22", esp32p4="GPIO35 and GPIO42"}
{IDF_TARGET_SOC_RMII_CRS_DV:default="", esp32="GPIO27", esp32p4="GPIO28, GPIO45 and GPIO51"}
{IDF_TARGET_SOC_RMII_RXD0:default="", esp32="GPIO25", esp32p4="GPIO29, GPIO46 and GPIO52"}
{IDF_TARGET_SOC_RMII_RXD1:default="", esp32="GPIO26", esp32p4="GPIO30, GPIO47 and GPIO53"}


:link_to_translation:`zh_CN:[中文]`

.. -------------------------------- Overview -----------------------------------

Overview
--------

.. only:: SOC_EMAC_SUPPORTED

    ESP-IDF provides a set of consistent and flexible APIs to support both internal Ethernet MAC (EMAC) controller and external SPI-Ethernet modules.

.. only:: not SOC_EMAC_SUPPORTED

    ESP-IDF provides a set of consistent and flexible APIs to support external SPI-Ethernet modules.

This programming guide is split into the following sections:

    1. :ref:`basic-ethernet-concepts`
    2. :ref:`driver-configuration-and-installation`
    3. :ref:`connect-driver-to-stack`
    4. :ref:`misc-operation-of-driver`

.. --------------------------- Basic Ethernet Concepts ------------------------------

.. _basic-ethernet-concepts:

Basic Ethernet Concepts
-----------------------

Ethernet is an asynchronous Carrier Sense Multiple Access with Collision Detect (CSMA/CD) protocol/interface. It is generally not well suited for low-power applications. However, with ubiquitous deployment, internet connectivity, high data rates, and limitless-range expandability, Ethernet can accommodate nearly all wired communications.

Normal IEEE 802.3 compliant Ethernet frames are between 64 and 1518 bytes in length. They are made up of five or six different fields: a destination MAC address (DA), a source MAC address (SA), a type/length field, a data payload, an optional padding field and a Cyclic Redundancy Check (CRC). Additionally, when transmitted on the Ethernet medium, a 7-byte preamble field and Start-of-Frame (SOF) delimiter byte are appended to the beginning of the Ethernet packet.

Thus the traffic on the twist-pair cabling appears as shown below:

.. rackdiag:: ../../../_static/diagrams/ethernet/data_frame_format.diag
    :caption: Ethernet Data Frame Format
    :align: center

Preamble and Start-of-Frame Delimiter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The preamble contains seven bytes of ``55H``. It allows the receiver to lock onto the stream of data before the actual frame arrives.

The Start-of-Frame Delimiter (SFD) is a binary sequence ``10101011`` (as seen on the physical medium). It is sometimes considered to be part of the preamble.

When transmitting and receiving data, the preamble and SFD bytes will be automatically generated or stripped from the packets.

Destination Address
^^^^^^^^^^^^^^^^^^^

The destination address field contains a 6-byte length MAC address of the device that the packet is directed to. If the Least Significant bit in the first byte of the MAC address is set, the address is a multicast destination. For example, 01-00-00-00-F0-00 and 33-45-67-89-AB-CD are multi-cast addresses, while 00-00-00-00-F0-00 and 32-45-67-89-AB-CD are not.

Packets with multi-cast destination addresses are designed to arrive and be important to a selected group of Ethernet nodes. If the destination address field is the reserved multicast address, i.e., FF-FF-FF-FF-FF-FF, the packet is a broadcast packet and it will be directed to everyone sharing the network. If the Least Significant bit in the first byte of the MAC address is clear, the address is a unicast address and will be designed for usage by only the addressed node.

Normally the EMAC controller incorporates receive filters which can be used to discard or accept packets with multi-cast, broadcast and/or unicast destination addresses. When transmitting packets, the host controller is responsible for writing the desired destination address into the transmit buffer.

Source Address
^^^^^^^^^^^^^^

The source address field contains a 6-byte length MAC address of the node which created the Ethernet packet. Users of Ethernet must generate a unique MAC address for each controller used. MAC addresses consist of two portions. The first three bytes are known as the Organizationally Unique Identifier (OUI). OUIs are distributed by the IEEE. The last three bytes are address bytes at the discretion of the company that purchased the OUI. For more information about MAC Address used in ESP-IDF, please see :ref:`MAC Address Allocation <MAC-Address-Allocation>`.

When transmitting packets, the assigned source MAC address must be written into the transmit buffer by the host controller.

Type/Length
^^^^^^^^^^^^^

The type/length field is a 2-byte field. If the value in this field is <= 1500 (decimal), it is considered a length field and it specifies the amount of non-padding data which follows in the data field. If the value is >= 1536, it represents the protocol the following packet data belongs to. The following are the most common type values:

  * IPv4 = 0800H
  * IPv6 = 86DDH
  * ARP = 0806H

Users implementing proprietary networks may choose to treat this field as a length field, while applications implementing protocols such as the Internet Protocol (IP) or Address Resolution Protocol (ARP), should program this field with the appropriate type defined by the protocol's specification when transmitting packets.

Payload
^^^^^^^

The payload field is a variable length field, anywhere from 0 to 1500 bytes. Larger data packets violates Ethernet standards and will be dropped by most Ethernet nodes.

This field contains the client data, such as an IP datagram.

Padding and FCS
^^^^^^^^^^^^^^^

The padding field is a variable length field added to meet the IEEE 802.3 specification requirements when small data payloads are used.

The DA, SA, type, payload, and padding of an Ethernet packet must be no smaller than 60 bytes in total. If the required 4-byte FCS field is added, packets must be no smaller than 64 bytes. If the payload field is less than 46-byte long, a padding field is required.

The FCS field is a 4-byte field that contains an industry-standard 32-bit CRC calculated with the data from the DA, SA, type, payload, and padding fields. Given the complexity of calculating a CRC, the hardware normally automatically generates a valid CRC and transmit it. Otherwise, the host controller must generate the CRC and place it in the transmit buffer.

Normally, the host controller does not need to concern itself with padding and the CRC which the hardware EMAC will also be able to automatically generate when transmitting and verify when receiving. However, the padding and CRC fields will be written into the receive buffer when packets arrive, so they may be evaluated by the host controller if needed.

.. note::
    Besides the basic data frame described above, there are two other common frame types in 10/100 Mbps Ethernet: control frames and VLAN-tagged frames. They are not supported in ESP-IDF.

.. ------------------------------ Driver Operation --------------------------------

.. _driver-configuration-and-installation:

Configure MAC and PHY
---------------------

The Ethernet driver is composed of two parts: MAC and PHY.

.. only:: SOC_EMAC_SUPPORTED

    The communication between MAC and PHY can have diverse choices: **MII** (Media Independent Interface), **RMII** (Reduced Media Independent Interface), etc.

    .. figure:: ../../../_static/rmii-interface.png
        :scale: 80 %
        :alt: Ethernet RMII Interface
        :figclass: align-center

        Ethernet RMII Interface

    One of the obvious differences between MII and RMII is signal consumption. MII usually costs up to 18 signals, while the RMII interface can reduce the consumption to 9.

    .. only:: esp32

        .. note::
            ESP-IDF only supports the RMII interface. Therefore, always set :cpp:member:`eth_esp32_emac_config_t::interface` to :cpp:enumerator:`eth_data_interface_t::EMAC_DATA_INTERFACE_RMII` or always select ``CONFIG_ETH_PHY_INTERFACE_RMII`` in the Kconfig option :ref:`CONFIG_ETH_PHY_INTERFACE`.

    .. only:: not esp32

        .. note::
            ESP-IDF only supports the RMII interface. Therefore, always set :cpp:member:`eth_esp32_emac_config_t::interface` to :cpp:enumerator:`eth_data_interface_t::EMAC_DATA_INTERFACE_RMII`.

    In RMII mode, both the receiver and transmitter signals are referenced to the ``REF_CLK``. ``REF_CLK`` **must be stable during any access to PHY and MAC**. Generally, there are three ways to generate the ``REF_CLK`` depending on the PHY device in your design:

    * Some PHY chips can derive the ``REF_CLK`` from its externally connected 25 MHz crystal oscillator (as seen the option **a** in the picture). In this case, you should configure :cpp:member:`eth_mac_clock_config_t::clock_mode` of :cpp:member:`eth_esp32_emac_config_t::clock_config` to :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_EXT_IN`.

    * Some PHY chip uses an externally connected 50MHz crystal oscillator or other clock sources, which can also be used as the ``REF_CLK`` for the MAC side (as seen the option **b** in the picture). In this case, you still need to configure :cpp:member:`eth_mac_clock_config_t::clock_mode` of :cpp:member:`eth_esp32_emac_config_t::clock_config` to :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_EXT_IN`.

    * Some EMAC controllers can generate the ``REF_CLK`` using an internal high-precision PLL (as seen the option **c** in the picture). In this case, you should configure :cpp:member:`eth_mac_clock_config_t::clock_mode` of :cpp:member:`eth_esp32_emac_config_t::clock_config` to :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_OUT`.

    .. only:: esp32

        .. note::
            The ``REF_CLK`` can be also configured via Project Configuration when :cpp:member:`eth_esp32_emac_config_t::clock_config::mode` of :cpp:member:`eth_esp32_emac_config_t::clock_config` is set to :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_DEFAULT`. Choose appropriately ``CONFIG_ETH_RMII_CLK_INPUT`` or ``CONFIG_ETH_RMII_CLK_OUTPUT`` option under :ref:`CONFIG_ETH_RMII_CLK_MODE` configuration based on your design as discussed above.

        .. warning::
            If the RMII clock mode is configured to :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_OUT` (or ``CONFIG_ETH_RMII_CLK_OUTPUT`` is selected), then ``GPIO0`` can be used to output the ``REF_CLK`` signal. See :cpp:enumerator:`emac_rmii_clock_gpio_t::EMAC_APPL_CLK_OUT_GPIO` or :ref:`CONFIG_ETH_RMII_CLK_OUTPUT_GPIO0` for more information.

            What is more, if you are not using PSRAM in your design, GPIO16 and GPIO17 are also available to output the reference clock signal. See :cpp:enumerator:`emac_rmii_clock_gpio_t::EMAC_CLK_OUT_GPIO` and :cpp:enumerator:`emac_rmii_clock_gpio_t::EMAC_CLK_OUT_180_GPIO` or :ref:`CONFIG_ETH_RMII_CLK_OUT_GPIO` for more information.

            If the RMII clock mode is configured to :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_EXT_IN` (or ``CONFIG_ETH_RMII_CLK_INPUT`` is selected), then ``GPIO0`` is the only choice to input the ``REF_CLK`` signal. Please note that ``GPIO0`` is also an important strapping GPIO on ESP32. If GPIO0 samples a low level during power-up, ESP32 will go into download mode. The system will get halted until a manually reset. The workaround for this issue is disabling the ``REF_CLK`` in hardware by default so that the strapping pin is not interfered by other signals in the boot stage. Then, re-enable the ``REF_CLK`` in the Ethernet driver installation stage.

            The ways to disable the ``REF_CLK`` signal can be:

            * Disable or power down the crystal oscillator (as the case **b** in the picture).

            * Force the PHY device to reset status (as the case **a** in the picture). **This could fail for some PHY device** (i.e., it still outputs signals to GPIO0 even in reset state).

    .. only:: not esp32

        .. note::
            If the RMII clock mode is configured to :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_OUT`, {IDF_TARGET_SOC_REF_CLK_OUT_GPIO} can be selected as output pin of the ``REF_CLK`` signal via IO_MUX.

            If the RMII clock mode is configured to :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_EXT_IN`, {IDF_TARGET_SOC_REF_CLK_IN_GPIO} can be selected as input pin for the ``REF_CLK`` signal via IO_MUX.

    .. only:: not SOC_EMAC_RMII_CLK_OUT_INTERNAL_LOOPBACK

        .. warning::
            If the RMII clock mode is configured to :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_OUT`, the ``REF_CLK`` output signal must be looped back to the EMAC externally. You have to configure :cpp:member:`eth_mac_clock_config_t::clock_mode` of :cpp:member:`eth_esp32_emac_config_t::clock_config_out_in` to :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_EXT_IN` and select GPIO number associated with ``REF_CLK`` input GPIO's ({IDF_TARGET_SOC_REF_CLK_IN_GPIO}).

            .. only:: esp32p4

                .. figure:: ../../../_static/rmii_ref_clk_esp32p4.png
                    :scale: 95 %
                    :alt: RMII REF_CKL Output Loopback
                    :figclass: align-center

                    RMII REF_CKL Output Loopback

    **No matter which RMII clock mode you select, you really need to take care of the signal integrity of REF_CLK in your hardware design!** Keep the trace as short as possible. Keep it away from RF devices and inductor elements.

    .. only:: not SOC_EMAC_USE_IO_MUX

        .. note::
            Signals used in the data plane are fixed to specific GPIOs via IO_MUX, they can not be modified to other GPIOs. Signals used in the control plane can be routed to any free GPIOs via Matrix. Please refer to :doc:`ESP32-Ethernet-Kit <../../hw-reference/esp32/get-started-ethernet-kit>` for hardware design example.

    .. only:: SOC_EMAC_USE_IO_MUX

        .. note::
            Signals used in the data plane can be configured to predefined set of GPIOs via IO_MUX for the RMII, see below table. The data plane GPIO configuration is performed by the driver based on content of :cpp:member:`eth_esp32_emac_config_t::emac_dataif_gpio`. Signals used in the control plane can be routed to any free GPIOs via GPIO Matrix.

            .. list-table:: {IDF_TARGET_NAME} RMII Data Plane GPIO
                :header-rows: 1
                :widths: 50 50
                :align: center

                * - Pin Name
                  - GPIO Number

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

You need to set up the necessary parameters for MAC and PHY respectively based on your Ethernet board design, and then combine the two together to complete the driver installation.

Basic common configuration for MAC layer is described in :cpp:class:`eth_mac_config_t`, including:

.. list::

    * :cpp:member:`eth_mac_config_t::sw_reset_timeout_ms`: software reset timeout value, in milliseconds. Typically, MAC reset should be finished within 100 ms.

    * :cpp:member:`eth_mac_config_t::rx_task_stack_size` and :cpp:member:`eth_mac_config_t::rx_task_prio`: the MAC driver creates a dedicated task to process incoming packets. These two parameters are used to set the stack size and priority of the task.

    * :cpp:member:`eth_mac_config_t::flags`: specifying extra features that the MAC driver should have, it could be useful in some special situations. The value of this field can be OR'd with macros prefixed with ``ETH_MAC_FLAG_``. For example, if the MAC driver should work when the cache is disabled, then you should configure this field with :c:macro:`ETH_MAC_FLAG_WORK_WITH_CACHE_DISABLE`.

.. only:: SOC_EMAC_SUPPORTED

    Specific configuration for **internal MAC module** is described in :cpp:class:`eth_esp32_emac_config_t`, including:

    .. list::

        * :cpp:member:`eth_esp32_emac_config_t::smi_mdc_gpio_num` and :cpp:member:`eth_esp32_emac_config_t::smi_mdio_gpio_num`: the GPIO number used to connect the SMI signals.

        * :cpp:member:`eth_esp32_emac_config_t::interface`: configuration of MAC Data interface to PHY (MII/RMII).

        * :cpp:member:`eth_esp32_emac_config_t::clock_config`: configuration of EMAC Interface clock (``REF_CLK`` mode and GPIO number in case of RMII).

        * :cpp:member:`eth_esp32_emac_config_t::intr_priority`: sets the priority of the MAC interrupt. If it is set to ``0`` or a negative value, the driver will allocate an interrupt with a default priority. Otherwise, the driver will use the given priority. Note that *Low* and *Medium* interrupt priorities (1 to 3) can be set since these can be handled in C.

        :SOC_EMAC_USE_IO_MUX: * :cpp:member:`eth_esp32_emac_config_t::emac_dataif_gpio`: configuration of EMAC MII/RMII data plane GPIO numbers.

        :not SOC_EMAC_RMII_CLK_OUT_INTERNAL_LOOPBACK: * :cpp:member:`eth_esp32_emac_config_t::clock_config_out_in`: configuration of EMAC input interface clock when ``REF_CLK`` signal is generated internally and is looped back to the EMAC externally. The mode must be always configured to :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_EXT_IN`. This option is valid only when configuration of :cpp:member:`eth_esp32_emac_config_t::clock_config` is set to :cpp:enumerator:`emac_rmii_clock_mode_t::EMAC_CLK_OUT`.

Configuration for PHY is described in :cpp:class:`eth_phy_config_t`, including:

.. list::

    * :cpp:member:`eth_phy_config_t::phy_addr`: multiple PHY devices can share the same SMI bus, so each PHY needs a unique address. Usually, this address is configured during hardware design by pulling up/down some PHY strapping pins. You can set the value from ``0`` to ``15`` based on your Ethernet board. Especially, if the SMI bus is shared by only one PHY device, setting this value to ``-1`` can enable the driver to detect the PHY address automatically.

    * :cpp:member:`eth_phy_config_t::reset_timeout_ms`: reset timeout value, in milliseconds. Typically, PHY reset should be finished within 100 ms.

    * :cpp:member:`eth_phy_config_t::autonego_timeout_ms`: auto-negotiation timeout value, in milliseconds. The Ethernet driver starts negotiation with the peer Ethernet node automatically, to determine to duplex and speed mode. This value usually depends on the ability of the PHY device on your board.

    * :cpp:member:`eth_phy_config_t::reset_gpio_num`: if your board also connects the PHY reset pin to one of the GPIO, then set it here. Otherwise, set this field to ``-1``.

ESP-IDF provides a default configuration for MAC and PHY in macro :c:macro:`ETH_MAC_DEFAULT_CONFIG` and :c:macro:`ETH_PHY_DEFAULT_CONFIG`.


Create MAC and PHY Instance
---------------------------

The Ethernet driver is implemented in an Object-Oriented style. Any operation on MAC and PHY should be based on the instance of the two.

.. only:: SOC_EMAC_SUPPORTED

    Internal EMAC + External PHY
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    .. highlight:: c

    ::

        eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();                      // apply default common MAC configuration
        eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG(); // apply default vendor-specific MAC configuration
        esp32_emac_config.smi_mdc_gpio_num = CONFIG_EXAMPLE_ETH_MDC_GPIO;            // alter the GPIO used for MDC signal
        esp32_emac_config.smi_mdio_gpio_num = CONFIG_EXAMPLE_ETH_MDIO_GPIO;          // alter the GPIO used for MDIO signal
        esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config); // create MAC instance

        eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();      // apply default PHY configuration
        phy_config.phy_addr = CONFIG_EXAMPLE_ETH_PHY_ADDR;           // alter the PHY address according to your board design
        phy_config.reset_gpio_num = CONFIG_EXAMPLE_ETH_PHY_RST_GPIO; // alter the GPIO used for PHY reset
        esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);     // create PHY instance
        // ESP-IDF officially supports several different Ethernet PHY chip driver
        // esp_eth_phy_t *phy = esp_eth_phy_new_rtl8201(&phy_config);
        // esp_eth_phy_t *phy = esp_eth_phy_new_lan8720(&phy_config);
        // esp_eth_phy_t *phy = esp_eth_phy_new_dp83848(&phy_config);

    Optional Runtime MAC Clock Configuration
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    EMAC ``REF_CLK`` can be optionally configured from the user application code.

    .. highlight:: c

    ::

        eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG(); // apply default vendor-specific MAC configuration

        // ...

        esp32_emac_config.interface = EMAC_DATA_INTERFACE_RMII;                      // alter EMAC Data Interface
        esp32_emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;               // select EMAC REF_CLK mode
        esp32_emac_config.clock_config.rmii.clock_gpio = EMAC_CLK_OUT_GPIO;          // select GPIO number used to input/output EMAC REF_CLK
        esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config); // create MAC instance


SPI-Ethernet Module
^^^^^^^^^^^^^^^^^^^

.. highlight:: c

::

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();      // apply default common MAC configuration
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();      // apply default PHY configuration
    phy_config.phy_addr = CONFIG_EXAMPLE_ETH_PHY_ADDR;           // alter the PHY address according to your board design
    phy_config.reset_gpio_num = CONFIG_EXAMPLE_ETH_PHY_RST_GPIO; // alter the GPIO used for PHY reset
    // Install GPIO interrupt service (as the SPI-Ethernet module is interrupt-driven)
    gpio_install_isr_service(0);
    // SPI bus configuration
    spi_device_handle_t spi_handle = NULL;
    spi_bus_config_t buscfg = {
        .miso_io_num = CONFIG_EXAMPLE_ETH_SPI_MISO_GPIO,
        .mosi_io_num = CONFIG_EXAMPLE_ETH_SPI_MOSI_GPIO,
        .sclk_io_num = CONFIG_EXAMPLE_ETH_SPI_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(CONFIG_EXAMPLE_ETH_SPI_HOST, &buscfg, 1));
    // Configure SPI device
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
    * When creating MAC and PHY instances for SPI-Ethernet modules (e.g., DM9051), the constructor function must have the same suffix (e.g., `esp_eth_mac_new_dm9051` and `esp_eth_phy_new_dm9051`). This is because we don not have other choices but the integrated PHY.

    * The SPI device configuration (i.e., `spi_device_interface_config_t`) may slightly differ for other Ethernet modules or to meet SPI timing on specific PCB. Please check out your module's specs and the examples in ESP-IDF.


Install Driver
--------------

To install the Ethernet driver, we need to combine the instance of MAC and PHY and set some additional high-level configurations (i.e., not specific to either MAC or PHY) in :cpp:class:`esp_eth_config_t`:

* :cpp:member:`esp_eth_config_t::mac`: instance that created from MAC generator (e.g., :cpp:func:`esp_eth_mac_new_esp32`).

* :cpp:member:`esp_eth_config_t::phy`: instance that created from PHY generator (e.g., :cpp:func:`esp_eth_phy_new_ip101`).

* :cpp:member:`esp_eth_config_t::check_link_period_ms`: Ethernet driver starts an OS timer to check the link status periodically, this field is used to set the interval, in milliseconds.

* :cpp:member:`esp_eth_config_t::stack_input`: In most Ethernet IoT applications, any Ethernet frame received by a driver should be passed to the upper layer (e.g., TCP/IP stack). This field is set to a function that is responsible to deal with the incoming frames. You can even update this field at runtime via function :cpp:func:`esp_eth_update_input_path` after driver installation.

* :cpp:member:`esp_eth_config_t::on_lowlevel_init_done` and :cpp:member:`esp_eth_config_t::on_lowlevel_deinit_done`: These two fields are used to specify the hooks which get invoked when low-level hardware has been initialized or de-initialized.

ESP-IDF provides a default configuration for driver installation in macro :c:macro:`ETH_DEFAULT_CONFIG`.

.. highlight:: c

::

    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); // apply default driver configuration
    esp_eth_handle_t eth_handle = NULL; // after the driver is installed, we will get the handle of the driver
    esp_eth_driver_install(&config, &eth_handle); // install driver

The Ethernet driver also includes an event-driven model, which sends useful and important events to user space. We need to initialize the event loop before installing the Ethernet driver. For more information about event-driven programming, please refer to :doc:`ESP Event <../system/esp_event>`.

.. highlight:: c

::

    /** Event handler for Ethernet events */
    static void eth_event_handler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data)
    {
        uint8_t mac_addr[6] = {0};
        /* we can get the ethernet driver handle from event data */
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

    esp_event_loop_create_default(); // create a default event loop that runs in the background
    esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL); // register Ethernet event handler (to deal with user-specific stuff when events like link up/down happened)

Start Ethernet Driver
---------------------

After driver installation, we can start Ethernet immediately.

.. highlight:: c

::

    esp_eth_start(eth_handle); // start Ethernet driver state machine

.. _connect-driver-to-stack:

Connect Driver to TCP/IP Stack
------------------------------

Up until now, we have installed the Ethernet driver. From the view of OSI (Open System Interconnection), we are still on level 2 (i.e., Data Link Layer). While we can detect link up and down events and gain MAC address in user space, it is infeasible to obtain the IP address, let alone send an HTTP request. The TCP/IP stack used in ESP-IDF is called LwIP. For more information about it, please refer to :doc:`LwIP <../../api-guides/lwip>`.

To connect the Ethernet driver to TCP/IP stack, follow these three steps:

1. Create a network interface for the Ethernet driver
2. Attach the network interface to the Ethernet driver
3. Register IP event handlers

For more information about the network interface, please refer to :doc:`Network Interface <esp_netif>`.

.. highlight:: c

::

    /** Event handler for IP_EVENT_ETH_GOT_IP */
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

    esp_netif_init()); // Initialize TCP/IP network interface (should be called only once in application)
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH(); // apply default network interface configuration for Ethernet
    esp_netif_t *eth_netif = esp_netif_new(&cfg); // create network interface for Ethernet driver

    esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)); // attach Ethernet driver to TCP/IP stack
    esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL); // register user defined IP event handlers
    esp_eth_start(eth_handle); // start Ethernet driver state machine

.. warning::
    It is recommended to fully initialize the Ethernet driver and network interface before registering the user's Ethernet/IP event handlers, i.e., register the event handlers as the last thing prior to starting the Ethernet driver. Such an approach ensures that Ethernet/IP events get executed first by the Ethernet driver or network interface so the system is in the expected state when executing the user's handlers.

.. _misc-operation-of-driver:

Misc Control of Ethernet Driver
-------------------------------

The following functions should only be invoked after the Ethernet driver has been installed.

* Stop Ethernet driver: :cpp:func:`esp_eth_stop`
* Update Ethernet data input path: :cpp:func:`esp_eth_update_input_path`
* Misc get/set of Ethernet driver attributes: :cpp:func:`esp_eth_ioctl`

.. highlight:: c

::

    /* get MAC address */
    uint8_t mac_addr[6];
    memset(mac_addr, 0, sizeof(mac_addr));
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
    ESP_LOGI(TAG, "Ethernet MAC Address: %02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    /* get PHY address */
    int phy_addr = -1;
    esp_eth_ioctl(eth_handle, ETH_CMD_G_PHY_ADDR, &phy_addr);
    ESP_LOGI(TAG, "Ethernet PHY Address: %d", phy_addr);

.. _flow-control:

Flow Control
------------

Ethernet on MCU usually has a limitation in the number of frames it can handle during network congestion, because of the limitation in RAM size. A sending station might be transmitting data faster than the peer end can accept it. The ethernet flow control mechanism allows the receiving node to signal the sender requesting the suspension of transmissions until the receiver catches up. The magic behind that is the pause frame, which was defined in IEEE 802.3x.

Pause frame is a special Ethernet frame used to carry the pause command, whose EtherType field is ``0x8808``, with the Control opcode set to ``0x0001``. Only stations configured for full-duplex operation may send pause frames. When a station wishes to pause the other end of a link, it sends a pause frame to the 48-bit reserved multicast address of ``01-80-C2-00-00-01``. The pause frame also includes the period of pause time being requested, in the form of a two-byte integer, ranging from ``0`` to ``65535``.

After the Ethernet driver installation, the flow control feature is disabled by default. You can enable it by:

.. highlight:: c

::

    bool flow_ctrl_enable = true;
    esp_eth_ioctl(eth_handle, ETH_CMD_S_FLOW_CTRL, &flow_ctrl_enable);

One thing that should be kept in mind is that the pause frame ability is advertised to the peer end by PHY during auto-negotiation. The Ethernet driver sends a pause frame only when both sides of the link support it.

.. -------------------------------- Examples -----------------------------------

Application Examples
--------------------

  * Ethernet basic example: :example:`ethernet/basic`
  * Ethernet iperf example: :example:`ethernet/iperf`
  * Ethernet to Wi-Fi AP "router": :example:`network/eth2ap`
  * Wi-Fi station to Ethernet "bridge": :example:`network/sta2eth`
  * Most protocol examples should also work for Ethernet: :example:`protocols`

.. ------------------------------ Advanced Topics -------------------------------

.. _advanced-topics:

Advanced Topics
---------------

Custom PHY Driver
^^^^^^^^^^^^^^^^^

There are multiple PHY manufacturers with wide portfolios of chips available. The ESP-IDF already supports several PHY chips however one can easily get to a point where none of them satisfies the user's actual needs due to price, features, stock availability, etc.

Luckily, a management interface between EMAC and PHY is standardized by IEEE 802.3 in Section 22.2.4 Management Functions. It defines provisions of the so-called "MII Management Interface" to control the PHY and gather status from the PHY. A set of management registers is defined to control chip behavior, link properties, auto-negotiation configuration, etc. This basic management functionality is addressed by :component_file:`esp_eth/src/esp_eth_phy_802_3.c` in ESP-IDF and so it makes the creation of a new custom PHY chip driver quite a simple task.

.. note::
    Always consult with PHY datasheet since some PHY chips may not comply with IEEE 802.3, Section 22.2.4. It does not mean you are not able to create a custom PHY driver, but it just requires more effort. You will have to define all PHY management functions.

The majority of PHY management functionality required by the ESP-IDF Ethernet driver is covered by the :component_file:`esp_eth/src/esp_eth_phy_802_3.c`. However, the following may require developing chip-specific management functions:

    * Link status which is almost always chip-specific
    * Chip initialization, even though not strictly required, should be customized to at least ensure that the expected chip is used
    * Chip-specific features configuration

**Steps to create a custom PHY driver:**

1. Define vendor-specific registry layout based on the PHY datasheet. See :component_file:`esp_eth/src/esp_eth_phy_ip101.c` as an example.
2. Prepare derived PHY management object info structure which:

    * must contain at least parent IEEE 802.3 :cpp:class:`phy_802_3_t` object
    * optionally contain additional variables needed to support non-IEEE 802.3 or customized functionality. See :component_file:`esp_eth/src/esp_eth_phy_ksz80xx.c` as an example.

3. Define chip-specific management call-back functions.
4. Initialize parent IEEE 802.3 object and re-assign chip-specific management call-back functions.

Once you finish the new custom PHY driver implementation, consider sharing it among other users via `IDF Component Registry <https://components.espressif.com/>`_.

.. ---------------------------- API Reference ----------------------------------

API Reference
-------------

.. include-build-file:: inc/eth_types.inc
.. include-build-file:: inc/esp_eth.inc
.. include-build-file:: inc/esp_eth_driver.inc
.. include-build-file:: inc/esp_eth_com.inc
.. include-build-file:: inc/esp_eth_mac.inc
.. include-build-file:: inc/esp_eth_phy.inc
.. include-build-file:: inc/esp_eth_phy_802_3.inc
.. include-build-file:: inc/esp_eth_netif_glue.inc
