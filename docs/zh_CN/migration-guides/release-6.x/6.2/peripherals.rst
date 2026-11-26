外设驱动
========

:link_to_translation:`en:[English]`

ADC
---

ADC eFuse 校准的内部实现已迁移至 ``esp_hal_ana_conv`` 组件。内部头文件 ``esp_efuse_rtc_calib.h`` 和 ``esp_efuse_rtc_table.h`` 已被移除。应用程序应使用公开的 :doc:`ADC 校准 API </api-reference/peripherals/adc/adc_calibration>`，不应依赖内部 HAL 接口。

DMA
---

统一 ``dma_burst_size`` 的默认值语义
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:cpp:member:`uhci_controller_config_t::dma_burst_size`、:cpp:member:`async_memcpy_config_t::dma_burst_size` 和 :cpp:member:`async_crc_config_t::dma_burst_size` 现在把 ``0`` 视为未设置，并回落到该驱动自己推荐的默认值。这与 I2S、SPI、LCD 的约定一致，也补上了原先的漏洞：配置结构体零初始化时只能关掉 burst，无法表达“使用驱动默认值”。

``1`` 是新引入的含义，表示显式关闭数据突发传输。单 beat 突发相对于非突发没有收益。

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - ``dma_burst_size``
     - 含义
   * - ``0``
     - 回落到该驱动推荐的默认值
   * - ``1``
     - 关闭数据突发传输
   * - ``N`` （``N > 1``，且为 2 的幂）
     - 用户指定的突发大小

.. only:: SOC_I2S_SUPPORTS_APLL

    I2S
    ---

    APLL 可调模式与时钟抖动
    ~~~~~~~~~~~~~~~~~~~~~~~

    I2S 的 STD、TDM、PDM TX 和 PDM RX 时钟配置结构体新增了 ``flags.clk_src_fixed``\ （默认为 ``false``\ ），用于选择 APLL 的使用方式：

    - ``false`` （默认）：使用独占的可调模式。驱动可以在初始化、时钟重配置和速率微调时调整 APLL 频率，以获得更合适的分频关系并减少由分频引入的时钟抖动。兼容的 STD/TDM 全双工通道可以作为一个整体共享该可调模式。
    - ``true`` ：使用固定共享模式。APLL 频率确定后，后续重配置和速率微调通常只调整 I2S 分频器。

    由于可调模式会独占 APLL，并且会优先为主通道选择整数分频，此前选择 ``I2S_CLK_SRC_APLL`` 的应用在迁移后可能遇到以下报错或行为变化：

    - 初始化第二个 APLL 使用者时返回 ``ESP_ERR_INVALID_STATE``\ ：可调模式不允许与其他外设、另一个 I2S 控制器、同一控制器上互不相关的单工 I2S 通道或第二个 PDM 通道共享 APLL。
    - 在少数高 MCLK 的非对称全双工配置下返回 ``ESP_ERR_INVALID_ARG``\ ：此时不存在既能让主通道整数分频、又落在从通道分频范围内的 APLL 频率。
    - 时钟重配置或调用 ``i2s_channel_tune_rate`` 会修改 APLL 频率（包括覆盖预先配置的 APLL 频率），而不是保持原有频率不变。

    如果遇到上述报错，需要与其他外设或互不相关的 I2S 通道共享 APLL，或希望初始化后保持 APLL 频率固定，请显式设置 ``flags.clk_src_fixed = true``\ 。需要注意，固定模式下在初始化后修改采样率可能会使用小数分频并引入周期抖动。有关模式选择、全双工规则以及抖动成因和规避方法，请参阅 :ref:`i2s-apll-access-mode` 和 :ref:`i2s-clock-jitter`\ 。

PPA
---

阻塞操作行为变更
~~~~~~~~~~~~~~~~~

对于任何阻塞的 PPA 操作，现在注册的 ``on_trans_done`` 回调函数不会在操作完成后被调用。用户需要手动在操作返回后执行必要的操作。对于非阻塞的 PPA 操作，所有行为都与之前一致。
