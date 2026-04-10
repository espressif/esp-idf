Peripherals
===============

:link_to_translation:`zh_CN:[中文]`

UART
-----

UART Wakeup API Update
~~~~~~~~~~~~~~~~~~~~~~~~

In ESP-IDF v6.1, the legacy UART wakeup APIs :cpp:func:`uart_set_wakeup_threshold` and :cpp:func:`uart_get_wakeup_threshold` have been marked as deprecated and will be removed in future versions. These APIs only support the RXD edge threshold wakeup mode (Mode 0).

The new unified API :cpp:func:`uart_wakeup_setup` provides a more flexible configuration approach and supports multiple wakeup modes:

- **Mode 0 (UART_WK_MODE_ACTIVE_THRESH)** - Active edge threshold wakeup (corresponds to the legacy API functionality)
- **Mode 1 (UART_WK_MODE_FIFO_THRESH)** - RX FIFO threshold wakeup
- **Mode 2 (UART_WK_MODE_START_BIT)** - Start bit detection wakeup
- **Mode 3 (UART_WK_MODE_CHAR_SEQ)** - Character sequence detection wakeup

**Migration Example:**

Old code:

.. code-block:: c

    // Set wakeup threshold
    ESP_ERROR_CHECK(uart_set_wakeup_threshold(UART_NUM_0, 3));
    ESP_ERROR_CHECK(esp_sleep_enable_uart_wakeup(UART_NUM_0));

    // Get wakeup threshold
    int threshold;
    ESP_ERROR_CHECK(uart_get_wakeup_threshold(UART_NUM_0, &threshold));

New code:

.. code-block:: c

    #include "driver/uart_wakeup.h"

    // Configure active edge threshold wakeup mode (corresponds to legacy API functionality)
    uart_wakeup_cfg_t wakeup_cfg = {
        .wakeup_mode = UART_WK_MODE_ACTIVE_THRESH,
        .rx_edge_threshold = 3,  // Corresponds to the wakeup_threshold parameter of the legacy API
    };
    ESP_ERROR_CHECK(uart_wakeup_setup(UART_NUM_0, &wakeup_cfg));
    ESP_ERROR_CHECK(esp_sleep_enable_uart_wakeup(UART_NUM_0));

    // Note: The new API does not have a direct corresponding get function
    // If you need to get the current configuration, you should save the configuration value yourself

Major Changes
^^^^^^^^^^^^^^^^^^^

1. **API Replacement**:
   - :cpp:func:`uart_set_wakeup_threshold` → :cpp:func:`uart_wakeup_setup`
   - :cpp:func:`uart_get_wakeup_threshold` → Removed

2. **Configuration Method**:
   - Legacy API uses a simple integer parameter
   - New API uses :cpp:type:`uart_wakeup_cfg_t` structure, supporting multiple wakeup modes

3. **Header File**:
   - New API requires including the ``driver/uart_wakeup.h`` header file

4. **Feature Extension**:
   - New API supports multiple wakeup modes, which can be selected based on chip capabilities
   - The availability of different wakeup modes depends on the chip's SOC capabilities (determined by ``SOC_UART_WAKEUP_SUPPORT_XXX_MODE`` macros)

Notes
^^^^^^^^^^

- Legacy API only supports Mode 0 (active edge threshold wakeup). When migrating, simply set ``wakeup_mode = UART_WK_MODE_ACTIVE_THRESH``
- The ``rx_edge_threshold`` parameter of the new API has the same meaning as the ``wakeup_threshold`` parameter of the legacy API
- If you need to get the current wakeup configuration at runtime, it is recommended to save the configuration value when calling :cpp:func:`uart_wakeup_setup`
- Different chips may support different wakeup modes. Please refer to the :ref:`UART Wakeup (Light-sleep Only) <uart_wakeup_light_sleep>` section.
