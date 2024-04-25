电源管理
================

:link_to_translation:`en:[English]`

概述
--------

ESP-IDF 中集成的电源管理算法可以根据应用程序组件的需求，调整外围总线 (APB) 频率和 CPU 频率，并使芯片进入 Light-sleep 模式，尽可能减少运行应用程序的功耗。

应用程序组件可以通过创建和获取电源管理锁来控制功耗。

例如：

- 对于从 APB 获得时钟频率的外设，其驱动可以要求在使用该外设时，将 APB 频率设置为 80 MHz。
- RTOS 可以要求 CPU 在有任务准备开始运行时以最高配置频率工作。
- 一些外设可能需要中断才能启用，因此其驱动也会要求禁用 Light-sleep 模式。

请求较高的 APB 频率或 CPU 频率以及禁用 Light-sleep 模式会增加功耗，因此请将组件使用的电源管理锁降到最少。

电源管理配置
-------------

编译时可使用 :ref:`CONFIG_PM_ENABLE` 选项启用电源管理功能。

启用电源管理功能将会增加中断延迟。额外延迟与多个因素有关，例如：CPU 频率、单/双核模式、是否需要进行频率切换等。CPU 频率为 240 MHz 且未启用频率调节时，最小额外延迟为 0.2 us；如果启用频率调节，且在中断入口将频率由 40 MHz 调节至 80 MHz，则最大额外延迟为 40 us。

通过调用 :cpp:func:`esp_pm_configure` 函数可以在应用程序中启用动态调频 (DFS) 功能和自动 Light-sleep 模式。此函数的参数 :cpp:class:`esp_pm_config_t` 定义了频率调节的相关设置。在此参数结构中，需要初始化以下三个字段：

.. list::

    - ``max_freq_mhz``：最大 CPU 频率 (MHz)，即获取 ``ESP_PM_CPU_FREQ_MAX`` 锁后所使用的频率。该字段通常设置为 :ref:`CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ`。

    :esp32 or esp32s2: - ``min_freq_mhz``：最小 CPU 频率 (MHz)，即未持有电源管理锁时所使用的频率。注意，10 MHz 是生成 1 MHz 的 REF_TICK 默认时钟所需的最小频率。

    :not esp32 and not esp32s2: - ``min_freq_mhz``：最小 CPU 频率 (MHz)，即未持有电源管理锁时所使用的频率。

    - ``light_sleep_enable``：没有获取任何管理锁时，决定系统是否需要自动进入 Light-sleep 状态 (``true``/``false``)。


  如果在 menuconfig 中启用了 :ref:`CONFIG_PM_DFS_INIT_AUTO` 选项，最大 CPU 频率将由 :ref:`CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ` 设置决定，最小 CPU 频率将锁定为 XTAL 频率。

.. note::

  自动 Light-sleep 模式基于 FreeRTOS Tickless Idle 功能，因此如果在 menuconfig 中没有启用 :ref:`CONFIG_FREERTOS_USE_TICKLESS_IDLE` 选项，在请求自动 Light-sleep 时，:cpp:func:`esp_pm_configure` 将会返回 `ESP_ERR_NOT_SUPPORTED` 错误。

.. note::

  Light-sleep 状态下，外设设有时钟门控，不会产生来自 GPIO 和内部外设的中断。:doc:`sleep_modes` 文档中所提到的唤醒源可用于从 Light-sleep 状态触发唤醒。

.. only:: SOC_PM_SUPPORT_EXT0_WAKEUP or SOC_PM_SUPPORT_EXT1_WAKEUP

  例如，EXT0 和 EXT1 唤醒源可以通过 GPIO 唤醒芯片。


电源管理锁
----------------------
{IDF_TARGET_MAX_CPU_FREQ: default="Not updated yet", esp32="80 MHz, 160 MHz, or 240 MHz", esp32s2="80 MHz, 160 MHz, 或 240 MHz", esp32s3="80 MHz, 160 MHz, 或 240 MHz", esp32c2="80 MHz 或 120 MHz", esp32c3="80 MHz 或 160 MHz", esp32c6="80 MHz 或 160 MHz"}

应用程序可以通过获取或释放管理锁来控制电源管理算法。应用程序获取电源管理锁后，电源管理算法的操作将受到下面的限制。释放电源管理锁后，限制解除。

电源管理锁设有获取/释放计数器，如果已多次获取电源管理锁，则需要将电源管理锁释放相同次数以解除限制。

{IDF_TARGET_NAME} 支持下表中三种电源管理锁。

.. list-table::
  :header-rows: 1
  :widths: 25 60

  * - 电源管理锁
    - 描述
  * - ``ESP_PM_CPU_FREQ_MAX``
    - 请求使用 :cpp:func:`esp_pm_configure` 将 CPU 频率设置为最大值。{IDF_TARGET_NAME} 可以将该值设置为 {IDF_TARGET_MAX_CPU_FREQ}。
  * - ``ESP_PM_APB_FREQ_MAX``
    - 请求将 APB 频率设置为最大值，{IDF_TARGET_NAME} 支持的最大频率为 80 MHz。
  * - ``ESP_PM_NO_LIGHT_SLEEP``
    - 禁止自动切换至 Light-sleep 模式。

{IDF_TARGET_NAME} 电源管理算法
--------------------------------

下表列出了启用动态调频时如何切换 CPU 频率和 APB 频率。可以使用 :cpp:func:`esp_pm_configure` 或 :ref:`CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ` 指定 CPU 最大频率。

.. include:: inc/power_management_{IDF_TARGET_PATH_NAME}.rst

如果没有获取任何管理锁，调用 :cpp:func:`esp_pm_configure` 将启动 Light-sleep 模式。Light-sleep 模式持续时间由以下因素决定：

- 处于阻塞状态的 FreeRTOS 任务数（有限超时）
- :doc:`高分辨率定时器 <esp_timer>` API 注册的计数器数量

也可以设置 Light-sleep 模式在最近事件（任务解除阻塞，或计时器超时）之前的持续时间，在持续时间结束后再唤醒芯片。

为了跳过不必要的唤醒，可以将 ``skip_unhandled_events`` 选项设置为 ``true`` 来初始化 ``esp_timer``。带有此标志的定时器不会唤醒系统，有助于减少功耗。


动态调频和外设驱动
------------------------------------------------

启用动态调频后，APB 频率可在一个 RTOS 滴答周期内多次更改。有些外设不受 APB 频率变更的影响，但有些外设可能会出现问题。例如，Timer Group 外设定时器会继续计数，但定时器计数的速度将随 APB 频率的变更而变更。

时钟频率不受 APB 频率影响的外设时钟源通常有 ``REF_TICK``, ``XTAL``, ``RC_FAST`` (i.e., ``RTC_8M``)。因此，为了保证外设在 DFS 期间的所有行为一致，建议在上述时钟中选择其一作为外设的时钟源。如果想要了解更多详情可以浏览每个外设 ”API 参考 > 外设 API“ 页面的 “电源管理” 章节。

目前以下外设驱动程序可感知动态调频，并在调频期间使用 ``ESP_PM_APB_FREQ_MAX`` 锁：

- SPI master
- I2C
- I2S（如果 APLL 锁在使用中，I2S 则会启用 ``ESP_PM_NO_LIGHT_SLEEP`` 锁）
- SDMMC

启用以下驱动程序时，将占用 ``ESP_PM_APB_FREQ_MAX`` 锁：

.. list::

    - **SPI slave**：从调用 :cpp:func:`spi_slave_initialize` 至 :cpp:func:`spi_slave_free` 期间。
    - **GPTimer**：从调用 :cpp:func:`gptimer_enable` 至 :cpp:func:`gptimer_disable` 期间。
    - **Ethernet**：从调用 :cpp:func:`esp_eth_driver_install` 至 :cpp:func:`esp_eth_driver_uninstall` 期间。
    - **WiFi**：从调用 :cpp:func:`esp_wifi_start` 至 :cpp:func:`esp_wifi_stop` 期间。如果启用了调制解调器睡眠模式，广播关闭时将释放此管理锁。
    :SOC_TWAI_SUPPORTED: - **TWAI**：从调用 :cpp:func:`twai_driver_install` 至 :cpp:func:`twai_driver_uninstall` 期间 (只有在 TWAI 时钟源选择为 :cpp:enumerator:`TWAI_CLK_SRC_APB` 的时候生效)。
    :SOC_BT_SUPPORTED and esp32: - **Bluetooth**：从调用 :cpp:func:`esp_bt_controller_enable` 至 :cpp:func:`esp_bt_controller_disable` 期间。如果启用了蓝牙调制解调器，广播关闭时将释放此管理锁。但依然占用 ``ESP_PM_NO_LIGHT_SLEEP`` 锁，除非将 :ref:`CONFIG_BTDM_CTRL_LOW_POWER_CLOCK` 选项设置为 “外部 32 kHz 晶振”。
    :SOC_BT_SUPPORTED and not esp32: - **Bluetooth**：从调用 :cpp:func:`esp_bt_controller_enable` 至 :cpp:func:`esp_bt_controller_disable` 期间。如果启用了蓝牙调制解调器，广播关闭时将释放此管理锁。但依然占用 ``ESP_PM_NO_LIGHT_SLEEP`` 锁。

以下外设驱动程序无法感知动态调频，应用程序需自己获取/释放管理锁：

.. list::

    - PCNT
    - Sigma-delta
    - 旧版定时器驱动 (Timer Group)
    :SOC_MCPWM_SUPPORTED: - MCPWM


Light-sleep 外设下电
-------------------------

.. only:: esp32c6 or esp32h2

    {IDF_TARGET_NAME} 支持在 Light-sleep 时掉电外设的电源域.

    如果在 menuconfig 中启用了 :ref:`CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP`，在初始化外设时，驱动会将外设工作的寄存器上下文注册到休眠备份链表中，在进入休眠前，``REG_DMA`` 外设会读取休眠备份链表中的配置，根据链表中的配置将外设的寄存器上下文备份至内存，``REG_DMA`` 也会在唤醒时将上下文从内存恢复到外设寄存中。

    目前 IDF 支持以下外设的 Light-sleep 上下文备份：
    - INT_MTX
    - TEE/APM
    - IO_MUX / GPIO
    - UART0
    - TIMG0
    - SPI0/1
    - SYSTIMER
    - RMT

    以下外设尚未支持：
    - ETM
    - TIMG1
    - ASSIST_DEBUG
    - Trace
    - Crypto: AES/ECC/HMAC/RSA/SHA/DS/XTA_AES/ECDSA
    - SPI2
    - I2S
    - PCNT
    - USB-Serial-JTAG
    - TWAI
    - LEDC
    - MCPWM
    - SARADC
    - SDIO
    - PARL_IO
    - UART1

    对于未支持 Light-sleep 上下文备份的外设，若启用了电源管理功能，应在外设工作时持有 ``ESP_PM_NO_LIGHT_SLEEP`` 锁以避免进入休眠导致外设工作上下文丢失。

    .. note::

        当外设电源域在睡眠期间断电时，IO_MUX 和 GPIO 模块都处于下电状态，这意味着芯片引脚的状态不会受这些模块控制。要在休眠期间保持 IO 的状态，需要在配置 GPIO 状态前后调用 :cpp:func:`gpio_hold_dis` 和 :cpp:func:`gpio_hold_en`。此操作可确保 IO 配置被锁存，防止 IO 在睡眠期间浮空。


API 参考
-------------

.. include-build-file:: inc/esp_pm.inc
