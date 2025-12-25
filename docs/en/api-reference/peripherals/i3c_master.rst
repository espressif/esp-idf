I3C master interface
====================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_I3C_INTERNAL_PULLUP_PIN:default="Not updated!", esp32p4="GPIO32/GPIO33"}

This document introduces the I3C master driver functionality of ESP-IDF. The table of contents is as follows:

.. contents::
    :local:
    :depth: 2

Overview
========

I3C is a serial synchronous half-duplex communication protocol and an enhanced version of the I2C protocol. While maintaining most compatibility with I2C, I3C provides higher speed, lower power consumption, and richer features.

For hardware-related information about I3C, please refer to the I3C Technical Reference Manual.

The main features of the I3C protocol include:

- **Backward compatibility with I2C**: I3C bus can support both I2C and I3C devices simultaneously
- **Higher speed**: I3C can reach up to 12.5 MHz, while I2C can reach up to 1 MHz
- **Static address assignment**: Manually assign dynamic addresses based on static addresses through the SETDASA procedure
- **Dynamic address assignment**: Automatically assign dynamic addresses through the ENTDAA procedure to avoid address conflicts
- **In-band interrupt (IBI)**: Supports slave devices sending interrupt requests through the I3C bus without additional interrupt lines
- **Common Command Code (CCC)**: Supports broadcast and direct CCC commands for bus management and device configuration

.. important::

    1. When using with I2C devices, ensure that I2C devices mounted on the I3C bus **must not** support or enable clock stretching, otherwise when I2C slaves stretch the clock, it will cause the hardware state machine to hang.
    2. The I3C frequency depends on circuit design and timing adjustment. Please refer to the I3C device manual you are using.
    3. Some I3C slave devices have strict timing requirements for their acknowledgment mechanism (ACK/NACK). Please refer to the I3C slave device manual you are using.

Quick Start
===========

This section will quickly guide you through using the I3C master driver. It demonstrates how to create a bus, add devices, and perform data transfers. The general usage flow is as follows:

.. blockdiag::
    :scale: 100%
    :caption: General usage flow of I3C driver (click image to view full size)
    :align: center

    blockdiag {
        default_fontsize = 14;
        node_width = 250;
        node_height = 80;
        class emphasis [color = pink, style = dashed];

        create_bus [label="Create I3C Bus\n(i3c_new_master_bus)"];
        add_device [label="Add Device\n(add_i2c_device / \nadd_i3c_static_device / \nscan_devices_by_entdaa)"];
        transfer [label="Data Transfer\n(transmit / receive / \ntransmit_receive)", class="emphasis"];
        cleanup [label="Remove Device and Bus\n(rm_device / del_master_bus)"];

        create_bus -> add_device -> transfer -> cleanup;
    }

Create I3C Bus
--------------

The I3C master bus is represented by :cpp:type:`i3c_master_bus_handle_t` in the driver. The driver internally maintains a resource pool that can manage multiple buses and allocates free bus ports when requested.

.. figure:: ../../../_static/diagrams/i3c/i3c_bus_structure.svg
    :align: center
    :alt: I3C Bus Structure

    I3C Bus Structure

When creating an I3C bus instance, we need to configure GPIO pins, clock source, frequency, and other parameters through :cpp:type:`i3c_master_bus_config_t`. These parameters will determine how the bus operates. The following code shows how to create a basic I3C bus:

.. code:: c

    #include "driver/i3c_master.h"

    i3c_master_bus_config_t i3c_mst_config = {
        .sda_io_num = I3C_MASTER_SDA_IO,                    // GPIO number for SDA signal line
        .scl_io_num = I3C_MASTER_SCL_IO,                    // GPIO number for SCL signal line
        .i3c_scl_freq_hz_od = 600 * 1000,                   // SCL clock frequency in Open-Drain mode, please refer to device manual for appropriate values
        .i3c_scl_freq_hz_pp = 2 * 1000 * 1000,              // SCL clock frequency in Push-Pull mode, please refer to device manual for appropriate values
        .i3c_sda_od_hold_time_ns = 25,                      // Hold time of SDA after SCL falling edge in Open-Drain mode (nanoseconds), recommended to set to 25, please refer to device manual for appropriate values
        .i3c_sda_pp_hold_time_ns = 0,                       // Hold time of SDA after SCL falling edge in Push-Pull mode (nanoseconds), default is 0, please refer to device manual for appropriate values
        .entdaa_device_num = 0,                             // Maximum number of devices allowed to be dynamically discovered through ENTDAA, range from [0x0, 0x7F], 0x0 means dynamic device discovery is not used
    };

    i3c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

.. note::

    The I3C protocol requires automatic switching between open-drain and push-pull modes during each transfer between the addressing phase and data transfer phase. On ESP32-P4, only {IDF_TARGET_I3C_INTERNAL_PULLUP_PIN} support automatic opening/closing of internal pull-up switches and support user adjustment of internal pull-up resistance values. When using other GPIOs, the internal pull-up may be insufficient, and it is recommended to add external pull-up resistors. However, in push-pull mode, this pull-up cannot be canceled, which may increase additional power consumption.

Add and Drive Legacy I2C Devices
---------------------------------

.. figure:: ../../../_static/diagrams/i3c/i3c_i2c_write.svg
    :align: center
    :alt: Write to legacy I2C device

    Write to legacy I2C device

.. figure:: ../../../_static/diagrams/i3c/i3c_i2c_read.svg
    :align: center
    :alt: Read from legacy I2C device

    Read from legacy I2C device

The I3C bus supports compatibility with legacy I2C devices. If you need to connect a legacy I2C device (such as EEPROM, sensors, etc.) to the I3C bus, please note that I2C slaves **must not** perform clock stretching during I3C communication. The specific process can be done as follows:

.. code:: c

    // 1. Create I3C bus (refer to the code above)
    i3c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    // 2. Add I2C device
    i3c_device_i2c_config_t i2c_dev_cfg = {
        .device_address = 0x50,        // 7-bit address of I2C device
        .scl_freq_hz = 100 * 1000,     // Clock frequency of I2C device (100 kHz)
    };
    i3c_master_i2c_device_handle_t i2c_dev_handle;
    ESP_ERROR_CHECK(i3c_master_bus_add_i2c_device(bus_handle, &i2c_dev_cfg, &i2c_dev_handle));

    // 3. Write data to I2C device
    uint8_t write_data[10] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
    ESP_ERROR_CHECK(i3c_master_i2c_device_transmit(i2c_dev_handle, write_data, sizeof(write_data), -1)); // -1 means infinite timeout

    // 4. Read data from I2C device
    uint8_t read_data[10] = {0};
    ESP_ERROR_CHECK(i3c_master_i2c_device_receive(i2c_dev_handle, read_data, sizeof(read_data), -1));

    // 5. Write-read combined transaction (write register address first, then read data, no STOP in between)
    uint8_t reg_addr = 0x00;
    uint8_t read_buffer[5] = {0};
    ESP_ERROR_CHECK(i3c_master_i2c_device_transmit_receive(i2c_dev_handle, &reg_addr, 1, read_buffer, sizeof(read_buffer), -1));

    // 6. Clean up resources
    ESP_ERROR_CHECK(i3c_master_bus_rm_i2c_device(i2c_dev_handle));
    ESP_ERROR_CHECK(i3c_del_master_bus(bus_handle));

In this scenario, we:

1. Created an I3C bus instance through :cpp:func:`i3c_new_master_bus`
2. Added an I2C device through :cpp:func:`i3c_master_bus_add_i2c_device`, which requires specifying the device's static address and clock frequency
3. Used :cpp:func:`i3c_master_i2c_device_transmit` to write data. By default, it works in blocking mode. For non-blocking mode, please refer to :ref:`dma-support`. The same applies to other transfer functions.
4. Used :cpp:func:`i3c_master_i2c_device_receive` to read data
5. Used :cpp:func:`i3c_master_i2c_device_transmit_receive` to execute write-read combined transactions (commonly used to write register address first, then read data, with no STOP bit in between)
6. Finally cleaned up resources

Add and Drive I3C Devices via SETDASA
--------------------------------------

For specific behaviors that may occur during I3C transfers, please refer to the standard I3C protocol. The following diagram is used to briefly explain the behavior in I3C transfers to understand the I3C transfer diagrams in this document:

.. figure:: ../../../_static/diagrams/i3c/i3c_icon.svg
    :align: center
    :alt: I3C Transfer Legend

    I3C Transfer Legend

If you know the static address of an I3C device, you can add the device using the SETDASA method:

.. figure:: ../../../_static/diagrams/i3c/i3c_setdasa.svg
    :align: center
    :alt: I3C Directed Dynamic Address Assignment

    I3C Directed Dynamic Address Assignment

.. code:: c

    // 1. Create I3C bus
    i3c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    // 2. Add I3C device (using SETDASA)
    i3c_device_i3c_config_t i3c_dev_cfg = {
        .dynamic_addr = 0x08,          // Dynamic address assigned to the device, can be any value except reserved addresses in the I3C protocol, or can be obtained through `i3c_master_get_valid_address_slot` to get an available dynamic address
        .static_addr = 0x74,            // Static address of the device (obtained from device manual)
    };
    i3c_master_i3c_device_handle_t i3c_dev_handle;
    ESP_ERROR_CHECK(i3c_master_bus_add_i3c_static_device(bus_handle, &i3c_dev_cfg, &i3c_dev_handle));

    // 3. Write data to I3C device
    uint8_t write_data[100] = {0};
    ESP_ERROR_CHECK(i3c_master_i3c_device_transmit(i3c_dev_handle, write_data, sizeof(write_data), -1));

    // 4. Read data from I3C device
    uint8_t read_data[100] = {0};
    ESP_ERROR_CHECK(i3c_master_i3c_device_receive(i3c_dev_handle, read_data, sizeof(read_data), -1));

    // 5. Write-read combined transaction
    uint8_t reg_addr = 0x12;
    uint8_t read_buffer[10] = {0};
    ESP_ERROR_CHECK(i3c_master_i3c_device_transmit_receive(i3c_dev_handle, &reg_addr, 1, read_buffer, sizeof(read_buffer), -1));

    // 6. Clean up resources
    ESP_ERROR_CHECK(i3c_master_bus_rm_i3c_device(i3c_dev_handle));
    ESP_ERROR_CHECK(i3c_del_master_bus(bus_handle));

In this scenario:

1. We use :cpp:func:`i3c_master_bus_add_i3c_static_device` to add an I3C device
2. We need to provide the device's static address (obtained from device manual) and the dynamic address to be assigned
3. The driver automatically executes the SETDASA procedure to assign the dynamic address to the device. If there is an address conflict, it will return ``ESP_ERR_INVALID_STATE``.
4. After that, we can use the dynamic address for data transfers through :cpp:func:`i3c_master_i3c_device_transmit` or :cpp:func:`i3c_master_i3c_device_receive` or :cpp:func:`i3c_master_i3c_device_transmit_receive`. By default, it works in blocking mode. For non-blocking mode, please refer to :ref:`dma-support`. The same applies to other transfer functions.
5. Finally clean up resources

Add and Drive I3C Devices via ENTDAA
-------------------------------------

If you don't know which I3C devices are on the bus, or want the system to automatically discover and assign addresses, you can use the ENTDAA method:

.. figure:: ../../../_static/diagrams/i3c/i3c_entdaa.svg
    :align: center
    :alt: I3C Automatic Dynamic Address Assignment

    I3C Automatic Dynamic Address Assignment

.. code:: c

    // 1. Create I3C bus (need to set entdaa_device_num)
    i3c_master_bus_config_t i3c_mst_config = {
        // ... other configurations ...
        .entdaa_device_num = 5,        // Maximum number of devices that can be dynamically discovered by the driver
    };
    i3c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    // 2. Scan I3C devices on the bus
    i3c_master_i3c_device_table_handle_t table_handle = NULL;
    ESP_ERROR_CHECK(i3c_master_scan_devices_by_entdaa(bus_handle, &table_handle));

    // 3. Get the number of discovered devices
    size_t device_count = 0;
    ESP_ERROR_CHECK(i3c_master_get_device_count(table_handle, &device_count));
    printf("Found %zu I3C devices\n", device_count);

    // 4. Iterate through all devices and get device information
    i3c_master_i3c_device_handle_t dev = NULL;
    for (size_t i = 0; i < device_count; i++) {
        i3c_master_i3c_device_handle_t dev_handle = NULL;
        ESP_ERROR_CHECK(i3c_master_get_device_handle(table_handle, i, &dev_handle));

        // Get device information
        i3c_device_information_t info;
        ESP_ERROR_CHECK(i3c_master_i3c_device_get_info(dev_handle, &info));
        printf("Device %d: Dynamic Addr=0x%02X, BCR=0x%02X, DCR=0x%02X, PID=0x%016llX\n",
               i, info.dynamic_addr, info.bcr, info.dcr, info.pid);

        if (info.pid == /* Device PID, obtained from device manual */) {
            dev = dev_handle;
            break;
        }
    }
    // Release device handle table, call when no longer needed
    ESP_ERROR_CHECK(i3c_master_free_device_handle_table(table_handle));

    // 5. Perform data transfer through transmit or receive
    ESP_ERROR_CHECK(i3c_master_i3c_device_transmit(dev, data, sizeof(data), -1));
    ESP_ERROR_CHECK(i3c_master_i3c_device_receive(dev, data, sizeof(data), -1));

In this scenario:

1. When creating the bus, we need to set `entdaa_device_num`, which represents the expected number of devices to be discovered
2. Use :cpp:func:`i3c_master_scan_devices_by_entdaa` to scan all I3C devices on the bus
3. The system automatically assigns dynamic addresses to each device
4. We can get the device count through :cpp:func:`i3c_master_get_device_count`
5. Get each device's handle through :cpp:func:`i3c_master_get_device_handle`
6. Use :cpp:func:`i3c_master_i3c_device_get_info` to get detailed device information (dynamic address, BCR, DCR, PID)
7. Perform data transfers through :cpp:func:`i3c_master_i3c_device_transmit` or :cpp:func:`i3c_master_i3c_device_receive` based on the obtained device information

.. note::

    :cpp:func:`i3c_master_scan_devices_by_entdaa` is thread-safe, and there will not be two threads addressing simultaneously. According to the protocol, when a slave is addressed and discovered by :cpp:func:`i3c_master_scan_devices_by_entdaa`, it no longer has the ability to respond to a second addressing. Therefore, there will be no address changes due to addressing in different threads. This interface supports adding new devices after initialization. To rescan, use the CCC mechanism to reset addresses on the I3C bus, or clear address information on the bus by power cycling.

Common Command Code (CCC) Transfer
-----------------------------------

The I3C protocol uses Common Command Code (CCC) for bus management and device configuration. You can use the :cpp:func:`i3c_master_transfer_ccc` function to send CCC commands.

CCC transfers can be broadcast (sent to all devices) or direct (sent to a specific device):

.. figure:: ../../../_static/diagrams/i3c/i3c_broadcast_ccc.svg
    :align: center
    :alt: I3C Broadcast Command

    I3C Broadcast Command

.. figure:: ../../../_static/diagrams/i3c/i3c_direct_ccc.svg
    :align: center
    :alt: I3C Direct Command

    I3C Direct Command

.. code:: c

    // Broadcast CCC command example: Send RSTDAA (Reset All Dynamic Addresses)
    i3c_master_ccc_transfer_config_t ccc_trans = {
        .ccc_command = I3C_CCC_RSTDAA,
        .direction = I3C_MASTER_TRANSFER_DIRECTION_WRITE,
        .device_address = 0,  // Broadcast command, this field is ignored
        .data = NULL,
        .data_size = 0,
    };
    ESP_ERROR_CHECK(i3c_master_transfer_ccc(bus_handle, &ccc_trans));

    // Direct CCC command example: Read device's GETPID (Get Device ID)
    uint8_t pid_data[6] = {0};
    ccc_trans = (i3c_master_ccc_transfer_config_t) {
        .ccc_command = I3C_CCC_GETPID,
        .direction = I3C_MASTER_TRANSFER_DIRECTION_READ,
        .device_address = 0x08,  // Target device address, which is the dynamic address
        .data = pid_data,
        .data_size = sizeof(pid_data),
    };
    ESP_ERROR_CHECK(i3c_master_transfer_ccc(bus_handle, &ccc_trans));

.. note::

    :cpp:func:`i3c_master_transfer_ccc` is always blocking and is not affected by DMA and asynchronous configuration. Users need to query the I3C protocol to know the specific format of CCC commands, and fill :cpp:member:`i3c_master_ccc_transfer_config_t::direction` as ``I3C_MASTER_TRANSFER_DIRECTION_READ`` or ``I3C_MASTER_TRANSFER_DIRECTION_WRITE`` and fill :cpp:member:`i3c_master_ccc_transfer_config_t::data` and :cpp:member:`i3c_master_ccc_transfer_config_t::data_size` according to the format of sending commands or obtaining values.

Resource Cleanup
----------------

When the previously installed I3C bus or device is no longer needed, call :cpp:func:`i3c_master_bus_rm_i3c_device` or :cpp:func:`i3c_master_bus_rm_i2c_device` to remove the device, then call :cpp:func:`i3c_del_master_bus` to reclaim resources and release the underlying hardware.

.. code:: c

    ESP_ERROR_CHECK(i3c_master_bus_rm_i3c_device(i3c_dev_handle));
    ESP_ERROR_CHECK(i3c_del_master_bus(bus_handle));

Advanced Features
=================

Clock Source and Timing Parameter Fine-tuning
---------------------------------------------

Clock Source Selection
^^^^^^^^^^^^^^^^^^^^^^

The clock source of the I3C bus can be selected through :cpp:member:`i3c_master_bus_config_t::clock_source`.

.. code:: c

    i3c_master_bus_config_t i3c_mst_config = {
        // ... other configurations ...
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT, // Default clock source
    };

.. note::

    When the I3C push-pull output frequency is greater than 3 MHz, please set the clock source to :cpp:enumerator:`i3c_master_clock_source_t::I3C_MASTER_CLK_SRC_PLL_F120M` or :cpp:enumerator:`i3c_master_clock_source_t::I3C_MASTER_CLK_SRC_PLL_F160M`.

The I3C driver provides rich timing parameter configuration options. You can adjust these parameters according to the actual hardware situation to optimize performance or solve timing issues.

Duty Cycle and Hold Time
^^^^^^^^^^^^^^^^^^^^^^^^^

Some I3C slave devices have strict timing requirements for their acknowledgment mechanism (ACK/NACK), such as requirements for SCL waveform duty cycle and SDA hold time. These parameters can be configured through the following configuration items.

.. code:: c

    i3c_master_bus_config_t i3c_mst_config = {
        // ... other configurations ...
        .i3c_scl_pp_duty_cycle = 0.5,          // Push-Pull mode duty cycle, usually 0.5 (default value 0 also means 0.5)
        .i3c_scl_od_duty_cycle = 0.5,          // Open-Drain mode duty cycle, usually 0.5 (default value 0 also means 0.5)
        .i3c_sda_od_hold_time_ns = 25,          // Open-Drain mode hold time, default 25 ns
        .i3c_sda_pp_hold_time_ns = 0,           // Push-Pull mode hold time, default 0 ns
    };

The specific values of these parameters need to be determined according to the device manual and actual testing.

Event Callbacks
---------------

The I3C driver supports an event callback mechanism that can notify the application when a transfer is complete or when an IBI interrupt is received.

When the I3C controller generates events such as send or receive completion, it notifies the CPU through interrupts. If you need to call a function when a specific event occurs, you can register event callbacks with the I3C driver's interrupt service routine (ISR) by calling :cpp:func:`i3c_master_i3c_device_register_event_callbacks` and :cpp:func:`i3c_master_i2c_device_register_event_callbacks` for I3C and I2C slaves respectively. Since these callback functions are called in the ISR, they should not involve blocking operations. You can check the suffix of the called API to ensure that only FreeRTOS APIs with the ISR suffix are called in the function. The callback function has a boolean return value indicating whether the callback unblocked a higher priority task.

For event callbacks of I2C slaves, please refer to i2c_master_i2c_event_callbacks_t.

    * :cpp:member:`i3c_master_i2c_event_callbacks_t::on_trans_done` can be set to a callback function for the master "transfer done" event. The function prototype is declared in :cpp:type:`i3c_master_i2c_callback_t`. Note that this callback function can only be used when the I2C slave device has DMA enabled and uses asynchronous transfer. For details, please refer to :ref:`dma-support`.

For event callbacks of I3C slaves, please refer to i3c_master_i3c_event_callbacks_t.

    * :cpp:member:`i3c_master_i3c_event_callbacks_t::on_trans_done` can be set to a callback function for the master "transfer done" event. The function prototype is declared in :cpp:type:`i3c_master_i3c_callback_t`. Note that this callback function can only be used when the I3C slave device has DMA enabled and uses asynchronous transfer. For details, please refer to :ref:`dma-support`.

    * :cpp:member:`i3c_master_i3c_event_callbacks_t::on_ibi` can be set to a callback function for IBI events. The function prototype is declared in :cpp:type:`i3c_master_ibi_callback_t`. For detailed information about IBI events, please refer to :ref:`in-band-interrupt`

.. note::

    Callback functions are executed in the ISR context, therefore:

    - Cannot perform blocking operations
    - Can only call FreeRTOS APIs with the ISR suffix
    - If ``CONFIG_I3C_MASTER_ISR_CACHE_SAFE`` is enabled, callback functions must be placed in IRAM

.. _in-band-interrupt:

In-Band Interrupt (IBI)
-----------------------

The I3C protocol supports In-Band Interrupt (IBI), allowing slave devices to send interrupt requests through the I3C bus without additional interrupt lines.

Configure IBI
^^^^^^^^^^^^^

The I3C bus configuration structure :cpp:type:`i3c_master_bus_config_t` contains IBI-related global configuration items:

- :cpp:member:`i3c_master_bus_config_t::ibi_rstart_trans_en` enables restart transaction on IBI. The I3C controller continues to execute the command that was interrupted by IBI after IBI completion. If IBI occurs during bus idle and the I3C transfer task is not empty, the I3C controller will continue to execute that task. If IBI conflicts with I3C controller transfer and wins arbitration, the interrupted task will continue to execute after IBI processing is complete.
- :cpp:member:`i3c_master_bus_config_t::ibi_silent_sir_rejected` when written as 0, does not notify the application layer when a slave interrupt request (SIR) is rejected. When written as 1, the IBI status is still written to the IBI FIFO and the application layer is notified.
- :cpp:member:`i3c_master_bus_config_t::ibi_no_auto_disable` if set, does not automatically disable IBI after the controller NACKs an In-Band interrupt, keeping in-band interrupt enabled.

You can use the :cpp:func:`i3c_master_i3c_device_ibi_config` function to configure IBI for a specific device:

.. code:: c

    i3c_ibi_config_t ibi_cfg = {
        .enable_ibi = true,
        .enable_ibi_payload = true,  // Allow IBI to carry payload
    };
    ESP_ERROR_CHECK(i3c_master_i3c_device_ibi_config(dev_handle, &ibi_cfg));

Handle IBI Events
^^^^^^^^^^^^^^^^^

Detailed information about IBI events will be provided from the callback through :cpp:type:`i3c_master_ibi_info_t`:

:cpp:member:`i3c_master_ibi_info_t::ibi_id` is the raw identifier of the IBI, usually encoded from the slave device's dynamic address; it is the raw value, i.e., dynamic address + read/write bit. :cpp:member:`i3c_master_ibi_info_t::ibi_sts` is the IBI status field reported by the controller. :cpp:member:`i3c_master_ibi_info_t::data_length` is the number of valid bytes in the payload buffer :cpp:member:`i3c_master_ibi_info_t::ibi_data`. :cpp:member:`i3c_master_ibi_info_t::ibi_data` is the optional payload bytes associated with the IBI. Only the first :cpp:member:`i3c_master_ibi_info_t::data_length` bytes are valid.

.. code:: c

    static bool i3c_ibi_callback(i3c_master_i3c_device_handle_t dev_handle, const i3c_master_ibi_info_t *ibi_info, void *user_ctx)
    {
        // Can copy IBI event data to user-provided context and do further processing in task
        // i3c_master_ibi_info_t is a user-defined structure, here including ibi_id and ibi_data_len, members can be added or removed according to actual needs
        i3c_master_ibi_info_t evt = {
            .ibi_id = ibi_info->ibi_id,
            .ibi_data_len = ibi_info->data_length,
        };

        return false;
    }

    i3c_master_i3c_event_callbacks_t cbs = {
        .on_ibi = i3c_ibi_callback,
    };
    ESP_ERROR_CHECK(i3c_master_i3c_device_register_event_callbacks(dev_handle, &cbs, NULL));

.. _dma-support:

DMA and Asynchronous Transfer
-----------------------------

The I3C driver supports DMA for large-capacity data transfers and asynchronous transfers, which can improve transfer efficiency and reduce CPU usage.

Enable DMA
^^^^^^^^^^

You can configure DMA for the bus through the :cpp:func:`i3c_master_bus_decorate_dma` function:

.. code:: c

    i3c_master_dma_config_t dma_config = {
        .max_transfer_size = 4096,  // Maximum transfer size (bytes)
        .dma_burst_size = 16,       // DMA burst size (bytes)
    };
    ESP_ERROR_CHECK(i3c_master_bus_decorate_dma(bus_handle, &dma_config));

Enable Asynchronous Transfer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When DMA is enabled, you can further enable asynchronous transfer to improve performance:

.. code:: c

    i3c_master_bus_config_t i3c_mst_config = {
        // ... other configurations ...
        .trans_queue_depth = 5,  // Set the depth of internal transfer queue
        .flags = {
            .enable_async_trans = 1,  // Enable asynchronous transfer
        }
    };

At this point, the I3C master transfer functions will return immediately after being called. When each transfer is complete, the :cpp:member:`i3c_master_i3c_event_callbacks_t::on_trans_done` callback function will be called to indicate the completion of a transfer. If you need to wait for the transfer to complete, you can call the :cpp:func:`i3c_master_bus_wait_all_done` function to wait for all transfers to complete:

.. code:: c

    // Start multiple asynchronous transfers
    ESP_ERROR_CHECK(i3c_master_i3c_device_transmit(dev_handle1, data1, size1, -1));
    ESP_ERROR_CHECK(i3c_master_i3c_device_transmit(dev_handle2, data2, size2, -1));
    ESP_ERROR_CHECK(i3c_master_i3c_device_transmit(dev_handle3, data3, size3, -1));

    // Wait for all transfers to complete
    ESP_ERROR_CHECK(i3c_master_bus_wait_all_done(bus_handle, -1));

Thread Safety
-------------

The following functions of the I3C driver are thread-safe and can be called from different RTOS tasks without additional lock protection:

Factory functions:
- :cpp:func:`i3c_new_master_bus`
- :cpp:func:`i3c_del_master_bus`

I3C master operation functions (thread safety guaranteed through bus operation signals):
- :cpp:func:`i3c_master_bus_add_i3c_static_device`
- :cpp:func:`i3c_master_bus_rm_i3c_device`
- :cpp:func:`i3c_master_i3c_device_transmit`
- :cpp:func:`i3c_master_i3c_device_receive`
- :cpp:func:`i3c_master_i3c_device_transmit_receive`
- :cpp:func:`i3c_master_i2c_device_transmit`
- :cpp:func:`i3c_master_i2c_device_receive`
- :cpp:func:`i3c_master_i2c_device_transmit_receive`
- :cpp:func:`i3c_master_transfer_ccc`

Cache Safety
------------

By default, when the cache is disabled (e.g., during SPI Flash write), I3C interrupts will be delayed, and event callback functions will not be able to execute on time, which will affect the system response of real-time applications.

This can be avoided by enabling the Kconfig option ``CONFIG_I3C_MASTER_ISR_CACHE_SAFE``. After enabling:

1. Interrupts can continue to run even when the cache is disabled
2. All functions used by the ISR are placed in IRAM
3. Driver objects are placed in DRAM (to prevent them from being accidentally mapped to PSRAM)

Enabling this option ensures interrupt operation when the cache is disabled, but will consume more IRAM.

.. note::

    After enabling this option, when the cache is disabled, ISR callback functions will continue to run. Therefore, you must ensure that the callback functions and their context are also IRAM-safe. At the same time, data transfer buffers must also be placed in DRAM.

About Low Power Consumption
----------------------------

When power management :ref:`CONFIG_PM_ENABLE` is enabled, the system may adjust or disable clock sources before entering sleep mode, which can cause I3C transfer errors.

To prevent this from happening, the I3C driver internally creates a power management lock. After calling a transfer function, this lock will be activated to ensure the system does not enter sleep mode, thus maintaining the correct operation of the timer. After the transfer is complete, the driver automatically releases the lock, allowing the system to enter sleep mode.

Kconfig Options
---------------

The following Kconfig options can be used to configure the I3C driver:

- :ref:`CONFIG_I3C_MASTER_ISR_CACHE_SAFE`: Ensure I3C interrupts work properly when cache is disabled (e.g., during SPI Flash write)
- :ref:`CONFIG_I3C_MASTER_ISR_HANDLER_IN_IRAM`: Place I3C master ISR handler in IRAM to improve performance and reduce cache misses
- :ref:`CONFIG_I3C_MASTER_ENABLE_DEBUG_LOG`: Enable I3C debug logging

About Resource Consumption
---------------------------

You can use the :doc:`/api-guides/tools/idf-size` tool to view the code and data consumption of the I3C driver. The following are the test prerequisites (using ESP32-P4 as an example):

- Compiler optimization level is set to ``-Os`` to ensure minimal code size.
- Default log level is set to ``ESP_LOG_INFO`` to balance debug information and performance.
- The following driver optimization options are disabled:
    - :ref:`CONFIG_I3C_MASTER_ISR_HANDLER_IN_IRAM` - ISR handler is not placed in IRAM.
    - :ref:`CONFIG_I3C_MASTER_ISR_CACHE_SAFE` - Cache safety option is not enabled.

**Note: The following data is not precise and is for reference only. Data may vary on different chip models.**

+------------------+------------+-------+------+-------+-------+------------+-------+------------+---------+
| Component Layer  | Total Size | DIRAM | .bss | .data | .text | Flash Code | .text | Flash Data | .rodata |
+==================+============+=======+======+=======+=======+============+=======+============+=========+
| hal              |         30 |     0 |    0 |     0 |     0 |         30 |    18 |          0 |      12 |
+------------------+------------+-------+------+-------+-------+------------+-------+------------+---------+
| driver           |       9249 |    12 |   12 |     0 |     0 |       9237 |  8666 |        571 |     571 |
+------------------+------------+-------+------+-------+-------+------------+-------+------------+---------+

Application Examples
====================

- :example:`peripherals/i3c/i3c_i2c_basic` demonstrates the basic steps of initializing the I3C master driver and reading data from the ICM42688 sensor using the I2C interface.

- :example:`peripherals/i3c/i3c_lsm6dscx` demonstrates how to read and write data from a connected LSM6DSOX sensor using I3C master mode, and supports in-band interrupt (IBI) event handling.

API Reference
=============

I3C Driver API
--------------

.. include-build-file:: inc/i3c_master.inc

I3C Driver I2C Slave API
------------------------

.. include-build-file:: inc/i3c_master_i2c.inc

I3C Driver Types
----------------

.. include-build-file:: inc/components/esp_hal_i3c/include/hal/i3c_master_types.inc

I3C HAL Types
-------------

.. include-build-file:: inc/components/esp_driver_i3c/include/driver/i3c_master_types.inc
