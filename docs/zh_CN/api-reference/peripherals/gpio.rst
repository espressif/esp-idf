GPIO & RTC GPIO
===============

:link_to_translation:`en:[English]`

GPIO 汇总
---------

.. include:: gpio/{IDF_TARGET_PATH_NAME}.inc
    :start-after: gpio-summary
    :end-before: ---

.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    .. only:: not SOC_LP_PERIPHERALS_SUPPORTED

        当 GPIO 连接到 RTC 低功耗和模拟子系统时，{IDF_TARGET_NAME} 芯片还单独支持 RTC GPIO。可在以下情况时使用这些管脚功能：

        .. only:: SOC_LP_PERIPHERALS_SUPPORTED

        当 GPIO 连接到 RTC 低功耗、模拟子系统、低功耗外设时，{IDF_TARGET_NAME} 芯片还单独支持 RTC GPIO。可在以下情况时使用这些管脚功能：

    .. list::

        - 处于 Deep-sleep 模式时
        :SOC_ULP_FSM_SUPPORTED: - :doc:`超低功耗协处理器 (ULP-FSM) <../../api-reference/system/ulp>` 运行时
        :SOC_RISCV_COPROC_SUPPORTED: - :doc:`超低功耗协处理器 (ULP-RISC-V) <../../api-reference/system/ulp-risc-v>` 运行时
        :SOC_LP_CORE_SUPPORTED: - :doc:`超低功耗协处理器 (ULP-LP-Core) <../../api-reference/system/ulp-lp-core>` 运行时
        - 使用 ADC/DAC 等模拟功能时
        :SOC_LP_PERIPHERALS_SUPPORTED: - 使用低功耗外设时，例如： LP_UART ， LP_I2C 等

获取 IO 管脚实时配置状态
--------------------------------------------

GPIO 驱动提供了一个函数 :cpp:func:`gpio_dump_io_configuration` 用来输出指定管脚的实时配置状态，包括上下拉、输入输出使能、管脚映射等。例如，以下命令可用于输出 GPIO4，GPIO8 与 GPIO26 的配置状态：

::

    gpio_dump_io_configuration(stdout, (1ULL << 4) | (1ULL << 18) | (1ULL << 26));

其输出信息如下：

::

    ================IO DUMP Start================
    IO[4] -
      Pullup: 1, Pulldown: 0, DriveCap: 2
      InputEn: 1, OutputEn: 0, OpenDrain: 0
      FuncSel: 1 (GPIO)
      GPIO Matrix SigIn ID: (simple GPIO input)
      SleepSelEn: 1

    IO[18] -
      Pullup: 0, Pulldown: 0, DriveCap: 2
      InputEn: 0, OutputEn: 1, OpenDrain: 0
      FuncSel: 1 (GPIO)
      GPIO Matrix SigOut ID: 256 (simple GPIO output)
      SleepSelEn: 1

    IO[26] **RESERVED** -
      Pullup: 1, Pulldown: 0, DriveCap: 2
      InputEn: 1, OutputEn: 0, OpenDrain: 0
      FuncSel: 0 (IOMUX)
      SleepSelEn: 1

    =================IO DUMP End==================

如果你想要查看所有管脚的配置状态，可以使用命令

::

    gpio_dump_all_io_configuration(stdout, SOC_GPIO_VALID_GPIO_MASK);

如果 IO 管脚通过 GPIO 交换矩阵连接到内部外设信号，输出信息打印中的外设信号 ID 定义可以在 :component_file:`soc/{IDF_TARGET_PATH_NAME}/include/soc/gpio_sig_map.h` 头文件中查看。``**RESERVED**`` 字样则表示此 IO 用于连接 SPI flash 或 PSRAM，强烈建议不要重新配置这些管脚用于其他功能。

.. only:: esp32c3 or esp32c6 or esp32h2 or esp32p4 or esp32s2 or esp32s3

    配置 USB PHY 管脚 为普通 GPIO 管脚
    ---------------------------------------

    要将 USB PHY 管脚配置为普通 GPIO 管脚，可使用函数 :cpp:func:`gpio_config`，请参考以下代码片段来进行配置。

    .. code-block:: c

        gpio_config_t usb_phy_conf = {
            .pin_bit_mask = (1ULL << USB_PHY_DP_PIN) | (1ULL << USB_PHY_DM_PIN),
            .mode = GPIO_MODE_INPUT_OUTPUT,
            .pull_up_en = 0,
            .pull_down_en = 0,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&usb_phy_conf);

.. only:: SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER or SOC_GPIO_FLEX_GLITCH_FILTER_NUM

    GPIO 毛刺过滤器
    ---------------

    {IDF_TARGET_NAME} 内置硬件的过滤器可以过滤掉 GPIO 输入端口上的毛刺信号，在一定程度上避免错误触发中断或者是错把噪声当成有效的外设信号。

    .. only:: SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER

        每个 GPIO 都可以使用独立的毛刺过滤器，该过滤器可以将那些脉冲宽度窄于 **2** 个采样时钟的信号剔除掉，该宽度无法配置。GPIO 对输入信号的采样时钟通常是 IO_MUX 的时钟源。在驱动中，此类过滤器称为 ``管脚毛刺过滤器``。可以调用 :cpp:func:`gpio_new_pin_glitch_filter` 函数创建一个过滤器句柄。过滤器的相关配置保存在 :cpp:type:`gpio_pin_glitch_filter_config_t` 结构体中。

        - :cpp:member:`gpio_pin_glitch_filter_config_t::gpio_num` 设置启用毛刺过滤器的 GPIO 编号。

    .. only:: SOC_GPIO_FLEX_GLITCH_FILTER_NUM

        {IDF_TARGET_FLEX_GLITCH_FILTER_NUM:default="8"}

        {IDF_TARGET_NAME} 提供了 {IDF_TARGET_FLEX_GLITCH_FILTER_NUM} 个灵活的毛刺过滤器，被过滤信号的脉冲宽度可以由软件进行配置。此类过滤器则称为 ``灵活毛刺过滤器``。每个过滤器都可以应用于任意 GPIO 输入，然而，将多个过滤器应用于同一 GPIO 上效果并不会叠加。可以调用 :cpp:func:`gpio_new_flex_glitch_filter` 函数来创建一个过滤器句柄。过滤器的相关配置保存在 :cpp:type:`gpio_flex_glitch_filter_config_t` 结构体中。

        - :cpp:member:`gpio_flex_glitch_filter_config_t::gpio_num` 设置启用毛刺过滤器的 GPIO 编号。
        - :cpp:member:`gpio_flex_glitch_filter_config_t::window_width_ns` 和 :cpp:member:`gpio_flex_glitch_filter_config_t::window_thres_ns` 是毛刺过滤器的关键参数。在:cpp:member:`gpio_flex_glitch_filter_config_t::window_width_ns` 时间内，任何脉冲信号，如果它的宽度小于 :cpp:member:`gpio_flex_glitch_filter_config_t::window_thres_ns` ，那么该脉冲信号就会被滤除掉。:cpp:member:`gpio_flex_glitch_filter_config_t::window_thres_ns` 的值不能大于 :cpp:member:`gpio_flex_glitch_filter_config_t::window_width_ns`。

    .. only:: SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER and SOC_GPIO_FLEX_GLITCH_FILTER_NUM

        请注意，``管脚毛刺过滤器`` 和 ``灵活毛刺过滤器`` 是各自独立的，支持为同一 GPIO 同时启用这两种过滤器。

    毛刺过滤器默认关闭，可调用 :cpp:func:`gpio_glitch_filter_enable` 使能过滤器。如需回收这个过滤器，可以调用 :cpp:func:`gpio_del_glitch_filter` 函数。在回收句柄前，请确保过滤器处于关闭状态，否则需调用 :cpp:func:`gpio_glitch_filter_disable`。


.. only:: SOC_GPIO_SUPPORT_PIN_HYS_FILTER

    GPIO 迟滞过滤器
    ---------------

    {IDF_TARGET_NAME} 支持输入引脚的硬件迟滞，这可以减少由于输入电压在逻辑 0、1 临界值附近时采样不稳定造成的 GPIO 中断误触，尤其是当输入逻辑电平转换较慢，电平建立时间较长时。

    .. only:: SOC_GPIO_SUPPORT_PIN_HYS_CTRL_BY_EFUSE

        每个引脚可以独立启用迟滞功能。默认情况下，它由 eFuse 控制，且处于关闭状态，但也可以由软件控制启用或禁用。你可以通过配置 :cpp:member:`gpio_config_t::hys_ctrl_mode` 来选择迟滞控制模式。迟滞控制模式会和其余 GPIO 配置一起在 :cpp:func:`gpio_config` 中生效。

        .. note::

            当迟滞功能由 eFuse 控制时，仍然可以独立的控制每个引脚的该功能，你需要 `烧断 eFuse <https://docs.espressif.com/projects/esptool/en/latest/esp32/espefuse/index.html>`_ ，以在特定 GPIO 上启用迟滞功能。

    .. only:: not SOC_GPIO_SUPPORT_PIN_HYS_CTRL_BY_EFUSE

        每个引脚可以独立启用迟滞功能。默认情况下，它处于关闭状态。你可以通过配置 :cpp:member:`gpio_config_t::hys_ctrl_mode` 来选择启用与否。迟滞控制模式会和其余 GPIO 配置一起在 :cpp:func:`gpio_config` 中生效。


应用示例
-------------------

* GPIO 输出和输入中断示例：:example:`peripherals/gpio/generic_gpio`。

API 参考 - 普通 GPIO
---------------------------

.. include-build-file:: inc/gpio.inc
.. include-build-file:: inc/gpio_types.inc


.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    API 参考 - RTC GPIO
    ------------------------

    .. include-build-file:: inc/rtc_io.inc
    .. include-build-file:: inc/lp_io.inc
    .. include-build-file:: inc/rtc_io_types.inc

.. only:: SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER or SOC_GPIO_FLEX_GLITCH_FILTER_NUM

      API 参考 - GPIO 毛刺过滤器
      --------------------------

      .. include-build-file:: inc/gpio_filter.inc
