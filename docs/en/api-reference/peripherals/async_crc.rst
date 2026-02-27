============================
Asynchronous CRC (Async CRC)
============================

:link_to_translation:`zh_CN:[中文]`

This document introduces the features of the Asynchronous CRC (Async CRC) driver in ESP-IDF. The table of contents is as follows:

.. contents::
    :local:
    :depth: 2

Overview
========

The Async CRC driver provides hardware-accelerated CRC calculation using the General DMA peripheral. It supports both AHB-GDMA and AXI-GDMA backends, offering flexible CRC computation with configurable polynomial, initial value, bit reversal options, and final XOR processing.

Key capabilities include:

- Hardware-accelerated CRC calculation using General DMA
- Support for 8-bit, 16-bit, and 32-bit CRC algorithms
- Asynchronous API with callback notifications
- Blocking API with timeout support
- Request queuing with configurable backlog size
- Support for both AHB and AXI DMA backends

Application scenarios include:

- Data integrity verification for communication protocols
- File and firmware checksum calculation
- Network packet validation
- Storage data verification

Quick Start
===========

This section provides a concise overview of how to use the Async CRC driver. Through practical examples, it demonstrates how to initialize the driver, configure CRC parameters, and perform both asynchronous and blocking CRC calculations.

The typical usage flow for async CRC is as follows:

.. blockdiag::
    :scale: 100%
    :caption: Async CRC driver's general usage flow (click to enlarge)
    :align: center

    blockdiag {
        default_fontsize = 14;
        node_width = 250;
        node_height = 80;
        class emphasis [color = pink, style = dashed];

        install [label="esp_async_crc_install_gdma_*"];
        calc [label="esp_async_crc_calc"];
        wait [label="Wait for callback"];
        process [label="Process result"];
        uninstall [label="esp_async_crc_uninstall"];

        install -> calc -> wait -> process;
        process -> calc [folded];
        calc -> uninstall [folded];
    }

Creating and Installing the Driver
----------------------------------

First, you need to install the Async CRC driver. The driver supports both AHB-GDMA and AXI-GDMA backends, depending on your chip's capabilities:

.. code:: c

    async_crc_handle_t crc_hdl = NULL;
    async_crc_config_t config = {
        .backlog = 8,          // Maximum pending requests in queue
        .dma_burst_size = 16,  // DMA burst size in bytes
    };

    // Install with AHB-GDMA backend (if available)
    ESP_ERROR_CHECK(esp_async_crc_install_gdma_ahb(&config, &crc_hdl));

    // Or install with AXI-GDMA backend (if available)
    // ESP_ERROR_CHECK(esp_async_crc_install_gdma_axi(&config, &crc_hdl));

.. note::

    **Choosing Between AHB-GDMA and AXI-GDMA Backends**

    The backend choice depends on your chip's capabilities and performance requirements:

    - **AHB-GDMA**: Available on most ESP chips. Connects to the AHB bus, suitable for general-purpose DMA operations. Best for:

      - Standard performance requirements
      - Compatibility across most ESP chip variants

    - **AXI-GDMA**: Available on higher-end ESP chips with AXI bus support. Provides higher bandwidth and better performance for memory-intensive operations. Best for:

      - High-throughput CRC calculations
      - Processing large amounts of data
      - Applications requiring maximum performance
      - Accessing external memory (PSRAM) with better efficiency

When creating a driver instance, you need to configure:

- **backlog**: Maximum number of pending CRC requests that can be queued. Higher values use more memory but provide better throughput for bursty workloads.
- **dma_burst_size**: DMA transfer burst size in bytes.

The driver handle ``crc_hdl`` is an opaque pointer that you use for all subsequent operations.

Performing Asynchronous CRC Calculation
---------------------------------------

The async API allows you to queue CRC calculations without blocking:

.. code:: c

    static bool crc_complete_callback(async_crc_handle_t crc_hdl, async_crc_event_data_t *edata, void *cb_args)
    {
        uint32_t result = edata->crc_result;
        // Further process the CRC result
        // e.g., send to a task via queue, log it, etc.
        return false;
    }

    // Configure CRC parameters for CRC-32
    async_crc_params_t params = {
        .width = 32,
        .polynomial = 0x04C11DB7,
        .init_value = 0xFFFFFFFF,
        .final_xor_value = 0xFFFFFFFF,
        .reverse_input = true,
        .reverse_output = true,
    };

    // Start async CRC calculation
    const char *data = "Hello, World!";
    size_t data_len = strlen(data);

    ESP_ERROR_CHECK(esp_async_crc_calc(crc_hdl, data, data_len, &params, crc_complete_callback, NULL));

The callback function is invoked in interrupt context when the CRC calculation completes. The callback receives:

- **crc_hdl**: The driver handle
- **edata**: Event data containing the CRC result
- **cb_args**: User-defined argument passed during ``esp_async_crc_calc``

Performing Blocking CRC Calculation
-----------------------------------

For simpler use cases or when async operations are not required, use the blocking API:

.. code:: c

    uint32_t crc_result = 0;
    async_crc_params_t params = {
        .width = 32,
        .polynomial = 0x04C11DB7,
        .init_value = 0xFFFFFFFF,
        .final_xor_value = 0xFFFFFFFF,
        .reverse_input = true,
        .reverse_output = true,
    };

    const char *data = "Hello, World!";
    size_t data_len = strlen(data);

    // Blocking CRC with 1000ms timeout
    ESP_ERROR_CHECK(esp_crc_calc_blocking(crc_hdl, data, data_len, &params, 1000, &crc_result));

    printf("CRC result: 0x%08X\n", crc_result);

The blocking API supports:

- **timeout_ms >= 0**: Wait for specified milliseconds
- **timeout_ms < 0**: Wait indefinitely

Uninstalling the Driver
------------------------

When the driver is no longer needed:

.. code:: c

    ESP_ERROR_CHECK(esp_async_crc_uninstall(crc_hdl));

The uninstall function returns :c:macro:`ESP_ERR_INVALID_STATE` if there are pending operations or if the CRC engine is busy. Ensure all operations complete before uninstalling.

CRC Parameter Configuration
============================

The Async CRC driver supports flexible CRC algorithm configuration through the :cpp:type:`async_crc_params_t` structure.

CRC Width
---------

The :cpp:member:`async_crc_params_t::width` field specifies the CRC bit width:

- **8**: 8-bit CRC (e.g., CRC-8, CRC-8/MAXIM)
- **16**: 16-bit CRC (e.g., CRC-16/CCITT, CRC-16/IBM)
- **32**: 32-bit CRC (e.g., CRC-32, CRC-32/BZIP2)

Polynomial
----------

The :cpp:member:`async_crc_params_t::polynomial` field specifies the CRC polynomial in hexadecimal format. Common polynomial values include:

- CRC-32: ``0x04C11DB7``
- CRC-16/CCITT: ``0x1021``
- CRC-16/IBM: ``0x8005``
- CRC-8/MAXIM: ``0x31``

Initial Value
-------------

The :cpp:member:`async_crc_params_t::init_value` field sets the initial CRC value before processing. Common initial values:

- ``0xFFFFFFFF`` for CRC-32
- ``0x0000`` for many CRC-16 variants
- ``0x00`` for many CRC-8 variants

Final XOR Value
---------------

The :cpp:member:`async_crc_params_t::final_xor_value` field specifies a value to XOR with the final CRC result. This is commonly ``0xFFFFFFFF`` for CRC-32 but can be ``0x0000`` for some variants.

Bit Reversal Options
--------------------

- :cpp:member:`async_crc_params_t::reverse_input` If true, reverses the bit order of each input byte before processing
- :cpp:member:`async_crc_params_t::reverse_output` If true, reverses the bit order of the final CRC result before applying the final XOR

These options affect the reflection settings for different CRC algorithms.

Common CRC Configurations
-------------------------

The following table lists common CRC configurations:

+----------------+----------+---------------+---------------+------------------+---------------+---------------+
| CRC Algorithm  | Width    | Polynomial    | Initial Value | Final XOR        | Reverse Input | Reverse Output|
+================+==========+===============+===============+==================+===============+===============+
| CRC-32         | 32       | 0x04C11DB7    | 0xFFFFFFFF    | 0xFFFFFFFF       | true          | true          |
+----------------+----------+---------------+---------------+------------------+---------------+---------------+
| CRC-16/CCITT   | 16       | 0x1021        | 0x0000        | 0x0000           | false         | false         |
+----------------+----------+---------------+---------------+------------------+---------------+---------------+
| CRC-16/IBM     | 16       | 0x8005        | 0x0000        | 0x0000           | true          | true          |
+----------------+----------+---------------+---------------+------------------+---------------+---------------+
| CRC-8/MAXIM    | 8        | 0x31          | 0x00          | 0x00             | true          | true          |
+----------------+----------+---------------+---------------+------------------+---------------+---------------+

Thread Safety
=============

The Async CRC driver is designed to be thread-safe and can be used from multiple tasks. The driver features a **race-free Finite State Machine (FSM)** architecture that ensures thread safety and proper handling of concurrent CRC requests.

Thread Safety Guarantees
------------------------

- All public APIs can be called from different tasks simultaneously
- The driver uses atomic operations and critical sections for internal state protection
- Request queuing ensures that concurrent calls are properly serialized

ISR Context Restrictions
------------------------

Neither the async API nor the blocking API can be called from interrupt context. Specifically:

- :cpp:func:`esp_async_crc_calc`: Involves memory allocation/free, DMA preparations, and logging functions that are not ISR-safe
- :cpp:func:`esp_crc_calc_blocking`: Uses synchronization primitives that may block

Callback Restrictions
---------------------

The callback function (:cpp:type:`async_crc_isr_cb_t`) is executed in interrupt context. Therefore:

- Do **not** perform blocking operations (e.g., ``vTaskDelay``, ``xQueueSend`` with timeout)
- Keep execution time minimal to avoid impacting system interrupt latency
- Do **not** allocate memory using ``malloc`` or similar functions
- Use only ISR-safe FreeRTOS APIs (e.g., ``xQueueSendFromISR``, ``xSemaphoreGiveFromISR``)
- Return ``true`` if the callback wakes a high-priority task

Example of callback using queue:

.. code:: c

    static bool crc_callback(async_crc_handle_t crc_hdl, async_crc_event_data_t *edata, void *cb_args)
    {
        QueueHandle_t queue = (QueueHandle_t)cb_args;
        BaseType_t high_task_awoken = pdFALSE;
        // Send result to a task via ISR-safe queue
        xQueueSendFromISR(queue, &edata->crc_result, &high_task_awoken);
        return high_task_awoken == pdTRUE;
    }

Buffer Requirements
===================

The Async CRC driver has specific requirements for data buffers.

Memory Type
-----------

Data buffers can be in internal memory (DRAM/IRAM) or external memory (PSRAM, Flash). The driver automatically handles both:

.. code:: c

    // Internal RAM
    static char internal_data[] = "Data in internal RAM";
    esp_async_crc_calc(crc_hdl, internal_data, strlen(internal_data), &params, callback, NULL);

    // External Flash
    static const char *flash_data = "Data in external Flash";
    esp_async_crc_calc(crc_hdl, flash_data, strlen(flash_data), &params, callback, NULL);

Performance Considerations
==========================

Backlog Configuration
---------------------

The ``backlog`` configuration affects performance:

- **Small backlog** (4-8): Lower memory usage, may cause backpressure under high load
- **Large backlog** (16+): Better throughput for bursty workloads, higher memory usage

Choose based on your application's memory constraints and workload pattern.

DMA Burst Size
--------------

The ``dma_burst_size`` affects DMA transfer efficiency:

- Larger burst sizes can improve throughput
- Typical values: 16, 32, 64 bytes

The optimal value depends on your chip's DMA controller capabilities.

Application Examples
====================

- :example:`peripherals/dma/async_crc` demonstrates how to use the Async CRC driver through an interactive console CLI.

API Reference
=============

Async CRC Driver Functions
--------------------------

.. include-build-file:: inc/esp_async_crc.inc
