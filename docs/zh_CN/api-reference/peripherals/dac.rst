数模转换器 (DAC)
=================

:link_to_translation:`en:[English]`

{IDF_TARGET_DAC_CH_1: default = "Not Updated!", esp32 = "GPIO25", esp32s2 = "GPIO17"}
{IDF_TARGET_DAC_CH_2: default = "Not Updated!", esp32 = "GPIO26", esp32s2 = "GPIO18"}
{IDF_TARGET_DAC_REF_PIN: default = "Not Updated!", esp32 = "VDD3P3_RTC", esp32s2 = "VDD3P3_RTC_IO"}

概况
----

{IDF_TARGET_NAME} 有两个 8 位数模转换器 (DAC) 通道，分别连接到 {IDF_TARGET_DAC_CH_1}（通道 1）和 {IDF_TARGET_DAC_CH_2}（通道 2）。每个 DAC 通道可以将数字值 0 ~ 255 转换成模拟电压 0 ~ Vref（此处的 Vref 为 {IDF_TARGET_DAC_REF_PIN} 引脚输入的参考电压，一般来说其输入的电压值应等于电源电压 VDD）。输出电压可按以下方式计算::

    out_voltage = Vref * digi_val / 255

DAC 外设支持以下列方式输出模拟信号：

1. 直接输出电压。DAC 通道持续输出某一指定电压。
2. 通过 DMA 输出连续模拟信号。DAC 以某一特定频率转换缓冲器中的数据。
3. 通过余弦波发生器输出余弦波。DAC 通道可以输出特定频率和振幅的余弦波。

其他模拟输出选项可参考 :doc:`Sigma-Delta 调制 <sdm>` 和 :doc:`LED PWM 控制器 <ledc>`。这两个模块均输出高频的 PWM/PDM 信号，也可借助硬件低通滤波输出较低频率的模拟信号。

DAC 文件结构
------------

.. figure:: ../../../_static/diagrams/dac/dac_file_structure.png
    :align: center
    :alt: DAC 文件结构

    DAC 文件结构


**需包含在 DAC 应用程序中的公共头文件包括：**

- ``dac.h``：原有 DAC 驱动的最上层头文件，只包含在使用原有驱动 API 的应用程序中。
- ``dac_oneshot.h``：新 DAC 驱动的最上层头文件，应包含在使用新驱动 API（单次模式）的应用程序中。
- ``dac_cosine.h``：新 DAC 驱动的最上层头文件，应包含在使用新驱动 API（余弦模式）的应用程序中。
- ``dac_continuous.h``：新 DAC 驱动的最上层头文件，应包含在使用新驱动 API（连续模式）的应用程序中。

.. note::

    原有驱动程序与新驱动程序无法共存。使用原有驱动需包含 ``dac.h``，使用新驱动需包含 ``dac_oneshot.h``、 ``dac_cosine.h`` 和 ``dac_continuous.h``。后续更新或将移除原有驱动程序。

功能概览
--------

资源管理
^^^^^^^^

{IDF_TARGET_NAME} 有两个 DAC 通道。通道的软件资源互相独立，用户可以根据具体情况调用 :cpp:type:`dac_oneshot_handle_t`、 :cpp:type:`dac_cosine_handle_t` 或 :cpp:type:`dac_continuous_handle_t` 来管理不同通道，但不支持在同一个通道上注册不同模式。

电压直接输出（单次/直接模式）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在这种模式下，DAC 通道每次调用 :cpp:func:`dac_oneshot_output_voltage` （可在 ISR 中调用）时都可以将一个 8 位数字转换为模拟值。直至下一次转换开始前，DAC 通道上都将维持该模拟电压。开始转换电压前，需要首先调用 :cpp:func:`dac_oneshot_new_channel` 来启用该 DAC 通道。

连续波输出（连续/DMA 模式）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

DAC 通道可以通过 DMA 连续转换数字信号，这种模式下有三种写入 DAC 数据的方法：

    1. 常规写入（同步）：一次性传输所有数据并在所有数据均已载入 DMA 缓冲区前保持阻塞状态。如果不再继续输入数据，电压将维持在最后的转换值。该模式通常用于传输音频等长信号。要连续转换数据，需要调用 :cpp:func:`dac_continuous_new_channels` 来分配连续通道句柄，调用 :cpp:func:`dac_continuous_enable` 来启用 DMA 转换，然后调用 :cpp:func:`dac_continuous_write` 来同步写入数据。示例可参考 :example:`peripherals/dac/dac_continuous/dac_audio`。
    2. 循环写入：在数据载入 DMA 缓冲区后，缓冲区中的数据将以非阻塞的方式被循环转换。但要注意，输入的缓冲区大小受 DMA 描述符数量和 DMA 缓冲区大小的限制。该模式通常用于传输如正弦波等需要重复的短信号。为了启用循环写入，需要在启用 DAC 连续模式后调用 :cpp:func:`dac_continuous_write_cyclically`。示例可参考 :example:`peripherals/dac/dac_continuous/signal_generator`。
    3. 异步写入。可根据事件回调异步传输数据。需要调用 :cpp:member:`dac_event_callbacks_t::on_convert_done` 以启用异步模式。用户在回调中可得到 :cpp:type:`dac_event_data_t`，其中包含 DMA 缓冲区的地址和长度，即允许用户直接将数据载入 DMA 缓冲区。启用异步写入前需要调用 :cpp:func:`dac_continuous_register_event_callback`、 :cpp:member:`dac_event_callbacks_t::on_convert_done` 和 :cpp:func:`dac_continuous_start_async_writing`。注意，异步写入一旦开始，回调函数将被持续触发。调用 :cpp:func:`dac_continuous_write_asynchronously` 可以在某个单独任务中或直接在回调函数中载入数据。示例可参考 :example:`peripherals/dac/dac_continuous/dac_audio`。

.. only:: esp32

    在 ESP32 上，DAC 的数字控制器可以在内部连接到 I2S0，并借用其 DMA 进行连续转换。虽然 DAC 转换仅需 8 位数据，但它必须是左移的 8 位（即 16 位中的高 8 位），以满足 I2S 通信格式。默认状态下驱动程序将自动扩充数据至 16 位，如需手动扩充，请在 menuconfig 中禁用 :ref:`CONFIG_DAC_DMA_AUTO_16BIT_ALIGN`。

    DAC 的数字控制器的时钟也来自 I2S0，有以下两种时钟源可选：

    - :cpp:enumerator:`dac_continuous_digi_clk_src_t::DAC_DIGI_CLK_SRC_PLL_D2` 支持 19.6 KHz 到若干 MHz 之间的频率。该时钟源为默认时钟源，也可通过选择 :cpp:enumerator:`dac_continuous_digi_clk_src_t::DAC_DIGI_CLK_SRC_DEFAULT` 来启用该时钟源。
    - :cpp:enumerator:`dac_continuous_digi_clk_src_t::DAC_DIGI_CLK_SRC_APLL` 支持 648 Hz 到若干 MHz 之间的频率。该时钟源可能会被其他外设占用而导致频率无法更改，此时除非 APLL 仍能准确分频得到 DAC DMA 的目标频率，否则将无法使用该时钟源。

.. only:: esp32s2

    在 ESP32-S2 上，DAC 的数字控制器可以在内部连接到 SPI3，并借用其 DMA 进行连续转换。

    DAC 的数字控制器的时钟源包括：

    - :cpp:enumerator:`dac_continuous_digi_clk_src_t::DAC_DIGI_CLK_SRC_APB` 支持 77 Hz 到若干 MHz 之间的频率。该时钟源为默认时钟源，也可通过选择 :cpp:enumerator:`dac_continuous_digi_clk_src_t::DAC_DIGI_CLK_SRC_DEFAULT` 来启用该时钟源。
    - :cpp:enumerator:`dac_continuous_digi_clk_src_t::DAC_DIGI_CLK_SRC_APLL` 支持 6 Hz 到若干 MHz 之间的频率。该时钟源可能会被其他外设占用，导致无法提供所需频率。该时钟源可能会被其他外设占用而导致频率无法更改，此时除非 APLL 仍能准确分频得到 DAC DMA 的目标频率，否则将无法使用该时钟源。


余弦波输出（余弦模式）
^^^^^^^^^^^^^^^^^^^^^^^^

DAC 外设中包含一个余弦波发生器，可以在通道上产生余弦波。用户可以配置余弦波的频率、振幅和相位。启用该模式需要先调用 :cpp:func:`dac_cosine_new_channel` 将 DAC 转换成余弦模式，然后调用 :cpp:func:`dac_cosine_start` 启动余弦波发生器。

目前，余弦波发生器仅有 ``RTC_FAST`` 一个时钟源，可通过选择 :cpp:enumerator:`dac_cosine_clk_src_t::DAC_COSINE_CLK_SRC_RTC_FAST` 来启用该时钟源。该时钟源为默认时钟源，与 :cpp:enumerator:`dac_cosine_clk_src_t::DAC_COSINE_CLK_SRC_RTC_DEFAULT` 相同。

电源管理
^^^^^^^^

启用电源管理时（即开启 :ref:`CONFIG_PM_ENABLE`），系统会在进入 Light-sleep 模式前调整或停止 DAC 时钟源，这可能会影响 DAC 信号，从而导致数据无法正确转换。

在连续模式下使用 DAC 驱动时，可以通过获取电源管理锁来防止系统在 DMA 或余弦波模式下改变或停止时钟源。时钟源为 APB 时，锁的类型将被设置为 :cpp:enumerator:`esp_pm_lock_type_t::ESP_PM_APB_FREQ_MAX`。时钟源为 APLL 时（仅在 DMA 模式下），锁的类型将被设置为 :cpp:enumerator:`esp_pm_lock_type_t::ESP_PM_NO_LIGHT_SLEEP`。在进行 DAC 转换时（即 DMA 或余弦波发生器运行时），驱动程序会保证在调用 :cpp:func:`dac_continuous_enable` 后获取电源管理锁。同样地，在调用 :cpp:func:`dac_continuous_disable` 时，驱动程序会释放锁。

IRAM 安全
^^^^^^^^^

默认情况下，由于写入或擦除 flash 等原因导致 cache 被禁用时，DAC 的 DMA 中断将产生延迟，无法及时执行 DMA EOF 中断。

在实时应用中，可通过启用 Kconfig 选项 :ref:`CONFIG_DAC_ISR_IRAM_SAFE` 来避免此种情况发生，启用后：

1. 即使在 cache 被禁用的情况下，也可以启用中断服务。

2. 驱动对象会被放入 DRAM（以防其被意外链接到 PSRAM）。

此时在 cache 被禁用时仍可以运行中断，但会增加 IRAM 内存消耗。

线程安全
^^^^^^^^

驱动程序可保证所有公共 DAC API 的线程安全，用户可以从不同的 RTOS 任务中调用这些 API，而不需要额外的锁来保护。注意，DAC 驱动使用 mutex 锁来保证线程安全，因此不允许在 ISR 中使用除了 :cpp:func:`dac_oneshot_output_voltage` 之外的 API。

Kconfig 选项
^^^^^^^^^^^^^

- :ref:`CONFIG_DAC_ISR_IRAM_SAFE` 控制默认 ISR 处理程序在 cache 被禁用时能否继续运行。更多信息可参考 `IRAM 安全 <#iram-safe>`__。
- :ref:`CONFIG_DAC_SUPPRESS_DEPRECATE_WARN` 控制是否在使用原有 DAC 驱动时关闭警告信息。
- :ref:`CONFIG_DAC_ENABLE_DEBUG_LOG` 用于启用调试日志输出。启用该选项将增加固件的二进制文件大小。

.. only:: esp32

    - :ref:`CONFIG_DAC_DMA_AUTO_16BIT_ALIGN` 会在驱动中自动将 8 位数据扩展为 16 位数据，以满足 I2S DMA 格式。

应用示例
--------

- :example:`peripherals/dac/dac_continuous/signal_generator` 演示了在 {IDF_TARGET_NAME} 上使用 DAC 驱动程序输出连续电压的两种方式：通过 DMA 传输、以及通过定时器中断。两种方式可以生成不同的波形，如正弦波、三角波、锯齿波和方波。
- :example:`peripherals/dac/dac_continuous/dac_audio` 演示了如何在 {IDF_TARGET_NAME} 上使用 DAC 驱动程序播放存储在 buffer 中的音频，每秒钟通过扬声器或耳机播放一次。
- :example:`peripherals/dac/dac_cosine_wave` 演示了如何在 {IDF_TARGET_NAME} 开发板上使用 DAC 驱动程序在两个通道上输出余弦波，并且可以通过示波器或内部的 ADC 通道进行监控。
- :example:`peripherals/dac/dac_oneshot` 演示了如何在 {IDF_TARGET_NAME} 上使用 DAC 驱动程序输出每 500 毫秒阶跃增加一次的电压，并且会定期复位为 0。输出的电压可通过 ADC 或可选的示波器进行监测。

API 参考
--------

.. include-build-file:: inc/dac_oneshot.inc
.. include-build-file:: inc/dac_cosine.inc
.. include-build-file:: inc/dac_continuous.inc
.. include-build-file:: inc/components/esp_driver_dac/include/driver/dac_types.inc
.. include-build-file:: inc/components/hal/include/hal/dac_types.inc
