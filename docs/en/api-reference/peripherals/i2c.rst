Inter-Integrated Circuit (I2C)
==============================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

I2C is a serial, synchronous, multi-device, half-duplex communication protocol that allows co-existence of multiple masters and slaves on the same bus. I2C uses two bidirectional open-drain lines: serial data line (SDA) and serial clock line (SCL), pulled up by resistors.

{IDF_TARGET_NAME} has {IDF_TARGET_SOC_HP_I2C_NUM} I2C controller(s) (also called port), responsible for handling communication on the I2C bus.

.. only:: not esp32c2

    A single I2C controller can be a master or a slave.

.. only:: esp32c2

    The I2C controller can only be a master.

.. only:: SOC_LP_I2C_SUPPORTED

    Additionally, the {IDF_TARGET_NAME} chip has 1 low-power (LP) I2C controller. It is the cut-down version of regular I2C. Usually, the LP I2C controller only support basic I2C functionality with a much smaller RAM size, and does not support slave mode. For a full list of difference between HP I2C and LP I2C, please refer to the *{IDF_TARGET_NAME} Technical Reference Manual* > *I2C Controller (I2C)* > *Features* [`PDF <{IDF_TARGET_TRM_EN_URL}#i2c>`__].

    You can use LP I2C peripheral when HP I2C is not sufficient for users' usage. But please note again the LP I2C does not support all HP I2C functions. Please read documentation before you use it.

Typically, an I2C slave device has a 7-bit address or 10-bit address. {IDF_TARGET_NAME} supports both I2C Standard-mode (Sm) and Fast-mode (Fm) which can go up to 100 kHz and 400 kHz respectively.

.. warning::

    The clock frequency of SCL in master mode should not be larger than 400 kHz.

.. note::

    The frequency of SCL is influenced by both the pull-up resistor and the wire capacitance. Therefore, it is strongly recommended to choose appropriate pull-up resistors to make the frequency accurate. The recommended value for pull-up resistors usually ranges from 1 kΩ to 10 kΩ.

    Keep in mind that the higher the frequency, the smaller the pull-up resistor should be (but not less than 1 kΩ). Indeed, large resistors will decline the current, which will increase the clock switching time and reduce the frequency. A range of 2 kΩ to 5 kΩ is recommended, but adjustments may also be necessary depending on their current draw requirements.

I2C Clock Configuration
-----------------------

.. list::

    - :cpp:enumerator:`i2c_clock_source_t::I2C_CLK_SRC_DEFAULT`: Default I2C source clock.
    :SOC_I2C_SUPPORT_XTAL: - :cpp:enumerator:`i2c_clock_source_t::I2C_CLK_SRC_XTAL`: External crystal for I2C clock source.
    :SOC_I2C_SUPPORT_RTC: - :cpp:enumerator:`i2c_clock_source_t::I2C_CLK_SRC_RC_FAST`: Internal 20 MHz RC oscillator for I2C clock source.
    :SOC_I2C_SUPPORT_APB: - :cpp:enumerator:`i2c_clock_source_t::I2C_CLK_SRC_APB`: APB clock as I2C clock source.
    :SOC_I2C_SUPPORT_REF_TICK: - :cpp:enumerator:`i2c_clock_source_t::I2C_CLK_SRC_REF_TICK`: 1 MHZ clock.

I2C File Structure
------------------

.. figure:: ../../../_static/diagrams/i2c/i2c_code_structure.png
    :align: center
    :alt: I2C file structure

    I2C file structure

**Public headers that need to be included in the I2C application**

- ``i2c.h``: The header file of legacy I2C APIs (for apps using legacy driver).
- ``i2c_master.h``: The header file that provides standard communication mode specific APIs (for apps using new driver with master mode).
- ``i2c_slave.h``: The header file that provides standard communication mode specific APIs (for apps using new driver with slave mode).

.. note::

    The legacy driver can't coexist with the new driver. Include ``i2c.h`` to use the legacy driver or the other two headers to use the new driver. Please keep in mind that the legacy driver is now deprecated and will be removed in future.

**Public headers that have been included in the headers above**

- ``i2c_types_legacy.h``: The legacy public types that are only used in the legacy driver.
- ``i2c_types.h``: The header file that provides public types.

Functional Overview
-------------------

The I2C driver offers following services:

- `Resource Allocation <#resource-allocation>`__ - covers how to allocate I2C bus with properly set of configurations. It also covers how to recycle the resources when they finished working.
- `I2C Master Controller <#i2c_master_controller>`__ - covers behavior of I2C master controller. Introduce data transmit, data receive, and data transmit and receive.
- `I2C Slave Controller <#i2c_slave_controller>`__ - covers behavior of I2C slave controller. Involve data transmit and data receive.
- `Power Management <#power-management>`__ - describes how different source clock will affect power consumption.
- `IRAM Safe <#iram-safe>`__ - describes tips on how to make the I2C interrupt work better along with a disabled cache.
- `Thread Safety <#thread-safety>`__ - lists which APIs are guaranteed to be thread safe by the driver.
- `Kconfig Options <#kconfig-options>`__ - lists the supported Kconfig options that can bring different effects to the driver.

Resource Allocation
^^^^^^^^^^^^^^^^^^^

The I2C master bus is represented by :cpp:type:`i2c_master_bus_handle_t` in the driver. The available ports are managed in a resource pool that allocates a free port on request.

Install I2C master bus and device
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The I2C master bus is designed based on bus-device model. So :cpp:type:`i2c_master_bus_config_t` and :cpp:type:`i2c_device_config_t` are required separately to allocate the I2C master bus instance and I2C device instance.

.. figure:: ../../../_static/diagrams/i2c/i2c_master_module.png
    :align: center
    :alt: I2C master bus-device module

    I2C master bus-device module

I2C master bus requires the configuration that specified by :cpp:type:`i2c_master_bus_config_t`:

- :cpp:member:`i2c_master_bus_config_t::i2c_port` sets the I2C port used by the controller.
- :cpp:member:`i2c_master_bus_config_t::sda_io_num` sets the GPIO number for the serial data bus (SDA).
- :cpp:member:`i2c_master_bus_config_t::scl_io_num` sets the GPIO number for the serial clock bus (SCL).
- :cpp:member:`i2c_master_bus_config_t::clk_source` selects the source clock for I2C bus. The available clocks are listed in :cpp:type:`i2c_clock_source_t`. For the effect on power consumption of different clock source, please refer to `Power Management <#power-management>`__  section.
- :cpp:member:`i2c_master_bus_config_t::glitch_ignore_cnt` sets the glitch period of master bus, if the glitch period on the line is less than this value, it can be filtered out, typically value is 7.
- :cpp:member:`i2c_master_bus_config_t::intr_priority` sets the priority of the interrupt. If set to ``0`` , then the driver will use a interrupt with low or medium priority (priority level may be one of 1, 2 or 3), otherwise use the priority indicated by :cpp:member:`i2c_master_bus_config_t::intr_priority`. Please use the number form (1, 2, 3) , not the bitmask form ((1<<1), (1<<2), (1<<3)).
- :cpp:member:`i2c_master_bus_config_t::trans_queue_depth` sets the depth of internal transfer queue. Only valid in asynchronous transaction.
- :cpp:member:`i2c_master_bus_config_t::enable_internal_pullup` enables internal pullups. Note: This is not strong enough to pullup buses under high-speed frequency. A suitable external pullup is recommended.
- :cpp:member:`i2c_master_bus_config_t::allow_pd` configures if the driver allows the system to power down the peripheral in light sleep mode. Before entering sleep, the system will backup the I2C register context, which will be restored later when the system exit the sleep mode. Powering down the peripheral can save more power, but at the cost of more memory consumed to save the register context. It's a tradeoff between power consumption and memory consumption. This configuration option relies on specific hardware feature, if you enable it on an unsupported chip, you will see error message like ``not able to power down in light sleep``.

If the configurations in :cpp:type:`i2c_master_bus_config_t` is specified, then :cpp:func:`i2c_new_master_bus` can be called to allocate and initialize an I2C master bus. This function will return an I2C bus handle if it runs correctly. Specifically, when there are no more I2C port available, this function will return :c:macro:`ESP_ERR_NOT_FOUND` error.

I2C master device requires the configuration that specified by :cpp:type:`i2c_device_config_t`:

- :cpp:member:`i2c_device_config_t::dev_addr_length` configure the address bit length of the slave device. It can be chosen from enumerator :cpp:enumerator:`I2C_ADDR_BIT_LEN_7` or :cpp:enumerator:`I2C_ADDR_BIT_LEN_10` (if supported).
- :cpp:member:`i2c_device_config_t::device_address` sets the I2C device raw address. Please parse the device address to this member directly. For example, the device address is 0x28, then parse 0x28 to :cpp:member:`i2c_device_config_t::device_address`, don't carry a write or read bit.
- :cpp:member:`i2c_device_config_t::scl_speed_hz` sets the SCL line frequency of this device.
- :cpp:member:`i2c_device_config_t::scl_wait_us` sets the SCL await time (in μs). Usually this value should not be very small because slave stretch will happen in pretty long time (It's possible even stretch for 12 ms). Set ``0`` means use default register value.


Once the :cpp:type:`i2c_device_config_t` structure is populated with mandatory parameters, :cpp:func:`i2c_master_bus_add_device` can be called to allocate an I2C device instance and mounted to the master bus then. This function will return an I2C device handle if it runs correctly. Specifically, when the I2C bus is not initialized properly, calling this function will result in a :c:macro:`ESP_ERR_INVALID_ARG` error.

.. code:: c

    #include "driver/i2c_master.h"

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

Get I2C master handle via port
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When the I2C master handle has been initialized in one module (e.g. the audio module), but it is not convenient to acquire this handle in another module (e.g. the video module). You can use the helper function, :cpp:func:`i2c_master_get_bus_handle` to retrieve the initialized handle via port. Ensure that the handle has already been initialized beforehand to avoid potential errors.

.. code:: c

    // Source File 1
    #include "driver/i2c_master.h"
    i2c_master_bus_handle_t bus_handle;
    i2c_master_bus_config_t i2c_mst_config = {
        ... // same as others
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    // Source File 2
    #include "driver/i2c_master.h"
    i2c_master_bus_handle_t handle;
    ESP_ERROR_CHECK(i2c_master_get_bus_handle(0, &handle));

.. only:: SOC_LP_I2C_SUPPORTED

    Install I2C master bus with LP I2C Peripheral
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Install I2C master bus with LP I2C peripheral is almost as same as how HP I2C peripheral is installed. However, there are still some difference should be taken focus on, including IOs, clock sources, I2C port number, etc. Following code will show how to install I2C master bus with LP_I2C.

    .. code:: c

        #include "driver/i2c_master.h"

        i2c_master_bus_config_t i2c_mst_config = {
            .clk_source = LP_I2C_SCLK_DEFAULT,    // clock source for LP I2C, might different from HP I2C
            .i2c_port = LP_I2C_NUM_0,             // Assign to LP I2C port
            .scl_io_num = 7,                      // SCL IO number. Please refer to technical reference manual
            .sda_io_num = 6,                      // SDA IO number. Please refer to technical reference manual
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = true,
        };

        i2c_master_bus_handle_t bus_handle;
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

        i2c_device_config_t dev_cfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = 0x58,
            .scl_speed_hz = 100000,
        };

        i2c_master_dev_handle_t dev_handle;
        ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

Uninstall I2C master bus and device
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If a previously installed I2C bus or device is no longer needed, it's recommended to recycle the resource by calling :cpp:func:`i2c_master_bus_rm_device` or :cpp:func:`i2c_del_master_bus`, so as to release the underlying hardware.

Please note that removing all devices attached to bus before delete the master bus.

Install I2C slave device
~~~~~~~~~~~~~~~~~~~~~~~~

I2C slave requires the configuration specified by :cpp:type:`i2c_slave_config_t`:

.. list::

    - :cpp:member:`i2c_slave_config_t::i2c_port` sets the I2C port used by the controller.
    - :cpp:member:`i2c_slave_config_t::sda_io_num` sets the GPIO number for serial data bus (SDA).
    - :cpp:member:`i2c_slave_config_t::scl_io_num` sets the GPIO number for serial clock bus (SCL).
    - :cpp:member:`i2c_slave_config_t::clk_source` selects the source clock for I2C bus. The available clocks are listed in :cpp:type:`i2c_clock_source_t`. For the effect on power consumption of different clock source, please refer to `Power Management <#power-management>`__  section.
    - :cpp:member:`i2c_slave_config_t::send_buf_depth` sets the sending software buffer length.
    - :cpp:member:`i2c_slave_config_t::receive_buf_depth` sets the receiving software buffer length.
    - :cpp:member:`i2c_slave_config_t::intr_priority` sets the priority of the interrupt. If set to ``0`` , then the driver will use a interrupt with low or medium priority (priority level may be one of 1, 2 or 3), otherwise use the priority indicated by :cpp:member:`i2c_slave_config_t::intr_priority`. Please use the number form (1, 2, 3), instead of the bitmask form ((1<<1), (1<<2), (1<<3)). Please pay attention that once the interrupt priority is set, it cannot be changed until :cpp:func:`i2c_del_slave_device` is called.
    - :cpp:member:`i2c_slave_config_t::addr_bit_len` Set this variable to ``I2C_ADDR_BIT_LEN_10`` if the slave should have a 10-bit address.
    - :cpp:member:`i2c_slave_config_t::allow_pd` If set, the driver will backup/restore the I2C registers before/after entering/exist sleep mode. By this approach, the system can power off I2C's power domain. This can save power, but at the expense of more RAM being consumed.
    :SOC_I2C_SLAVE_SUPPORT_BROADCAST: - :cpp:member:`i2c_slave_config_t::broadcast_en` Set this to true to enable the slave broadcast. When the slave receives the general call address 0x00 from the master and the R/W bit followed is 0, it responds to the master regardless of its own address.
    - :cpp:member:`i2c_slave_config_t::enable_internal_pullup` Set this to enable internal pull-up. Even though, an output pull-up resistance is strongly recommended.

Once the :cpp:type:`i2c_slave_config_t` structure is populated with mandatory parameters, :cpp:func:`i2c_new_slave_device` can be called to allocate and initialize an I2C master bus. This function will return an I2C bus handle if it runs correctly. Specifically, when there are no more I2C port available, this function will return :c:macro:`ESP_ERR_NOT_FOUND` error.

.. code:: c

    i2c_slave_config_t i2c_slv_config = {
        .i2c_port = I2C_SLAVE_NUM,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = ESP_SLAVE_ADDR,
        .send_buf_depth = 100,
        .receive_buf_depth = 100,
    };

    i2c_slave_dev_handle_t slave_handle;
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

Uninstall I2C slave device
~~~~~~~~~~~~~~~~~~~~~~~~~~

If a previously installed I2C bus is no longer needed, it's recommended to recycle the resource by calling :cpp:func:`i2c_del_slave_device`, so that to release the underlying hardware.


I2C Master Controller
^^^^^^^^^^^^^^^^^^^^^

After installing the I2C master driver by :cpp:func:`i2c_new_master_bus`, {IDF_TARGET_NAME} is ready to communicate with other I2C devices. I2C APIs allow the standard transactions. Like the wave as follows:

.. wavedrom:: /../_static/diagrams/i2c/i2c_trans_wave.json

I2C Master Write
~~~~~~~~~~~~~~~~

After installing I2C master bus successfully, you can simply call :cpp:func:`i2c_master_transmit` to write data to the slave device. The principle of this function can be explained by following chart.

In order to organize the process, the driver uses a command link, that should be populated with a sequence of commands and then passed to I2C controller for execution.

.. figure:: ../../../_static/diagrams/i2c/i2c_master_write_slave.png
    :align: center
    :alt: I2C master write to slave

    I2C master write to slave

Simple example for writing data to slave:

.. code:: c

    #define DATA_LENGTH 100
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT_NUM_0,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
    };
    i2c_master_bus_handle_t bus_handle;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, data_wr, DATA_LENGTH, -1));


I2C master write also supports transmit multi-buffer in one transaction. Take following transaction as a simple example:

.. code:: c

    uint8_t control_phase_byte = 0;
    size_t control_phase_size = 0;
    if (/*condition*/) {
        control_phase_byte = 1;
        control_phase_size = 1;
    }

    uint8_t *cmd_buffer = NULL;
    size_t cmd_buffer_size = 0;
    if (/*condition*/) {
        uint8_t cmds[4] = {BYTESHIFT(lcd_cmd, 3), BYTESHIFT(lcd_cmd, 2), BYTESHIFT(lcd_cmd, 1), BYTESHIFT(lcd_cmd, 0)};
        cmd_buffer = cmds;
        cmd_buffer_size = 4;
    }

    uint8_t *lcd_buffer = NULL;
    size_t lcd_buffer_size = 0;
    if (buffer) {
        lcd_buffer = (uint8_t*)buffer;
        lcd_buffer_size = buffer_size;
    }

    i2c_master_transmit_multi_buffer_info_t lcd_i2c_buffer[3] = {
        {.write_buffer = &control_phase_byte, .buffer_size = control_phase_size},
        {.write_buffer = cmd_buffer, .buffer_size = cmd_buffer_size},
        {.write_buffer = lcd_buffer, .buffer_size = lcd_buffer_size},
    };

    i2c_master_multi_buffer_transmit(handle, lcd_i2c_buffer, sizeof(lcd_i2c_buffer) / sizeof(i2c_master_transmit_multi_buffer_info_t), -1);


I2C Master Read
~~~~~~~~~~~~~~~

After installing I2C master bus successfully, you can simply call :cpp:func:`i2c_master_receive` to read data from the slave device. The principle of this function can be explained by following chart.

.. figure:: ../../../_static/diagrams/i2c/i2c_master_read_slave.png
    :align: center
    :alt: I2C master read from slave

    I2C master read from slave

Simple example for reading data from slave:

.. code:: c

    #define DATA_LENGTH 100
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT_NUM_0,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
    };
    i2c_master_bus_handle_t bus_handle;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    i2c_master_receive(dev_handle, data_rd, DATA_LENGTH, -1);

I2C Master Write and Read
~~~~~~~~~~~~~~~~~~~~~~~~~

Some I2C device needs write configurations before reading data from it. Therefore, an interface called :cpp:func:`i2c_master_transmit_receive` can help. The principle of this function can be explained by following chart.

.. figure:: ../../../_static/diagrams/i2c/i2c_master_write_read_slave.png
    :align: center
    :alt: I2C master write to slave and read from slave

    I2C master write to slave and read from slave

Please note that no STOP condition bit is inserted between the write and read operations; therefore, this function is suited to read a register from an I2C device. A simple example for writing and reading from a slave device:

.. code:: c

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
    uint8_t buf[20] = {0x20};
    uint8_t buffer[2];
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, buf, sizeof(buf), buffer, 2, -1));

I2C Master Probe
~~~~~~~~~~~~~~~~

I2C driver can use :cpp:func:`i2c_master_probe` to detect whether the specific device has been connected on I2C bus. If this function return ``ESP_OK``, that means the device has been detected.

.. important::

    Pull-ups must be connected to the SCL and SDA pins when this function is called. If you get `ESP_ERR_TIMEOUT` while `xfer_timeout_ms` was parsed correctly, you should check the pull-up resistors. If you do not have proper resistors nearby, setting `flags.enable_internal_pullup` as true is also acceptable.

.. figure:: ../../../_static/diagrams/i2c/i2c_master_probe.png
    :align: center
    :alt: I2C master probe

    I2C master probe

Simple example for probing an I2C device:

.. code:: c

    i2c_master_bus_config_t i2c_mst_config_1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config_1, &bus_handle));
    ESP_ERROR_CHECK(i2c_master_probe(bus_handle, 0x22, -1));
    ESP_ERROR_CHECK(i2c_del_master_bus(bus_handle));


I2C Master Execute Customized Transactions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Not all I2C devices strictly adhere to the standard I2C protocol, as different manufacturers may implement custom variations. For example, some devices require the address to be shifted, while others do not. Similarly, certain devices mandate acknowledgment (ACK) checks for specific operations, whereas others might not. To accommodate these variations, :cpp:func:`i2c_master_execute_defined_operations` function allow developers to define and execute fully customized I2C transactions. This flexibility ensures seamless communication with non-standard devices by tailoring the transaction sequence, addressing, and acknowledgment behavior to the device's specific requirements.

.. note::

    If you want to define your address in :cpp:type:`i2c_operation_job_t`, please set :cpp:member:`i2c_device_config_t::device_address` as ``I2C_DEVICE_ADDRESS_NOT_USED`` to skip internal address configuration in driver.

For address configuration of user defined transactions, given that the device address is ``0x20``, there are two situations. See following example:

.. code:: c

    i2c_device_config_t i2c_device = {
        .device_address = I2C_DEVICE_ADDRESS_NOT_USED,
        .scl_speed_hz = 100 * 1000,
        .scl_wait_us = 20000,
    };

    i2c_master_dev_handle_t dev_handle;

    i2c_master_bus_add_device(bus_handle, &i2c_device, &dev_handle);

    // Situation one: The device does not allow device address shift
    uint8_t address1 = 0x20;
    i2c_operation_job_t i2c_ops1[] = {
        { .command = I2C_MASTER_CMD_START },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .ack_check = false, .data = (uint8_t *) &address1, .total_bytes = 1 } },
        { .command = I2C_MASTER_CMD_STOP },
    };

    // Situation one: The device address should be left shifted by one byte to include a write bit or a read bit (official protocol)
    uint8_t address2 = (0x20 << 1 | 0); // (0x20 << 1 | 1)
    i2c_operation_job_t i2c_ops2[] = {
        { .command = I2C_MASTER_CMD_START },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .ack_check = false, .data = (uint8_t *) &address2, .total_bytes = 1 } },
        { .command = I2C_MASTER_CMD_STOP },
    };

Some devices do not require an address, and allow direct transaction with data:

.. code:: c

    uint8_t data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

    i2c_operation_job_t i2c_ops[] = {
        { .command = I2C_MASTER_CMD_START },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .ack_check = false, .data = (uint8_t *)data, .total_bytes = 8 } },
        { .command = I2C_MASTER_CMD_STOP },
    };

    i2c_master_execute_defined_operations(dev_handle, i2c_ops, sizeof(i2c_ops) / sizeof(i2c_operation_job_t), -1);

The principle of read operations is the same as that of write operations. Note to always ensure the last byte read before the stop condition is a ``NACK``. An example is as follows:

.. code:: c

    uint8_t address = (0x20 << 1 | 1);
    uint8_t rcv_data[10] = {};

    i2c_operation_job_t i2c_ops[] = {
        { .command = I2C_MASTER_CMD_START },
        { .command = I2C_MASTER_CMD_WRITE, .write = { .ack_check = false, .data = (uint8_t *) &address, .total_bytes = 1 } },
        { .command = I2C_MASTER_CMD_READ, .read = { .ack_value = I2C_ACK_VAL, .data = (uint8_t *)rcv_data, .total_bytes = 9 } },
        { .command = I2C_MASTER_CMD_READ, .read = { .ack_value = I2C_NACK_VAL, .data = (uint8_t *)(rcv_data + 9), .total_bytes = 1 } }, // This must be NACK
        { .command = I2C_MASTER_CMD_STOP },
    };

    i2c_master_execute_defined_operations(dev_handle, i2c_ops, sizeof(i2c_ops) / sizeof(i2c_operation_job_t), -1);

I2C Slave Controller
^^^^^^^^^^^^^^^^^^^^

After installing the I2C slave driver by :cpp:func:`i2c_new_slave_device`, {IDF_TARGET_NAME} is ready to communicate with other I2C masters as a slave.

The I2C slave is not as active as the I2C master, which knows when to send data and when to receive it. The I2C slave is very passive in most cases, meaning the I2C slave's ability to send and receive data is largely dependent on the master's actions. Therefore, we implement two callback functions in the driver to handle read and write requests from the I2C master.

I2C Slave Write
~~~~~~~~~~~~~~~

You can get I2C slave write event by registering :cpp:member:`i2c_slave_event_callbacks_t::on_request` callback. Then, in a task where the request event is triggered, you can call ``i2c_slave_write`` to send data.

A simple example for transmitting data:

.. code:: c

    // Prepare a callback function
    static bool i2c_slave_request_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_request_event_data_t *evt_data, void *arg)
    {
        i2c_slave_event_t evt = I2C_SLAVE_EVT_TX;
        BaseType_t xTaskWoken = 0;
        xQueueSendFromISR(context->event_queue, &evt, &xTaskWoken);
        return xTaskWoken;
    }

    // Register callback in a task
    i2c_slave_event_callbacks_t cbs = {
        .on_request = i2c_slave_request_cb,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(context.handle, &cbs, &context));

    // Wait for request event and send data in a task
    static void i2c_slave_task(void *arg)
    {
        uint8_t buffer_size = 64;
        uint32_t write_len;
        uint8_t *data_buffer;

        while (true) {
            i2c_slave_event_t evt;
            if (xQueueReceive(context->event_queue, &evt, 10) == pdTRUE) {
                ESP_ERROR_CHECK(i2c_slave_write(handle, data_buffer, buffer_size, &write_len, 1000));
            }
        }
        vTaskDelete(NULL);
    }

I2C Slave Read
~~~~~~~~~~~~~~

Same as write event, you can get I2C slave read event by registering :cpp:member:`i2c_slave_event_callbacks_t::on_receive` callback. Then, in a task where the request event is triggered, you can save the data and do what you want.

A simple example for receiving data:

.. code:: c

    // Prepare a callback function
    static bool i2c_slave_receive_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg)
    {
        i2c_slave_event_t evt = I2C_SLAVE_EVT_RX;
        BaseType_t xTaskWoken = 0;
        // You can get data and length via i2c_slave_rx_done_event_data_t
        xQueueSendFromISR(context->event_queue, &evt, &xTaskWoken);
        return xTaskWoken;
    }

    // Register callback in a task
    i2c_slave_event_callbacks_t cbs = {
        .on_receive = i2c_slave_receive_cb,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(context.handle, &cbs, &context));

Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

I2C master callbacks
~~~~~~~~~~~~~~~~~~~~

When an I2C master bus triggers an interrupt, a specific event will be generated and notify the CPU. If you have some functions that need to be called when those events occurred, you can hook your functions to the ISR (Interrupt Service Routine) by calling :cpp:func:`i2c_master_register_event_callbacks`. Since the registered callback functions are called in the interrupt context, users should ensure the callback function doesn't attempt to block (e.g. by making sure that only FreeRTOS APIs with ``ISR`` suffix are called from the function). The callback functions are required to return a boolean value, to tell the ISR whether a high priority task is woken up by it.

I2C master event callbacks are listed in the :cpp:type:`i2c_master_event_callbacks_t`.

Although I2C is a synchronous communication protocol, asynchronous behavior is supported by registering above callbacks. In this way, I2C APIs will be non-blocking interface. But note that on the same bus, only one device can adopt asynchronous operation.

.. important::

    I2C master asynchronous transaction is still an experimental feature (The issue is that when asynchronous transaction is very large, it will cause memory problem).

- :cpp:member:`i2c_master_event_callbacks_t::on_recv_done` sets a callback function for master "transaction-done" event. The function prototype is declared in :cpp:type:`i2c_master_callback_t`.

I2C slave callbacks
~~~~~~~~~~~~~~~~~~~

When an I2C slave bus triggers an interrupt, a specific event will be generated and notify the CPU. If you have some function that needs to be called when those events occurred, you can hook your function to the ISR (Interrupt Service Routine) by calling :cpp:func:`i2c_slave_register_event_callbacks`. Since the registered callback functions are called in the interrupt context, users should ensure the callback function doesn't attempt to block (e.g. by making sure that only FreeRTOS APIs with ``ISR`` suffix are called from the function). The callback function has a boolean return value, to tell the caller whether a high priority task is woken up by it.

I2C slave event callbacks are listed in the :cpp:type:`i2c_slave_event_callbacks_t`.

.. list::

    - :cpp:member:`i2c_slave_event_callbacks_t::on_request` sets a callback function for request event.
    - :cpp:member:`i2c_slave_event_callbacks_t::on_receive` sets a callback function for receive event. The function prototype is declared in :cpp:type:`i2c_slave_received_callback_t`.

Power Management
^^^^^^^^^^^^^^^^

.. only:: SOC_I2C_SUPPORT_APB

    When the power management is enabled (i.e. :ref:`CONFIG_PM_ENABLE` is on), the system will adjust or stop the source clock of I2C FIFO before going into Light-sleep mode, thus potentially changing the I2C signals and leading to transmitting or receiving invalid data.

    However, the driver can prevent the system from changing APB frequency by acquiring a power management lock of type :cpp:enumerator:`ESP_PM_APB_FREQ_MAX`. Whenever user creates an I2C bus that has selected :cpp:enumerator:`I2C_CLK_SRC_APB` as the clock source, the driver will guarantee that the power management lock is acquired when I2C operations begin and the lock will be released automatically when I2C operations finish.

.. only:: SOC_I2C_SUPPORT_REF_TICK

    If the controller clock source is selected to :cpp:enumerator:`I2C_CLK_SRC_REF_TICK`, then the driver won't install power management lock for it, which is more suitable for a low power application as long as the source clock can still provide sufficient resolution.

.. only:: SOC_I2C_SUPPORT_XTAL

    If the controller clock source is selected to :cpp:enumerator:`I2C_CLK_SRC_XTAL`, then the driver won't install power management lock for it, which is more suitable for a low power application as long as the source clock can still provide sufficient resolution.

IRAM Safe
^^^^^^^^^

By default, the I2C interrupt will be deferred when the cache is disabled for reasons like writing or erasing flash. Thus the event callback functions will not get executed in time, which is not expected in a real-time application.

There's a Kconfig option :ref:`CONFIG_I2C_ISR_IRAM_SAFE` that will:

1. Enable the interrupt being serviced even when cache is disabled.
2. Place all functions that used by the ISR into IRAM.
3. Place driver object into DRAM (in case it's mapped to PSRAM by accident).

This will allow the interrupt to run while the cache is disabled but will come at the cost of increased IRAM consumption.

Thread Safety
^^^^^^^^^^^^^

The factory function :cpp:func:`i2c_new_master_bus` and :cpp:func:`i2c_new_slave_device` are guaranteed to be thread safe by the driver, which means that the functions can be called from different RTOS tasks without protection by extra locks.

I2C master operation functions are also guaranteed to be thread safe by bus operation semaphore.

- :cpp:func:`i2c_master_transmit`
- :cpp:func:`i2c_master_multi_buffer_transmit`
- :cpp:func:`i2c_master_transmit_receive`
- :cpp:func:`i2c_master_receive`
- :cpp:func:`i2c_master_probe`

I2C slave operation functions are also guaranteed to be thread safe by bus operation semaphore.

- :cpp:func:`i2c_slave_write`

Other functions are not guaranteed to be thread-safe. Thus, you should avoid calling them in different tasks without mutex protection.

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_I2C_ISR_IRAM_SAFE` controls whether the default ISR handler can work when cache is disabled, see also `IRAM Safe <#iram-safe>`__ for more information.
- :ref:`CONFIG_I2C_ENABLE_DEBUG_LOG` is used to enable the debug log at the cost of increased firmware binary size.

Application Examples
--------------------

- :example:`peripherals/i2c/i2c_basic` demonstrates the basic steps to initialize the I2C master driver and read data from a MPU9250 sensor.

- :example:`peripherals/i2c/i2c_eeprom` demonstrates how to use the I2C master mode to read and write data from a connected EEPROM.

- :example:`peripherals/i2c/i2c_tools` demonstrates how to use the I2C Tools for developing I2C related applications, providing command-line tools for configuring the I2C bus, scanning for devices, reading and setting registers, and examining registers.

- :example:`peripherals/i2c/i2c_slave_network_sensor` demonstrates how to use the I2C slave for developing I2C related applications, providing how I2C slave can behave as a network sensor, and use event callbacks to receive and send data.

API Reference
-------------

.. include-build-file:: inc/i2c_master.inc

.. only:: SOC_I2C_SUPPORT_SLAVE

    .. include-build-file:: inc/i2c_slave.inc

.. include-build-file:: inc/components/esp_driver_i2c/include/driver/i2c_types.inc
.. include-build-file:: inc/components/hal/include/hal/i2c_types.inc
