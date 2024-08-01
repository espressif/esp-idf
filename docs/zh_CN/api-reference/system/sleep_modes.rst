睡眠模式
===========

:link_to_translation:`en:[English]`

{IDF_TARGET_SPI_POWER_DOMAIN:default="VDD_SPI", esp32="VDD_SDIO"}

概述
--------

{IDF_TARGET_NAME} 具有 Light-sleep 和 Deep-sleep 两种睡眠节能模式。根据应用所使用的功能，还有一些细分的子睡眠模式。关于这些睡眠模式和其子模式，参见 :ref:`sleep_modes`。另外，还可以配置一些断电选项来进一步减少功耗，请参见 :ref:`power_down_options` 。

睡眠模式有多种唤醒源。这些唤醒源也可以组合在一起，此时任何一个唤醒源都可以触发唤醒。:ref:`api-reference-wakeup-source` 详细描述了这些唤醒源，以及配置 API。

断电选项和唤醒源的配置不是必要的，并且可以在进入睡眠模式前的任意时候进行。

最后，应用通过调用开始睡眠的 API 来使芯片进入其中一种睡眠模式，更多详情请参见 :ref:`enter_sleep`。当唤醒条件满足，芯片就会从睡眠中被唤醒。关于如何获取唤醒原因，请参见 :ref:`wakeup_cause` 。关于醒来后如何处理唤醒源，请参见 :ref:`disable_sleep_wakeup_source` 。

.. _sleep_modes:

睡眠节能模式
----------------------

在 Light-sleep 模式下，数字外设、CPU、以及大部分 RAM 都使用时钟门控，同时其供电电压降低。退出该模式后，数字外设、CPU 和 RAM 恢复运行，并且内部状态将被保留。

在 Deep-sleep 模式下，CPU、大部分 RAM、以及所有由时钟 APB_CLK 驱动的数字外设都会被断电。芯片上继续处于供电状态的部分仅包括：

    .. list::

        - RTC 控制器
        :SOC_ULP_SUPPORTED: - ULP 协处理器
        :SOC_RTC_FAST_MEM_SUPPORTED: - RTC 高速内存
        :SOC_RTC_SLOW_MEM_SUPPORTED: - RTC 低速内存

.. only:: SOC_WIFI_SUPPORTED and SOC_BT_SUPPORTED

    睡眠模式下的 Wi-Fi 和 Bluetooth 功能
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    在 Light-sleep 和 Deep-sleep 模式下，无线外设会被断电。因此，在进入这两种睡眠模式前，应用程序必须调用恰当的函数（:cpp:func:`nimble_port_stop`、:cpp:func:`nimble_port_deinit`、:cpp:func:`esp_bluedroid_disable`、:cpp:func:`esp_bluedroid_deinit`、:cpp:func:`esp_bt_controller_disable`、:cpp:func:`esp_bt_controller_deinit` 或 :cpp:func:`esp_wifi_stop`）来禁用 Wi-Fi 和 Bluetooth。在 Light-sleep 或 Deep-sleep 模式下，即使不调用这些函数也无法连接 Wi-Fi 和 Bluetooth。

    如需保持 Wi-Fi 和 Bluetooth 连接，请启用 Wi-Fi 和 Bluetooth Modem-sleep 模式和自动 Light-sleep 模式（请参阅 :doc:`电源管理 API <power_management>`）。在这两种模式下，Wi-Fi 和 Bluetooth 驱动程序发出请求时，系统将自动从睡眠中被唤醒，从而保持连接。

.. only:: SOC_WIFI_SUPPORTED and not SOC_BT_SUPPORTED

    睡眠模式下的 Wi-Fi 功能
    ^^^^^^^^^^^^^^^^^^^^^^^^^^

    在 Light-sleep 和 Deep-sleep 模式下，无线外设会被断电。因此，在进入这两种睡眠模式前，应用程序必须调用恰当的函数 (:cpp:func:`esp_wifi_stop`) 来禁用 Wi-Fi。在 Light-sleep 或 Deep-sleep 模式下，即使不调用此函数也无法连接 Wi-Fi。

    如需保持 Wi-Fi 连接，请启用 Wi-Fi Modem-sleep 模式和自动 Light-sleep 模式（请参阅 :doc:`电源管理 API <power_management>`）。在这两种模式下，Wi-Fi 驱动程序发出请求时，系统将自动从睡眠中被唤醒，从而保持与 AP 的连接。

.. only:: esp32s2 or esp32s3 or esp32c2 or esp32c3

    子睡眠模式
    ^^^^^^^^^^^^^^^

    下表首行表示子睡眠模式，首列表示不同模式支持的功能。在睡眠模式下，支持更多功能的模式功耗可能更大。睡眠系统会自动选择满足用户功能需求且功耗最小的模式。

    Deep-sleep：

    .. list-table::
       :widths: auto
       :header-rows: 2

       * -
         - DSLP_ULTRA_LOW
         - DSLP_DEFAULT
         - DSLP_8MD256/
       * -
         -
         -
         - DSLP_ADC_TSENS
       * - ULP/触摸传感器（仅限 ESP32-S2、ESP32-S3）
         - Y
         - Y
         - Y
       * - RTC IO 输入/高温下 RTC 内存
         -
         - Y
         - Y
       * - ADC_TSEN_MONITOR
         -
         -
         - Y
       * - 8MD256 作为 RTC_SLOW_CLK 时钟源
         -
         -
         - Y

    功能：

    1. RTC IO 输入/高温下 RTC 内存（试验功能）：将 RTC IO 用作输入管脚，或在高温下使用 RTC 内存。禁用上述功能，芯片可进入超低功耗模式。由 API :cpp:func:`rtc_sleep_enable_ultra_low` 控制。

    2. ADC_TSEN_MONITOR：在 monitor 模式下使用 ADC/温度传感器（由 ULP 控制），通过 :cpp:func:`ulp_adc_init` 或其更高级别的 API 启用。仅适用于支持 monitor 模式的 ESP32-S2 和 ESP32-S3 芯片。

    3. 8MD256 作为 RTC_SLOW_CLK 时钟源：通过 ``CONFIG_RTC_CLK_SRC_INT_8MD256`` 选择 8MD256 作为 RTC_SLOW_CLK 时钟源时，芯片在 Deep-sleep 模式下将自动进入该子睡眠模式.

    Light-sleep：

    .. list-table::
       :widths: auto
       :header-rows: 2

       * -
         - LSLP_DEFAULT
         - LSLP_ADC_TSENS
         - LSLP_8MD256
         - LSLP_LEDC8M/
       * -
         -
         -
         -
         - LSLP_XTAL_FPU
       * - ULP/触摸传感器（仅限 ESP32-S2、ESP32-S3）
         - Y
         - Y
         - Y
         - Y
       * - RTC IO 输入/高温下 RTC 内存
         - Y
         - Y
         - Y
         - Y
       * - ADC_TSEN_MONITOR
         -
         - Y
         - Y
         - Y
       * - 8MD256 作为 RTC_SLOW_CLK 时钟源
         -
         -
         - Y
         - Y
       * - 数字外设使用 8 MHz RC 时钟源
         -
         -
         -
         - Y
       * - 保持 XTAL 时钟开启
         -
         -
         -
         - Y

    功能：（了解 Deep-sleep 模式，请参考前文 8MD256 和 ADC_TSEN_MONITOR 功能描述）

    1. 数字外设使用 8 MHz RC 时钟源：目前，只有 LEDC 在 Light-sleep 模式下使用该时钟源。当 LEDC 选用该时钟源时，此功能将自动启用。

    2. 保持 XTAL 时钟开启：在 Light-sleep 模式下保持 XTAL 时钟开启，由 ``ESP_PD_DOMAIN_XTAL`` 电源域控制。

    .. only:: esp32s2

        {IDF_TARGET_NAME} 的 LSLP_8MD256、LSLP_LEDC8M 和 LSLP_XTAL_FPU 功能使用相同的功耗模式。

    .. only:: esp32s3

        {IDF_TARGET_NAME} 的默认模式现已支持 ADC_TSEN_MONITOR 功能。

    .. only:: esp32c2 or esp32c3

        {IDF_TARGET_NAME} 不支持 ADC_TSEN_MONITOR 和 LSLP_ADC_TSENS 功能。

.. _api-reference-wakeup-source:

唤醒源
---------

通过 API ``esp_sleep_enable_X_wakeup`` 可启用唤醒源。唤醒源在芯片被唤醒后并不会被禁用，若你不再需要某些唤醒源，可通过 API :cpp:func:`esp_sleep_disable_wakeup_source` 将其禁用，详见 :ref:`disable_sleep_wakeup_source`。

以下是 {IDF_TARGET_NAME} 所支持的唤醒源。

定时器
^^^^^^^^

RTC 控制器中内嵌定时器，可用于在预定义的时间到达后唤醒芯片。时间精度为微秒，但其实际分辨率依赖于为 RTC_SLOW_CLK 所选择的时钟源。

.. only:: SOC_ULP_SUPPORTED

    关于 RTC 时钟选项的更多细节，请参考 **{IDF_TARGET_NAME} 技术参考手册** > **ULP 协处理器** [`PDF <{IDF_TARGET_TRM_EN_URL}#ulp>`__]。

在这种唤醒模式下，无需为睡眠模式中的 RTC 外设或内存供电。

调用 :cpp:func:`esp_sleep_enable_timer_wakeup` 函数可启用使用定时器唤醒睡眠模式。

.. only:: SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP

    触摸传感器
    ^^^^^^^^^^^^^^

    RTC IO 模块中包含这样一个逻辑——当发生触摸传感器中断时，触发唤醒。要启用此唤醒源，用户需要在芯片进入睡眠模式前配置触摸传感器中断功能。

    .. only:: esp32

        ESP32 修订版 0 和 1 仅在 RTC 外设没有被强制供电时支持该唤醒源（即 ESP_PD_DOMAIN_RTC_PERIPH 应被设置为 ESP_PD_OPTION_AUTO）。

    可调用 :cpp:func:`esp_sleep_enable_touchpad_wakeup` 函数来启用该唤醒源。

.. only:: SOC_PM_SUPPORT_EXT0_WAKEUP

    外部唤醒 (``ext0``)
    ^^^^^^^^^^^^^^^^^^^^^^

    RTC IO 模块中包含这样一个逻辑——当某个 RTC GPIO 被设置为预定义的逻辑值时，触发唤醒。RTC IO 是 RTC 外设电源域的一部分，因此如果该唤醒源被请求，RTC 外设将在 Deep-sleep 模式期间保持供电。

    在此模式下，RTC IO 模块被使能，因此也可以使用内部上拉或下拉电阻。配置时，应用程序需要在调用函数 :cpp:func:`esp_deep_sleep_start` 前先调用函数 :cpp:func:`rtc_gpio_pullup_en` 和 :cpp:func:`rtc_gpio_pulldown_en`。

    .. only:: esp32

        在 ESP32 修订版 0 和 1 中，此唤醒源与 ULP 和触摸传感器唤醒源都不兼容。

    可调用 :cpp:func:`esp_sleep_enable_ext0_wakeup` 函数来启用此唤醒源。

    .. warning::

        从睡眠模式中唤醒后，用于唤醒的 IO pad 将被配置为 RTC IO。因此，在将该 pad 用作数字 GPIO 之前，请调用 :cpp:func:`rtc_gpio_deinit` 函数对其进行重新配置。

.. only:: SOC_PM_SUPPORT_EXT1_WAKEUP

    外部唤醒 (``ext1``)
    ^^^^^^^^^^^^^^^^^^^^^^

    RTC 控制器中包含使用多个 RTC GPIO 触发唤醒的逻辑。从以下两个逻辑函数中任选其一，均可触发 ext1 唤醒：

    .. only:: esp32

        - 当任意一个所选管脚为高电平时唤醒 (ESP_EXT1_WAKEUP_ANY_HIGH)
        - 当所有所选管脚为低电平时唤醒 (ESP_EXT1_WAKEUP_ALL_LOW)

    .. only:: not esp32

        - 当任意一个所选管脚为高电平时唤醒 (ESP_EXT1_WAKEUP_ANY_HIGH)
        - 当任意一个所选管脚为低电平时唤醒 (ESP_EXT1_WAKEUP_ANY_LOW)

    此唤醒源由 RTC 控制器实现。区别于 ``ext0`` 唤醒源，在 RTC 外设断电的情况下此唤醒源同样支持唤醒。虽然睡眠期间 RTC IO 所在的 RTC 外设电源域将会断电，但是 ESP-IDF 会自动在系统进入睡眠前锁定唤醒管脚的状态并在退出睡眠时解除锁定，所以仍然可为唤醒管脚配置内部上拉或下拉电阻::

        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
        gpio_pullup_dis(gpio_num);
        gpio_pulldown_en(gpio_num);

    如果我们关闭 ``RTC_PERIPH`` 域，我们将使用 HOLD 功能在睡眠期间维持管脚上的上拉和下拉电阻。所选管脚的 HOLD 功能会在系统真正进入睡眠前被开启，这有助于进一步减小睡眠时的功耗::

        rtc_gpio_pullup_dis(gpio_num);
        rtc_gpio_pulldown_en(gpio_num);

    如果某些芯片缺少 ``RTC_PERIPH`` 域，我们只能使用 HOLD 功能来在睡眠期间维持管脚上的上拉和下拉电阻::

        gpio_pullup_dis(gpio_num);
        gpio_pulldown_en(gpio_num);

    可调用 :cpp:func:`esp_sleep_enable_ext1_wakeup_io` 函数可用于增加 ext1 唤醒 IO 并设置相应的唤醒电平。

    可调用 :cpp:func:`esp_sleep_disable_ext1_wakeup_io` 函数可用于移除 ext1 唤醒 IO。

    .. only:: SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN

        当前的 RTC 控制器也包含更强大的逻辑，允许配置的 IO 同时使用不同的唤醒电平。这可以通过 :cpp:func:`esp_sleep_enable_ext1_wakeup_io` 函数来进行配置。

    .. only:: not SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN

       .. note::

           由于硬件限制，当我们将多个 IO 用于 EXT1 唤醒，此时不允许将这些 IO 的唤醒模式配置成不同的电平，在 :cpp:func:`esp_sleep_enable_ext1_wakeup_io` 已有相应的内部检查机制。

    .. warning::

        - 使用 EXT1 唤醒源时，用于唤醒的 IO pad 将被配置为 RTC IO。因此，在将该 pad 用作数字 GPIO 之前，请调用 :cpp:func:`rtc_gpio_deinit` 函数对其进行重新配置。

        - RTC 外设在默认情况下配置为断电，此时，唤醒 IO 在进入睡眠状态前将被设置为保持状态。因此，从 Light-sleep 状态唤醒芯片后，请调用 ``rtc_gpio_hold_dis`` 来禁用保持功能，以便对管脚进行重新配置。对于 Deep-sleep 唤醒，此问题已经在应用启动阶段解决。

.. only:: SOC_ULP_SUPPORTED

    ULP 协处理器唤醒
    ^^^^^^^^^^^^^^^^^^^^^^

    当芯片处于睡眠模式时，ULP 协处理器仍然运行，可用于轮询传感器、监视 ADC 或触摸传感器的值，并在检测到特殊事件时唤醒芯片。ULP 协处理器是 RTC 外设电源域的一部分，运行存储在 RTC 低速内存中的程序。如果这一唤醒源被请求，RTC 低速内存将会在睡眠期间保持供电状态。RTC 外设会在 ULP 协处理器开始运行程序前自动上电；一旦程序停止运行，RTC 外设会再次自动断电。

    .. only:: esp32

        ESP32 修订版 0 和 1 仅在 RTC 外设没有被强制供电时支持该唤醒（即 ESP_PD_DOMAIN_RTC_PERIPH 应被设置为 ESP_PD_OPTION_AUTO）。

    可调用 :cpp:func:`esp_sleep_enable_ulp_wakeup` 函数来启用此唤醒源。

.. only:: SOC_RTCIO_WAKE_SUPPORTED

    GPIO 唤醒（仅适用于 Light-sleep 模式）
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    .. only:: SOC_PM_SUPPORT_EXT0_WAKEUP or SOC_PM_SUPPORT_EXT1_WAKEUP

        除了上述 EXT0 和 EXT1 唤醒源之外，还有一种从外部唤醒 Light-sleep 模式的方法——使用函数 :cpp:func:`gpio_wakeup_enable`。启用该唤醒源后，可将每个管脚单独配置为在高电平或低电平时唤醒。EXT0 和 EXT1 唤醒源只能用于 RTC IO，但此唤醒源既可以用于 RTC IO，可也用于数字 IO。

    .. only:: not (SOC_PM_SUPPORT_EXT0_WAKEUP or SOC_PM_SUPPORT_EXT1_WAKEUP)

       此外，还有一种从外部唤醒 Light-sleep 模式的方法。启用该唤醒源后，可将每个管脚单独配置为在高电平或低电平时调用 :cpp:func:`gpio_wakeup_enable` 函数触发唤醒。此唤醒源既可以用于 RTC IO，可也用于数字 IO。

    可调用 :cpp:func:`esp_sleep_enable_gpio_wakeup` 函数来启用此唤醒源。

    .. warning::

        在进入 Light-sleep 模式前，请查看将要驱动的 GPIO 管脚的电源域。如果有管脚属于 {IDF_TARGET_SPI_POWER_DOMAIN} 电源域，必须将此电源域配置为在睡眠期间保持供电。

        例如，在 ESP32-WROOM-32 开发板上，GPIO16 和 GPIO17 连接到 {IDF_TARGET_SPI_POWER_DOMAIN} 电源域。如果这两个管脚被配置为在睡眠期间保持高电平，则需将对应电源域配置为保持供电。为此，可以使用函数 :cpp:func:`esp_sleep_pd_config()`::

            esp_sleep_pd_config(ESP_PD_DOMAIN_VDDSDIO, ESP_PD_OPTION_ON);

    .. only:: SOC_PM_SUPPORT_TOP_PD

       .. note::

            .. only::  SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP

                在 Light-sleep 模式下，如果设置 Kconfig 选项 :ref:`CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP`，为了继续使用 :cpp:func:`gpio_wakeup_enable` 用于 GPIO 唤醒， 需要先调用 :cpp:func:`rtc_gpio_init` 和 :cpp:func:`rtc_gpio_set_direction`，用于设置 RTC IO 为输入模式。

                或者， 可以使用直接调用 :cpp:func:`esp_deep_sleep_enable_gpio_wakeup` 用于 GPIO 唤醒，因为此时 digital IO 的电源域已经被关闭，这个情况类似于进入 Deep-sleep。

            .. only::  not SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP

                在 Light-sleep 模式下，如果设置 Kconfig 选项 :ref:`CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP`，为了继续使用 :cpp:func:`gpio_wakeup_enable` 用于 GPIO 唤醒， 需要先调用 :cpp:func:`rtc_gpio_init` 和 :cpp:func:`rtc_gpio_set_direction`，用于设置 RTC IO 为输入模式。

.. only:: not SOC_RTCIO_WAKE_SUPPORTED

    GPIO 唤醒
    ^^^^^^^^^^^

    任何一个 IO 都可以用作外部输入管脚，将芯片从 Light-sleep 状态唤醒。调用 :cpp:func:`gpio_wakeup_enable` 函数可以将任意管脚单独配置为在高电平或低电平触发唤醒。此后，应调用 :cpp:func:`esp_sleep_enable_gpio_wakeup` 函数来启用此唤醒源。

    此外，可将由 VDD3P3_RTC 电源域供电的 IO 用于芯片的 Deep-sleep 唤醒。调用 :cpp:func:`esp_deep_sleep_enable_gpio_wakeup` 函数可以配置相应的唤醒管脚和唤醒触发电平，该函数用于启用相应管脚的 Deep-sleep 唤醒功能。

    .. only:: SOC_PM_SUPPORT_TOP_PD

       .. note::

            .. only::  SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP

                在 Light-sleep 模式下，如果设置 Kconfig 选项 :ref:`CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP`，可以使用直接调用 :cpp:func:`esp_deep_sleep_enable_gpio_wakeup` 用于 GPIO 唤醒，因为此时 digital IO 的电源域已经被关闭，这个情况类似于进入 Deep-sleep。

UART 唤醒（仅适用于 Light-sleep 模式）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

当 {IDF_TARGET_NAME} 从外部设备接收 UART 输入时，通常需要在输入数据可用时唤醒芯片。UART 外设支持在 RX 管脚上观测到一定数量的上升沿时，将芯片从 Light-sleep 模式中唤醒。调用 :cpp:func:`uart_set_wakeup_threshold` 函数可设置被观测上升沿的数量。请注意，触发唤醒的字符（及该字符前的所有字符）在唤醒后不会被 UART 接收，因此在发送数据之前，外部设备通常需要首先向 {IDF_TARGET_NAME} 额外发送一个字符以触发唤醒。

可调用 :cpp:func:`esp_sleep_enable_uart_wakeup` 函数来启用此唤醒源。

使用 UART 唤醒之后，在芯片 Active 模式下需要让 UART 接受一些数据用来清零内部的唤醒指示信号。不然的话，下一次 UART 唤醒的触发将只需要比配置的阈值少两个上升沿的数量。

    .. only:: SOC_PM_SUPPORT_TOP_PD

       .. note::

           在 Light-sleep 模式下，设置 Kconfig 选项 :ref:`CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP` 将使 UART 唤醒失效。

.. _disable_sleep_wakeup_source:

禁用睡眠模式唤醒源
^^^^^^^^^^^^^^^^^^^^^^^^^^^

调用 API :cpp:func:`esp_sleep_disable_wakeup_source` 可以禁用给定唤醒源的触发器，从而禁用该唤醒源。此外，如果将参数设置为 ``ESP_SLEEP_WAKEUP_ALL``，该函数可用于禁用所有触发器。

.. _power_down_options:

断电选项
--------------

应用程序可以使用 API :cpp:func:`esp_sleep_pd_config` 强制 RTC 外设和 RTC 内存进入特定断电模式。在 Deep-sleep 模式下，你还可以通过隔离一些 IO 来进一步降低功耗。

RTC 外设和内存断电
^^^^^^^^^^^^^^^^^^^^^

默认情况下，调用函数 :cpp:func:`esp_deep_sleep_start` 和 :cpp:func:`esp_light_sleep_start` 后，所有唤醒源不需要的 RTC 电源域都会被断电。可调用函数 :cpp:func:`esp_sleep_pd_config` 来修改这一设置。

.. only:: esp32

    注意：在 ESP32 修订版 1 中，RTC 高速内存在 Deep-sleep 期间将总是保持使能，以保证复位后可运行 Deep-sleep stub。如果应用程序在 Deep-sleep 模式后无需复位，你也可以对其进行修改。

.. only:: SOC_RTC_SLOW_MEM_SUPPORTED

    如果程序中的某些值被放入 RTC 低速内存中（例如使用 ``RTC_DATA_ATTR`` 属性），RTC 低速内存将默认保持供电。如果有需要，也可以使用函数 :cpp:func:`esp_sleep_pd_config` 对其进行修改。

.. only:: not SOC_RTC_SLOW_MEM_SUPPORTED and SOC_RTC_FAST_MEM_SUPPORTED

    {IDF_TARGET_NAME} 中只有 RTC 高速内存，因此，如果程序中的某些值被标记为 ``RTC_DATA_ATTR``、``RTC_SLOW_ATTR`` 或 ``RTC_FAST_ATTR`` 属性，那么所有这些值都将被存入 RTC 高速内存，默认情况下保持供电。如有需要，也可以使用函数 :cpp:func:`esp_sleep_pd_config` 对其进行修改。

flash 断电
^^^^^^^^^^

默认情况下，调用函数 :cpp:func:`esp_light_sleep_start` 后，flash **不会** 断电，因为在 sleep 过程中断电 flash 存在风险。具体而言，flash 断电需要时间，但是在此期间，系统有可能被唤醒，导致 flash 重新被上电。此时，断电尚未完成又重新上电的硬件行为有可能导致 flash 无法正常工作。

理论上讲，在 flash 完全断电后可以仅唤醒系统，然而现实情况是 flash 断电所需的时间很难预测。如果用户为 flash 供电电路添加了滤波电容，断电所需时间可能会更长。此外，即使可以预知 flash 彻底断电所需的时间，有时也不能通过设置足够长的睡眠时间来确保 flash 断电的安全（比如，突发的异步唤醒源会使得实际的睡眠时间不可控）。

.. warning::

    如果在 flash 的供电电路上添加了滤波电容，那么应当尽一切可能避免 flash 断电。

因为这些不可控的因素，ESP-IDF 很难保证 flash 断电的绝对安全。因此 ESP-IDF 不推荐用户断电 flash。对于一些功耗敏感型应用，可以通过设置 Kconfig 配置项 :ref:`CONFIG_ESP_SLEEP_FLASH_LEAKAGE_WORKAROUND` 来减少 Light-sleep 期间 flash 的功耗。这种方式在几乎所有场景下都要比断电 flash 更好，兼顾了安全性和功耗。

.. only:: SOC_SPIRAM_SUPPORTED

    值得一提的是，PSRAM 也有一个类似的 Kconfig 配置项 :ref:`CONFIG_ESP_SLEEP_PSRAM_LEAKAGE_WORKAROUND`。

考虑到有些用户能够充分评估断电 flash 的风险，并希望通过断电 flash 来获得更低的功耗，因此 ESP-IDF 提供了两种断电 flash 的机制：

    .. list::

        - 设置 Kconfig 配置项 :ref:`CONFIG_ESP_SLEEP_POWER_DOWN_FLASH` 将使 ESP-IDF 以一个严格的条件来断电 flash。严格的条件具体指的是，RTC timer 是唯一的唤醒源 **且** 睡眠时间比 flash 彻底断电所需时间更长。
        - 调用函数 ``esp_sleep_pd_config(ESP_PD_DOMAIN_VDDSDIO, ESP_PD_OPTION_OFF)`` 将使 ESP-IDF 以一个宽松的条件来断电 flash。宽松的条件具体指的是 RTC timer 唤醒源未被使能 **或** 睡眠时间比 flash 彻底断电所需时间更长。

.. note::

    .. list::

        - Light-sleep 模式下，ESP-IDF 没有提供保证 flash 一定会被断电的机制。
        - 不管用户的配置如何，函数 :cpp:func:`esp_deep_sleep_start` 都会强制断电 flash。

配置 IO（仅适用于 Deep-sleep）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

一些 {IDF_TARGET_NAME} IO 在默认情况下启用内部上拉或下拉电阻。如果这些管脚在 Deep-sleep 模式下中受外部电路驱动，电流流经这些上下拉电阻时，可能会增加电流消耗。

.. only:: SOC_RTCIO_HOLD_SUPPORTED and SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    想要隔离这些管脚以避免额外的电流消耗，请调用 :cpp:func:`rtc_gpio_isolate` 函数。

    例如，在 ESP32-WROVER 模组上，GPIO12 在外部上拉，但其在 ESP32 芯片中也有内部下拉。这意味着在 Deep-sleep 模式中，电流会流经这些外部和内部电阻，使电流消耗超出可能的最小值。

    在函数 :cpp:func:`esp_deep_sleep_start` 前增加以下代码即可避免额外电流消耗::

        rtc_gpio_isolate(GPIO_NUM_12);

.. only:: esp32c2 or esp32c3

    在 Deep-sleep 模式中：
        - 数字 GPIO (GPIO6 ~ 21) 处于高阻态。
        - RTC GPIO (GPIO0 ~ 5) 可能处于以下状态：
            - 如果未启用保持 (hold) 功能，RTC GPIO 将处于高阻态。
            - 如果启用保持功能，RTC GPIO 管脚将会在保持功能开启时处于锁存状态。

.. _enter_sleep:

进入睡眠模式
----------------------

应用程序通过 API :cpp:func:`esp_light_sleep_start` 或 :cpp:func:`esp_deep_sleep_start` 进入 Light-sleep 或 Deep-sleep 模式。此时，系统将按照被请求的唤醒源和断电选项配置有关的 RTC 控制器参数。

允许在未配置唤醒源的情况下进入睡眠模式。在此情况下，芯片将一直处于睡眠模式，直到从外部被复位。

UART 输出处理
^^^^^^^^^^^^^^^^^^^^

在进入睡眠模式之前，调用函数 :cpp:func:`esp_deep_sleep_start` 会冲刷掉 UART FIFO 缓存。

当使用函数 :cpp:func:`esp_light_sleep_start` 进入 Light-sleep 模式时，UART FIFO 将不会被冲刷。与之相反，UART 输出将被暂停，FIFO 中的剩余字符将在 Light-sleep 唤醒后被发送。

.. _wakeup_cause:

检查睡眠唤醒原因
---------------------------

:cpp:func:`esp_sleep_get_wakeup_cause` 函数可用于检测是何种唤醒源在睡眠期间被触发。

.. only:: SOC_TOUCH_SENSOR_SUPPORTED

    对于触摸传感器唤醒源，可以调用函数 :cpp:func:`esp_sleep_get_touchpad_wakeup_status` 来确认触发唤醒的触摸管脚。

.. only:: SOC_PM_SUPPORT_EXT1_WAKEUP

    对于 ext1 唤醒源，可以调用函数 :cpp:func:`esp_sleep_get_ext1_wakeup_status` 来确认触发唤醒的触摸管脚。

应用程序示例
-------------------

.. list::

    - :example:`protocols/sntp` 演示如何实现 Deep-sleep 模式的基本功能，周期性唤醒 ESP 模块，以从 NTP 服务器获取时间。
    :SOC_WIFI_SUPPORTED: - :example:`wifi/power_save` 演示如何通过 Wi-Fi Modem-sleep 模式和自动 Light-sleep 模式保持 Wi-Fi 连接。
    :SOC_BT_SUPPORTED: - :example:`bluetooth/nimble/power_save` 演示如何通过 Bluetooth Modem-sleep 模式和自动 Light-sleep 模式保持 Bluetooth 连接。
    :SOC_ULP_SUPPORTED: - :example:`system/deep_sleep` 演示如何使用 Deep-sleep 唤醒触发器和 ULP 协处理器编程。
    :not SOC_ULP_SUPPORTED: - :example:`system/deep_sleep` 演示如何通过 {IDF_TARGET_NAME} 的唤醒源，如 RTC 定时器, GPIO, EXT0, EXT1, 触摸传感器等，触发 Deep-sleep 唤醒。
    - :example:`system/light_sleep` 演示如何使用  {IDF_TARGET_NAME} 的唤醒源，如定时器，GPIO，触摸传感器等，触发 Light-sleep 唤醒。

API 参考
-------------

.. include-build-file:: inc/esp_sleep.inc
