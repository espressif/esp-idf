USB Device Stack
=================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_USB_DP_GPIO_NUM:default="20", esp32h4="22"}
{IDF_TARGET_USB_DM_GPIO_NUM:default="19", esp32h4="21"}
{IDF_TARGET_USB_EP_NUM: default="6", esp32p4="15"}
{IDF_TARGET_USB_EP_NUM_INOUT:default="5", esp32p4="8"}
{IDF_TARGET_USB_EP_NUM_IN:default="1", esp32p4="7"}

Overview
--------

The ESP-IDF USB Device Stack (hereinafter referred to as the Device Stack) enables USB Device support on {IDF_TARGET_NAME}. By using the Device Stack, {IDF_TARGET_NAME} can be programmed with any well defined USB device functions (e.g., keyboard, mouse, camera), a custom function (aka vendor-specific class), or a combination of those functions (aka a composite device).

The Device Stack is built around the TinyUSB stack, but extends TinyUSB with some minor features and modifications for better integration with ESP-IDF. The Device stack is distributed as a managed component via the `ESP Component Registry <https://components.espressif.com/components/espressif/esp_tinyusb>`__.

Features
--------

- Multiple supported device classes (CDC, HID, MIDI, MSC)
- Composite devices
- Vendor specific classes
- Maximum of {IDF_TARGET_USB_EP_NUM} endpoints

    - {IDF_TARGET_USB_EP_NUM_INOUT} IN/OUT endpoints
    - {IDF_TARGET_USB_EP_NUM_IN} IN endpoints

- VBUS monitoring for self-powered devices

.. Todo: Refactor USB hardware connect into a separate guide

Hardware Connection
-------------------

.. only:: esp32s2 or esp32s3 or esp32h4

    The {IDF_TARGET_NAME} routes the USB D+ and D- signals to GPIOs {IDF_TARGET_USB_DP_GPIO_NUM} and {IDF_TARGET_USB_DM_GPIO_NUM} respectively. For USB device functionality, these GPIOs should be connected to the bus in some way (e.g., via a Micro-B port, USB-C port, or directly to standard-A plug).

.. only:: esp32p4

    The {IDF_TARGET_NAME} routes the USB D+ and D- signals to their dedicated pins. For USB device functionality, these pins should be connected to the bus in some way (e.g., via a Micro-B port, USB-C port, or directly to standard-A plug).

.. figure:: ../../../_static/usb-board-connection.png
    :align: center
    :alt: Connection of an USB GPIOs directly to a USB standard-A plug
    :figclass: align-center

.. only:: esp32s2 or esp32s3 or esp32h4

    .. note::

        If you are using an {IDF_TARGET_NAME} development board with two USB ports, the port labeled "USB" will already be connected to the D+ and D- GPIOs.

.. note::

    Self-powered devices must also connect VBUS through a voltage divider or comparator. For more details, please refer to :ref:`self-powered-device`.

.. only:: esp32s3

    External PHY Configuration
    --------------------------

    The {IDF_TARGET_NAME} contains two USB controllers: USB-OTG and USB-Serial-JTAG. However, both controllers share a **single PHY**, which means only one can operate at a time. To use USB Device functionality while the USB-Serial-JTAG is active (e.g., for debugging or flashing), an **external PHY** is required, since the PHY is used by USB-Serial-JTAG.

    .. note::
        An external PHY is not the only way to enable debugging alongside USB Host or Device functionality. It is also possible to switch the debugging interface from USB-Serial-JTAG to plain JTAG by burning the appropriate eFuses. For details, refer to document :doc:`JTAG Debugging <../../api-guides/jtag-debugging/index>` in ESP-IDF Programming Guide for your target.

    {IDF_TARGET_NAME} supports connecting external PHY ICs. This becomes especially relevant when full-speed USB device functionality is needed while the USB-Serial-JTAG controller is also in use. Various external PHY ICs may require different hardware modifications. Please refer to each IC's datasheet for specifics. A general connection diagram below is provided for reference. For more information, please refer to `Use an external PHY <https://docs.espressif.com/projects/esp-iot-solution/en/latest/usb/usb_overview/usb_phy.html#use-an-external-phy>`__.

    .. figure:: ../../../_static/usb_device/usb_fs_phy_sp5301.png
       :align: center
       :alt: usb_fs_phy_sp5301

       A typical circuit diagram for an external PHY

    **List of Tested External PHY ICs:**

    - **SP5301** — Directly supported by {IDF_TARGET_NAME}. See the guide above for schematic and routing information.
    - **TUSB1106** — Directly supported by {IDF_TARGET_NAME}. Works with the external-PHY driver via GPIO mapping. Follow the reference wiring in the TUSB1106 datasheet (power-supply options and recommended series resistors on D+/D–).
    - **STUSB03E** — Requires signal routing using an analog switch. See example below.

    .. figure:: ../../../_static/usb_device/ext_phy_schematic_stusb03e.png
       :align: center
       :alt: External PHY with Analog Switch Schematic (Device mode)

       Example connection using STUSB03E and analog switch (Device mode)

    .. note::
        This schematic is a minimal example intended only to demonstrate the external PHY connection. It omits other essential components and signals (e.g., VCC, GND, RESET) required for a complete, functional {IDF_TARGET_NAME} design.
        The schematic includes both a +5 V rail (usually from USB VBUS) and a VCC rail. VCC should match the chip supply voltage (usually 3.3 V). Ensure that the external PHY and the chip are powered from the same voltage domain. If designing a self-powered USB device, connect VBUSDET signal from the external PHY to {IDF_TARGET_NAME} for mandatory VBUS monitoring.

    Hardware configuration is handled via GPIO mapping to the PHY's pins. Any unused pins (e.g., :cpp:member:`usb_phy_ext_io_conf_t::suspend_n_io_num`, :cpp:member:`usb_phy_ext_io_conf_t::fs_edge_sel_io_num`) **must be set to -1**.

    .. note::
        The :cpp:member:`usb_phy_ext_io_conf_t::suspend_n_io_num` pin is **currently not supported** and does not need to be connected.
        The :cpp:member:`usb_phy_ext_io_conf_t::fs_edge_sel_io_num` pin is optional and only required if switching between low-speed and full-speed modes is needed.

    Starting from version 2.0, the ESP TinyUSB Device Stack supports external PHY usage. To use an external PHY in device mode:

    1. Configure the GPIO mapping and PHY using :cpp:type:`usb_phy_config_t`.
    2. Create the PHY using :cpp:func:`usb_new_phy()`.
    3. Use :cpp:func:`TINYUSB_DEFAULT_CONFIG()` to initialize :cpp:type:`tinyusb_config_t`.
    4. Set the `phy.skip_setup` field of :cpp:type:`tinyusb_config_t` to ``true`` to bypass PHY reinitialization and use the externally configured PHY.

    **Example Code:**

    .. code-block:: c

        // GPIO configuration for external PHY
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

        // Configuration and initialization of external PHY for OTG controller (Device mode)
        const usb_phy_config_t phy_config = {
            .controller = USB_PHY_CTRL_OTG,
            .target = USB_PHY_TARGET_EXT,
            .otg_mode = USB_OTG_MODE_DEVICE,
            .otg_speed = USB_PHY_SPEED_FULL,
            .ext_io_conf = &ext_io_conf
        };

        usb_phy_handle_t phy_hdl;
        ESP_ERROR_CHECK(usb_new_phy(&phy_config, &phy_hdl));

        // Initialize TinyUSB with default configuration (event handler can be set if needed)
        tinyusb_config_t config = TINYUSB_DEFAULT_CONFIG();
        config.phy.skip_setup = true;

        tinyusb_driver_install(&config);

    This setup ensures that the USB Device stack uses the **external PHY** instead of attempting to configure the internal one.

Device Stack Structure
----------------------

The basis of the Device Stack is TinyUSB, where the Device Stack implements the following features on top of TinyUSB:

- Customization of USB descriptors
- Serial device support
- Redirecting of standard streams through the Serial device
- Storage Media (SPI-Flash and SD-Card) for USB Device MSC Class.
- A task within the encapsulated device stack that handles TinyUSB servicing

Component Dependency
^^^^^^^^^^^^^^^^^^^^

The Device Stack is distributed via the `ESP Component Registry <https://components.espressif.com/components/espressif/esp_tinyusb>`__. Thus, to use it, please add the Device Stack component as dependency using the following command:

.. code:: bash

    idf.py add-dependency esp_tinyusb

Configuration Options
^^^^^^^^^^^^^^^^^^^^^

Multiple aspects of the Device Stack can be configured using menuconfig. These include:

- The verbosity of the TinyUSB's log
- Device Stack task related options
- Default device/string descriptor options
- Class specific options

.. _descriptors-configuration:

Descriptor Configuration
^^^^^^^^^^^^^^^^^^^^^^^^

The :cpp:type:`tinyusb_config_t` structure provides USB descriptor related fields that should be initialized.

The following descriptors should be initialized for both full-speed and high-speed devices:

- :cpp:member:`device_descriptor`
- :cpp:member:`string_descriptor`

Full-speed devices should initialize the following field to provide their configuration descriptor:

- :cpp:member:`configuration_descriptor`

.. only:: esp32p4

    High-speed devices should initialize the following fields to provide configuration descriptors at each speed:

    - :cpp:member:`fs_configuration_descriptor`
    - :cpp:member:`hs_configuration_descriptor`
    - :cpp:member:`qualifier_descriptor`

    .. note::

        Both :cpp:member:`fs_configuration_descriptor` and :cpp:member:`hs_configuration_descriptor` must be present to comply with USB 2.0 specification.

The Device Stack will instantiate a USB device based on the descriptors provided in the fields described above when :cpp:func:`tinyusb_driver_install` is called.

The Device Stack also provides default descriptors that can be installed by setting the corresponding field in :cpp:func:`tinyusb_driver_install` to ``NULL``. Default descriptors include:

- Default device descriptor: Enabled by setting :cpp:member:`device_descriptor` to ``NULL``. Default device descriptor will use the values set by the corresponding menuconfig options (e.g., PID, VID, bcdDevice etc).
- Default string descriptor: Enabled by setting :cpp:member:`string_descriptor` to ``NULL``. Default string descriptors will use the value set by corresponding menuconfig options (e.g., manufacturer, product, and serial string descriptor options).
- Default configuration descriptor. Some classes that rarely require custom configuration (such as CDC and MSC) will provide default configuration descriptors. These can be enabled by setting associated configuration descriptor field to ``NULL``:

    - :cpp:member:`configuration_descriptor`: full-speed descriptor for full-speed devices only
    - :cpp:member:`fs_configuration_descriptor`: full-speed descriptor for high-speed devices
    - :cpp:member:`hs_configuration_descriptor`: high-speed descriptor for high-speed devices

.. note::

    For backward compatibility, when Device Stack supports high-speed, the field :cpp:member:`configuration_descriptor` could be used instead of :cpp:member:`fs_configuration_descriptor` for full-speed configuration descriptor.

Installation
------------

To install the Device Stack, please call :cpp:func:`tinyusb_driver_install`. The Device Stack's configuration is specified in a :cpp:type:`tinyusb_config_t` structure that is passed as an argument to :cpp:func:`tinyusb_driver_install`.

.. note::

    The :cpp:type:`tinyusb_config_t` structure can be zero-initialized (e.g., ``const tinyusb_config_t tusb_cfg = { 0 };``) or partially (as shown below). For any member that is initialized to ``0`` or ``NULL``, the stack uses its default configuration values for that member, see example below.

.. code-block:: c

    const tinyusb_config_t partial_init = {
        .device_descriptor = NULL,  // Use the default device descriptor specified in Menuconfig
        .string_descriptor = NULL,  // Use the default string descriptors specified in Menuconfig
        .external_phy = false,      // Use internal PHY
    #if (TUD_OPT_HIGH_SPEED)
        .fs_configuration_descriptor = NULL, // Use the default full-speed configuration descriptor according to settings in Menuconfig
        .hs_configuration_descriptor = NULL, // Use the default high-speed configuration descriptor according to settings in Menuconfig
        .qualifier_descriptor = NULL,  // Use the default qualifier descriptor, with values from default device descriptor
    #else
        .configuration_descriptor = NULL,   // Use the default configuration descriptor according to settings in Menuconfig
    #endif // TUD_OPT_HIGH_SPEED

    };

.. _self-powered-device:

Self-Powered Device
-------------------

USB specification mandates self-powered devices to monitor voltage levels on USB's VBUS signal. As opposed to bus-powered devices, a self-powered device can be fully functional even without a USB connection. The self-powered device detects connection and disconnection events by monitoring the VBUS voltage level. VBUS is considered valid if it rises above 4.75 V and invalid if it falls below 4.35 V.

On the {IDF_TARGET_NAME}, this will require using a GPIO to act as a voltage sensing pin to detect when VBUS goes above/below the prescribed thresholds. However, {IDF_TARGET_NAME} pins are 3.3 V tolerant. Thus, even if VBUS rises/falls above/below the thresholds mentioned above, it would still appear as a logic HIGH to the {IDF_TARGET_NAME}. Thus, in order to detect the VBUS valid condition, users can do one of the following:

- Connect VBUS to a voltage comparator chip/circuit that detects the thresholds described above (i.e., 4.35 V and 4.75 V), and outputs a 3.3 V logic level to the {IDF_TARGET_NAME} indicating whether VBUS is valid or not.
- Use a resistor voltage divider that outputs (0.75 x Vdd) if VBUS is 4.4 V (see figure below).

.. note::

    In either case, the voltage on the sensing pin must be logic low within 3 ms after the device is unplugged from the USB host.

.. figure:: ../../../_static/diagrams/usb/usb_vbus_voltage_monitor.png
    :align: center
    :alt: Simple voltage divider for VBUS monitoring
    :figclass: align-center

    Simple voltage divider for VBUS monitoring

To use this feature, in :cpp:type:`tinyusb_config_t`, you must set :cpp:member:`self_powered` to ``true`` and :cpp:member:`vbus_monitor_io` to GPIO number that is used for VBUS monitoring.

USB Serial Device (CDC-ACM)
---------------------------

If the CDC option is enabled in Menuconfig, the USB Serial Device can be initialized with :cpp:func:`tusb_cdc_acm_init` according to the settings from :cpp:type:`tinyusb_config_cdcacm_t`, see example below.

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

To specify callbacks, you can either set the pointer to your :cpp:type:`tusb_cdcacm_callback_t` function in the configuration structure or call :cpp:func:`tinyusb_cdcacm_register_callback` after initialization.

USB Serial Console
^^^^^^^^^^^^^^^^^^

The USB Serial Device allows the redirection of all standard input/output streams (stdin, stdout, stderr) to USB. Thus, calling standard library input/output functions such as ``printf()`` will result into the data being sent/received over USB instead of UART.

Users should call :cpp:func:`esp_tusb_init_console` to switch the standard input/output streams to USB, and :cpp:func:`esp_tusb_deinit_console` to switch them back to UART.

USB Mass Storage Device (MSC)
-----------------------------

If the MSC ``CONFIG_TINYUSB_MSC_ENABLED`` option is enabled in Menuconfig, the ESP Chip can be used as USB MSC Device. The storage media (SPI-Flash or SD-Card) can be initialized as shown below.

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


- SD-Card

.. code-block:: c

    static esp_err_t storage_init_sdmmc(sdmmc_card_t **card)
    {
        ***
        sdmmc_host_t host = SDMMC_HOST_DEFAULT();
        sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
        // For SD Card, set bus width to use

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

MSC Performance Optimization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Single-Buffer Approach**

The single-buffer approach improves performance by using a dedicated buffer to temporarily store incoming write data instead of processing it immediately in the callback.

- **Configurable buffer size**: The buffer size is set via ``CONFIG_TINYUSB_MSC_BUFSIZE``, allowing users to balance performance and memory usage.

This approach ensures that USB transactions remain fast while avoiding potential delays caused by storage operations.

**USB MSC Drive Performance**

.. only:: esp32s3

    .. list-table::
        :header-rows: 1
        :widths: 20 20 20

        * - FIFO Size
          - Read Speed
          - Write Speed

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

        * - FIFO Size
          - Read Speed
          - Write Speed

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

        SD card support is not available for {IDF_TARGET_NAME} in MSC device mode.

    **SPI Flash Performance:**

    .. list-table::
        :header-rows: 1
        :widths: 20 20

        * - FIFO Size
          - Write Speed

        * - 512 B
          - 5.59 KB/s

        * - 8192 B
          - 21.54 KB/s

.. only:: esp32h4

    .. note::

        SD card support is not available for {IDF_TARGET_NAME} in MSC device mode.

    **SPI Flash Performance:**

    .. list-table::
        :header-rows: 1
        :widths: 20 20

        * - FIFO Size
          - Write Speed

        * - 512 B
          - 4.48 KB/s

        * - 8192 B
          - 22.33 KB/s

Performance Limitations:

- **Internal SPI Flash performance** is constrained by architectural limitations where program execution and storage access share the same flash chip. This results in program execution being **suspended during flash writes**, significantly impacting performance.
- **Internal SPI Flash usage is intended primarily for demonstration purposes.** For practical use cases requiring higher performance, it is recommended to use **external storage such as an SD card or an external SPI flash chip, where supported.**

.. only:: esp32s3 or esp32p4

    SD cards are not affected by this constraint, explaining their higher performance gains.

Application Examples
--------------------

The examples can be found in the directory :example:`peripherals/usb/device`.

- :example:`peripherals/usb/device/tusb_console` demonstrates how to set up {IDF_TARGET_NAME} to get log output via a Serial Device connection using the TinyUSB component, applicable for any Espressif boards that support USB-OTG.
- :example:`peripherals/usb/device/tusb_serial_device` demonstrates how to set up {IDF_TARGET_NAME} to function as a USB Serial Device using the TinyUSB component, with the ability to be configured as a double serial device.
- :example:`peripherals/usb/device/tusb_midi` demonstrates how to set up {IDF_TARGET_NAME} to function as a USB MIDI Device, outputting a MIDI note sequence via the native USB port using the TinyUSB component.
- :example:`peripherals/usb/device/tusb_hid` demonstrates how to implement a USB keyboard and mouse using the TinyUSB component, which sends 'key a/A pressed & released' events and moves the mouse in a square trajectory upon connection to a USB host.
- :example:`peripherals/usb/device/tusb_msc` demonstrates how to use the USB capabilities to create a Mass Storage Device that can be recognized by USB-hosts, allowing access to its internal data storage, with support for SPI Flash and SD MMC Card storage media.
- :example:`peripherals/usb/device/tusb_composite_msc_serialdevice` demonstrates how to set up {IDF_TARGET_NAME} to function simultaneously as both a USB Serial Device and an MSC device (SPI-Flash as the storage media) using the TinyUSB component.

.. only:: not esp32p4 and not esp32h4

  - :example:`peripherals/usb/device/tusb_ncm` demonstrates how to transmit Wi-Fi data to a Linux or Windows host via USB using the Network Control Model (NCM), a sub-class of Communication Device Class (CDC) USB Device for Ethernet-over-USB applications, with the help of a TinyUSB component.
