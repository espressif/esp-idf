USB Device Stack
=================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_USB_DP_GPIO_NUM:default="20"}
{IDF_TARGET_USB_DM_GPIO_NUM:default="19"}
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

.. only:: esp32s2 or esp32s3

    The {IDF_TARGET_NAME} routes the USB D+ and D- signals to GPIOs {IDF_TARGET_USB_DP_GPIO_NUM} and {IDF_TARGET_USB_DM_GPIO_NUM} respectively. For USB device functionality, these GPIOs should be connected to the bus in some way (e.g., via a Micro-B port, USB-C port, or directly to standard-A plug).

.. only:: esp32p4

    The {IDF_TARGET_NAME} routes the USB D+ and D- signals to their dedicated pins. For USB device functionality, these pins should be connected to the bus in some way (e.g., via a Micro-B port, USB-C port, or directly to standard-A plug).

.. figure:: ../../../_static/usb-board-connection.png
    :align: center
    :alt: Connection of an USB GPIOs directly to a USB standard-A plug
    :figclass: align-center

.. only:: esp32s2 or esp32s3

    .. note::

        If you are using an {IDF_TARGET_NAME} development board with two USB ports, the port labeled "USB" will already be connected to the D+ and D- GPIOs.

.. note::

    Self-powered devices must also connect VBUS through a voltage divider or comparator. For more details, please refer to :ref:`self-powered-device`.

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
        .external_phy = false,      // Use internal USB PHY
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


Application Examples
--------------------

The table below describes the code examples available in the directory :example:`peripherals/usb/device`:

.. list-table::
   :widths: 35 65
   :header-rows: 1

   * - Code Example
     - Description
   * - :example:`peripherals/usb/device/tusb_console`
     - How to set up {IDF_TARGET_NAME} chip to get log output via Serial Device connection
   * - :example:`peripherals/usb/device/tusb_serial_device`
     - How to set up {IDF_TARGET_NAME} chip to work as a USB Serial Device
   * - :example:`peripherals/usb/device/tusb_midi`
     - How to set up {IDF_TARGET_NAME} chip to work as a USB MIDI Device
   * - :example:`peripherals/usb/device/tusb_hid`
     - How to set up {IDF_TARGET_NAME} chip to work as a USB Human Interface Device
   * - :example:`peripherals/usb/device/tusb_msc`
     - How to set up {IDF_TARGET_NAME} chip to work as a USB Mass Storage Device
   * - :example:`peripherals/usb/device/tusb_composite_msc_serialdevice`
     - How to set up {IDF_TARGET_NAME} chip to work as a Composite USB Device (MSC + CDC)
