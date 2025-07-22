Parallel IO RX Driver
=====================

:link_to_translation:`zh_CN:[中文]`

This document describes the functionality of the Parallel IO RX driver in ESP-IDF. The table of contents is as follows:

.. contents::
    :local:
    :depth: 2

Introduction
------------

The Parallel IO RX unit is part of the general parallel interface, hereinafter referred to as the RX unit. It supports data reception from external devices via GDMA on a parallel bus. Given the flexibility of IO data, the RX unit can be used as a general interface to connect various peripherals. The main application scenarios of this driver include:

- High-speed data acquisition, such as camera and sensor data reading
- High-speed parallel communication with other hosts as a slave device
- Logic analyzer and signal monitoring applications

Quick Start
-----------

This section will quickly guide you on how to use the RX unit driver. Through a simple example demonstrating data reception with a soft delimiter, it shows how to create and start an RX unit, initiate a receive transaction, and register event callback functions. The general usage process is as follows:

Creating and Enabling the RX Unit
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

First, we need to create an RX unit instance. The following code shows how to create an RX unit instance:

.. code:: c

    parlio_rx_unit_handle_t rx_unit = NULL;
    parlio_rx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,      // Select the default clock source
        .data_width = 4,                        // Data width is 4 bits
        .clk_in_gpio_num = -1,                  // Do not use an external clock source
        .clk_out_gpio_num = EXAMPLE_PIN_CLK,    // Output clock pin
        .valid_gpio_num = EXAMPLE_PIN_VALID,    // Valid signal pin
        .data_gpio_nums = {
            EXAMPLE_PIN_DATA0,
            EXAMPLE_PIN_DATA1,
            EXAMPLE_PIN_DATA2,
            EXAMPLE_PIN_DATA3,
            [4 ... (PARLIO_RX_UNIT_MAX_DATA_WIDTH - 1)] = -1,
        },
        .exp_clk_freq_hz = 1 * 1000 * 1000,     // Expected clock frequency is 1 MHz
        .trans_queue_depth = 10,                // Transaction queue depth is 10
        .max_recv_size = 1024,                  // Maximum receive size is 1024 bytes
    };
    // Create RX unit instance
    ESP_ERROR_CHECK(parlio_new_rx_unit(&config, &rx_unit));
    // Enable RX unit and reset transaction queue
    ESP_ERROR_CHECK(parlio_rx_unit_enable(rx_unit, true));

When creating an RX unit instance, we need to configure parameters such as the clock source, data width, and expected clock frequency through :cpp:type:`parlio_rx_unit_config_t`. Then call the :cpp:func:`parlio_new_rx_unit` function to create a new RX unit instance, which will return a handle pointing to the new instance. The instance handle is essentially a pointer to the RX unit memory object, of type :cpp:type:`parlio_rx_unit_handle_t`.

The following are the configuration parameters of the :cpp:type:`parlio_rx_unit_config_t` structure and their explanations:

.. list::
    -  :cpp:member:`parlio_rx_unit_config_t::clk_src` Sets the clock source of the RX unit. Available clock sources are listed in :cpp:type:`parlio_clock_source_t`, and only one can be selected. Different clock sources vary in resolution, accuracy, and power consumption.
    -  :cpp:member:`parlio_rx_unit_config_t::clk_in_gpio_num` Uses an external clock as the clock source, setting the corresponding GPIO number for clock input. Otherwise, set to -1, and the driver will use the internal :cpp:member:`parlio_rx_unit_config_t::clk_src` as the clock source.
    -  :cpp:member:`parlio_rx_unit_config_t::ext_clk_freq_hz` The frequency of the external input clock source, valid only when :cpp:member:`parlio_rx_unit_config_t::clk_in_gpio_num` is not -1.
    -  :cpp:member:`parlio_rx_unit_config_t::exp_clk_freq_hz` Sets the expected sample/bit clock frequency, which is divided from the internal or external clock regarding the clock source.
    -  :cpp:member:`parlio_rx_unit_config_t::clk_out_gpio_num` The GPIO number for the output clock signal (if supported). Set to -1 if not needed.
    -  :cpp:member:`parlio_rx_unit_config_t::data_width` The data bus width of the RX unit, must be a power of 2 and not greater than {IDF_TARGET_SOC_PARLIO_RX_UNIT_MAX_DATA_WIDTH}.
    -  :cpp:member:`parlio_rx_unit_config_t::data_gpio_nums` The GPIO numbers for RX data, unused GPIOs should be set to -1.
    -  :cpp:member:`parlio_rx_unit_config_t::valid_gpio_num` The GPIO number for the valid signal, set to -1 if not used. The valid signal indicates whether the data on the data lines are valid.
    -  :cpp:member:`parlio_rx_unit_config_t::trans_queue_depth` The depth of the internal transaction queue. The deeper the queue, the more transactions can be prepared in the pending queue.
    -  :cpp:member:`parlio_rx_unit_config_t::max_recv_size` The maximum receive size per transaction (in bytes). This decides the number of DMA nodes will be used for each transaction.
    -  :cpp:member:`parlio_rx_unit_config_t::flags` Usually used to fine-tune some behaviors of the driver, including the following options
    -  :cpp:member:`parlio_rx_unit_config_t::flags::free_clk` Whether the input external clock is a free-running clock. A free-running clock will always keep running (e.g. I2S bclk), a non-free-running clock will start when there are data transporting and stop when the bus idle (e.g. SPI).
    :SOC_PARLIO_RX_CLK_SUPPORT_GATING: -  :cpp:member:`parlio_rx_unit_config_t::flags::clk_gate_en` Enable RX clock gating, the output clock will be controlled by the valid gpio.
    :SOC_PARLIO_SUPPORT_SLEEP_RETENTION: -  :cpp:member:`parlio_rx_unit_config_t::flags::allow_pd` Set to allow power down. When this flag set, the driver will backup/restore the PARLIO registers before/after entering/exist sleep mode.

.. note::

    If all RX units in the current chip have been requested, the :cpp:func:`parlio_new_rx_unit` function will return the :c:macro:`ESP_ERR_NOT_FOUND` error.

The RX unit must be enabled before use. The enable function :cpp:func:`parlio_rx_unit_enable` can switch the internal state machine of the driver to the active state, which also includes some system service requests/registrations, such as requesting a power management lock and resetting the transaction queue. The corresponding disable function is :cpp:func:`parlio_rx_unit_disable`, which will release all system services.

Creating Delimiters
^^^^^^^^^^^^^^^^^^^

Before initiating receive transactions, we need to create delimiters that define when a frame starts and ends. The RX unit supports three types of delimiters:

**Level Delimiter**: Uses a level signal to split valid data into frames.

.. code:: c

    parlio_rx_delimiter_handle_t level_delimiter = NULL;
    parlio_rx_level_delimiter_config_t level_config = {
        .valid_sig_line_id = 4,                     // Use data line 4 as valid signal input
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,      // Sample on positive edge
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB, // Pack bits from MSB
        .eof_data_len = 256,                        // End of frame interrupt triggers after 256 bytes, if set to 0, the EOF will be triggered when the valid signal is disabled
        .timeout_ticks = 1000,                      // Timeout interrupt triggers after 1000 clock ticks since the valid signal is disabled but no enough data for EOF. If set to 0, the timeout interrupt will not be triggered
        .flags = {
            .active_low_en = false,                 // Active high level
        },
    };
    ESP_ERROR_CHECK(parlio_new_rx_level_delimiter(&level_config, &level_delimiter));

**Pulse Delimiter**: Uses pulse signals to split valid data into frames.

.. code:: c

    parlio_rx_delimiter_handle_t pulse_delimiter = NULL;
    parlio_rx_pulse_delimiter_config_t pulse_config = {
        .valid_sig_line_id = 4,                     // Use data line 4 as valid signal input
        .sample_edge = PARLIO_SAMPLE_EDGE_NEG,      // Sample on negative edge
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB, // Pack bits from MSB
        .eof_data_len = 128,                        // End of frame interrupt triggers after 128 bytes, if set to 0, the EOF will be triggered when the valid signal is disabled
        .timeout_ticks = 500,                       // Timeout interrupt triggers after 500 clock ticks since the valid signal is disabled but no enough data for EOF. If set to 0, the timeout interrupt will not be triggered
        .flags = {
            .start_bit_included = false,            // Start bit not included in data
            .end_bit_included = false,              // End bit not included in data
            .has_end_pulse = true,                  // Has end pulse to terminate
            .pulse_invert = false,                  // Do not invert pulse
        },
    };
    ESP_ERROR_CHECK(parlio_new_rx_pulse_delimiter(&pulse_config, &pulse_delimiter));

**Soft Delimiter**: Uses software-defined data length to split valid data into frames.

.. code:: c

    parlio_rx_delimiter_handle_t soft_delimiter = NULL;
    parlio_rx_soft_delimiter_config_t soft_config = {
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,      // Sample on positive edge
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB, // Pack bits from MSB
        .eof_data_len = 512,                        // End of frame after 512 bytes, since there is no other end condition, the soft delimiter must set this field
        .timeout_ticks = 0,                         // No timeout
    };
    ESP_ERROR_CHECK(parlio_new_rx_soft_delimiter(&soft_config, &soft_delimiter));

Initiating RX Receive Transactions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After enabling the RX unit and creating delimiters, we can configure receive parameters and call :cpp:func:`parlio_rx_unit_receive` to start the RX transaction. The following code shows how to initiate an RX unit receive transaction:

.. code:: c

    #define PAYLOAD_SIZE 512

    // Allocate DMA compatible buffer
    uint8_t *payload = heap_caps_calloc(1, PAYLOAD_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);

    // Configure RX unit receive parameters
    parlio_receive_config_t receive_config = {
        .delimiter = soft_delimiter,        // Use the soft delimiter created above
        .flags = {
            .partial_rx_en = false,         // Disable partial receive mode
            .indirect_mount = false,        // Direct mount to user buffer to DMA
        },
    };

    // Start soft delimiter (required for soft delimiter only)
    ESP_ERROR_CHECK(parlio_rx_soft_delimiter_start_stop(rx_unit, soft_delimiter, true));

    // Start receive transaction
    ESP_ERROR_CHECK(parlio_rx_unit_receive(rx_unit, payload, PAYLOAD_SIZE, &receive_config));

    // Wait for receive transaction to complete
    ESP_ERROR_CHECK(parlio_rx_unit_wait_all_done(rx_unit, 5000)); // Wait up to 5 seconds

    // Stop soft delimiter
    ESP_ERROR_CHECK(parlio_rx_soft_delimiter_start_stop(rx_unit, soft_delimiter, false));

The RX unit receives data in bytes, and the received data length depends on the delimiter configuration. Calling :cpp:func:`parlio_rx_unit_receive` starts the RX transaction, which requires parameters such as the unit handle, payload buffer, and payload size (in **bytes**). Additionally, specific configurations for the reception should be provided in :cpp:type:`parlio_receive_config_t`.

The following are the configuration parameters of the :cpp:type:`parlio_receive_config_t` structure and their explanations:

.. list::

    - :cpp:member:`parlio_receive_config_t::delimiter` The delimiter to be used for this receive transaction.
    - :cpp:member:`parlio_receive_config_t::flags` Usually used to fine-tune some behaviors of the reception, including the following options
    - :cpp:member:`parlio_receive_config_t::flags::partial_rx_en` Whether this is an infinite transaction that supposed to receive continuously and partially.
    - :cpp:member:`parlio_receive_config_t::flags::indirect_mount` Enable this flag to use an INTERNAL DMA buffer instead of the user payload buffer. The data will be copied to the payload in every interrupt.

:cpp:func:`parlio_rx_unit_receive` internally constructs a transaction descriptor and sends it to the work queue, which is usually scheduled in the ISR context. Therefore, when :cpp:func:`parlio_rx_unit_receive` returns, the transaction may not have started yet. Note that you cannot recycle or modify the contents of the payload before the transaction ends. By registering event callbacks through :cpp:func:`parlio_rx_unit_register_event_callbacks`, you can be notified when the transaction is complete. To ensure all pending transactions are completed, you can also call :cpp:func:`parlio_rx_unit_wait_all_done`, providing a blocking receive function.

Registering Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Since :cpp:func:`parlio_rx_unit_receive` is an asynchronous interface, we may want to know when the receive transaction is complete or when partial data is received. The following code shows how to register event callbacks:

.. code:: c

    static bool on_partial_receive_callback(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_ctx)
    {
        // Called when partial data is received (for infinite transactions). You can do simple processing in the callback, such as queueing, task operations, or copying the received data to the user buffer.
        return false; // Return true if high priority task should be woken up
    }

    static bool on_receive_done_callback(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_ctx)
    {
        // Called when receive transaction is complete
        BaseType_t high_task_wakeup = pdFalse;
        TaskHandle_t task = (TaskHandle_t)user_ctx;

        // Notify the waiting task
        vTaskNotifyGiveFromISR(task, &high_task_wakeup);
        return (high_task_wakeup == pdTRUE);
    }

    static bool on_timeout_callback(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_ctx)
    {
        // Called when receive timeout occurs
        return false;
    }

    parlio_rx_event_callbacks_t cbs = {
        .on_partial_receive = on_partial_receive_callback,
        .on_receive_done = on_receive_done_callback,
        .on_timeout = on_timeout_callback,
    };
    ESP_ERROR_CHECK(parlio_rx_unit_register_event_callbacks(rx_unit, &cbs, xTaskGetCurrentTaskHandle()));

When the RX unit generates events such as receive done or timeout, it will notify the CPU via interrupts. If you need to call a function when a specific event occurs, you can call :cpp:func:`parlio_rx_unit_register_event_callbacks` to register event callbacks to the RX unit driver's interrupt service routine (ISR). Since the callback function is called in the ISR, complex operations (including any operations that may cause blocking) should be avoided in the callback function to avoid affecting the system's real-time performance.

For the event callbacks supported by the RX unit, refer to :cpp:type:`parlio_rx_event_callbacks_t`:

- :cpp:member:`parlio_rx_event_callbacks_t::on_partial_receive` Sets the callback function for the "partial data received" event, with the function prototype declared as :cpp:type:`parlio_rx_callback_t`.
- :cpp:member:`parlio_rx_event_callbacks_t::on_receive_done` Sets the callback function for the "receive complete" event, with the function prototype declared as :cpp:type:`parlio_rx_callback_t`.
- :cpp:member:`parlio_rx_event_callbacks_t::on_timeout` Sets the callback function for the "receive timeout" event, with the function prototype declared as :cpp:type:`parlio_rx_callback_t`. The timeout ticks is determined by the :cpp:member:`parlio_rx_level_delimiter_config_t::timeout_ticks`, :cpp:member:`parlio_rx_pulse_delimiter_config_t::timeout_ticks` or :cpp:member:`parlio_rx_soft_delimiter_config_t::timeout_ticks`.

Resource Recycling
^^^^^^^^^^^^^^^^^^

When the RX unit is no longer needed, the :cpp:func:`parlio_del_rx_unit` function should be called to release software and hardware resources. Ensure the RX unit is disabled before deletion. Also remember to delete the delimiters.

.. code:: c

    ESP_ERROR_CHECK(parlio_rx_unit_disable(rx_unit));
    ESP_ERROR_CHECK(parlio_del_rx_unit(rx_unit));
    ESP_ERROR_CHECK(parlio_del_rx_delimiter(soft_delimiter));
    free(payload);

Advanced Features
-----------------

After understanding the basic usage, we can further explore more advanced features of the RX unit driver.

Using an External Clock as the RX Unit Clock Source
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The RX unit can choose various clock sources, among which the external clock source is special. We enable the external clock source input by configuring :cpp:member:`parlio_rx_unit_config_t::clk_src`, :cpp:member:`parlio_rx_unit_config_t::clk_in_gpio_num`, and :cpp:member:`parlio_rx_unit_config_t::ext_clk_freq_hz`:

.. code-block:: c
   :emphasize-lines: 3,5,6

    parlio_rx_unit_handle_t rx_unit = NULL;
    parlio_rx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_EXTERNAL,         // Select external clock source
        .data_width = 4,                            // Data width is 4 bits
        .clk_in_gpio_num = EXAMPLE_PIN_CLK_IN,      // Set external clock source input pin
        .ext_clk_freq_hz = 10 * 1000 * 1000,       // External clock source frequency is 10 MHz
        .exp_clk_freq_hz = 10 * 1000 * 1000,       // Expected clock frequency matches external
        .valid_gpio_num = EXAMPLE_PIN_VALID,        // Valid signal pin
        .data_gpio_nums = {
            EXAMPLE_PIN_DATA0,
            EXAMPLE_PIN_DATA1,
            EXAMPLE_PIN_DATA2,
            EXAMPLE_PIN_DATA3,
            [4 ... (PARLIO_RX_UNIT_MAX_DATA_WIDTH - 1)] = -1,
        },
        .trans_queue_depth = 10,
        .max_recv_size = 1024,
        .flags = {
            .free_clk = true,                       // External clock is free-running
        },
    };
    // Create RX unit instance
    ESP_ERROR_CHECK(parlio_new_rx_unit(&config, &rx_unit));
    // Enable RX unit
    ESP_ERROR_CHECK(parlio_rx_unit_enable(rx_unit, true));

.. note::
    When using an external clock source, ensure that :cpp:member:`parlio_rx_unit_config_t::ext_clk_freq_hz` matches the actual frequency of the external clock for proper operation.

Infinite Receive Transactions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The RX unit supports infinite receive transactions where it continuously receives data in a streaming fashion. This is useful for applications like logic analyzers or continuous data monitoring:

.. code:: c

    // Configure infinite receive transaction
    parlio_receive_config_t receive_config = {
        .delimiter = soft_delimiter,
        .flags = {
            .partial_rx_en = true,          // Enable infinite/partial receive mode
            .indirect_mount = true,         // Use internal buffer to avoid data corruption
        },
    };

    // Start soft delimiter
    ESP_ERROR_CHECK(parlio_rx_soft_delimiter_start_stop(rx_unit, soft_delimiter, true));

    // Start infinite receive transaction
    ESP_ERROR_CHECK(parlio_rx_unit_receive(rx_unit, payload, PAYLOAD_SIZE, &receive_config));

    // The transaction will continue indefinitely, with partial receive callbacks being triggered as data is received.
    // Use parlio_rx_soft_delimiter_start_stop to stop the transaction when needed.

    vTaskDelay(pdMS_TO_TICKS(5000)); // Let it run for 5 seconds

    // Stop the infinite transaction
    ESP_ERROR_CHECK(parlio_rx_soft_delimiter_start_stop(rx_unit, soft_delimiter, false));

In infinite receive mode, the :cpp:member:`parlio_rx_event_callbacks_t::on_partial_receive` callback will be triggered each time the internal buffer is filled, and the data will be copied to the user buffer if :cpp:member:`parlio_receive_config_t::flags::indirect_mount` is enabled.

ISR Context Receive
^^^^^^^^^^^^^^^^^^^

For applications requiring very low latency, the RX unit driver provides :cpp:func:`parlio_rx_unit_receive_from_isr` which can be called from ISR context, such as within event callbacks:

.. code:: c

    static bool on_receive_done_isr_callback(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_ctx)
    {
        // Queue another receive transaction immediately from ISR context
        parlio_receive_config_t *config = (parlio_receive_config_t *)user_ctx;
        uint8_t *next_buffer = get_next_buffer(); // User-defined function

        bool hp_task_woken = false;
        esp_err_t ret = parlio_rx_unit_receive_from_isr(rx_unit, next_buffer, BUFFER_SIZE, config, &hp_task_woken);
        if (ret != ESP_OK) {
            // Handle error
        }

        return hp_task_woken;
    }

Power Management
^^^^^^^^^^^^^^^^

When power management :ref:`CONFIG_PM_ENABLE` is enabled, the system may adjust or disable the clock source before entering sleep, causing the RX unit's internal time base to not work as expected.

To prevent this, the RX unit driver internally creates a power management lock. The type of lock is set according to different clock sources. The driver will acquire the lock in :cpp:func:`parlio_rx_unit_enable` and release the lock in :cpp:func:`parlio_rx_unit_disable`. This means that regardless of the power management policy, the system will not enter sleep mode, and the clock source will not be disabled or adjusted between these two functions, ensuring that any RX transaction can work normally.

.. only:: SOC_PARLIO_SUPPORT_SLEEP_RETENTION

    In addition to turning off the clock source, the system can also turn off the RX unit's power to further reduce power consumption when entering sleep mode. To achieve this, set :cpp:member:`parlio_rx_unit_config_t::allow_pd` to ``true``. Before the system enters sleep mode, the RX unit's register context will be backed up to memory and restored when the system wakes up. Note that enabling this option can reduce power consumption but will increase memory usage.

Thread Safety
^^^^^^^^^^^^^

The driver uses critical sections to ensure atomic operations on registers. Key members in the driver handle are also protected by critical sections. The driver's internal state machine uses atomic instructions to ensure thread safety, and uses thread-safe FreeRTOS queues to manage receive transactions. Therefore, RX unit driver APIs can be used in a multi-threaded environment without extra locking.

Cache Safety
^^^^^^^^^^^^

When the file system performs Flash read/write operations, the system temporarily disables the Cache function to avoid errors when loading instructions and data from Flash. This will cause the RX unit's interrupt handler to be unresponsive during this period, preventing user callback functions from being executed in time. If you want the interrupt handler to run normally while the Cache is disabled, you can enable the :ref:`CONFIG_PARLIO_RX_ISR_CACHE_SAFE` option.

.. note::

    Note that after enabling this option, all interrupt callback functions and their context data **must reside in internal memory**. Because when the Cache is disabled, the system cannot load data and instructions from external memory.

.. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND or SOC_SPIRAM_XIP_SUPPORTED

    .. note::

        When the following options are enabled, the Cache will not be disabled automatically during Flash read/write operations. You don't have to enable the :ref:`CONFIG_PARLIO_RX_ISR_CACHE_SAFE`.

        .. list::
            :SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND: - :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND`
            :SOC_SPIRAM_XIP_SUPPORTED: - :ref:`CONFIG_SPIRAM_XIP_FROM_PSRAM`

Performance
^^^^^^^^^^^

To improve the real-time response capability of interrupt handling, the RX unit driver provides the :ref:`CONFIG_PARLIO_RX_ISR_HANDLER_IN_IRAM` option. Enabling this option will place the interrupt handler in internal RAM, reducing the latency caused by cache misses when loading instructions from Flash.

.. note::

    However, user callback functions and context data called by the interrupt handler may still be located in Flash, and cache miss issues will still exist. Users need to place callback functions and data in internal RAM, for example, using :c:macro:`IRAM_ATTR` and :c:macro:`DRAM_ATTR`.

And please also take care that, when the :cpp:member:`parlio_receive_config_t::flags::indirect_mount` option is enabled, the driver will use an internal DMA buffer instead of the user payload buffer. The data will be copied to the payload in the interrupt. Therefore, using this option will slightly reduce the data throughput efficiency.

Other Kconfig Options
^^^^^^^^^^^^^^^^^^^^^

- :ref:`CONFIG_PARLIO_ENABLE_DEBUG_LOG` option allows forcing the enablement of all debug logs of the RX unit driver, regardless of the global log level setting. Enabling this option can help developers obtain more detailed log information during debugging, making it easier to locate and solve problems. This option is shared with the TX unit driver.

Resource Consumption
^^^^^^^^^^^^^^^^^^^^

Use the :doc:`/api-guides/tools/idf-size` tool to view the code and data consumption of the RX unit driver. The following are the test conditions (taking ESP32-H2 as an example):

- The compiler optimization level is set to ``-Os`` to ensure the minimum code size.
- The default log level is set to ``ESP_LOG_INFO`` to balance debugging information and performance.
- The following driver optimization options are disabled:
    - :ref:`CONFIG_PARLIO_RX_ISR_HANDLER_IN_IRAM` - The interrupt handler is not placed in IRAM.
    - :ref:`CONFIG_PARLIO_RX_ISR_CACHE_SAFE` - The Cache safety option is not enabled.

**Note that the following data is not precise and is for reference only. The data may vary on different chip models and different versions of IDF.**

+-----------------+------------+-------+------+-------+-------+------------+---------+-------+
| Component Layer | Total Size | DIRAM | .bss | .data | .text | Flash Code | .rodata | .text |
+=================+============+=======+======+=======+=======+============+=========+=======+
| soc             | 100        | 0     | 0    | 0     | 0     | 100        | 0       | 100   |
+-----------------+------------+-------+------+-------+-------+------------+---------+-------+
| hal             | 18         | 0     | 0    | 0     | 0     | 18         | 0       | 18    |
+-----------------+------------+-------+------+-------+-------+------------+---------+-------+
| driver          | 9666       | 0     | 0    | 0     | 0     | 9666       | 618     | 9048  |
+-----------------+------------+-------+------+-------+-------+------------+---------+-------+

In addition, each RX unit handle dynamically allocates about ``700`` bytes of memory from the heap (transaction queue depth is 10). If the :cpp:member:`parlio_rx_unit_config_t::flags::allow_pd` option is enabled, each RX unit will consume an additional ``32`` bytes of memory during sleep to save the register context.

Application Examples
---------------------

* :example:`peripherals/parlio/parlio_rx/logic_analyzer` demonstrates how to use the Parallel IO RX peripheral to implement a logic analyzer. This analyzer can sample data on multiple GPIOs at high frequency, monitor internal or external signals, and save the raw sampled data to Flash or output it through a TCP stream.

API Reference
-------------

.. include-build-file:: inc/parlio_rx.inc
