
USB Device Driver
=================

{IDF_TARGET_USB_DP_GPIO_NUM:default="20"}
{IDF_TARGET_USB_DM_GPIO_NUM:default="19"}
{IDF_TARGET_USB_EP_NUM:default="6"}
{IDF_TARGET_USB_EP_NUM_INOUT:default="5"}
{IDF_TARGET_USB_EP_NUM_IN:default="1"}

Overview
--------

The driver allows you to use {IDF_TARGET_NAME} chips to develop USB devices on a top of TinyUSB stack. TinyUSB is integrated with ESP-IDF to provide USB features of the framework. Using this driver the chip works as simple or composite device supporting several USB devices simultaneously.

TinyUSB stack is distributed via `IDF Component Registry <https://components.espressif.com/components/espressif/esp_tinyusb>`_.

Our USB-OTG implementation is limited to {IDF_TARGET_USB_EP_NUM} USB endpoints ({IDF_TARGET_USB_EP_NUM_INOUT} IN/OUT endpoints and {IDF_TARGET_USB_EP_NUM_IN} IN endpoint) . Please note that enabling Secure Boot or flash encryption disables the USB-OTG USB stack in the ROM, disallowing updates via the serial emulation or Device Firmware Update (DFU) on that port. For more details, please refer to `technical reference manual <{IDF_TARGET_TRM_EN_URL}>`_.

Features
--------

- Configuration of device and string USB descriptors
- USB Serial Device (CDC-ACM)
- Input and output streams through USB Serial Device
- Other USB classes (MIDI, MSC, HID...) support directly via TinyUSB
- VBUS monitoring for self-powered devices

Hardware USB Connection
-----------------------

- Any board with the {IDF_TARGET_NAME} chip with USB connectors or with exposed USB's D+ and D- (DATA+/DATA-) pins.

If the board has no USB connector but has the pins, connect pins directly to the host (e.g. with do-it-yourself cable from any USB connection cable).

On {IDF_TARGET_NAME}, connect GPIO {IDF_TARGET_USB_DP_GPIO_NUM} and {IDF_TARGET_USB_DM_GPIO_NUM} to D+/D- respectively:


.. figure:: ../../../_static/usb-board-connection.png
    :align: center
    :alt: Connection of an ESP board to a USB host
    :figclass: align-center

Self-powered devices must also connect VBUS through voltage divider or comparator, more details in :ref:`self-powered-device` subchapter.

Driver Structure
----------------

As the basis is used the TinyUSB stack.

On top of it the driver implements:

- Customization of USB descriptors
- Serial device support
- Redirecting of standard streams through the Serial device
- Encapsulated driver's task servicing the TinyUSB

Configuration
-------------

Via Menuconfig options you can specify:

- Several of descriptor's parameters (see: Descriptors Configuration bellow)
- USB Serial low-level Configuration
- The verbosity of the TinyUSB's log
- Disable the TinyUSB main task (for the custom implementation)

Descriptors Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^

The driver's descriptors are provided by :cpp:type:`tinyusb_config_t` structure's :cpp:member:`device_descriptor`, :cpp:member:`configuration_descriptor` and :cpp:member:`string_descriptor` members. Therefore, you should initialize :cpp:type:`tinyusb_config_t` with your desired descriptors before calling :cpp:func:`tinyusb_driver_install` to install the driver.

However, the driver also provides default descriptors. You can install the driver with default device and string descriptors by setting the :cpp:member:`device_descriptor` and :cpp:member:`string_descriptor` members of :cpp:type:`tinyusb_config_t` to `NULL` before calling :cpp:func:`tinyusb_driver_install`. To lower your development effort we also provide default configuration descriptor for CDC and MSC class, as these classes rarely require custom configuration. The driver's default device descriptor is specified using Menuconfig, where the following fields should be configured:

- PID
- VID
- bcdDevice
- Manufacturer
- Product name
- Name of CDC or MSC device if it is On
- Serial number

If you want to use your own descriptors with extended modification, you can define them during the driver installation process.

Install Driver
--------------

To initialize the driver, users should call :cpp:func:`tinyusb_driver_install`. The driver's configuration is specified in a :cpp:type:`tinyusb_config_t` structure that is passed as an argument to :cpp:func:`tinyusb_driver_install`.

 Note that the :cpp:type:`tinyusb_config_t` structure can be zero initialized (e.g. ``const tinyusb_config_t tusb_cfg = { 0 };``) or partially (as shown below). For any member that is initialized to `0` or `NULL`, the driver will use its default configuration values for that member (see example below)

.. code-block:: c

    const tinyusb_config_t partial_init = {
        .device_descriptor = NULL,  // Use default device descriptor specified in Menuconfig
        .string_descriptor = NULL,  // Use default string descriptors specified in Menuconfig
        .external_phy = false,      // Use internal USB PHY
        .configuration_descriptor = NULL, // Use default configuration descriptor according to settings in Menuconfig
    };

.. _self-powered-device:

Self-Powered Device
-------------------

USB specification mandates self-powered devices to monitor voltage level on USB's VBUS signal. As opposed to bus-powered devices, a self-powered device can be fully functional even without USB connection. The self-powered device detects connection and disconnection events by monitoring the VBUS voltage level. VBUS is considered valid if it rises above 4.75V and invalid if it falls below 4.35V.

No {IDF_TARGET_NAME} pin is 5V tolerant, so you must connect the VBUS to {IDF_TARGET_NAME} via a comparator with voltage thresholds as described above, or use a simple resistor voltage divider that will output (0.75 x Vdd) if VBUS is 4.4V (see figure below). In both cases, voltage on the sensing pin must be logic low within 3ms after the device is unplugged from USB host.

.. figure:: ../../../_static/diagrams/usb/usb_vbus_voltage_monitor.png
    :align: center
    :alt: Simple voltage divider for VBUS monitoring
    :figclass: align-center

    Simple voltage divider for VBUS monitoring

To use this feature, in :cpp:type:`tinyusb_config_t` you must set :cpp:member:`self_powered` to ``true`` and :cpp:member:`vbus_monitor_io` to GPIO number that will be used for VBUS monitoring.

USB Serial Device (CDC-ACM)
---------------------------

If the CDC option is enabled in Menuconfig, the USB Serial Device can be initialized with :cpp:func:`tusb_cdc_acm_init` according to the settings from :cpp:type:`tinyusb_config_cdcacm_t` (see example below).

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

To specify callbacks you can either set the pointer to your :cpp:type:`tusb_cdcacm_callback_t` function in the configuration structure or call :cpp:func:`tinyusb_cdcacm_register_callback` after initialization.

USB Serial Console
^^^^^^^^^^^^^^^^^^

The driver allows to redirect all standard application streams (stdinm stdout, stderr) to the USB Serial Device and return them to UART using :cpp:func:`esp_tusb_init_console`/:cpp:func:`esp_tusb_deinit_console` functions.

USB Mass Storage Device (MSC)
-----------------------------

If the MSC CONFIG_TINYUSB_MSC_ENABLED option is enabled, the USB MSC Device can be initialized as shown below (see example below).

.. code-block:: c

    static uint8_t const desc_configuration[] = {
        // Config number, interface count, string index, total length, attribute, power in mA
        TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

        // Interface number, string index, EP Out & EP In address, EP size
        TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EDPT_MSC_OUT, EDPT_MSC_IN, TUD_OPT_HIGH_SPEED ? 512 : 64),
    };

    static tusb_desc_device_t descriptor_config = {
        .bLength = sizeof(descriptor_config),
        .bDescriptorType = TUSB_DESC_DEVICE,
        .bcdUSB = 0x0200,
        .bDeviceClass = TUSB_CLASS_MISC,
        .bDeviceSubClass = MISC_SUBCLASS_COMMON,
        .bDeviceProtocol = MISC_PROTOCOL_IAD,
        .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
        .idVendor = 0x303A,
        .idProduct = 0x4002,
        .bcdDevice = 0x100,
        .iManufacturer = 0x01,
        .iProduct = 0x02,
        .iSerialNumber = 0x03,
        .bNumConfigurations = 0x01
    };

    static char const *string_desc_arr[] = {
        (const char[]) { 0x09, 0x04 },  // 0: is supported language is English (0x0409)
        "TinyUSB",                      // 1: Manufacturer
        "TinyUSB Device",               // 2: Product
        "123456",                       // 3: Serials
        "Example MSC",                  // 4. MSC
    };

    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = &descriptor_config,
        .string_descriptor = string_desc_arr,
        .external_phy = false,
        .configuration_descriptor = desc_configuration,
    };
    tinyusb_driver_install(&tusb_cfg);

The mandatory callbacks that are required to be implemented are

.. code-block:: c

    void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
    bool tud_msc_test_unit_ready_cb(uint8_t lun)
    void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size)
    bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
    int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
    int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
    int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize)

Application Examples
--------------------

The table below describes the code examples available in the directory :example:`peripherals/usb/`.

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
