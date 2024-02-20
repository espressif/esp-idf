时钟树
==========

:link_to_translation:`en:[English]`

{IDF_TARGET_RC_FAST_VAGUE_FREQ: default="17.5", esp32="8", esp32s2="8", esp32h2="8"}

{IDF_TARGET_RC_FAST_ADJUSTED_FREQ: default="17.5", esp32="8.5", esp32s2="8.5", esp32h2="8.5"}

{IDF_TARGET_XTAL_FREQ: default="40", esp32="2 ~ 40", esp32c2="40/26", esp32h2="32"}

{IDF_TARGET_RC_SLOW_VAGUE_FREQ: default="136", esp32="150", esp32s2="90"}

{IDF_TARGET_OSC_SLOW_PIN: default="GPIO0", esp32c2="pin0（时钟信号频率不超过 136 kHz 时）", "esp32c6="GPIO0", esp32h2="GPIO13"}

{IDF_TARGET_NAME} 的时钟子系统用于从一系列根时钟中提取并分配系统/模块时钟。时钟树驱动程序负责维护系统时钟的基本功能，并管理模块时钟间的复杂关系。

本文档首先介绍了根时钟和模块时钟，随后介绍了可供用户调用的时钟树 API，调用这些 API，可以监测模块时钟的运行状态。

简介
------------

本节列出了 {IDF_TARGET_NAME} 支持的根时钟和模块时钟的定义，这些定义通常用于驱动程序配置，有助于为外设选择合适的时钟源。

根时钟
^^^^^^^^^^^

根时钟会产生可靠的时钟信号，经各种门、复用器、分频器或倍频器传递，这些时钟信号最终成为 CPU 内核、Wi-Fi、蓝牙、RTC 及外设等功能模块的时钟源。

{IDF_TARGET_NAME} 的根时钟列在 :cpp:type:`soc_root_clk_t` 中：

    .. list::

        - 内部 {IDF_TARGET_RC_FAST_VAGUE_FREQ} MHz RC 振荡器 (RC_FAST)

            此 RC 振荡器可产生约 {IDF_TARGET_RC_FAST_ADJUSTED_FREQ} MHz 的时钟信号输出，标识为 ``RC_FAST_CLK``。

            .. only:: SOC_CLK_RC_FAST_D256_SUPPORTED

                此约 {IDF_TARGET_RC_FAST_ADJUSTED_FREQ} MHz 的信号也会传入可配置的分频器，默认情况下，该分频器会将输入的时钟频率分频 256 倍，生成信号  ``RC_FAST_D256_CLK``。

                在运行时，通过在 ``RC_FAST_D256_CLK`` 上校准，可以计算 ``RC_FAST_CLK`` 的实际频率。

            .. only:: not SOC_CLK_RC_FAST_D256_SUPPORTED and SOC_CLK_RC_FAST_SUPPORT_CALIBRATION

                在运行时，通过校准，可以计算 ``RC_FAST_CLK`` 的实际频率。

            .. only:: not SOC_CLK_RC_FAST_SUPPORT_CALIBRATION

                在运行时，无法通过校准计算 ``RC_FAST_CLK`` 的实际频率，但仍可以将时钟信号引出到 GPIO 管脚，通过示波器或逻辑分析仪获取频率。

        - 外部 {IDF_TARGET_XTAL_FREQ} MHz 晶振 (XTAL)

        - 内部 {IDF_TARGET_RC_SLOW_VAGUE_FREQ} kHz RC 振荡器 (RC_SLOW)

            此 RC 振荡器产生约 {IDF_TARGET_RC_SLOW_VAGUE_FREQ} kHz 的时钟信号输出，标识为 ``RC_SLOW_CLK``。在运行时，通过校准，可以计算该时钟信号的实际频率。

        .. only:: SOC_CLK_XTAL32K_SUPPORTED

            - 外部 32 kHz 晶振 - 可选 (XTAL32K)

                .. only:: esp32

                    ``XTAL32K_CLK`` 的时钟源可以是连接到 ``32K_XP`` 和 ``32K_XN`` 管脚的 32 kHz 晶振，也可以是外部电路生成的 32 kHz 时钟信号。如果使用外部电路生成的时钟信号，该信号必须连接到 ``32K_XN`` 管脚，并且在 ``32K_XP`` 管脚和地之间连接一个 1 nF 的电容。此时，``32K_XP`` 管脚不能用作 GPIO 管脚。

                .. only:: esp32p4

                    ``XTAL32K_CLK`` 的时钟源是连接到 ``XTAL_32K_P`` 和 ``XTAL_32K_N`` 管脚的 32 kHz 晶振。

                .. only:: not esp32 and not esp32p4

                     ``XTAL32K_CLK`` 的时钟源可以是连接到 ``XTAL_32K_P`` 和 ``XTAL_32K_N`` 管脚的 32 kHz 晶振，也可以是外部电路生成的 32 kHZ 时钟信号。如果使用外部电路生成的时钟信号，该信号必须连接到 ``XTAL_32K_P`` 管脚。

                通过校准，可以计算 ``XTAL32K_CLK`` 的实际频率。

        .. only:: SOC_CLK_OSC_SLOW_SUPPORTED

            - 外部慢速时钟 - 可选 (OSC_SLOW)

                将外部电路生成的时钟信号连接到 {IDF_TARGET_OSC_SLOW_PIN}，可作为 ``RTC_SLOW_CLK`` 的时钟源。通过校准，可以计算该时钟信号的实际频率。

        .. only:: SOC_CLK_RC32K_SUPPORTED

            - 内部 32 kHz RC 振荡器 (RC32K)

                在运行时，通过校准，可以计算该时钟信号的实际频率。

与晶振产生的信号相比，从 RC 振荡器电路产生的信号通常精度较低，且容易受环境影响。因此，{IDF_TARGET_NAME} 为 ``RTC_SLOW_CLK`` 提供了几种时钟源选项，可以根据对系统时间精度和对功耗的要求选择。更多详情，请参阅 :ref:`rtc-clock-source-choice`。

模块时钟
^^^^^^^^^^^^^

{IDF_TARGET_NAME} 的可用模块时钟在 :cpp:type:`soc_module_clk_t` 中列出，每个模块时钟都有其唯一 ID。查阅文档中的枚举值，即可获取各模块时钟的详细信息。

使用 API
---------

时钟树驱动程序提供了一个一体化接口，可以获取模块时钟的频率，即 :cpp:func:`esp_clk_tree_src_get_freq_hz`。通过该函数，你可以在任何时刻，通过提供时钟名称 :cpp:enum:`soc_module_clk_t` 和指定返回频率值的精度级别 :cpp:enum:`esp_clk_tree_src_freq_precision_t`，获取时钟频率。

API 参考
-------------

.. include-build-file:: inc/clk_tree_defs.inc
.. include-build-file:: inc/esp_clk_tree.inc
