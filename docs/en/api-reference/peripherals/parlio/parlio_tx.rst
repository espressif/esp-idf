Parallel IO TX Driver
=====================

:link_to_translation:`zh_CN:[中文]`

This document describes the functionality of the Parallel IO TX driver in ESP-IDF. The table of contents is as follows:

.. contents::
    :local:
    :depth: 2

Introduction
------------

The Parallel IO TX unit is part of the general parallel interface, hereinafter referred to as the TX unit. It supports data communication between external devices and internal memory via GDMA on a parallel bus. Given the flexibility of IO data, the TX unit can be used as a general interface to connect various peripherals. The main application scenarios of this driver include:

- Driving LCD, LED displays
- High-speed parallel communication with other devices
- Simulating the timing of other peripherals when the number of peripherals is insufficient.

Quick Start
-----------

This section will quickly guide you on how to use the TX unit driver. Through a simple example simulating QPI (Quad Peripheral Interface) transmission timing, it demonstrates how to create and start a TX unit, initiate a transmission transaction, and register event callback functions. The general usage process is as follows:

Creating and Enabling the TX Unit
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

First, we need to create a TX unit instance. The following code shows how to create a TX unit instance to simulate QPI:

.. code:: c

    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,      // Select the default clock source
        .data_width = 4,                        // Data width is 4 bits
        .clk_in_gpio_num = -1,                  // Do not use an external clock source
        .valid_gpio_num = EXAMPLE_PIN_CS,       // Use the valid signal as chip select
        .clk_out_gpio_num = EXAMPLE_PIN_CLK,
        .data_gpio_nums = {
            EXAMPLE_PIN_DATA0,
            EXAMPLE_PIN_DATA1,
            EXAMPLE_PIN_DATA2,
            EXAMPLE_PIN_DATA3,
        },
        .output_clk_freq_hz = 10 * 1000 * 1000, // Output clock frequency is 10 MHz
        .trans_queue_depth = 32,                // Transaction queue depth is 32
        .max_transfer_size = 256,               // Maximum transfer size is 256 bytes
        .sample_edge = PARLIO_SAMPLE_EDGE_NEG,  // Sample data on the falling edge of the clock
        .flags = {
            .invert_valid_out = true, // The valid signal is high by default, inverted to simulate the chip select signal CS in QPI timing
        }
    };
    // Create TX unit instance
    ESP_ERROR_CHECK(parlio_new_tx_unit(&config, &tx_unit));
    // Enable TX unit
    ESP_ERROR_CHECK(parlio_tx_unit_enable(tx_unit));

When creating a TX unit instance, we need to configure parameters such as the clock source, data width, and output clock frequency through :cpp:type:`parlio_tx_unit_config_t`. Then call the :cpp:func:`parlio_new_tx_unit` function to create a new TX unit instance, which will return a handle pointing to the new instance. The instance handle is essentially a pointer to the TX unit memory object, of type :cpp:type:`parlio_tx_unit_handle_t`.

The following are the configuration parameters of the :cpp:type:`parlio_tx_unit_config_t` structure and their explanations:

.. list::
    -  :cpp:member:`parlio_tx_unit_config_t::clk_src` Sets the clock source of the TX unit. Available clock sources are listed in :cpp:type:`parlio_clock_source_t`, and only one can be selected. Different clock sources vary in resolution, accuracy, and power consumption.
    -  :cpp:member:`parlio_tx_unit_config_t::clk_in_gpio_num` Uses an external clock as the clock source, setting the corresponding GPIO number for clock input. Otherwise, set to -1, and the driver will use the internal :cpp:member:`parlio_tx_unit_config_t::clk_src` as the clock source. This option has higher priority than :cpp:member:`parlio_tx_unit_config_t::clk_src`.
    -  :cpp:member:`parlio_tx_unit_config_t::input_clk_src_freq_hz` The frequency of the external input clock source, valid only when :cpp:member:`parlio_tx_unit_config_t::clk_in_gpio_num` is not -1.
    -  :cpp:member:`parlio_tx_unit_config_t::output_clk_freq_hz` Sets the frequency of the output clock, derived from the internal or external clock source. Note that not all frequencies can be achieved, and the driver will automatically adjust to the nearest frequency when the set frequency cannot be achieved.
    -  :cpp:member:`parlio_tx_unit_config_t::clk_out_gpio_num` The GPIO number for the output clock signal.
    -  :cpp:member:`parlio_tx_unit_config_t::data_width` The data bus width of the TX unit, must be a power of 2 and not greater than {IDF_TARGET_SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH}.
    -  :cpp:member:`parlio_tx_unit_config_t::data_gpio_nums` The GPIO numbers for TX data, unused GPIOs should be set to -1.
    -  :cpp:member:`parlio_tx_unit_config_t::valid_gpio_num` The GPIO number for the valid signal, set to -1 if not used. The valid signal stays high level when the TX unit is transmitting data. Note that enabling the valid signal in some specific chips will occupy the MSB data bit, reducing the maximum data width of the TX unit by 1 bit. In this case, the maximum configurable data bus width is :c:macro:`SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH` / 2. Please check the return value of :cpp:func:`parlio_new_tx_unit`.
    -  :cpp:member:`parlio_tx_unit_config_t::valid_start_delay` The number of clock cycles the valid signal will stay high level before the TX unit starts transmitting data. This configuration option depends on specific hardware features, and if enabled on unsupported chips or configured with invalid values, you will see an error message like ``invalid valid delay``.
    -  :cpp:member:`parlio_tx_unit_config_t::valid_stop_delay` The number of clock cycles the valid signal will stay high level after the TX unit finishes transmitting data. This configuration option depends on specific hardware features, and if enabled on unsupported chips or configured with invalid values, you will see an error message like ``invalid valid delay``.
    -  :cpp:member:`parlio_tx_unit_config_t::trans_queue_depth` The depth of the internal transaction queue. The deeper the queue, the more transactions can be prepared in the pending queue.
    -  :cpp:member:`parlio_tx_unit_config_t::max_transfer_size` The maximum transfer size per transaction (in bytes).
    -  :cpp:member:`parlio_tx_unit_config_t::dma_burst_size` The DMA burst transfer size (in bytes), must be a power of 2.
    -  :cpp:member:`parlio_tx_unit_config_t::sample_edge` The data sampling edge of the TX unit.
    -  :cpp:member:`parlio_tx_unit_config_t::bit_pack_order` Sets the order of data bits within a byte (valid only when data width < 8).
    -  :cpp:member:`parlio_tx_unit_config_t::flags` Usually used to fine-tune some behaviors of the driver, including the following options
    -  :cpp:member:`parlio_tx_unit_config_t::flags::invert_valid_out` Determines whether to invert the valid signal before sending it to the GPIO pin.
    :SOC_PARLIO_TX_CLK_SUPPORT_GATING: -  :cpp:member:`parlio_tx_unit_config_t::flags::clk_gate_en` Enables TX unit clock gating, the output clock will be controlled by the MSB bit of the data bus, i.e., by writing a high level to :cpp:member:`parlio_tx_unit_config_t::data_gpio_nums` [:c:macro:`SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH` - 1] to enable clock output, and a low level to disable it. In this case, the data bus width needs to be configured as :c:macro:`SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH`. Note that if both the valid signal output and clock gating are enabled, clock gating can come from the valid signal. there is no limit on the data bus width. (Note that in some chips, the valid signal occupies the MSB data bit, so the maximum configurable data bus width is :c:macro:`SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH` / 2)
    :SOC_PARLIO_SUPPORT_SLEEP_RETENTION: -  :cpp:member:`parlio_tx_unit_config_t::flags::allow_pd` Configures whether the driver allows the system to turn off the peripheral power in sleep mode. Before entering sleep, the system will back up the TX unit register context, and these contexts will be restored when the system exits sleep mode. Turning off the peripheral can save more power, but at the cost of consuming more memory to save the register context. You need to balance power consumption and memory usage. This configuration option depends on specific hardware features, and if enabled on unsupported chips, you will see an error message like ``register back up is not supported``.

.. note::

    If all TX units in the current chip have been requested, the :cpp:func:`parlio_new_tx_unit` function will return the :c:macro:`ESP_ERR_NOT_FOUND` error.

The TX unit must be enabled before use. The enable function :cpp:func:`parlio_tx_unit_enable` can switch the internal state machine of the driver to the active state, which also includes some system service requests/registrations, such as requesting a power management lock. The corresponding disable function is :cpp:func:`parlio_tx_unit_disable`, which will release all system services.

.. note::

    When calling the :cpp:func:`parlio_tx_unit_enable` and :cpp:func:`parlio_tx_unit_disable` functions, they need to be used in pairs. This means you cannot call the :cpp:func:`parlio_tx_unit_enable` or :cpp:func:`parlio_tx_unit_disable` function twice in a row. This paired calling principle ensures the correct management and release of resources.

.. note::

    Please note that after the TX unit is enabled, it will check the current work queue. If there are pending transmission transactions in the queue, the driver will immediately initiate a transmission.

Initiating TX Transmission Transactions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After enabling the TX unit, we can configure some parameters for the transmission and call the :cpp:func:`parlio_tx_unit_transmit` to start the TX transaction. The following code shows how to initiate a TX unit transmission transaction:

.. code:: c

    #define PAYLOAD_SIZE 128

    // Configure TX unit transmission parameters
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00, // All data lines are low in idle state
    };

    // Prepare the data to be sent
    uint8_t payload[PAYLOAD_SIZE] = {0};
    for (int i = 0; i < PAYLOAD_SIZE; i++) {
        payload[i] = i;
    }

    // The first call to parlio_tx_unit_transmit will start the transmission immediately as there is no ongoing transaction
    ESP_ERROR_CHECK(parlio_tx_unit_transmit(tx_unit, payload, PAYLOAD_SIZE * sizeof(uint8_t) * 8, &transmit_config));
    // The second call to parlio_tx_unit_transmit may queue the transaction if the previous one is not completed, and it will be scheduled in the ISR context after the previous transaction is completed
    ESP_ERROR_CHECK(parlio_tx_unit_transmit(tx_unit, payload, PAYLOAD_SIZE * sizeof(uint8_t) * 8, &transmit_config));
    // (Optional) Wait for the TX unit to complete all transactions
    ESP_ERROR_CHECK(parlio_tx_unit_wait_all_done(tx_unit, -1));

The TX unit transmits data in bits, and the transmission bit length must be a multiple of the corresponding bus width. Calling :cpp:func:`parlio_tx_unit_transmit`  to start the TX transaction, which requires parameters such as the unit handle, payload buffer, and payload size (in **bits**). Additionally, specific configurations for the transmission should be provided in :cpp:type:`parlio_transmit_config_t`.

The following are the configuration parameters of the :cpp:type:`parlio_transmit_config_t` structure and their explanations:

.. list::

    - :cpp:member:`parlio_transmit_config_t::idle_value` Sets the value on the data lines when the TX unit is idle after transmission. This value will remain even after calling :cpp:func:`parlio_tx_unit_disable` to disable the TX unit.
    :SOC_BITSCRAMBLER_SUPPORTED: - :cpp:member:`parlio_transmit_config_t::bitscrambler_program` The pointer to the bitscrambler program binary file. Set to ``NULL`` if the bitscrambler is not used in this transmission.
    - :cpp:member:`parlio_transmit_config_t::flags` Usually used to fine-tune some behaviors of the transmission, including the following options
    - :cpp:member:`parlio_transmit_config_t::flags::queue_nonblocking` Sets whether the function needs to wait when the transmission queue is full. If this value is set to ``true``, the function will immediately return the error code :c:macro:`ESP_ERR_INVALID_STATE` when the queue is full. Otherwise, the function will block the current thread until there is space in the transmission queue.
    :SOC_PARLIO_TX_SUPPORT_LOOP_TRANSMISSION: - :cpp:member:`parlio_transmit_config_t::flags::loop_transmission` Setting this to ``true`` enables infinite loop transmission. In this case, the transmission will not stop unless manually calling :cpp:func:`parlio_tx_unit_disable`, and no "trans_done" event will be generated. Since the loop is controlled by DMA, the TX unit can generate periodic sequences with minimal CPU intervention.

:cpp:func:`parlio_tx_unit_transmit` internally constructs a transaction descriptor and sends it to the work queue, which is usually scheduled in the ISR context. Therefore, when :cpp:func:`parlio_tx_unit_transmit` returns, the transaction may not have started yet. Note that you cannot recycle or modify the contents of the payload before the transaction ends. By registering event callbacks through :cpp:func:`parlio_tx_unit_register_event_callbacks`, you can be notified when the transaction is complete. To ensure all pending transactions are completed, you can also call :cpp:func:`parlio_tx_unit_wait_all_done`, providing a blocking send function.

With simple configuration, we can send data in QPI format, as shown in the waveform below:

.. wavedrom:: /../_static/diagrams/parlio/parlio_tx/sim_qpi_waveform.json

Registering Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Since :cpp:func:`parlio_tx_unit_transmit` is an asynchronous interface, we may want to know when the transmission transaction is complete. The following code shows how to register an event callback for the transmission transaction done:

.. code:: c

    static bool test_parlio_tx_done_callback(parlio_tx_unit_handle_t tx_unit, const parlio_tx_done_event_data_t *edata, void *user_ctx)
    {
        // General process for handling event callbacks:
        // 1. Retrieve user context data from user_ctx (passed in from test_parlio_tx_done_callback)
        // 2. Perform user-defined operations
        // 3. Return whether a high-priority task was woken up during the above operations to notify the scheduler to switch tasks

        BaseType_t high_task_wakeup = pdFalse;
        // Use FreeRTOS task handle as user context
        TaskHandle_t task = (TaskHandle_t)user_ctx;
        // Send task notification to the specified task upon transmission done
        vTaskNotifyGiveFromISR(task, &high_task_wakeup);
        // Return whether a high-priority task was woken up by this function
        return (high_task_wakeup == pdTRUE);
    }

    parlio_tx_event_callbacks_t cbs = {
        // Set test_parlio_tx_done_callback as the event callback function for transmission done
        .on_trans_done = test_parlio_tx_done_callback,
    };
    ESP_ERROR_CHECK(parlio_tx_unit_register_event_callbacks(tx_unit, &cbs, xTaskGetCurrentTaskHandle()));

When the TX unit generates events such as transmission done, it will notify the CPU via interrupts. If you need to call a function when a specific event occurs, you can call :cpp:func:`parlio_tx_unit_register_event_callbacks` to register event callbacks to the TX unit driver's interrupt service routine (ISR). Since the callback function is called in the ISR, complex operations (including any operations that may cause blocking) should be avoided in the callback function to avoid affecting the system's real-time performance. :cpp:func:`parlio_tx_unit_register_event_callbacks` also allows users to pass a context pointer to access user-defined data in the callback function.

For the event callbacks supported by the TX unit, refer to :cpp:type:`parlio_tx_event_callbacks_t`:

- :cpp:member:`parlio_tx_event_callbacks_t::on_trans_done` Sets the callback function for the "transmission complete" event, with the function prototype declared as :cpp:type:`parlio_tx_done_callback_t`.

Resource Recycling
^^^^^^^^^^^^^^^^^^

When the TX unit is no longer needed, the :cpp:func:`parlio_del_tx_unit` function should be called to release software and hardware resources. Ensure the TX unit is disabled before deletion.

.. code:: c

    ESP_ERROR_CHECK(parlio_tx_unit_disable(tx_unit));
    ESP_ERROR_CHECK(parlio_del_tx_unit(tx_unit));

Advanced Features
-----------------

After understanding the basic usage, we can further explore more advanced features of the TX unit driver.

Using an External Clock as the TX Unit Clock Source
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The TX unit can choose various clock sources, among which the external clock source is special. We enable the external clock source input by configuring :cpp:member:`parlio_tx_unit_config_t::clk_src`, :cpp:member:`parlio_tx_unit_config_t::clk_in_gpio_num`, and :cpp:member:`parlio_tx_unit_config_t::input_clk_src_freq_hz`:

.. code-block:: c
   :emphasize-lines: 3,5,6

    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_EXTERNAL,         // Select external clock source
        .data_width = 4,                            // Data width is 4 bits
        .clk_in_gpio_num = EXAMPLE_PIN_CLK_IN,      // Set external clock source input pin
        .input_clk_src_freq_hz = 10 * 1000 * 1000,  // External clock source frequency is 10 MHz
        .valid_gpio_num = -1,                       // Do not use valid signal
        .clk_out_gpio_num = EXAMPLE_PIN_CLK_OUT,
        .data_gpio_nums = {
            EXAMPLE_PIN_DATA0,
            EXAMPLE_PIN_DATA1,
            EXAMPLE_PIN_DATA2,
            EXAMPLE_PIN_DATA3,
        },
        .output_clk_freq_hz = 5 * 1000 * 1000, // Output clock frequency is 5 MHz. Note that it cannot exceed the input clock frequency
        .trans_queue_depth = 32,
        .max_transfer_size = 256,
        .sample_edge = PARLIO_SAMPLE_EDGE_NEG,  // Sample data on the falling edge of the clock
    };
    // Create TX unit instance
    ESP_ERROR_CHECK(parlio_new_tx_unit(&config, &tx_unit));
    // Enable TX unit
    ESP_ERROR_CHECK(parlio_tx_unit_enable(tx_unit));

   #define PAYLOAD_SIZE 64

    // Configure TX unit transmission parameters
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00, // All data lines are low in idle state
    };

    // Prepare the data to be sent
    uint8_t payload[PAYLOAD_SIZE] = {0};
    for (int i = 0; i < PAYLOAD_SIZE; i++) {
        payload[i] = i;
    }

    // Start transmission transaction
    ESP_ERROR_CHECK(parlio_tx_unit_transmit(tx_unit, payload, PAYLOAD_SIZE * sizeof(uint8_t) * 8, &transmit_config));

The waveform of the external clock input is shown below:

.. wavedrom:: /../_static/diagrams/parlio/parlio_tx/external_clock_input_waveform.json

.. note::
    The ratio of :cpp:member:`parlio_tx_unit_config_t::input_clk_src_freq_hz` to :cpp:member:`parlio_tx_unit_config_t::output_clk_freq_hz` determines the internal clock division factor of the TX unit.
    When the actual frequency of the external clock differs from :cpp:member:`parlio_tx_unit_config_t::input_clk_src_freq_hz`, the actual output clock frequency generated by the TX unit will also change accordingly.

.. only:: SOC_PARLIO_TX_SUPPORT_LOOP_TRANSMISSION

    Infinite Loop Transmission
    ^^^^^^^^^^^^^^^^^^^^^^^^^^

    {IDF_TARGET_NAME} supports infinite loop transmission, where the TX unit can generate periodic sequences without CPU intervention. By configuring :cpp:member:`parlio_transmit_config_t::flags::loop_transmission`, we can enable infinite loop transmission

    .. code-block:: c
       :emphasize-lines: 32

        parlio_tx_unit_handle_t tx_unit = NULL;
        parlio_tx_unit_config_t config = {
            .clk_src = PARLIO_CLK_SRC_DEFAULT,      // Select the default clock source
            .data_width = 4,                        // Data width is 4 bits
            .clk_in_gpio_num = -1,                  // Do not use an external clock source
            .valid_gpio_num = -1,                   // Do not use valid signal
            .clk_out_gpio_num = EXAMPLE_PIN_CLK,
            .data_gpio_nums = {
                EXAMPLE_PIN_DATA0,
                EXAMPLE_PIN_DATA1,
                EXAMPLE_PIN_DATA2,
                EXAMPLE_PIN_DATA3,
            },
            .output_clk_freq_hz = 10 * 1000 * 1000, // Output clock frequency is 10 MHz
            .trans_queue_depth = 32,
            .max_transfer_size = 256,
            .sample_edge = PARLIO_SAMPLE_EDGE_NEG,  // Sample data on the falling edge of the clock
            .flags = {
                .invert_valid_out = true,  // The valid signal is high by default, inverted to simulate the chip select signal CS in QPI timing
            }
        };
        // Create TX unit instance
        ESP_ERROR_CHECK(parlio_new_tx_unit(&config, &tx_unit));
        // Enable TX unit
        ESP_ERROR_CHECK(parlio_tx_unit_enable(tx_unit));

        #define PAYLOAD_SIZE 64

        // Configure TX unit transmission parameters
        parlio_transmit_config_t transmit_config = {
            .idle_value = 0x00, // All data lines are low in idle state
            .loop_transmission = true, // Enable infinite loop transmission
        };

        // Prepare the data to be sent
        uint8_t payload[PAYLOAD_SIZE] = {0};
        for (int i = 0; i < PAYLOAD_SIZE; i++) {
            payload[i] = i;
        }

        // Start loop transmission transaction
        ESP_ERROR_CHECK(parlio_tx_unit_transmit(tx_unit, payload, PAYLOAD_SIZE * sizeof(uint8_t) * 8, &transmit_config));

    The waveform of the loop transmission is shown below:

    .. wavedrom:: /../_static/diagrams/parlio/parlio_tx/loop_transmission_waveform.json

    In this case, the transmission will not stop unless manually calling :cpp:func:`parlio_tx_unit_disable`, and no "trans_done" event will be generated.

    .. note::

        If you need to modify the transmission payload after enabling infinite loop transmission, you can configure :cpp:member:`parlio_transmit_config_t::flags::loop_transmission` and call :cpp:func:`parlio_tx_unit_transmit` again with a new payload buffer. The driver will switch to the new buffer after the old buffer is completely transmitted. Therefore, users need to maintain two buffers to avoid data inconsistency caused by premature modification or recycling of the old buffer.

.. only:: SOC_BITSCRAMBLER_SUPPORTED

    .. _parlio-tx-bitscrambler-decorator:

    Custom Bitstream Generation with BitScrambler
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    We can use the :doc:`BitScrambler </api-reference/peripherals/bitscrambler>` assembly code to control the data on the DMA path, thereby implementing some simple encoding work. Compared to using the CPU for encoding, the BitScrambler has higher performance and does not consume CPU resources, but is limited by the limited instruction memory of the BitScrambler, so it cannot implement complex encoding work.

    After writing the BitScrambler program, we can enable it by calling :cpp:func:`parlio_tx_unit_decorate_bitscrambler`. And configure the :cpp:member:`parlio_transmit_config_t::bitscrambler_program` to point to the binary file of the BitScrambler program. Different transmission transactions can use different BitScrambler programs. The binary file must conform to the BitScrambler assembly language specification, and will be loaded into the BitScrambler's instruction memory at runtime. For details on how to write and compile the BitScrambler program, please refer to :doc:`BitScrambler Programming Guide </api-reference/peripherals/bitscrambler>`.

    :cpp:func:`parlio_tx_unit_decorate_bitscrambler` and :cpp:func:`parlio_tx_unit_undecorate_bitscrambler` need to be used in pairs. When deleting the TX unit, you need to call :cpp:func:`parlio_tx_unit_undecorate_bitscrambler` first to remove the BitScrambler.

Power Management
^^^^^^^^^^^^^^^^

When power management :ref:`CONFIG_PM_ENABLE` is enabled, the system may adjust or disable the clock source before entering sleep, causing the TX unit's internal time base to not work as expected.

To prevent this, the TX unit driver internally creates a power management lock. The type of lock is set according to different clock sources. The driver will acquire the lock in :cpp:func:`parlio_tx_unit_enable` and release the lock in :cpp:func:`parlio_tx_unit_disable`. This means that regardless of the power management policy, the system will not enter sleep mode, and the clock source will not be disabled or adjusted between these two functions, ensuring that any TX transaction can work normally.

.. only:: SOC_PARLIO_SUPPORT_SLEEP_RETENTION

    In addition to turning off the clock source, the system can also turn off the TX unit's power to further reduce power consumption when entering sleep mode. To achieve this, set :cpp:member:`parlio_tx_unit_config_t::allow_pd` to ``true``. Before the system enters sleep mode, the TX unit's register context will be backed up to memory and restored when the system wakes up. Note that enabling this option can reduce power consumption but will increase memory usage. Therefore, when using this feature, you need to balance power consumption and memory usage.

Thread Safety
^^^^^^^^^^^^^

The driver uses critical sections to ensure atomic operations on registers. Key members in the driver handle are also protected by critical sections. The driver's internal state machine uses atomic instructions to ensure thread safety, and use thread-safe FreeRTOS queues to manage transmit transactions. Therefore, TX unit driver APIs can be used in a multi-threaded environment without extra locking.

Cache Safety
^^^^^^^^^^^^

When the file system performs Flash read/write operations, the system temporarily disables the Cache function to avoid errors when loading instructions and data from Flash. This will cause the TX unit's interrupt handler to be unresponsive during this period, preventing user callback functions from being executed in time. If you want the interrupt handler to run normally while the Cache is disabled, you can enable the :ref:`CONFIG_PARLIO_TX_ISR_CACHE_SAFE` option.

.. note::

    Note that after enabling this option, all interrupt callback functions and their context data **must reside in internal memory**. Because when the Cache is disabled, the system cannot load data and instructions from external memory.

.. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND or SOC_SPIRAM_XIP_SUPPORTED

    .. note::

        When the following options are enabled, the Cache will not be disabled automatically during Flash read/write operations. You don't have to enable the :ref:`CONFIG_PARLIO_TX_ISR_CACHE_SAFE`.

        .. list::
            :SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND: - :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND`
            :SOC_SPIRAM_XIP_SUPPORTED: - :ref:`CONFIG_SPIRAM_XIP_FROM_PSRAM`

Performance
^^^^^^^^^^^

To improve the real-time response capability of interrupt handling, the TX unit driver provides the :ref:`CONFIG_PARLIO_TX_ISR_HANDLER_IN_IRAM` option. Enabling this option will place the interrupt handler in internal RAM, reducing the latency caused by cache misses when loading instructions from Flash.

.. note::

    However, user callback functions and context data called by the interrupt handler may still be located in Flash, and cache miss issues will still exist. Users need to place callback functions and data in internal RAM, for example, using :c:macro:`IRAM_ATTR` and :c:macro:`DRAM_ATTR`.

Other Kconfig Options
^^^^^^^^^^^^^^^^^^^^^

- :ref:`CONFIG_PARLIO_ENABLE_DEBUG_LOG` option allows forcing the enablement of all debug logs of the TX unit driver, regardless of the global log level setting. Enabling this option can help developers obtain more detailed log information during debugging, making it easier to locate and solve problems. This option is shared with the RX unit driver.

Resource Consumption
^^^^^^^^^^^^^^^^^^^^

Use the :doc:`/api-guides/tools/idf-size` tool to view the code and data consumption of the TX unit driver. The following are the test conditions (taking ESP32-H2 as an example):

- The compiler optimization level is set to ``-Os`` to ensure the minimum code size.
- The default log level is set to ``ESP_LOG_INFO`` to balance debugging information and performance.
- The following driver optimization options are disabled:
    - :ref:`CONFIG_PARLIO_TX_ISR_HANDLER_IN_IRAM` - The interrupt handler is not placed in IRAM.
    - :ref:`CONFIG_PARLIO_TX_ISR_CACHE_SAFE` - The Cache safety option is not enabled.

**Note that the following data is not precise and is for reference only. The data may vary on different chip models and different versions of IDF.**

+-----------------+------------+-------+------+-------+-------+------------+---------+-------+
| Component Layer | Total Size | DIRAM | .bss | .data | .text | Flash Code | .rodata | .text |
+=================+============+=======+======+=======+=======+============+=========+=======+
| soc             | 92         | 0     | 0    | 0     | 0     | 92         | 0       | 92    |
+-----------------+------------+-------+------+-------+-------+------------+---------+-------+
| hal             | 18         | 0     | 0    | 0     | 0     | 18         | 0       | 18    |
+-----------------+------------+-------+------+-------+-------+------------+---------+-------+
| driver          | 6478       | 12    | 12   | 0     | 0     | 6466       | 586     | 5880  |
+-----------------+------------+-------+------+-------+-------+------------+---------+-------+

In addition, each TX unit handle dynamically allocates about ``800`` bytes of memory from the heap (transmission queue depth is 4). If the :cpp:member:`parlio_tx_unit_config_t::flags::allow_pd` option is enabled, each TX unit will consume an additional ``32`` bytes of memory during sleep to save the register context.

Application Examples
---------------------

.. list::

    - :example:`peripherals/parlio/parlio_tx/simple_rgb_led_matrix` demonstrates how to use the TX unit driver of {IDF_TARGET_NAME} to support HUB75 interface RGB LED matrix panels and use the LVGL library to display simple UI elements.
    :SOC_PARLIO_TX_SUPPORT_LOOP_TRANSMISSION: - :example:`peripherals/parlio/parlio_tx/advanced_rgb_led_matrix` demonstrates how to use the infinite loop transmission feature of the TX unit of {IDF_TARGET_NAME} to support HUB75 interface RGB LED matrix panels. Compared to the simple_rgb_led_matrix example, it does not require manual loop scanning and is more flexible.
    :SOC_PARLIO_SUPPORT_SPI_LCD: - :example:`peripherals/lcd/parlio_simulate` demonstrates how to use the TX unit driver of the parallel IO peripheral to drive screens with SPI or I80 interfaces.

API Reference
-------------

.. include-build-file:: inc/parlio_tx.inc
