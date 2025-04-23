UART DMA (UHCI)
===============

:link_to_translation:`zh_CN:[中文]`

This document describes the functionality of the UART DMA(UHCI) driver in ESP-IDF. The table of contents is as follows:

.. contents::
    :local:
    :depth: 2

Introduction
------------

This document shows how to use UART and DMA together for transmitting or receiving large data volumes using high baud rates. {IDF_TARGET_SOC_UART_HP_NUM} HP UART controllers on {IDF_TARGET_NAME} share one group of DMA TX/RX channels via host controller interface (HCI). This document assumes that UART DMA is controlled by UHCI entity.

.. note::

    The UART DMA shares the HCI hardware with Bluetooth, so please don't use BT HCI together with UART DMA, even if they use different UART ports.

Quick Start
-----------

This section will quickly guide you on how to use the UHCI driver. Through a simple example including transmitting and receiving, it demonstrates how to create and start a UHCI, initiate a transmit and receive transactions, and register event callback functions. The general usage process is as follows:

Creating and Enabling the UHCI controller
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

UHCI controller requires the configuration specified by :cpp:type:`uhci_controller_config_t`.

If the configurations in :cpp:type:`uhci_controller_config_t` is specified, users can call :cpp:func:`uhci_new_controller` to allocate and initialize a uhci controller. This function will return a uhci controller handle if it runs correctly. Besides, UHCI must work with the installed UART driver. As a reference, see the code below.

.. code:: c

    #define EX_UART_NUM 1       // Define UART port number

    // For uart port configuration, please refer to UART programming guide.
    // Please double-check as the baud rate might be limited by serial port chips.
    uart_config_t uart_config = {
        .baud_rate = 1 * 1000 * 1000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    //UART parameter config
    ESP_ERROR_CHECK(uart_param_config(EX_UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(EX_UART_NUM, UART_TX_IO, UART_RX_IO, -1, -1));

    uhci_controller_config_t uhci_cfg = {
        .uart_port = EX_UART_NUM,                 // Connect uart port to UHCI hardware.
        .tx_trans_queue_depth = 30,               // Queue depth of transaction queue.
        .max_receive_internal_mem = 10 * 1024,    // internal memory usage, for more information, please refer to API reference.
        .max_transmit_size = 10 * 1024,           // Maximum transfer size in one transaction, in bytes.
        .dma_burst_size = 32,                     // Burst size.
        .rx_eof_flags.idle_eof = 1,               // When to trigger a end of frame event, you can choose `idle_eof`, `rx_brk_eof`, `length_eof`, for more information, please refer to API reference.
    };

    uhci_controller_handle_t uhci_ctrl;

    ESP_ERROR_CHECK(uhci_new_controller(&uhci_cfg, &uhci_ctrl));

Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

When an event occurs on the UHCI controller (e.g., transmission or receiving is completed), the CPU is notified of this event via an interrupt. If there is a function that needs to be called when a particular events occur, you can register a callback for that event with the ISR for UHCI (Interrupt Service Routine) by calling :cpp:func:`uhci_register_event_callbacks` for both TX and RX respectively. Since the registered callback functions are called in the interrupt context, the user should ensure that the callback function is non-blocking, e.g., by making sure that only FreeRTOS APIs with the ``FromISR`` suffix are called from within the function. The callback function has a boolean return value used to indicate whether a higher priority task has been unblocked by the callback.

The UHCI event callbacks are listed in the :cpp:type:`uhci_event_callbacks_t`:

- :cpp:member:`uhci_event_callbacks_t::on_tx_trans_done` sets a callback function for the "trans-done" event. The function prototype is declared in :cpp:type:`uhci_tx_done_callback_t`.

- :cpp:member:`uhci_event_callbacks_t::on_rx_trans_event` sets a callback function for "receive" event. The function prototype is declared in :cpp:type:`uhci_rx_event_callback_t`.

.. note::

    The "rx-trans-event" is not equivalent to "receive-finished". This callback can also be called at a "partial-received" time, for many times during one receive transaction, which can be notified by :cpp:member:`uhci_rx_event_data_t::flags::totally_received`.

Users can save their own context in :cpp:func:`uhci_register_event_callbacks` as well, via the parameter ``user_data``. The user data is directly passed to each callback function.

In the callback function, users can fetch the event-specific data that is filled by the driver in the ``edata``. Note that the ``edata`` pointer is **only** valid during the callback, please do not try to save this pointer and use that outside of the callback function.

The TX event data is defined in :cpp:type:`uhci_tx_done_event_data_t`:

- :cpp:member:`uhci_tx_done_event_data_t::buffer` indicates the buffer has been sent out.

The RX event data is defined in :cpp:type:`uhci_rx_event_data_t`:

- :cpp:member:`uhci_rx_event_data_t::data` points to the received data. The data is saved in the ``buffer`` parameter of the :cpp:func:`uhci_receive` function. Users should not free this receive buffer before the callback returns.
- :cpp:member:`uhci_rx_event_data_t::recv_size` indicates the number of received data. This value is not larger than the ``buffer_size`` parameter of :cpp:func:`uhci_receive` function.
- :cpp:member:`uhci_rx_event_data_t::flags::totally_received` indicates whether the current received buffer is the last one in the transaction.

Initiating UHCI Transmission
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:cpp:func:`uhci_transmit` is a non-blocking function, which means this function will immediately return after you call it. The related callback can be obtained via :cpp:member:`uhci_event_callbacks_t::on_tx_trans_done` to indicate that the transaction is done. The function :cpp:func:`uhci_wait_all_tx_transaction_done` can be used to indicate that all transactions are finished.

Data can be transmitted via UHCI as follows:

.. code:: c

    uint8_t data_wr[DATA_LENGTH];
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    ESP_ERROR_CHECK(uhci_transmit(uhci_ctrl, data_wr, DATA_LENGTH));
    // Wait all transaction finishes
    ESP_ERROR_CHECK(uhci_wait_all_tx_transaction_done(uhci_ctrl, -1));

Initiating UHCI Reception
^^^^^^^^^^^^^^^^^^^^^^^^^

:cpp:func:`uhci_receive` is a non-blocking function, which means this function will immediately return after it is called. The related callback can be obtained via :cpp:member:`uhci_rx_event_data_t::recv_size` to indicate the receive event. It can be useful to determine if a transaction has been finished.

Data can be transmitted via UHCI as follows:

.. code:: c

    // global variable: handle of queue.
    QueueHandle_t uhci_queue;

    IRAM_ATTR static bool s_uhci_rx_event_cbs(uhci_controller_handle_t uhci_ctrl, const uhci_rx_event_data_t *edata, void *user_ctx)
    {
        // parameter `user_ctx` is parsed by the third parameter of function `uhci_register_event_callbacks`
        uhci_context_t *ctx = (uhci_context_t *)user_ctx;
        BaseType_t xTaskWoken = 0;
        uhci_event_t evt = 0;
        if (edata->flags.totally_received) {
            evt = UHCI_EVT_EOF;
            ctx->receive_size += edata->recv_size;
            memcpy(ctx->p_receive_data, edata->data, edata->recv_size);
        } else {
            evt = UHCI_EVT_PARTIAL_DATA;
            ctx->receive_size += edata->recv_size;
            memcpy(ctx->p_receive_data, edata->data, edata->recv_size);
            ctx->p_receive_data += edata->recv_size;
        }

        xQueueSendFromISR(ctx->uhci_queue, &evt, &xTaskWoken);
        return xTaskWoken;
    }

    // In task
    uhci_event_callbacks_t uhci_cbs = {
        .on_rx_trans_event = s_uhci_rx_event_cbs,
    };

    // Register callback and start reception.
    ESP_ERROR_CHECK(uhci_register_event_callbacks(uhci_ctrl, &uhci_cbs, ctx));
    ESP_ERROR_CHECK(uhci_receive(uhci_ctrl, pdata, 100));

    uhci_event_t evt;
    while (1) {
        // A queue in task for receiving event triggered by UHCI.
        if (xQueueReceive(ctx->uhci_queue, &evt, portMAX_DELAY) == pdTRUE) {
            if (evt == UHCI_EVT_EOF) {
                printf("Received size: %d\n", ctx->receive_size);
                break;
            }
        }
    }

In the API :cpp:func:`uhci_receive` interface, the parameter `read_buffer` is a buffer that must be provided by the user, and parameter `buffer_size` represents the size of the buffer supplied by the user. In the configuration structure of the UHCI controller, the parameter :cpp:member:`uhci_controller_config_t::max_receive_internal_mem` specifies the desired size of the internal DMA working space. The software allocates a certain number of DMA nodes based on this working space size. These nodes form a circular linked list.

When a node is filled, but the reception has not yet completed, the event :cpp:member:`uhci_event_callbacks_t::on_rx_trans_event` will be triggered, accompanied by :cpp:member:`uhci_rx_event_data_t::flags::totally_received` set to 0. When all the data has been fully received, the :cpp:member:`uhci_event_callbacks_t::on_rx_trans_event` event will be triggered again with :cpp:member:`uhci_rx_event_data_t::flags::totally_received` set to 1.

This mechanism allows the user to achieve continuous and fast reception using a relatively small buffer, without needing to allocate a buffer the same size as the total data being received.

.. note::

    The parameter `read_buffer` of :cpp:func:`uhci_receive` cannot be freed until receive finishes.

Uninstall UHCI controller
^^^^^^^^^^^^^^^^^^^^^^^^^

If a previously installed UHCI controller is no longer needed, it's recommended to recycle the resource by calling :cpp:func:`uhci_del_controller`, so that the underlying hardware is released.

.. code:: c

    ESP_ERROR_CHECK(uhci_del_controller(uhci_ctrl));

Advanced Features
-----------------

As the basic usage has been covered, it's time to explore more advanced features of the UHCI driver.

Power Management
^^^^^^^^^^^^^^^^

When power management is enabled, i.e., :ref:`CONFIG_PM_ENABLE` is on, the system may adjust or disable the clock source before going to sleep. As a result, the FIFO inside the UHCI can't work as expected.

The driver can prevent the above issue by creating a power management lock. The lock type is set based on different clock sources. The driver will acquire the lock in :cpp:func:`uhci_receive` or :cpp:func:`uhci_transmit`, and release it in the transaction-done interrupt. That means, any UHCI transactions between these two functions are guaranteed to work correctly and stably.

Cache Safe
^^^^^^^^^^

By default, the interrupt on which UHCI relies is deferred when the Cache is disabled for reasons such as writing or erasing the main flash. Thus, the transaction-done interrupt fails to be handled in time, which is unacceptable in a real-time application. What is worse, when the UHCI transaction relies on **ping-pong** interrupt to successively encode or copy the UHCI buffer, a delayed interrupt can lead to an unpredictable result.

There is a Kconfig option :ref:`CONFIG_UHCI_ISR_CACHE_SAFE` that has the following features:

1. Enable the interrupt being serviced even when the cache is disabled
2. Place all functions used by the ISR into IRAM [1]_
3. Place the driver object into DRAM in case it is mapped to PSRAM by accident

This Kconfig option allows the interrupt handler to run while the cache is disabled but comes at the cost of increased IRAM consumption.

Resource Consumption
^^^^^^^^^^^^^^^^^^^^

Use the :doc:`/api-guides/tools/idf-size` tool to check the code and data consumption of the UHCI driver. The following are the test results under 2 different conditions (using ESP32-C3 as an example):

**Note that the following data are not exact values and are for reference only; they may differ on different chip models.**

Resource consumption when :ref:`CONFIG_UHCI_ISR_CACHE_SAFE` is enabled:

.. list-table:: Resource Consumption
    :widths: 10 10 10 10 10 10 10 10 10
    :header-rows: 1

    * - Component Layer
      - Total Size
      - DIRAM
      - .bss
      - .data
      - .text
      - Flash Code
      - Flash Data
      - .rodata
    * - UHCI
      - 5733
      - 680
      - 8
      - 34
      - 638
      - 4878
      - 175
      - 175

Resource consumption when :ref:`CONFIG_UHCI_ISR_CACHE_SAFE` is disabled:

.. list-table:: Resource Consumption
    :widths: 10 10 10 10 10 10 10 10 10 10
    :header-rows: 1

    * - Component Layer
      - Total Size
      - DIRAM
      - .bss
      - .data
      - .text
      - Flash Code
      - .text
      - Flash Data
      - .rodata
    * - UHCI
      - 5479
      - 42
      - 8
      - 34
      - 0
      - 5262
      - 5262
      - 175
      - 175

Performance
^^^^^^^^^^^

To improve the real-time response capability of interrupt handling, the UHCI driver provides the :ref:`CONFIG_UHCI_ISR_HANDLER_IN_IRAM` option. Enabling this option will place the interrupt handler in internal RAM, reducing the latency caused by cache misses when loading instructions from Flash.

.. note::

    However, user callback functions and context data called by the interrupt handler may still be located in Flash, and cache miss issues will still exist. Users need to place callback functions and data in internal RAM, for example, using :c:macro:`IRAM_ATTR` and :c:macro:`DRAM_ATTR`.

Thread Safety
^^^^^^^^^^^^^

The factory function :cpp:func:`uhci_new_controller`, :cpp:func:`uhci_register_event_callbacks` and :cpp:func:`uhci_del_controller` are guaranteed to be thread safe by the driver, which means, user can call them from different RTOS tasks without protection by extra locks.

Other Kconfig Options
^^^^^^^^^^^^^^^^^^^^^

- :ref:`CONFIG_UHCI_ENABLE_DEBUG_LOG` is allowed for the forced enabling of all debug logs for the UHCI driver, regardless of the global log level setting. Enabling this option can help developers obtain more detailed log information during the debugging process, making it easier to locate and resolve issues, but it will increase the size of the firmware binary.

Application Examples
--------------------

- :example:`peripherals/uart/uart_dma_ota` demonstrates how to use the uart dma for fast OTA the chip firmware with 1M baud rate speed.

API Reference
-------------

.. include-build-file:: inc/uhci.inc
.. include-build-file:: inc/components/esp_driver_uart/include/driver/uhci_types.inc
.. include-build-file:: inc/components/hal/include/hal/uhci_types.inc

.. [1]
   The callback function, e.g., :cpp:member:`uhci_event_callbacks_t::on_tx_trans_done`, :cpp:member:`uhci_event_callbacks_t::on_rx_trans_event` and the functions invoked by itself should also reside in IRAM, users need to take care of this by themselves.
