时钟树
==========

:link_to_translation:`en:[English]`

{IDF_TARGET_RC_FAST_VAGUE_FREQ: default="17.5", esp32="8", esp32s2="8", esp32h2="8", esp32h21="20", esp32h4="20"}

{IDF_TARGET_RC_FAST_ADJUSTED_FREQ: default="17.5", esp32="8.5", esp32s2="8.5", esp32h2="8.5", esp32h21="20", esp32h4="20"}

{IDF_TARGET_XTAL_FREQ: default="40", esp32="2 ~ 40", esp32c2="40/26", esp32h2="32", esp32c5="48", esp32h21="32", esp32h4="32"}

{IDF_TARGET_RC_SLOW_VAGUE_FREQ: default="136", esp32="150", esp32s2="90", esp32h21="600", esp32h4="600"}

{IDF_TARGET_OSC_SLOW_PIN: default="GPIO0", esp32c2="pin0（时钟信号频率不超过 136 kHz 时）", "esp32c6="GPIO0", esp32h2="GPIO13", esp32h21="GPIO11", esp32h4="GPIO5"}

{IDF_TARGET_NAME} 的时钟子系统用于从一系列根时钟中提取并分配系统/模块时钟。时钟树驱动程序负责维护系统时钟的基本功能，并管理模块时钟间的复杂关系。

本文档首先介绍了根时钟和模块时钟，随后介绍可供用户调用的时钟树 API。通过这些 API，可以查询模块时钟的运行频率，并配置支持频率调整的模块时钟。

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

                .. only:: esp32p4 or esp32s31

                    ``XTAL32K_CLK`` 的时钟源是连接到 ``XTAL_32K_P`` 和 ``XTAL_32K_N`` 管脚的 32 kHz 晶振。

                .. only:: not esp32 and not esp32p4 and not esp32s31

                     ``XTAL32K_CLK`` 的时钟源可以是连接到 ``XTAL_32K_P`` 和 ``XTAL_32K_N`` 管脚的 32 kHz 晶振，也可以是外部电路生成的 32 kHZ 时钟信号。如果使用外部电路生成的时钟信号，该信号必须连接到 ``XTAL_32K_P`` 管脚。

                通过校准，可以计算 ``XTAL32K_CLK`` 的实际频率。

        .. only:: SOC_CLK_OSC_SLOW_SUPPORTED

            - 外部慢速时钟 - 可选 (OSC_SLOW)

                将外部电路生成的时钟信号连接到 {IDF_TARGET_OSC_SLOW_PIN}，可作为 ``RTC_SLOW_CLK`` 的时钟源。通过校准，可以计算该时钟信号的实际频率。

与晶振产生的信号相比，从 RC 振荡器电路产生的信号通常精度较低，且容易受环境影响。因此，{IDF_TARGET_NAME} 为 ``RTC_SLOW_CLK`` 提供了几种时钟源选项，可以根据对系统时间精度和对功耗的要求选择。更多详情，请参阅 :ref:`rtc-clock-source-choice`。

模块时钟
^^^^^^^^^^^^^

{IDF_TARGET_NAME} 的可用模块时钟在 :cpp:type:`soc_module_clk_t` 中列出，每个模块时钟都有其唯一 ID。查阅文档中的枚举值，即可获取各模块时钟的详细信息。

使用 API
---------

时钟树驱动程序提供了一个一体化接口，可以获取模块时钟的频率，即 :cpp:func:`esp_clk_tree_src_get_freq_hz`。通过该函数，你可以在任何时刻，通过提供时钟名称 :cpp:enum:`soc_module_clk_t` 和指定返回频率值的精度级别 :cpp:enum:`esp_clk_tree_src_freq_precision_t`，获取时钟频率。

对于支持频率配置的模块时钟，可以调用 :cpp:func:`esp_clk_tree_src_set_freq_hz` 请求设置频率。由于硬件分频或量化限制，实际频率可能与期望频率存在细微差异，因此应通过该函数的输出参数获取实际配置的频率。

.. only:: SOC_CLK_APLL_SUPPORTED

    APLL 资源管理
    ^^^^^^^^^^^^^

    APLL 是由多个外设共享的系统时钟源。驱动程序申请 APLL 时会声明是否需要调整其频率，时钟树根据当前使用情况在以下模式之间切换：

    - **空闲模式**：当前没有外设使用 APLL。此时可以调用 :cpp:func:`esp_clk_tree_src_set_freq_hz` 预配置频率；时钟树会保存计算出的频率及配置参数，并在首个外设申请 APLL 时将配置应用到硬件。首个申请者决定 APLL 进入固定模式还是可调模式。
    - **固定模式**：默认的共享模式，允许多个不需要调节 APLL 频率的外设同时使用。APLL 尚未获得有效配置时，第一个成功设置的频率将成为共享频率；配置生效后，仅接受与当前实际频率相同的幂等设置请求，设置其他频率将返回 :c:macro:`ESP_ERR_INVALID_STATE`。固定模式下不接受可调模式申请。
    - **可调模式**：独占模式，仅允许一个需要调节 APLL 频率的外设使用。持有者可以通过 :cpp:func:`esp_clk_tree_src_set_freq_hz` 修改 APLL 频率；在该外设释放 APLL 前，其他固定或可调模式申请都会失败。

    最后一个使用者释放 APLL 后，时钟树会关闭 APLL、清除当前频率配置并恢复为空闲模式。后续申请者可以重新选择固定或可调模式。

    .. note::

        建议在应用程序初始化早期、相关外设驱动初始化之前预配置 APLL，以便多个外设使用统一的时钟源频率。外设驱动初始化后，不应通过 :cpp:func:`esp_clk_tree_src_set_freq_hz` 手动调整 APLL，否则可能导致外设的实际工作时钟与驱动配置不一致。

API 参考
-------------

.. include-build-file:: inc/clk_tree_defs.inc
.. include-build-file:: inc/esp_clk_tree.inc
