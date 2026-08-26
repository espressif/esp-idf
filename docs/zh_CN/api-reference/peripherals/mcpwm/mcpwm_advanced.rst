==============
MCPWM 高级主题
==============

.. contents::
    :local:
    :depth: 2

分辨率与共享时钟
================

``resolution_hz`` 是定时器 Tick 频率，每个 Tick 持续 ``1 / resolution_hz`` 秒。驱动从所选源时钟计算分频器：请求频率能整除时优先精确满足；不能整除时，优先使组时钟尽可能高，再选择最接近的子模块频率。

组分频器由定时器共享。在捕获时钟来自组时钟的芯片上，捕获定时器也共享它。多个对象需要不同分辨率时，按同一单调顺序（从高到低或从低到高）创建，避免任意混排。

下面这张时钟树图展示单一源时钟如何逐级分频展开。组分频器在整个组内只固定一次，每个定时器只在它之上再加自己的预分频器：

.. mermaid::

    flowchart LR
        src["源时钟<br/>(如 80 MHz)"]:::src -->|"÷ 组分频<br/>整组共享"| grp["组时钟<br/>(如 40 MHz)"]:::grp
        grp -->|"÷ 定时器预分频"| pwm["PWM 定时器<br/>resolution_hz = 10 MHz"]:::mod
        grp -->|"÷ 捕获预分频"| cap["捕获定时器<br/>resolution_hz = 20 MHz"]:::mod
        classDef src fill:#fef3c7,stroke:#d97706,color:#78350f
        classDef grp fill:#dbeafe,stroke:#2563eb,color:#172554
        classDef mod fill:#dcfce7,stroke:#16a34a,color:#14532d

因为组分频器是共享的，只会为所有定时器确定一次。之后若再请求不同分辨率，就无法改动这个已固定的分频器，驱动会尽量让组时钟保持较高，并为新子模块选择最接近的预分频。

电源管理与睡眠
==============

启用电源管理后，:cpp:func:`mcpwm_timer_enable()` 和 :cpp:func:`mcpwm_capture_timer_enable()` 会持有 :cpp:enumerator:`esp_pm_lock_type_t::ESP_PM_NO_LIGHT_SLEEP` 锁，以确保定时器时钟频率稳定；调用 :cpp:func:`mcpwm_timer_disable()` 或 :cpp:func:`mcpwm_capture_timer_disable()` 释放该锁。

定时器配置和捕获定时器配置中都有一个同名的 :cpp:member:`allow_pd <mcpwm_timer_config_t::flags::allow_pd>` 字段（后者见 :cpp:type:`mcpwm_capture_timer_config_t`），设置它可允许睡眠时关闭 MCPWM 电源域，驱动会备份和恢复寄存器，但会消耗额外 RAM。此功能取决于目标芯片。

ISR 与线程安全
==============

定时器、比较器、故障、操作器制动和捕获回调都在 ISR 上下文运行，必须非阻塞并使用 ISR 安全的 RTOS 调用。组中第一个注册的回调确定共享中断优先级；后续事件应使用相同优先级。

:cpp:func:`mcpwm_new_timer()` 等工厂函数线程安全。:cpp:func:`mcpwm_timer_set_period()` 和 :cpp:func:`mcpwm_comparator_set_compare_value()` 可在 ISR 中调用。其他控制 API 通常不是线程安全的，多任务访问同一对象时应自行串行化。

缓存安全与实时操作
==================

正常情况下，缓存禁用时（如 Flash 操作期间）MCPWM 中断响应会被延迟。当回调必须在缓存禁用时继续运行时，启用 :menuitem:`CONFIG_MCPWM_ISR_CACHE_SAFE`；这会将 ISR 所需代码放入 IRAM，对象放入 DRAM，增加内部 RAM 使用。

.. note::

    启用该选项后，即使缓存被禁用，MCPWM 中断仍会立即响应，不会被推迟。但中断处理链路上的所有代码——包括你注册的回调函数以及回调内部调用的所有子函数——都必须位于 IRAM 中：缓存关闭时 CPU 无法从 Flash 取指，一旦调用任何仍留在 Flash 中的函数，CPU 就会崩溃。该选项只负责把驱动自身的 ISR 代码放入 IRAM，你的回调函数及其调用的子函数需要自行显式放到 IRAM（例如用 ``IRAM_ATTR`` 修饰）。

:menuitem:`CONFIG_MCPWM_CTRL_FUNC_IN_IRAM` 还会将 :cpp:func:`mcpwm_timer_set_period()` 和 :cpp:func:`mcpwm_comparator_set_compare_value()` 放入 IRAM，这样即使缓存关闭，这些调用仍可正常工作——例如你可以在缓存被禁用的上下文（如 Flash 擦写期间）中重新调整 PWM 周期或占空比，而无需等待缓存重新启用。

Kconfig 选项
==============

.. list::

    - :menuitem:`CONFIG_MCPWM_ISR_CACHE_SAFE` 启用缓存安全中断。
    - :menuitem:`CONFIG_MCPWM_CTRL_FUNC_IN_IRAM` 将选定控制函数放入 IRAM。
    - :menuitem:`CONFIG_MCPWM_ENABLE_DEBUG_LOG` 强制 MCPWM 驱动编译并打印自身的调试日志，忽略全局日志设置，仅对驱动本身将运行时日志等级提升到冗长——不影响其他模块。代价是固件体积增大。
