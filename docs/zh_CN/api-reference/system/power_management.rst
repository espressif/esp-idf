电源管理
================

:link_to_translation:`en:[English]`

概述
--------

ESP-IDF 中集成的电源管理算法可以根据应用程序组件的需求，调整外围总线 (APB) 频率、CPU 频率，并使芯片进入 Light-sleep 模式，尽可能减少运行应用程序的功耗。

应用程序组件可以通过创建和获取电源管理锁来控制功耗。

例如：

- 对于从 APB 获得时钟频率的外设，其驱动可以要求在使用该外设时，将 APB 频率设置为 80 MHz。
- RTOS 可以要求 CPU 在有任务准备开始运行时以最高配置频率工作。
- 一些外设可能需要中断才能启用，因此其驱动也会要求禁用 Light-sleep 模式。

因为请求较高的 APB 频率或 CPU 频率，以及禁用 Light-sleep 模式会增加功耗，请将组件使用的电源管理锁降到最少。

电源管理配置
-------------

编译时可使用 :ref:`CONFIG_PM_ENABLE` 选项启用电源管理功能。

启用电源管理功能将会增加中断延迟。额外延迟与多个因素有关，例如：CPU 频率、单/双核模式、是否需要进行频率切换等。CPU 频率为 240 MHz 且未启用频率调节时，最小额外延迟为 0.2 us；如果启用频率调节，且在中断入口将频率由 40 MHz 调节至 80 MHz，则最大额外延迟为 40 us。

应用程序可以通过调用 :cpp:func:`esp_pm_configure` 函数启用动态调频 (DFS) 功能和自动 Light-sleep 模式。此函数的参数为 :cpp:class:`esp_pm_config_{IDF_TARGET_PATH_NAME}_t`，定义了频率调节的相关设置。在此参数结构中，需要初始化下面三个字段：

- ``max_freq_mhz``：最大 CPU 频率 (MHz)，即获取 ``ESP_PM_CPU_FREQ_MAX`` 锁后所使用的频率。该字段通常设置为 :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_DEFAULT_CPU_FREQ_MHZ`。

- ``min_freq_mhz``：最小 CPU 频率 (MHz)，即仅获取 ``ESP_PM_APB_FREQ_MAX`` 锁后所使用的频率。该字段可设置为晶振 (XTAL) 频率值，或者 XTAL 频率值除以整数。注意，10 MHz 是生成 1 MHz 的 REF_TICK 默认时钟所需的最小频率。

- ``light_sleep_enable``：没有获取任何管理锁时，决定系统是否需要自动进入 Light-sleep 状态 (``true``/``false``)。

或者，如果在 menuconfig 中启用了 :ref:`CONFIG_PM_DFS_INIT_AUTO` 选项，最大 CPU 频率将由 :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_DEFAULT_CPU_FREQ_MHZ` 设置决定，最小 CPU 频率将锁定为 XTAL 频率。

.. note::

  1. 自动 Light-sleep 模式基于 FreeRTOS Tickless Idle 功能，因此如果在 menuconfig 中没有启用 :ref:`CONFIG_FREERTOS_USE_TICKLESS_IDLE` 选项，在请求自动 Light-sleep 时，:cpp:func:`esp_pm_configure` 将会返回 `ESP_ERR_NOT_SUPPORTED` 错误。
  2. 在 Light-sleep 状态下，外设设有时钟门控，不会产生来自 GPIO 和内部外设的中断。:doc:`sleep_modes` 文档中所提到的唤醒源可用于从 Light-sleep 状态触发唤醒。例如，EXT0 和 EXT1 唤醒源就可以通过 GPIO 唤醒芯片。

电源管理锁
----------------------

应用程序可以通过获取或释放管理锁来控制电源管理算法。应用程序获取电源管理锁后，电源管理算法的操作将受到下面的限制。释放电源管理锁后，限制解除。

电源管理锁设有获取/释放计数器，如果已多次获取电源管理锁，则需要将电源管理锁释放相同次数以解除限制。

{IDF_TARGET_NAME} 支持下表中所述的三种电源管理锁。

============================  ======================================================
电源管理锁                          描述
============================  ======================================================
``ESP_PM_CPU_FREQ_MAX``       请求使用 :cpp:func:`esp_pm_configure` 将 CPU 频率设置为最大值。{IDF_TARGET_NAME} 可以将该值设置为 80 MHz、160 MHz 或 240 MHz。

``ESP_PM_APB_FREQ_MAX``       请求将 APB 频率设置为最大值，{IDF_TARGET_NAME} 支持的最大频率为 80 MHz。

``ESP_PM_NO_LIGHT_SLEEP``     禁止自动切换至 Light-sleep 模式。
============================  ======================================================

{IDF_TARGET_NAME} 电源管理算法
--------------------------------

下表列出了启用动态调频时如何切换 CPU 频率和 APB 频率。您可以使用 :cpp:func:`esp_pm_configure` 或者 :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_DEFAULT_CPU_FREQ_MHZ` 指定 CPU 最大频率。


.. only:: esp32

   .. include:: inc/power_management_esp32.rst

.. only:: not esp32

   .. include:: inc/power_management_esp32s2_and_later.rst

如果没有获取任何管理锁，调用 :cpp:func:`esp_pm_configure` 将启动 Light-sleep 模式。 Light-sleep 模式持续时间由以下因素决定：

- 处于阻塞状态的 FreeRTOS 任务数（有限超时）
- :doc:`高分辨率定时器 <esp_timer>` API 注册的计数器数量

您也可以设置 Light-sleep 模式在最近事件（任务解除阻塞，或计时器超时）之前持续多久才唤醒芯片。

动态调频和外设驱动
------------------------------------------------

启用动态调频后，APB 频率可在一个 RTOS 滴答周期内多次更改。有些外设不受 APB 频率变更的影响，但有些外设可能会出现问题。例如，Timer Group 外设定时器会继续计数，但定时器计数的速度将随 APB 频率的变更而变更。

下面的外设不受 APB 频率变更的影响：

- **UART**：如果 REF_TICK 用作时钟源，则 UART 不受 APB 频率变更影响。请查看 :cpp:class:`uart_config_t` 中的 `use_ref_tick`。
- **LEDC**：如果 REF_TICK 用作时钟源，则 LEDC 不受 APB 频率变更影响。请查看 :cpp:func:`ledc_timer_config` 函数。
- **RMT**：如果 REF_TICK 用作时钟源，则 RMT 不受 APB 频率变更影响。请查看 :cpp:class:`rmt_config_t` 结构体中的 `flags` 成员以及 `RMT_CHANNEL_FLAGS_ALWAYS_ON` 宏。

目前以下外设驱动程序可感知动态调频，并在调频期间使用 ``ESP_PM_APB_FREQ_MAX`` 锁：

- SPI master
- I2C
- I2S（如果 APLL 锁在使用中，I2S 则会启用 ``ESP_PM_NO_LIGHT_SLEEP`` 锁）
- SDMMC

启用以下驱动程序时，将占用 ``ESP_PM_APB_FREQ_MAX`` 锁：


.. list::

    - **SPI slave**：从调用 :cpp:func:`spi_slave_initialize` 至 :cpp:func:`spi_slave_free` 期间。
    - **Ethernet**：从调用 :cpp:func:`esp_eth_driver_install` 至 :cpp:func:`esp_eth_driver_uninstall` 期间。
    - **WiFi**：从调用 :cpp:func:`esp_wifi_start` 至 :cpp:func:`esp_wifi_stop` 期间。如果启用了调制解调器睡眠模式，广播关闭时将释放此管理锁。
    - **TWAI**：从调用 :cpp:func:`twai_driver_install` 至 :cpp:func:`twai_driver_uninstall` 期间。
    :SOC_BT_SUPPORTED: - **Bluetooth**：从调用 :cpp:func:`esp_bt_controller_enable` 至 :cpp:func:`esp_bt_controller_disable` 期间。如果启用了蓝牙调制解调器，广播关闭时将释放此管理锁。但依然占用 ``ESP_PM_NO_LIGHT_SLEEP`` 锁。

以下外设驱动程序无法感知动态调频，应用程序需自己获取/释放管理锁：

.. list::

    - PCNT
    - Sigma-delta
    - Timer group
    :esp32: - MCPWM


API 参考
-------------

.. include-build-file:: inc/esp_pm.inc
.. include-build-file:: inc/pm.inc

