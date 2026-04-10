外设驱动
=========

:link_to_translation:`en:[English]`

UART
------

UART 唤醒 API 更新
~~~~~~~~~~~~~~~~~~~~~

在 ESP-IDF v6.1 中，旧的 UART 唤醒 API :cpp:func:`uart_set_wakeup_threshold` 和 :cpp:func:`uart_get_wakeup_threshold` 已被标记为废弃，并将在未来版本中移除。这些 API 仅支持基于 RXD 边沿阈值的唤醒模式（Mode 0）。

新的统一 API :cpp:func:`uart_wakeup_setup` 提供了更灵活的配置方式，支持多种唤醒模式：

- **Mode 0 (UART_WK_MODE_ACTIVE_THRESH)** - 边沿阈值唤醒（对应旧 API 的功能）
- **Mode 1 (UART_WK_MODE_FIFO_THRESH)** - RX FIFO 阈值唤醒
- **Mode 2 (UART_WK_MODE_START_BIT)** - 起始位检测唤醒
- **Mode 3 (UART_WK_MODE_CHAR_SEQ)** - 字符序列检测唤醒

**迁移示例：**

旧代码：

.. code-block:: c

    // 设置唤醒阈值
    ESP_ERROR_CHECK(uart_set_wakeup_threshold(UART_NUM_0, 3));
    ESP_ERROR_CHECK(esp_sleep_enable_uart_wakeup(UART_NUM_0));

    // 获取唤醒阈值
    int threshold;
    ESP_ERROR_CHECK(uart_get_wakeup_threshold(UART_NUM_0, &threshold));

新代码：

.. code-block:: c

    #include "driver/uart_wakeup.h"

    // 配置边沿阈值唤醒模式（对应旧 API 的功能）
    uart_wakeup_cfg_t wakeup_cfg = {
        .wakeup_mode = UART_WK_MODE_ACTIVE_THRESH,
        .rx_edge_threshold = 3,  // 对应旧 API 的 wakeup_threshold 参数
    };
    ESP_ERROR_CHECK(uart_wakeup_setup(UART_NUM_0, &wakeup_cfg));
    ESP_ERROR_CHECK(esp_sleep_enable_uart_wakeup(UART_NUM_0));

    // 注意：新 API 没有直接对应的获取函数
    // 如果需要获取当前配置，需要自行保存配置值

主要变化
^^^^^^^^^^^

1. **API 替换**：
   - :cpp:func:`uart_set_wakeup_threshold` → :cpp:func:`uart_wakeup_setup`
   - :cpp:func:`uart_get_wakeup_threshold` → 已移除

2. **配置方式**：
   - 旧 API 使用简单的整数参数
   - 新 API 使用 :cpp:type:`uart_wakeup_cfg_t` 结构体，支持多种唤醒模式

3. **头文件**：
   - 新 API 需要包含 ``driver/uart_wakeup.h`` 头文件

4. **功能扩展**：
   - 新 API 支持多种唤醒模式，可根据芯片能力选择使用
   - 不同唤醒模式的可用性取决于芯片的 SOC 能力（通过 ``SOC_UART_WAKEUP_SUPPORT_XXX_MODE`` 宏判断）

注意事项
^^^^^^^^^^^

- 旧 API 仅支持 Mode 0（边沿阈值唤醒），迁移时只需设置 ``wakeup_mode = UART_WK_MODE_ACTIVE_THRESH``
- 新 API 的 ``rx_edge_threshold`` 参数与旧 API 的 ``wakeup_threshold`` 参数含义相同
- 如果需要在运行时获取当前唤醒配置，建议在调用 :cpp:func:`uart_wakeup_setup` 时保存配置值
- 不同芯片支持的唤醒模式可能不同，请参考 :ref:`UART 唤醒（仅适用于 Light-sleep 模式） <uart_wakeup_light_sleep>` 部分。
